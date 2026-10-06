"""
OC32 HAL 配置文件生成工具 (standalone exe)

支持多芯片型号，生成完整的 hal 目录（含 oc32_hal_conf.h 及其他 HAL 源文件）
"""
import json
import os
import sys
import shutil
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
HAL_SRC_DIR = ROOT_DIR / "hal"  # 源 HAL 目录（包含 inc/ 和 src/）

# Load chips config
def load_chips_config():
    config = {}
    for f in CHIPS_DIR.glob("*.json"):
        with open(f, "r", encoding="utf-8") as fh:
            data = json.load(fh)
            config[f.stem] = data
    return config

CHIPS = load_chips_config()


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


# --- Config rendering ---
def render_conf_template(chip_config, user_settings):
    tmpl_path = HAL_SRC_DIR / "oc32_hal_conf_template.h"
    with open(tmpl_path, "r", encoding="utf-8") as f:
        content = f.read()

    ihosc = chip_config.get("ihosc_freq", 24000000)
    ilosc = chip_config.get("ilosc_freq", 4000)
    cdr = chip_config.get("cdr_freq", 12000000)
    sclk = chip_config.get("sclk_freq", 99000000)
    tick = user_settings["tick_priority"]
    debug_uart = user_settings["debug_uart"]
    int_priority = chip_config["int_priority"]
    dma_channels = chip_config["dma_channels"]
    features = chip_config["features"]
    user_features = user_settings.get("features", {})

    name = chip_config.get("name", "OC32")
    content = content.replace("__HAL_CONFIG_NAME__", name)

    # Clock definitions
    xhosc = user_settings["xhosc_freq"]
    content = content.replace("__HAL_CONFIG_XHOSC_FREQ__", str(xhosc * 1000000))
    content = content.replace("__HAL_CONFIG_IHOSC_FREQ__", str(ihosc))
    content = content.replace("__HAL_CONFIG_ILOSC_FREQ__", str(ilosc))
    content = content.replace("__HAL_CONFIG_CDR_FREQ__", str(cdr))
    content = content.replace("__HAL_CONFIG_SCLK_FREQ__", str(sclk))

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
    content = content.replace("__HAL_CONFIG_TICK_PRIORITY__", f'{tick}U')
    content = content.replace("__HAL_CONFIG_TICK_FREQ__", f'HAL_TICK_FREQ_{user_settings["tick_freq"]}')
    content = content.replace("__HAL_CONFIG_UART_DEBUG_SUB__", _gen_uart_debug_sub(debug_uart))

    # Int enum — generate PRI0..PRI(n-1) entries
    int_enum_lines = []
    for i in range(int_priority):
        suffix = f"{i:02d}" if i >= 10 else str(i)
        comma = "," if i < int_priority - 1 else ""
        int_enum_lines.append(f"OC32_INT_PRI{suffix}       = {i}U{comma}")
    content = content.replace("__HAL_CONFIG_INT_PRI_ENUM__", "\n".join(int_enum_lines))    

    # DMA enum — generate CH0..CH(n-1) entries
    dma_enum_lines = []
    for i in range(dma_channels):
        suffix = f"{i:02d}" if i >= 10 else str(i)
        comma = "," if i < dma_channels - 1 else ""
        dma_enum_lines.append(f"OC32_DMA_CH{suffix}       = {i}U{comma}")
    content = content.replace("__HAL_CONFIG_DMA_CH_ENUM__", "\n".join(dma_enum_lines))

    # DMA channel map — build from dma_map{N} fields in chip config (0..15)
    max_channels = 16
    dma_map = {}  # channel_index -> peripheral_name
    for i in range(max_channels):
        key = f"dma_map{i}"
        if key in chip_config:
            dma_map[i] = chip_config[key]
    dma_map_lines = []
    for i in range(max_channels):
        periph = dma_map.get(i, f"reserved{i-10 if i >= 10 else i}")
        # normalize reserved name
        if periph.startswith("reserved"):
            label = periph
        else:
            label = periph
        dma_map_lines.append(f"#define OC32_DMA_{label:<16}((uint32_t){i}U)")
    content = content.replace("__HAL_CONFIG_DMA_CH_MAP__", "\n".join(dma_map_lines))

    # UART debug section
    uart_feature = next((m for m in features if m["key"] == "UART"), None)
    if (uart_feature and user_features.get("UART", False)):
        content = content.replace("__HAL_CONFIG_UART_DEBUG__", "#define HAL_UART_DEBUG_ENABLE\n")
        content = content.replace("__HAL_CONFIG_UART_DEBUG_BR__",
            f"#define HAL_UART_DEBUG_BR   {user_settings['baud_rate']} /* 9600/14400 .... ~ 1000000 */\n")
    else:
        content = content.replace("__HAL_CONFIG_UART_DEBUG__", "\n")
        content = content.replace("__HAL_CONFIG_UART_DEBUG_BR__", "\n")

    # User modules enable section — driven by GUI checkbox state from user_settings
    module_lines = []
    comment_lines = []
    for mod in features:
        key = mod["key"]
        always = mod.get("always", False)
        if always:
            module_lines.append(f"#define HAL_{key}_ENABLE")
        else:
            deps = mod.get("dependencies", [])
            macro_key = mod.get("macro_name")
            macro_name = f"HAL_{macro_key}_ENABLE" if macro_key else f"HAL_{key}_ENABLE"
            checked = user_features.get(key, False)
            if deps and checked:
                module_lines.append(f"#define {macro_name}")
            elif checked:
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
        self.root.geometry("650x500")
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
        main.add(left, width=320)

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

        # Middle: Clock settings
        sys_frame = _colored_label_frame(left, "System Settings", "blue")
        sys_frame.pack(fill=X, padx=2, pady=2)

        # XHOSC selection
        xhosc_frame = Frame(sys_frame)
        xhosc_frame.pack(fill=X, pady=2)
        Label(xhosc_frame, text="XHOSC Frequency (MHz):").pack(side=LEFT, anchor=W)
        self.var_xhosc = IntVar(value=4)
        self.xhosc_combo = ttk.Combobox(xhosc_frame, textvariable=self.var_xhosc,
                                       values=list(range(1, 25)), state="readonly", width=8)
        self.xhosc_combo.pack(side=RIGHT, padx=(4, 0))

        # Tick priority
        tick_int_frame = Frame(sys_frame)
        tick_int_frame.pack(fill=X, pady=2)
        Label(tick_int_frame, text="Tick Interrupt Priority (0: Lowest):").pack(side=LEFT, anchor=W)
        self.var_tick = IntVar(value=0)
        self.tick_combo = ttk.Combobox(tick_int_frame, textvariable=self.var_tick,
                                       values=["0", "1", "2", "3"], state="readonly", width=8)
        self.tick_combo.pack(side=RIGHT, padx=(4, 0))

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

        # Bottom: Output dir
        out_frame = _colored_label_frame(left, "Output Directory", "green")
        out_frame.pack(fill=X, padx=2, pady=2, expand=True)
        self.var_outdir = StringVar(value=self.output_dir)
        Entry(out_frame, textvariable=self.var_outdir).pack(fill=X, pady=2)
        Button(out_frame, text="Browse...", command=self._browse_outdir).pack(pady=4)

        # Right panel: Module selection + Preview
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
        self.mod_canvas.create_window((0, 0), window=self.mod_scroll_frame, anchor="nw")
        self.mod_canvas.configure(yscrollcommand=mod_scrollbar.set)

        self.mod_canvas.pack(side=LEFT, fill=BOTH, expand=True)
        mod_scrollbar.pack(side=RIGHT, fill=Y)

        # Buttons
        btn_frame = Frame(left)
        btn_frame.pack(fill=X, padx=2, pady=10)
        Button(btn_frame, text="Generate Config", command=self._generate,
               width=20).pack(pady=4)
        Button(btn_frame, text="Open Output Dir", command=self._open_output,
               width=20).pack(pady=4)

        # Init preview
        self._on_chip_changed(None)

    def _on_chip_changed(self, event):
        key = self.var_chip.get()
        new_config = CHIPS.get(key)

        self.chip_config = new_config
        self._last_chip_key = key

        # Clear old module checkboxes
        for child in self.mod_scroll_frame.winfo_children():
            child.destroy()
        self.module_vars.clear()

        # Create new module checkboxes
        if self.chip_config:
            optional_features = [m for m in self.chip_config["features"] if not m.get("always", False)]
            for idx, mod in enumerate(optional_features):
                frame = Frame(self.mod_scroll_frame)
                frame.pack(fill=X, pady=2)
                Label(frame, text=f"{mod['key']}:  {mod.get('description', '')}").pack(side=LEFT)
                var = IntVar(value=0)
                Checkbutton(frame, variable=var).pack(side=RIGHT)
                self.module_vars[f"mod_{idx}"] = var

        self._update_preview()

    def _update_preview(self):
        pass

    def _collect_settings(self):
        features = {}
        if not self.chip_config:
            return {
                "xhosc_freq": self.var_xhosc.get(),
                "tick_priority": self.var_tick.get(),
                "tick_freq": self.var_tick_freq.get(),
                "debug_uart": self.var_debug.get(),
                "features": features,
            }
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
            "xhosc_freq": self.var_xhosc.get(),
            "tick_priority": self.var_tick.get(),
            "tick_freq": self.var_tick_freq.get(),
            "debug_uart": self.var_debug.get(),
            "baud_rate": self.var_baud.get(),
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
        for src_dir in [HAL_SRC_DIR / "inc", HAL_SRC_DIR / "src"]:
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
        os.startfile(str(out_path))


def main():
    root = Tk()
    App(root)
    root.mainloop()


if __name__ == "__main__":
    main()
