"""OC32 HAL 配置生成 — 纯逻辑部分（无 GUI）。

从 hal_conf_generator.py 拆分而来。此文件不依赖任何 GUI 框架，
包含：资源路径解析、芯片配置加载、模板渲染，以及把渲染结果写盘并
复制 HAL 源文件的 generate_config()。

ui.py（customtkinter 界面）依赖本模块。
"""
import json
import os
import sys
import shutil
from pathlib import Path

# --- 资源路径（兼容开发环境与 PyInstaller 打包） ---

def _resolve_root() -> Path:
    """返回包含 chips/ 与 hal/ 的项目根目录。

    - 打包（PyInstaller onefile）：资源解压到 sys._MEIPASS，返回它。
    - 开发：本文件位于 <root>/customtk/ 下，根目录是其上一级。
    """
    if getattr(sys, "frozen", False):
        return Path(sys._MEIPASS)
    return Path(__file__).resolve().parent.parent


ROOT_DIR = _resolve_root()
CHIPS_DIR = ROOT_DIR / "chips"
HAL_SRC_DIR = ROOT_DIR / "hal"  # 源 HAL 目录（包含 chip/, inc/ 和 src/）


# --- 芯片配置加载 ---

def load_chips_config() -> dict:
    config = {}
    for f in CHIPS_DIR.glob("*.json"):
        with open(f, "r", encoding="utf-8") as fh:
            data = json.load(fh)
            config[f.stem] = data
    return config


CHIPS = load_chips_config()

# 时钟源全集（芯片 JSON 未提供 'clk_sources' 时的默认值，也是过滤基准）
ALL_CLOCK_SOURCES = ["IHOSC", "ILOSC", "XHOSC", "XLOSC"]


# --- 模板渲染辅助 ---

def _gen_uart_debug_sub(uart_name):
    if uart_name == "UART0":
        return '#define HAL_UART0_DEBUG\n/* #define HAL_UART1_DEBUG */'
    else:
        return '/* #define HAL_UART0_DEBUG */\n#define HAL_UART1_DEBUG'


# 调试串口选项标签 -> HAL 宏枚举值（见 oc32_hal_uart.h）
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
    """生成调试串口 校验位/全双工/引脚 选项的 #define 行。"""
    lines = []
    parity = user_settings.get("uart_parity", "None")
    duplex = user_settings.get("uart_duplex", "Full")
    pins = user_settings.get("uart_pins", "TX0 & RX0")
    lines.append(f"#define HAL_UART_DEBUG_PARITY   {_UART_PARITY_MACROS[parity]}")
    lines.append(f"#define HAL_UART_DEBUG_DUPLEX   {_UART_DUPLEX_MACROS[duplex]}")
    lines.append(f"#define HAL_UART_DEBUG_PINS     {_UART_PINS_MACROS[pins]}")
    return "\n".join(lines)


def _gen_uart_debug_block(user_settings, features, user_features):
    """渲染整个 UART 调试块；未选中 UART 功能时返回空串（避免留下空 #ifdef 残块）。"""
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


# --- 配置渲染 ---

def render_conf_template(chip_config, user_settings):
    tmpl_path = HAL_SRC_DIR / "oc32_hal_conf_template.h"
    with open(tmpl_path, "r", encoding="utf-8") as f:
        content = f.read()

    ihosc = chip_config.get("ihosc_freq", 24000000)
    ilosc = chip_config.get("ilosc_freq", 4000)
    int_priority = chip_config["int_priority"]
    dma_channels = chip_config["dma_channels"]
    features = chip_config["features"]
    user_features = user_settings.get("features", {})

    name = chip_config.get("name", "OC32")
    content = content.replace("__HAL_CONFIG_NAME__", name)

    # 时钟定义
    xhosc = user_settings.get("xhosc_freq", 4)
    xlosc = user_settings.get("xlosc_freq", 32.768)
    content = content.replace("__HAL_CONFIG_XHOSC_FREQ__", str(int(xhosc) * 1000000))
    content = content.replace("__HAL_CONFIG_XLOSC_FREQ__", str(int(round(float(xlosc) * 1000))))
    content = content.replace("__HAL_CONFIG_IHOSC_FREQ__", str(ihosc))
    content = content.replace("__HAL_CONFIG_ILOSC_FREQ__", str(ilosc))
    # 系统时钟：用户输入的 MHz -> Hz
    sclk_mhz = user_settings.get("sclk_mhz", 99)
    content = content.replace("__HAL_CONFIG_SCLK_FREQ__", str(int(sclk_mhz) * 1000000))

    # USB CDR：芯片定义 cdr_freq 且开关打开时才输出
    cdr_enabled = user_settings.get("cdr_enable", False)
    if chip_config.get("cdr_freq") is not None and cdr_enabled:
        cdr_lines = (
            f"#define HAL_RCM_CDR_ENABLE\n"
            f"#define OC32_CDR_FREQ ((uint32_t){chip_config['cdr_freq']}U)     /* USB CDR output frequency in Hz */"
        )
    else:
        cdr_lines = ""
    content = content.replace("__HAL_CONFIG_CDR_LINES__", cdr_lines)

    # 时钟源
    clk_src = user_settings.get("clk_src", "IHOSC").upper()
    content = content.replace("__HAL_CONFIG_CLK_SRC__", f"OC32_CLK_SRC_{clk_src}")
    # PLL
    pll = user_settings.get("pll", "Disable").upper()
    content = content.replace("__HAL_CONFIG_PLL__", f"OC32_CLK_PLL_{pll}")
    # PLL 频率过监测
    pll_om = user_settings.get("pll_om", "Disable").upper()
    content = content.replace("__HAL_CONFIG_PLL_OM__", f"OC32_CLK_PLL_OM_{pll_om}")

    # ILOSC 真实/伪 标记：仅当 ILOSC 是该芯片支持的时钟源时才输出该宏
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

    # FLASH 定义（十六进制，带注释，匹配参考输出格式）
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

    # 中断优先级枚举 — 生成 PRI0..PRI(n-1)
    int_enum_lines = []
    for i in range(int_priority):
        suffix = f"{i:02d}" if i >= 10 else str(i)
        comma = "," if i < int_priority - 1 else ""
        int_enum_lines.append(f"OC32_HIC_PRI{suffix}       = {i}U{comma}")
    content = content.replace("__HAL_CONFIG_HIC_PRI_ENUM__", "\n".join(int_enum_lines))

    # DMA 通道枚举 — 生成 CH0..CH(n-1)
    dma_ch_lines = []
    for i in range(dma_channels):
        suffix = f"{i:02d}" if i >= 10 else str(i)
        dma_ch_lines.append(f"OC32_DMA_CH{suffix}       = {i}U,")
    content = content.replace("__HAL_CONFIG_DMA_CH__", "\n".join(dma_ch_lines))

    # DMA 通道映射 — 来自芯片 JSON 的 dma_map
    dma_map = chip_config.get("dma_map", [])
    dma_map_lines = []
    for i, periph in enumerate(dma_map):
        dma_map_lines.append(f"OC32_DMA_{periph:<16} = {i}U,")
    content = content.replace("__HAL_CONFIG_DMA_MAP__", "\n".join(dma_map_lines))

    # HIC 通道映射 — 来自芯片 JSON 的 hic_map
    hic_map = chip_config.get("hic_map", [])
    hic_ch_lines = []
    hic_map_lines = []
    for i, periph in enumerate(hic_map):
        hic_ch_lines.append(f"OC32_HIC_CH_{periph:<16} = {i}U,")
        hic_map_lines.append(f"OC32_HIC_{periph:<16} = 1U << {i},")
    content = content.replace("__HAL_CONFIG_HIC_CH__", "\n".join(hic_ch_lines))
    content = content.replace("__HAL_CONFIG_HIC_MAP__", "\n".join(hic_map_lines))

    # UART 调试段 — 输出整个 #define + #ifdef 块，或什么都不输出
    content = content.replace("__HAL_CONFIG_UART_DEBUG_BLOCK__",
        _gen_uart_debug_block(user_settings, features, user_features))

    # 用户模块启用段 — 由 GUI 勾选状态驱动
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


# --- 写盘 + 复制 HAL 源文件 ---

def generate_config(chip_config, user_settings, out_path) -> Path:
    """把渲染结果写入 <out_path>/hal/inc/oc32_hal_conf.h，并原样复制其余
    inc/ src/ chip/ 目录（跳过已生成的 oc32_hal_conf.h）。返回生成目录。"""
    out_path = Path(out_path)
    out_path.mkdir(parents=True, exist_ok=True)

    content = render_conf_template(chip_config, user_settings)
    out_file = out_path / "hal" / "inc" / "oc32_hal_conf.h"
    out_file.parent.mkdir(parents=True, exist_ok=True)
    with open(out_file, "w", encoding="utf-8") as f:
        f.write(content)

    for src_dir in [HAL_SRC_DIR / "inc", HAL_SRC_DIR / "src", HAL_SRC_DIR / "chip"]:
        if src_dir.exists():
            dst_dir = out_path / "hal" / src_dir.name
            dst_dir.mkdir(parents=True, exist_ok=True)
            for f in src_dir.glob("*"):
                if f.name != "oc32_hal_conf.h":
                    shutil.copy2(f, dst_dir / f.name)

    return out_path / "hal"
