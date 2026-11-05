# OC32 HAL Config Generator — CustomTkinter 版本

OC32 HAL Config Generator 的 CustomTkinter 版本，拆分为两个文件：

| 文件 | 职责 |
|---|---|
| `gen.py` | **生成逻辑**（无 GUI 依赖）：资源路径解析、芯片配置加载（`CHIPS`）、模板渲染（`render_conf_template`）、写盘 + 复制 HAL 源文件（`generate_config`）。可独立测试、可被 GUI 之外复用。 |
| `ui.py` | **GUI**（CustomTkinter）：窗口、全部控件、事件处理（芯片切换、PLL/时钟源门控、DEBUG 驱动调试块、生成/打开目录）。仅依赖 `gen.py`。 |

## 运行

```bash
pip install customtkinter
python customtk/ui.py
```

> 开发模式下，`gen.py` 通过 `Path(__file__).resolve().parent.parent` 定位仓库根目录，
> 从而找到 `chips/` 与 `hal/`。打包（PyInstaller onefile）时改读 `sys._MEIPASS`，
> 把 `chips`/`hal` 目录通过 `--add-data` 一并打包即可。

## 与原 Tkinter 版的对应关系

- 生成结果与原 Tkinter 单文件版**逐字节一致**（已验证：相同芯片/参数下渲染输出与
  生成的 `hal/` 目录树完全相同）。
- 界面改用 CustomTkinter 浅色主题，布局为「左：芯片 + 系统设置 + 输出目录/按钮；
  右：功能模块复选框（DEBUG 复选框驱动底部的调试串口选项块，未勾选时禁用并置灰）」。
- 行为保持一致：时钟源按芯片 `clk_sources` 过滤、XHOSC/XLOSC 门控、USB CDR 仅当芯片
  定义 `cdr_freq` 时可用、DEBUG 勾选才启用调试串口选项、生成时自动补全依赖模块。

## 依赖

- Python 3.8+
- `customtkinter`（`pip install customtkinter`）

## 第三方许可（商用发布）

本 GUI 依赖以下第三方库，均为宽松的商用许可（MIT / BSD）：

- **CustomTkinter** — MIT（CC0）
- **darkdetect**（CustomTkinter 运行期依赖）— BSD
- **Tk/Tcl-Tk**（tkinter 底层）— Tcl/Tk BSD 风格
- **PyInstaller** — GPL v2 + Bootloader 例外（仅构建期工具，Bootloader 例外允许商用打包，不强制公开本项目源码）

完整许可文本与归属声明见仓库根目录的 **`THIRD-PARTY-LICENSES.txt`**；
商用发布时请随附该文件，并在产品「关于/许可」界面中保留相应归属说明。
