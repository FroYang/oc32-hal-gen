"""
OC32 HAL 配置文件生成工具 (standalone exe)

支持多芯片型号，生成完整的 hal 目录（含 oc32_hal_conf.h 及其他 HAL 源文件）
"""
import json
import os
import sys
import shutil
import subprocess
from pathlib import Path
from tkinter import *
from tkinter import ttk, filedialog, messagebox

# Get resource path for bundled apps
def get_resource_path(relative_path):
    """Get absolute path to resource, works for dev and for PyInstaller bundle"""
    if hasattr(sys, '_MEIPASS'):
        return os.path.join(sys._MEIPASS, relative_path)
    return os.path.join(os.path.dirname(__file__), relative_path)

# Paths
ROOT_DIR = Path(__file__).parent
CHIPS_DIR = ROOT_DIR / "chips"
HAL_SRC_DIR = ROOT_DIR / "hal"  # 源 HAL 目录（包含 chip/, inc/ 和 src/）

# Load chips config
def load_chips_config():
    config = {}
    for f in CHIPS_DIR.glob("*.json"):
        with open(f, "r", encoding="utf-8") as fh:
            data = json.load(fh)
            config[f.stem] = data
    return config

CHIPS = load_chips_config()

# Canonical clock-source list. Used by the generator (default when a chip omits
# 'clk_sources') and as the base set for filtering.
ALL_CLOCK_SOURCES = ["IHOSC", "ILOSC", "XHOSC", "XLOSC"]


def _colored_label_frame(parent, text, color, padx=8, pady=6):
    """自定义带颜色标题的 LabelFrame，label 用 Canvas 实现以支持 font/fg 属性"""
    outer = Frame(parent)
    outer.pack(fill=X, padx=2, pady=2)
    label_bg = "#f0f0f0"
    title_bar = Frame(outer, bg=label_bg, height=22)
    title_bar.pack(fill=X)
    title_bar.pack_propagate(False)
    canvas = Canvas(title_bar, width=400, height=22, bg=label_bg,
                    highlightthickness=0)
    canvas.pack(side=LEFT, fill=BOTH, expand=True)
    canvas.create_text(6, 11, anchor=W, text=text,
                       font=("Microsoft YaHei", 9, "bold"), fill=color)
    sep = Frame(outer, bg="#cccccc", height=1)
    sep.pack(fill=X)
    content = Frame(outer, padx=padx, pady=pady)
    content.pack(fill=BOTH, expand=True)
    return content


def _gen_uart_debug_sub(uart_name):
    if uart_name == "UART0":
        return '#define HAL_UART0_DEBUG\n/* #define HAL_UART1_DEBUG */'
    else:
        return '/* #define HAL_UART0_DEBUG */\n#define HAL_UART1_DEBUG'


# Debug UART option labels -> HAL macro enum values from oc32_hal_uart.h.
# NOTE: the HAL parity enum has no explicit "None" member; "None" maps to the
# zero value (UART_PAR_ODD = 0), the closest available setting.
_UART_PARITY_MACROS = {
    "None":       "UART_PAR_NONE",
    "Odd":        "UART_PAR_ODD",
    "Even":       "UART_PAR_EVEN",
    "Always 0":   "UART_PAR_0",
    "Always 1":   "UART_PAR_1",
}
_UART_DUPLEX_MACROS = {
    "Full": "UART_FULL_DUPLEX",
    "Half (use TX pin only)": "UART_HALF_DUPLEX",
}
_UART_PINS_MACROS = {
    "TX0 & RX0": "UART_PINS_0",
    "TX1 & RX1": "UART_PINS_1",
}


def _gen_uart_debug_opts(user_settings):
    """Generate #define lines for Debug UART parity/duplex/pins options."""
    lines = []
    parity = user_settings.get("uart_parity", "None")
    duplex = user_settings.get("uart_duplex", "Full")
    pins = user_settings.get("uart_pins", "TX0 & RX0")
    lines.append(f"#define HAL_UART_DEBUG_PARITY   {_UART_PARITY_MACROS[parity]}")
    lines.append(f"#define HAL_UART_DEBUG_DUPLEX   {_UART_DUPLEX_MACROS[duplex]}")
    lines.append(f"#define HAL_UART_DEBUG_PINS     {_UART_PINS_MACROS[pins]}")
    return "\n".join(lines)


def _gen_uart_debug_block(user_settings, features, user_features):
    """Render the entire UART-debug block, or an empty string when the UART
    feature is not selected (so no floating/empty #ifdef block is emitted).

    The block is gated on the UART *feature* being enabled in the chip's
    `features` list; baud rate, UART0/1 select, and the parity/duplex/pins
    options all live inside the #ifdef so they only exist when active.
    """
    uart_feature = next((m for m in features if m["key"] == "UART"), None)
    if not (uart_feature and user_features.get("UART", False)):
        return ""

    block = (
        "#define HAL_UART_DEBUG_ENABLE\n"
        "\n"
        "#ifdef HAL_UART_DEBUG_ENABLE\n"
        f"#define HAL_UART_DEBUG_BR   {user_settings['baud_rate']} /* 9600/14400 .... ~ 1000000 */\n"
        f"{_gen_uart_debug_sub(user_settings['debug_uart'])}\n"
        f"{_gen_uart_debug_opts(user_settings)}\n"
        "#endif"
    )
    return block


# --- Config rendering ---
def render_conf_template(chip_config, user_settings):
    tmpl_path = HAL_SRC_DIR / "oc32_hal_conf_template.h"
    with open(tmpl_path, "r", encoding="utf-8") as f:
        content = f.read()

    ihosc = chip_config.get("ihosc_freq", 24000000)
    ilosc = chip_config.get("ilosc_freq", 4000)
    debug_uart = user_settings["debug_uart"]
    int_priority = chip_config["int_priority"]
    dma_channels = chip_config["dma_channels"]
    features = chip_config["features"]
    user_features = user_settings.get("features", {})

    name = chip_config.get("name", "OC32")
    content = content.replace("__HAL_CONFIG_NAME__", name)

    # Clock definitions
    xhosc = user_settings.get("xhosc_freq", 4)
    xlosc = user_settings.get("xlosc_freq", 32.768)
    content = content.replace("__HAL_CONFIG_XHOSC_FREQ__", str(int(xhosc) * 1000000))
    content = content.replace("__HAL_CONFIG_XLOSC_FREQ__", str(int(round(float(xlosc) * 1000))))
    content = content.replace("__HAL_CONFIG_IHOSC_FREQ__", str(ihosc))
    content = content.replace("__HAL_CONFIG_ILOSC_FREQ__", str(ilosc))
    # System Clock: user-entered value in MHz -> Hz
    sclk_mhz = user_settings.get("sclk_mhz", 99)
    content = content.replace("__HAL_CONFIG_SCLK_FREQ__", str(int(sclk_mhz) * 1000000))

    # USB CDR: drives HAL_RCM_CDR_ENABLE + OC32_CDR_FREQ directly.
    # Only emitted when the chip defines cdr_freq and the toggle is on.
    cdr_enabled = user_settings.get("cdr_enable", False)
    if chip_config.get("cdr_freq") is not None and cdr_enabled:
        cdr_lines = (
            f"#define HAL_RCM_CDR_ENABLE\n"
            f"#define OC32_CDR_FREQ ((uint32_t){chip_config['cdr_freq']}U)     /* USB CDR output frequency in Hz */"
        )
    else:
        cdr_lines = ""
    content = content.replace("__HAL_CONFIG_CDR_LINES__", cdr_lines)

    # Clock source
    clk_src = user_settings.get("clk_src", "IHOSC").upper()
    content = content.replace("__HAL_CONFIG_CLK_SRC__", f"OC32_CLK_SRC_{clk_src}")
    # PLL
    pll = user_settings.get("pll", "Disable").upper()
    content = content.replace("__HAL_CONFIG_PLL__", f"OC32_CLK_PLL_{pll}")
    # PLL Frequency Over-Monitor
    pll_om = user_settings.get("pll_om", "Disable").upper()
    content = content.replace("__HAL_CONFIG_PLL_OM__", f"OC32_CLK_PLL_OM_{pll_om}")

    # ILOSC real vs. fake marker. 'ilosc_real' (default false) only matters when
    # ILOSC is a supported clock source for this chip.
    ilosc_avail = "ILOSC" in (
        set(chip_config.get("clk_sources")) if chip_config.get("clk_sources")
        else set(ALL_CLOCK_SOURCES)
    )
    if ilosc_avail:
        ilosc_macro = "OC32_CLK_ILOSC_REAL" if chip_config.get("ilosc_real", False) \
            else "OC32_CLK_ILOSC_FAKE"
        content = content.replace("__HAL_CONFIG_ILOSC_TYPE__", f"#define {ilosc_macro}")
    else:
        content = content.replace("__HAL_CONFIG_ILOSC_TYPE__", "")

    # FLASH definitions — match reference output format (hex, with comments)
    flash_kb = chip_config["flash_size"]
    zone0_kb = chip_config["zone0_size"]
    zone1_kb = chip_config["zone1_size"]
    content = content.replace("__HAL_CONFIG_FLASH_SIZE__", f'0x{flash_kb * 1024:X}U')
    content = content.replace("/* Flash comment */", f'/* {flash_kb}K Flash */')
    content = content.replace("__HAL_CONFIG_ZONE0_SIZE__", f'0x{zone0_kb * 1024:X}U')
    content = content.replace("/* Zone0 comment */", f'/* {zone0_kb} KB Boot zone */')
    content = content.replace("__HAL_CONFIG_ZONE1_SIZE__", f'0x{zone1_kb * 1024:X}U')
    content = content.replace("/* Zone1 comment */", f'/* {zone1_kb} KB User zone */')
    content = content.replace("/* Bank comment */", f'/* {flash_kb // 128} banks in a {flash_kb}k flash*/')

    # VDD / Tick
    content = content.replace("__HAL_CONFIG_VDD_VALUE__", f'{chip_config["vdd_value"]}U')
    content = content.replace("__HAL_CONFIG_TICK_FREQ__", f'HAL_TICK_FREQ_{user_settings["tick_freq"]}')

    # Int enum — generate PRI0..PRI(n-1) entries
    int_enum_lines = []
    for i in range(int_priority):
        suffix = f"{i:02d}" if i >= 10 else str(i)
        comma = "," if i < int_priority - 1 else ""
        int_enum_lines.append(f"OC32_HIC_PRI{suffix}       = {i}U{comma}")
    content = content.replace("__HAL_CONFIG_HIC_PRI_ENUM__", "\n".join(int_enum_lines))    

    # DMA ch enum — generate CH0..CH(n-1) entries
    dma_ch_lines = []
    for i in range(dma_channels):
        suffix = f"{i:02d}" if i >= 10 else str(i)
        dma_ch_lines.append(f"OC32_DMA_CH{suffix}       = {i}U,")
    content = content.replace("__HAL_CONFIG_DMA_CH__", "\n".join(dma_ch_lines))

    # DMA channel map — build from dma_map array in chip config
    dma_map = chip_config.get("dma_map", [])
    dma_map_lines = []
    for i, periph in enumerate(dma_map):
        label = periph
        dma_map_lines.append(f"OC32_DMA_{label:<16} = {i}U,")
    content = content.replace("__HAL_CONFIG_DMA_MAP__", "\n".join(dma_map_lines))

    # HIC channel map — build from hic_map array in chip config
    hic_map = chip_config.get("hic_map", [])
    hic_ch_lines = []
    hic_map_lines = []
    for i, periph in enumerate(hic_map):
        label = periph
        hic_ch_lines.append(f"OC32_HIC_CH_{label:<16} = {i}U,")
        hic_map_lines.append(f"OC32_HIC_{label:<16} = 1U << {i},")
    content = content.replace("__HAL_CONFIG_HIC_CH__", "\n".join(hic_ch_lines))
    content = content.replace("__HAL_CONFIG_HIC_MAP__", "\n".join(hic_map_lines))

    # UART debug section — emit the whole #define + #ifdef block, or nothing.
    # This keeps the generated conf.h clean when the UART feature is off (no
    # floating/empty #ifdef block left behind).
    content = content.replace("__HAL_CONFIG_UART_DEBUG_BLOCK__",
        _gen_uart_debug_block(user_settings, features, user_features))

    # User modules enable section — driven by GUI checkbox state from user_settings
    module_lines = []
    comment_lines = []
    for mod in features:
        key = mod["key"]
        always = mod.get("always", False)
        macro_key = mod.get("macro_name")
        macro_name = f"HAL_{macro_key}_ENABLE" if macro_key else f"HAL_{key}_ENABLE"
        if always:
            module_lines.append(f"#define {macro_name}")
        elif user_features.get(key, False):
            module_lines.append(f"#define {macro_name}")
        else:
            comment_lines.append(f"   /* #define {macro_name} */")
    content = content.replace("__HAL_CONFIG_USER_MODULES__",
        "\n".join(module_lines + comment_lines))

    return content


# --- App ---
class App:
    def __init__(self, root):
        self.root = root
        self.root.title("OC32 HAL Config Generator")
        self.root.geometry("750x650")
        self.root.resizable(True, True)

        self.chip_config = None
        self.output_dir = str(Path.home() / "Desktop" )
        self.module_vars = {}      # key -> IntVar (persisted across chip changes)
        self._last_chip_key = None # track last selected chip

        self._build_ui()

    def _build_ui(self):
        main = PanedWindow(self.root, orient=HORIZONTAL)
        main.pack(fill=BOTH, expand=True, padx=10, pady=10)

        # Left panel: Settings
        left = Frame(main)
        main.add(left, width=400)

        # Top: Chip selection
        chip_frame = _colored_label_frame(left, "Chip Selection", "red")
        chip_frame.pack(fill=X, padx=2, pady=2)

        self.var_chip = StringVar(value=list(CHIPS.keys())[0])
        chip_combo = ttk.Combobox(
            chip_frame, textvariable=self.var_chip,
            values=list(CHIPS.keys()), state="readonly", width=22
        )
        chip_combo.pack(pady=4)
        chip_combo.bind("<<ComboboxSelected>>", self._on_chip_changed)

        # Middle: System settings — scrollable so the many rows never push the
        # Output-dir + button rows below the visible area.
        sys_outer = Frame(left)
        sys_outer.pack(fill=X, padx=2, pady=2)
        sys_label_bg = "#f0f0f0"
        sys_title_bar = Frame(sys_outer, bg=sys_label_bg, height=22)
        sys_title_bar.pack(fill=X)
        sys_title_bar.pack_propagate(False)
        sys_title_canvas = Canvas(sys_title_bar, width=400, height=22, bg=sys_label_bg,
                                  highlightthickness=0)
        sys_title_canvas.pack(side=LEFT, fill=BOTH, expand=True)
        sys_title_canvas.create_text(6, 11, anchor=W, text="System Settings",
                                    font=("Microsoft YaHei", 9, "bold"), fill="blue")
        sys_title_sep = Frame(sys_outer, bg="#cccccc", height=1)
        sys_title_sep.pack(fill=X)

        sys_scroll_canvas = Canvas(sys_outer, highlightthickness=0)
        sys_scroll_sb = Scrollbar(sys_outer, orient=VERTICAL, command=sys_scroll_canvas.yview)
        sys_frame = Frame(sys_scroll_canvas, padx=8, pady=6)
        sys_frame.bind(
            "<Configure>",
            lambda e: sys_scroll_canvas.configure(scrollregion=sys_scroll_canvas.bbox("all"))
        )
        sys_scroll_canvas.create_window((0, 0), window=sys_frame, anchor="nw")
        sys_scroll_canvas.configure(yscrollcommand=sys_scroll_sb.set)
        # Keep the settings scrollable and compact so the left panel never
        # forces the window taller than needed.
        sys_scroll_canvas.config(height=380)
        sys_scroll_canvas.pack(side=LEFT, fill=BOTH, expand=True)
        sys_scroll_sb.pack(side=RIGHT, fill=Y)

        # Clock source (new, at the front)
        clk_src_frame = Frame(sys_frame)
        clk_src_frame.pack(fill=X, pady=2)
        Label(clk_src_frame, text="Clock Source:").pack(side=LEFT, anchor=W)
        self.var_clk_src = StringVar(value="IHOSC")
        self.clk_src_combo = ttk.Combobox(clk_src_frame, textvariable=self.var_clk_src,
                                          values=["IHOSC", "ILOSC", "XHOSC", "XLOSC"],
                                          state="readonly", width=8)
        self.clk_src_combo.pack(side=RIGHT, padx=(4, 0))

        # PLL (new)
        pll_frame = Frame(sys_frame)
        pll_frame.pack(fill=X, pady=2)
        Label(pll_frame, text="PLL:").pack(side=LEFT, anchor=W)
        self.var_pll = StringVar(value="Disable")
        self.pll_combo = ttk.Combobox(pll_frame, textvariable=self.var_pll,
                                       values=["Enable", "Disable"], state="readonly", width=8)
        self.pll_combo.pack(side=RIGHT, padx=(4, 0))
        self.pll_combo.bind("<<ComboboxSelected>>", self._on_pll_changed)

        # PLL Frequency Over-Monitor (new, placed right after PLL)
        pll_om_frame = Frame(sys_frame)
        pll_om_frame.pack(fill=X, pady=2)
        self._pll_om_label = Label(pll_om_frame, text="PLL Frequency Over Monitor:")
        self._pll_om_label.pack(side=LEFT, anchor=W)
        self.var_pll_om = StringVar(value="Disable")
        self.pll_om_combo = ttk.Combobox(pll_om_frame, textvariable=self.var_pll_om,
                                         values=["Enable", "Disable"], state="readonly", width=8)
        self.pll_om_combo.pack(side=RIGHT, padx=(4, 0))

        # USB CDR (new) — visible/usable only when chip defines cdr_freq
        cdr_frame = Frame(sys_frame)
        cdr_frame.pack(fill=X, pady=2)
        self._cdr_label = Label(cdr_frame, text="USB CDR:")
        self._cdr_label.pack(side=LEFT, anchor=W)
        self.var_cdr = StringVar(value="Disable")
        self.cdr_combo = ttk.Combobox(cdr_frame, textvariable=self.var_cdr,
                                      values=["Enable", "Disable"], state="readonly", width=8)
        self.cdr_combo.pack(side=RIGHT, padx=(4, 0))

        # System Clock (new, user-entered MHz) — replaces chip sclk_freq
        sclk_frame = Frame(sys_frame)
        sclk_frame.pack(fill=X, pady=2)
        self._sclk_label = Label(sclk_frame, text="System Clock (MHz):")
        self._sclk_label.pack(side=LEFT, anchor=W)
        self.var_sclk = IntVar(value=99)
        self.sclk_entry = Entry(sclk_frame, textvariable=self.var_sclk, width=8)
        self.sclk_entry.pack(side=RIGHT, padx=(4, 0))

        # XHOSC frequency (user-entered, MHz)
        xhosc_frame = Frame(sys_frame)
        xhosc_frame.pack(fill=X, pady=2)
        self._xhosc_label = Label(xhosc_frame, text="XHOSC Frequency (MHz):")
        self._xhosc_label.pack(side=LEFT, anchor=W)
        self.var_xhosc = IntVar(value=4)
        self.xhosc_entry = Entry(xhosc_frame, textvariable=self.var_xhosc, width=8)
        self.xhosc_entry.pack(side=RIGHT, padx=(4, 0))

        # XLOSC frequency (user-entered, kHz, supports decimals e.g. 32.768)
        xlosc_frame = Frame(sys_frame)
        xlosc_frame.pack(fill=X, pady=2)
        self._xlosc_label = Label(xlosc_frame, text="XLOSC Frequency (kHz):")
        self._xlosc_label.pack(side=LEFT, anchor=W)
        self.var_xlosc = StringVar(value="32.768")
        self.xlosc_entry = Entry(xlosc_frame, textvariable=self.var_xlosc, width=10)
        self.xlosc_entry.pack(side=RIGHT, padx=(4, 0))

        # Tick frequency
        tick_freq_frame = Frame(sys_frame)
        tick_freq_frame.pack(fill=X, pady=2)
        Label(tick_freq_frame, text="Tick Frequency:").pack(side=LEFT, anchor=W)
        self.var_tick_freq = StringVar(value="1KHz")
        self.tick_freq_combo = ttk.Combobox(tick_freq_frame, textvariable=self.var_tick_freq,
                                           values=["10Hz", "100Hz", "1KHz"], state="readonly", width=8)
        self.tick_freq_combo.pack(side=RIGHT, padx=(4, 0))

        # Debug UART selection
        debug_uart_frame = Frame(sys_frame)
        debug_uart_frame.pack(fill=X, pady=2)
        Label(debug_uart_frame, text="Debug UART:").pack(side=LEFT, anchor=W)
        self.var_debug = StringVar(value="UART1")
        self.debug_combo = ttk.Combobox(debug_uart_frame, textvariable=self.var_debug,
                                      values=["UART0", "UART1"], state="readonly", width=8)
        self.debug_combo.pack(side=RIGHT, padx=(4, 0))

        # Baud rate
        uart_baud_frame = Frame(sys_frame)
        uart_baud_frame.pack(fill=X, pady=2)
        Label(uart_baud_frame, text="Debug UART Baud Rate:").pack(side=LEFT, anchor=W)
        self.var_baud = StringVar(value="115200")
        self.baud_combo = ttk.Combobox(uart_baud_frame, textvariable=self.var_baud,
                                       values=["9600", "14400", "19200", "38400",
                                               "57600", "115200", "230400", "460800", "921600", "1000000"],
                                       state="readonly", width=10)
        self.baud_combo.pack(side=RIGHT, padx=(4, 0))

        # Debug UART Parity
        uart_parity_frame = Frame(sys_frame)
        uart_parity_frame.pack(fill=X, pady=2)
        Label(uart_parity_frame, text="Debug UART Parity:").pack(side=LEFT, anchor=W)
        self.var_uart_parity = StringVar(value="None")
        self.uart_parity_combo = ttk.Combobox(uart_parity_frame, textvariable=self.var_uart_parity,
                                               values=["None", "Odd", "Even", "Always 0", "Always 1"],
                                               state="readonly", width=16)
        self.uart_parity_combo.pack(side=RIGHT, padx=(4, 0))

        # Debug UART Duplex
        uart_duplex_frame = Frame(sys_frame)
        uart_duplex_frame.pack(fill=X, pady=2)
        Label(uart_duplex_frame, text="Debug UART Duplex:").pack(side=LEFT, anchor=W)
        self.var_uart_duplex = StringVar(value="Full")
        self.uart_duplex_combo = ttk.Combobox(uart_duplex_frame, textvariable=self.var_uart_duplex,
                                              values=["Full", "Half (use TX pin only)"],
                                              state="readonly", width=16)
        self.uart_duplex_combo.pack(side=RIGHT, padx=(4, 0))

        # Debug UART Pins
        uart_pins_frame = Frame(sys_frame)
        uart_pins_frame.pack(fill=X, pady=2)
        Label(uart_pins_frame, text="Debug UART Pins:").pack(side=LEFT, anchor=W)
        self.var_uart_pins = StringVar(value="TX0 & RX0")
        self.uart_pins_combo = ttk.Combobox(uart_pins_frame, textvariable=self.var_uart_pins,
                                            values=["TX0 & RX0", "TX1 & RX1"],
                                            state="readonly", width=16)
        self.uart_pins_combo.pack(side=RIGHT, padx=(4, 0))

        # Output dir (left panel). Buttons live in the persistent bottom bar so
        # they are always visible regardless of panel height.
        out_frame = _colored_label_frame(left, "Output Directory", "green")
        out_frame.pack(fill=X, padx=2, pady=2)
        self.var_outdir = StringVar(value=self.output_dir)
        Entry(out_frame, textvariable=self.var_outdir).pack(fill=X, pady=2)
        Button(out_frame, text="Browse...", command=self._browse_outdir).pack(pady=0)

        # Action buttons — packed right below the Output Directory row so they
        # sit compactly next to Browse instead of in a far-away bottom bar.
        self._btn_bar = Frame(left)
        self._btn_bar.pack(fill=X, padx=2, pady=(0, 2))
        Button(self._btn_bar, text="Generate Config", command=self._generate,
               width=20).pack(side=LEFT, padx=4, pady=2)
        Button(self._btn_bar, text="Open Output Dir", command=self._open_output,
               width=20).pack(side=LEFT, padx=4, pady=2)

        # Right panel: Module selection
        right = Frame(main)
        main.add(right)

        # Module selection frame — fills all of right panel
        mod_outer = Frame(right)
        mod_outer.pack(fill=BOTH, expand=True, padx=2, pady=2)
        mod_label_bg = "#f0f0f0"
        mod_title_bar = Frame(mod_outer, bg=mod_label_bg, height=22)
        mod_title_bar.pack(fill=X)
        mod_title_bar.pack_propagate(False)
        mod_canvas_title = Canvas(mod_title_bar, width=400, height=22, bg=mod_label_bg,
                                  highlightthickness=0)
        mod_canvas_title.pack(side=LEFT, fill=BOTH, expand=True)
        mod_canvas_title.create_text(6, 11, anchor=W, text="Feature Configure",
                                     font=("Microsoft YaHei", 9, "bold"), fill="purple")
        mod_sep = Frame(mod_outer, bg="#cccccc", height=1)
        mod_sep.pack(fill=X)
        mod_frame = Frame(mod_outer, padx=8, pady=6)
        mod_frame.pack(fill=BOTH, expand=True)

        self.mod_canvas = Canvas(mod_frame)
        mod_scrollbar = Scrollbar(mod_frame, orient=VERTICAL, command=self.mod_canvas.yview)
        self.mod_scroll_frame = Frame(self.mod_canvas)
        self.mod_scroll_frame.bind(
            "<Configure>",
            lambda e: self.mod_canvas.configure(scrollregion=self.mod_canvas.bbox("all"))
        )
        self.mod_canvas.configure(yscrollcommand=mod_scrollbar.set)

        # Keep the inner frame (and thus the RIGHT-aligned checkboxes) stretched to
        # the canvas width so they stay flush to the right edge of the panel.
        # A canvas window only honors the requested width unless it is told to fill;
        # so we re-create the window each time the canvas is resized with a matching
        # width, which makes each fill=X row span the whole panel.
        self._mod_canvas_win_id = None

        def _resync_mod_width(event):
            if self._mod_canvas_win_id is not None:
                self.mod_canvas.delete(self._mod_canvas_win_id)
            self._mod_canvas_win_id = self.mod_canvas.create_window(
                (0, 0), window=self.mod_scroll_frame,
                anchor="nw", width=event.width
            )
            self.mod_scroll_frame.configure(width=event.width)

        self.mod_canvas.bind("<Configure>", _resync_mod_width)

        self.mod_canvas.pack(side=LEFT, fill=BOTH, expand=True)
        mod_scrollbar.pack(side=RIGHT, fill=Y)

        # Init module list
        # Initialize PLL-dependent controls' grayout state
        self._on_pll_changed(None)

        # Initialize oscillator support gating from the default chip
        self._apply_osc_support(CHIPS.get(list(CHIPS.keys())[0]))
        # Restrict clock-source options from the default chip
        self._apply_clk_sources(CHIPS.get(list(CHIPS.keys())[0]))

        self._on_chip_changed(None)

    def _on_pll_changed(self, event=None):
        """When PLL is disabled, gray out & disable the PLL over-monitor
        and System Clock controls (they only make sense with PLL on)."""
        pll_on = self.var_pll.get() == "Enable"
        state = "normal" if pll_on else "disabled"
        fg = "black" if pll_on else "#808080"
        # PLL Freq Over Monitor
        self.pll_om_combo.config(state=state)
        self._pll_om_label.config(fg=fg)
        if not pll_on:
            self.var_pll_om.set("Disable")
        # System Clock
        self.sclk_entry.config(state=state, fg=fg,
                               disabledforeground=fg if pll_on else "#808080")

    ALL_CLOCK_SOURCES = ["IHOSC", "ILOSC", "XHOSC", "XLOSC"]

    def _apply_clk_sources(self, chip_config):
        """Rebuild the Clock Source combobox to the sources the chip JSON lists
        in 'clk_sources' (defaults to all four when the key is absent)."""
        sources = list(self.ALL_CLOCK_SOURCES)
        if chip_config and chip_config.get("clk_sources"):
            sources = [s for s in self.ALL_CLOCK_SOURCES
                       if s in chip_config["clk_sources"]]
        self.clk_src_combo.config(values=sources)
        # Snap selection to a valid source (fall back to first if current not listed)
        cur = self.var_clk_src.get()
        if cur not in sources:
            self.var_clk_src.set(sources[0] if sources else "IHOSC")

    def _apply_osc_support(self, chip_config):
        """Enable/disable the XHOSC & XLOSC frequency entries based on whether
        the chip's 'clk_sources' list includes them (single source of truth).
        Absent 'clk_sources' -> both enabled (backward compatible)."""
        if chip_config and chip_config.get("clk_sources"):
            sources = set(chip_config["clk_sources"])
        else:
            sources = set(self.ALL_CLOCK_SOURCES)
        xhosc_on = "XHOSC" in sources
        xlosc_on = "XLOSC" in sources
        for entry, label, on in (
            (self.xhosc_entry, self._xhosc_label, xhosc_on),
            (self.xlosc_entry, self._xlosc_label, xlosc_on),
        ):
            state = "normal" if on else "disabled"
            fg = "black" if on else "#808080"
            entry.config(state=state, fg=fg,
                         disabledforeground="#808080" if not on else "black")
            label.config(fg=fg)

    def _on_chip_changed(self, event):
        key = self.var_chip.get()
        new_config = CHIPS.get(key)

        self.chip_config = new_config
        self._last_chip_key = key

        # Gray out / disable the USB CDR control when the chip has no cdr_freq
        cdr_supported = bool(new_config) and new_config.get("cdr_freq") is not None
        self.cdr_combo.config(state="normal" if cdr_supported else "disabled")
        self._cdr_label.config(fg="black" if cdr_supported else "#808080")
        if not cdr_supported:
            self.var_cdr.set("Disable")

        # Gate XHOSC/XLOSC frequency entries per chip JSON
        self._apply_osc_support(new_config)
        # Restrict clock-source options per chip JSON
        self._apply_clk_sources(new_config)

        # Clear old module checkboxes
        for child in self.mod_scroll_frame.winfo_children():
            child.destroy()
        self.module_vars.clear()

        # Create new module checkboxes
        if self.chip_config:
            optional_features = [m for m in self.chip_config["features"] if not m.get("always", False)]
            for idx, mod in enumerate(optional_features):
                frame = Frame(self.mod_scroll_frame)
                # fill=BOTH + expand makes each row stretch to the full panel
                # width, so the RIGHT-packed checkbox hugs the right edge.
                frame.pack(fill=X, pady=2)
                Label(frame, text=f"{mod['key']}:  {mod.get('description', '')}").pack(side=LEFT)
                var = IntVar(value=0)
                Checkbutton(frame, variable=var).pack(side=RIGHT, padx=(0, 8))
                self.module_vars[f"mod_{idx}"] = var

    def _collect_settings(self):
        def _safe_int(var, default):
            try:
                return int(var.get())
            except (TypeError, ValueError):
                return default

        def _safe_float(var, default):
            try:
                return float(var.get())
            except (TypeError, ValueError):
                return default

        def _base():
            return {
                "xhosc_freq": _safe_int(self.var_xhosc, 4),
                "xlosc_freq": _safe_float(self.var_xlosc, 32.768),
                "clk_src": self.var_clk_src.get(),
                "pll": self.var_pll.get(),
                "pll_om": self.var_pll_om.get(),
                "cdr_enable": self.var_cdr.get() == "Enable",
                "sclk_mhz": _safe_int(self.var_sclk, 99),
                "tick_freq": self.var_tick_freq.get(),
                "debug_uart": self.var_debug.get(),
                "uart_parity": self.var_uart_parity.get(),
                "uart_duplex": self.var_uart_duplex.get(),
                "uart_pins": self.var_uart_pins.get(),
            }

        features = {}
        if not self.chip_config:
            base = _base()
            base["features"] = features
            return base
        optional_features = [m for m in self.chip_config["features"] if not m.get("always", False)]
        for idx, mod in enumerate(optional_features):
            var = self.module_vars.get(f"mod_{idx}")
            if var is not None:
                key = mod["key"]
                val = bool(var.get())
                features[key] = val
                # Auto-check dependencies
                if val:
                    for dep in mod.get("dependencies", []):
                        features[dep] = True
        return {
            "xhosc_freq": _safe_int(self.var_xhosc, 4),
            "xlosc_freq": _safe_float(self.var_xlosc, 32.768),
            "clk_src": self.var_clk_src.get(),
            "pll": self.var_pll.get(),
            "pll_om": self.var_pll_om.get(),
            "cdr_enable": self.var_cdr.get() == "Enable",
            "sclk_mhz": _safe_int(self.var_sclk, 99),
            "tick_freq": self.var_tick_freq.get(),
            "debug_uart": self.var_debug.get(),
            "baud_rate": self.var_baud.get(),
            "uart_parity": self.var_uart_parity.get(),
            "uart_duplex": self.var_uart_duplex.get(),
            "uart_pins": self.var_uart_pins.get(),
            "features": features,
        }

    def _generate(self):
        if not self.chip_config:
            messagebox.showerror("Error", "Please select a chip first")
            return

        settings = self._collect_settings()
        content = render_conf_template(self.chip_config, settings)

        out_path = Path(self.var_outdir.get())
        out_path.mkdir(parents=True, exist_ok=True)

        # 生成 oc32_hal_conf.h 到 inc/ 子目录
        out_file = out_path / "hal" / "inc" / "oc32_hal_conf.h"
        out_file.parent.mkdir(parents=True, exist_ok=True)
        with open(out_file, "w", encoding="utf-8") as f:
            f.write(content)

        # 复制其他 HAL 文件（跳过已生成的 oc32_hal_conf.h）
        for src_dir in [HAL_SRC_DIR / "inc", HAL_SRC_DIR / "src", HAL_SRC_DIR / "chip"]:
            if src_dir.exists():
                dst_dir = out_path / "hal" / src_dir.name
                dst_dir.mkdir(parents=True, exist_ok=True)
                for f in src_dir.glob("*"):
                    if f.name != "oc32_hal_conf.h":
                        shutil.copy2(f, dst_dir / f.name)

        messagebox.showinfo("Success", f"HAL directory generated!\n\n{out_path / 'hal'}")

    def _browse_outdir(self):
        d = filedialog.askdirectory(initialdir=self.var_outdir.get())
        if d:
            self.var_outdir.set(d)

    def _open_output(self):
        out_path = Path(self.var_outdir.get())
        out_path.mkdir(parents=True, exist_ok=True)
        if sys.platform == "win32":
            os.startfile(str(out_path))
        elif sys.platform == "darwin":
            subprocess.run(["open", str(out_path)])
        else:
            subprocess.run(["xdg-open", str(out_path)])


def main():
    root = Tk()
    App(root)
    root.mainloop()


if __name__ == "__main__":
    main()