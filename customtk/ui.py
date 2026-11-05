"""OC32 HAL 配置生成 — customtkinter 界面部分。

从 hal_conf_generator.py 拆分而来：本文件只负责 GUI（窗口、控件、事件处理），
所有生成/渲染逻辑放在同目录的 gen.py。

运行：python customtk/ui.py      （开发）
打包：可用 PyInstaller，把 customtk/ 一并加入 --add-data，入口指向 ui.py。
依赖：pip install customtkinter
"""
import os
import sys
import subprocess
from pathlib import Path
from tkinter import filedialog

import customtkinter as ctk
from tkinter import messagebox

import gen


class App(ctk.CTk):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.title("OC32 HAL Config Generator")
        self.geometry("670x570")
        self.minsize(560, 480)

        ctk.set_appearance_mode("light")
        ctk.set_default_color_theme("blue")

        self.chip_config = None
        self.output_dir = str(Path.home() / "Desktop")
        self.module_vars = {}          # "mod_{idx}" -> ctk.BooleanVar
        self._feature_checkboxes = []

        self._build_ui()

    # ------------------------------------------------------------------ 辅助

    def _section(self, parent, title, color):
        """带彩色标题的分区块（CustomTkinter 没有 LabelFrame，用 Frame 模拟）。"""
        box = ctk.CTkFrame(parent, fg_color="transparent")
        box.pack(fill="x", padx=4, pady=4)
        ctk.CTkLabel(box, text=title,
                      font=("Segoe UI", 13, "bold"), text_color=color,
                      anchor="w").pack(fill="x", padx=6, pady=(4, 2))
        body = ctk.CTkFrame(box)
        body.pack(fill="both", expand=True)
        body.grid_columnconfigure(0, weight=1)
        return body

    @staticmethod
    def _row(frame, label_text):
        """一行：左标签 + 右控件（控件由调用方 grid 到 column=1）。"""
        row = ctk.CTkFrame(frame, fg_color="transparent")
        row.pack(fill="x", padx=6, pady=3)
        row.grid_columnconfigure(0, weight=1)
        ctk.CTkLabel(row, text=label_text, anchor="w").grid(
            row=0, column=0, sticky="w", padx=(4, 4))
        return row

    # ------------------------------------------------------------------ UI

    def _build_ui(self):
        # 根布局：左（设置） + 中（占位） + 右（功能/调试选项）
        self.grid_columnconfigure(0, weight=40)
        self.grid_columnconfigure(1, weight=1)
        self.grid_columnconfigure(2, weight=55)
        self.grid_rowconfigure(0, weight=1)

        left = ctk.CTkFrame(self)
        left.grid(row=0, column=0, sticky="nsew", padx=(10, 2), pady=10)
        left.grid_columnconfigure(0, weight=1)

        right = ctk.CTkFrame(self)
        right.grid(row=0, column=2, sticky="nsew", padx=(2, 10), pady=10)
        right.grid_columnconfigure(0, weight=1)

        self._build_left(left)
        self._build_right(right)

        # 初始化（顺序很重要：先设默认芯片的门控，再触发芯片变更）
        default = next(iter(gen.CHIPS), None)
        self._on_pll_changed()
        self._apply_osc_support(gen.CHIPS.get(default))
        self._apply_clk_sources(gen.CHIPS.get(default))
        self._on_chip_changed()

    # ---- 左面板：芯片选择 + 系统设置 + 输出目录 + 按钮 ----

    def _build_left(self, left):
        # 芯片选择
        chip_body = self._section(left, "Chip Selection", "#e57373")
        self.var_chip = ctk.StringVar(value=next(iter(gen.CHIPS)))
        self.chip_combo = ctk.CTkComboBox(
            chip_body, values=list(gen.CHIPS.keys()), state="readonly",
            command=self._on_chip_changed)
        self.chip_combo.set(self.var_chip.get())
        self.chip_combo.grid(row=0, column=0, sticky="ew", padx=6, pady=4)

        # 系统设置
        sys_body = self._section(left, "System Settings", "#64b5f6")

        self.var_clk_src = ctk.StringVar(value="IHOSC")
        r = self._row(sys_body, "Clock Source:")
        self.clk_src_combo = ctk.CTkComboBox(r, values=list(gen.ALL_CLOCK_SOURCES),
                                              state="readonly")
        self.clk_src_combo.set(self.var_clk_src.get())
        self.clk_src_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))

        self.var_pll = ctk.StringVar(value="Disable")
        r = self._row(sys_body, "PLL:")
        self.pll_combo = ctk.CTkComboBox(r, values=["Enable", "Disable"],
                                         state="readonly",
                                         command=lambda _: self._on_pll_changed())
        self.pll_combo.set(self.var_pll.get())
        self.pll_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))

        self.var_pll_om = ctk.StringVar(value="Disable")
        r = self._row(sys_body, "PLL Freq Over Monitor:")
        self.pll_om_combo = ctk.CTkComboBox(r, values=["Enable", "Disable"],
                                             state="readonly")
        self.pll_om_combo.set(self.var_pll_om.get())
        self.pll_om_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))

        self.var_cdr = ctk.StringVar(value="Disable")
        r = self._row(sys_body, "PLL Reference Selects USB-CDR:")
        self.cdr_combo = ctk.CTkComboBox(r, values=["Enable", "Disable"],
                                         state="readonly")
        self.cdr_combo.set(self.var_cdr.get())
        self.cdr_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))

        self.var_sclk = ctk.StringVar(value="99")
        r = self._row(sys_body, "System Clock (MHz):")
        self.sclk_entry = ctk.CTkEntry(r, width=90)
        self.sclk_entry.insert(0, self.var_sclk.get())
        self.sclk_entry.grid(row=0, column=1, sticky="e", padx=(4, 4))

        self.var_xhosc = ctk.StringVar(value="4")
        r = self._row(sys_body, "XHOSC Frequency (MHz):")
        self.xhosc_entry = ctk.CTkEntry(r, width=90)
        self.xhosc_entry.insert(0, self.var_xhosc.get())
        self.xhosc_entry.grid(row=0, column=1, sticky="e", padx=(4, 4))

        self.var_xlosc = ctk.StringVar(value="32.768")
        r = self._row(sys_body, "XLOSC Frequency (kHz):")
        self.xlosc_entry = ctk.CTkEntry(r, width=110)
        self.xlosc_entry.insert(0, self.var_xlosc.get())
        self.xlosc_entry.grid(row=0, column=1, sticky="e", padx=(4, 4))

        self.var_tick_freq = ctk.StringVar(value="1KHz")
        r = self._row(sys_body, "Tick Frequency:")
        self.tick_freq_combo = ctk.CTkComboBox(r, values=["10Hz", "100Hz", "1KHz"],
                                               state="readonly")
        self.tick_freq_combo.set(self.var_tick_freq.get())
        self.tick_freq_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))

        # 输出目录 + 按钮 —— 固定（side=bottom）在左面板最底部
        self._build_left_bottom(left)

    def _build_left_bottom(self, left):
        out_body = self._section(left, "Output Directory", "#81c784")
        out_body.pack(side="bottom", fill="x")
        self.var_outdir = ctk.StringVar(value=self.output_dir)
        self.outdir_entry = ctk.CTkEntry(out_body, textvariable=self.var_outdir)
        self.outdir_entry.grid(row=0, column=0, sticky="ew", padx=6, pady=4)
        ctk.CTkButton(out_body, text="Browse...", width=100,
                      command=self._browse_outdir).grid(row=1, column=0, pady=(0, 4))

        btn_bar = ctk.CTkFrame(left, fg_color="transparent")
        btn_bar.pack(side="bottom", fill="x", padx=4, pady=(2, 6))
        ctk.CTkButton(btn_bar, text="Generate Config",
                      command=self._generate, width=150).pack(side="left", padx=4)
        ctk.CTkButton(btn_bar, text="Open Output Dir",
                      command=self._open_output, width=150).pack(side="left", padx=4)

    # ---- 右面板：功能模块 + 调试串口选项 ----

    def _build_right(self, right):
        # 功能模块（可滚动，DEBUG 复选框驱动底部调试块）
        feat_body = self._section(right, "Feature Configure", "#ba68c8")
        feat_body.pack(side="top", fill="both", expand=True)

        self.feature_scroll = ctk.CTkScrollableFrame(feat_body, fg_color="transparent")
        self.feature_scroll.grid(row=0, column=0, sticky="nsew")
        feat_body.grid_rowconfigure(0, weight=1)

        # 调试串口选项块 —— 固定在右面板底部，DEBUG 未勾选时禁用/置灰
        uart_body = self._section(right, "Debug UART Options", "#26c6da")
        uart_body.pack(side="bottom", fill="x")
        self.uart_block_widgets = []  # [(label, combo)]

        self.var_debug = ctk.StringVar(value="UART1")
        r = self._row(uart_body, "Module:")
        self.debug_combo = ctk.CTkComboBox(r, values=["UART0", "UART1"],
                                           state="readonly")
        self.debug_combo.set(self.var_debug.get())
        self.debug_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self.uart_block_widgets.append((r.winfo_children()[0], self.debug_combo))

        self.var_baud = ctk.StringVar(value="115200")
        r = self._row(uart_body, "Baud Rate:")
        self.baud_combo = ctk.CTkComboBox(
            r, values=["9600", "14400", "19200", "38400", "57600", "115200",
                       "230400", "460800", "921600", "1000000"], state="readonly")
        self.baud_combo.set(self.var_baud.get())
        self.baud_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self.uart_block_widgets.append((r.winfo_children()[0], self.baud_combo))

        self.var_uart_parity = ctk.StringVar(value="None")
        r = self._row(uart_body, "Parity:")
        self.uart_parity_combo = ctk.CTkComboBox(
            r, values=["None", "Odd", "Even", "Always 0", "Always 1"],
            state="readonly")
        self.uart_parity_combo.set(self.var_uart_parity.get())
        self.uart_parity_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self.uart_block_widgets.append((r.winfo_children()[0], self.uart_parity_combo))

        self.var_uart_duplex = ctk.StringVar(value="Full")
        r = self._row(uart_body, "Duplex:")
        self.uart_duplex_combo = ctk.CTkComboBox(
            r, values=["Full", "Half (use TX pin only)"], state="readonly")
        self.uart_duplex_combo.set(self.var_uart_duplex.get())
        self.uart_duplex_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self.uart_block_widgets.append((r.winfo_children()[0], self.uart_duplex_combo))

        self.var_uart_pins = ctk.StringVar(value="TX0 & RX0")
        r = self._row(uart_body, "Pins:")
        self.uart_pins_combo = ctk.CTkComboBox(r, values=["TX0 & RX0", "TX1 & RX1"],
                                               state="readonly")
        self.uart_pins_combo.set(self.var_uart_pins.get())
        self.uart_pins_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self.uart_block_widgets.append((r.winfo_children()[0], self.uart_pins_combo))

        # 初始置灰，直到 DEBUG 功能被勾选
        self._set_uart_block_state(False)

    # ---------------------------------------------------------------- handlers

    def _on_pll_changed(self, event=None):
        """PLL 关闭时，禁用并置灰 PLL 过监测与系统时钟控件。"""
        pll_on = self.var_pll.get() == "Enable"
        state = "normal" if pll_on else "disabled"
        self.pll_om_combo.configure(state=state)
        if not pll_on:
            self.var_pll_om.set("Disable")
            self.pll_om_combo.set("Disable")
        self.sclk_entry.configure(state=state)

    def _apply_clk_sources(self, chip_config):
        """按芯片 clk_sources 过滤时钟源下拉项。"""
        sources = list(gen.ALL_CLOCK_SOURCES)
        if chip_config and chip_config.get("clk_sources"):
            sources = [s for s in gen.ALL_CLOCK_SOURCES
                       if s in chip_config["clk_sources"]]
        self.clk_src_combo.configure(values=sources)
        cur = self.clk_src_combo.get()
        if cur not in sources:
            self.clk_src_combo.set(sources[0] if sources else "IHOSC")

    def _apply_osc_support(self, chip_config):
        """按芯片 clk_sources 启用/禁用 XHOSC、XLOSC 输入框。"""
        if chip_config and chip_config.get("clk_sources"):
            sources = set(chip_config["clk_sources"])
        else:
            sources = set(gen.ALL_CLOCK_SOURCES)
        xhosc_on = "XHOSC" in sources
        xlosc_on = "XLOSC" in sources
        self.xhosc_entry.configure(state="normal" if xhosc_on else "disabled")
        self.xlosc_entry.configure(state="normal" if xlosc_on else "disabled")

    def _on_chip_changed(self, event=None):
        key = self.chip_combo.get()
        new_config = gen.CHIPS.get(key)
        self.chip_config = new_config

        # CDR 仅当芯片定义 cdr_freq 时可用
        cdr_supported = bool(new_config) and new_config.get("cdr_freq") is not None
        self.cdr_combo.configure(state="normal" if cdr_supported else "disabled")
        if not cdr_supported:
            self.var_cdr.set("Disable")
            self.cdr_combo.set("Disable")

        self._apply_osc_support(new_config)
        self._apply_clk_sources(new_config)

        # 重建功能模块复选框
        for cb in self._feature_checkboxes:
            cb.destroy()
        self._feature_checkboxes = []
        self.module_vars.clear()

        if self.chip_config:
            optional = [m for m in self.chip_config["features"]
                        if not m.get("always", False)]
            for idx, mod in enumerate(optional):
                var = ctk.BooleanVar(value=False)
                label = f"{mod['key']}:  {mod.get('description', '')}"
                if mod["key"] == "DEBUG":
                    cb = ctk.CTkCheckBox(self.feature_scroll, text=label,
                                         variable=var, command=self._toggle_uart_block)
                else:
                    cb = ctk.CTkCheckBox(self.feature_scroll, text=label, variable=var)
                cb.pack(fill="x", pady=2, anchor="e")
                self._feature_checkboxes.append(cb)
                self.module_vars[f"mod_{idx}"] = var
            # 切换芯片时把调试块重置为置灰
            self._set_uart_block_state(False)

    def _toggle_uart_block(self):
        """DEBUG 功能复选框命令 —— 把其勾选状态镜像到调试串口选项块。"""
        key = self.chip_combo.get()
        cfg = gen.CHIPS.get(key)
        enabled = False
        if cfg:
            optional = [m for m in cfg.get("features", [])
                        if not m.get("always", False)]
            for i, mod in enumerate(optional):
                if mod["key"] == "DEBUG":
                    enabled = bool(self.module_vars.get(f"mod_{i}").get())
                    break
        self._set_uart_block_state(enabled)

    def _set_uart_block_state(self, enabled):
        """启用或置灰调试串口选项块。"""
        gray = "gray40" if not enabled else "gray20"
        for lbl, combo in self.uart_block_widgets:
            combo.configure(state=("normal" if enabled else "disabled"),
                           text_color=gray)
            lbl.configure(text_color=gray)

    # ---------------------------------------------------------------- settings

    @staticmethod
    def _safe_int(widget, default):
        try:
            return int(widget.get())
        except (TypeError, ValueError):
            return default

    @staticmethod
    def _safe_float(widget, default):
        try:
            return float(widget.get())
        except (TypeError, ValueError):
            return default

    def _collect_settings(self):
        base = {
            "xhosc_freq": self._safe_int(self.xhosc_entry, 4),
            "xlosc_freq": self._safe_float(self.xlosc_entry, 32.768),
            "clk_src": self.clk_src_combo.get(),
            "pll": self.pll_combo.get(),
            "pll_om": self.pll_om_combo.get(),
            "cdr_enable": self.cdr_combo.get() == "Enable",
            "sclk_mhz": self._safe_int(self.sclk_entry, 99),
            "tick_freq": self.tick_freq_combo.get(),
            "debug_uart": self.debug_combo.get(),
            "baud_rate": self.baud_combo.get(),
            "uart_parity": self.uart_parity_combo.get(),
            "uart_duplex": self.uart_duplex_combo.get(),
            "uart_pins": self.uart_pins_combo.get(),
        }

        features = {}
        if not self.chip_config:
            base["features"] = features
            return base
        optional = [m for m in self.chip_config["features"]
                    if not m.get("always", False)]
        for idx, mod in enumerate(optional):
            var = self.module_vars.get(f"mod_{idx}")
            if var is not None:
                val = bool(var.get())
                features[mod["key"]] = val
                if val:  # 自动勾选依赖
                    for dep in mod.get("dependencies", []):
                        features[dep] = True
        base["features"] = features
        return base

    # ---------------------------------------------------------------- actions

    def _generate(self):
        if not self.chip_config:
            messagebox.showerror("Error", "Please select a chip first")
            return
        settings = self._collect_settings()
        try:
            out = gen.generate_config(self.chip_config, settings, self.var_outdir.get())
        except Exception as exc:  # noqa: BLE001
            messagebox.showerror("Error", f"生成失败：{exc}")
            return
        messagebox.showinfo("Success", f"HAL 目录已生成！\n\n{out}")

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
    app = App()
    app.mainloop()


if __name__ == "__main__":
    main()
