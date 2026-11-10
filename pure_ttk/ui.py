"""pure_ttk 实验版：完全去掉 CustomTkinter，纯 tkinter + ttk。

目的：作为"彻底去 Canvas 开销"的完整对照。CTk 的每个控件都是 Canvas 手绘，
窗口拖拽/缩放时会重画所有 Canvas；而纯 tkinter+ttk 全用系统原生控件，
无 Canvas 重绘，理论上最跟手。

与 ../customtk 版共用 gen.py（生成逻辑不变，输出逐字节一致）。
运行：python pure_ttk/ui.py
"""
import os
import sys
import subprocess
from pathlib import Path
from tkinter import Tk, Label, Frame, StringVar, BooleanVar, Canvas, Scrollbar, filedialog, messagebox
from tkinter import ttk

sys.path.insert(0, str(Path(__file__).resolve().parent))
import gen


class App(Tk):
    COLORS = {"panel": "#f8f9fa", "row": "#ffffff",
              "sec_chip": "#e57373", "sec_sys": "#64b5f6",
              "sec_out": "#81c784", "sec_feat": "#ba68c8",
              "sec_uart": "#26c6da", "text": "#1a1a1a",
              "text_off": "#9e9e9e"}

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.title("OC32 HAL Config Generator")
        self.geometry("670x570")
        self.minsize(560, 480)
        style = ttk.Style()
        try:
            style.theme_use("clam")
        except Exception:
            pass
        self._cfg = style.configure

        self.chip_config = None
        self.output_dir = str(Path.home() / "Desktop")
        self.module_vars = {}
        self._feature_checkboxes = []

        self._sys_defaults = {
            "clk_src": "IHOSC", "pll": "Disable", "pll_om": "Disable",
            "cdr": "Disable", "sclk": "99", "xhosc": "4",
            "xlosc": "32.768", "tick_freq": "1KHz",
        }
        self._build_ui()

    # ------------------------------------------------------------ 布局辅助

    def _section(self, parent, title, color):
        """带彩色标题的分区块。返回 (box, body)。"""
        box = Frame(parent, bg=self.COLORS["panel"])
        Label(box, text=title, bg=self.COLORS["panel"],
              fg=color, font=("Segoe UI", 13, "bold"),
              anchor="w").pack(fill="x", padx=6, pady=(4, 2))
        body = Frame(box, bg=self.COLORS["panel"])
        body.pack(fill="both", expand=True)
        body.grid_columnconfigure(0, weight=1)
        return box, body

    def _row(self, parent, label_text, text_color="#1a1a1a"):
        row = Frame(parent, bg=self.COLORS["row"])
        row.pack(fill="x", padx=6, pady=3)
        row.grid_columnconfigure(0, weight=1)
        Label(row, text=label_text, bg=self.COLORS["row"],
              fg=text_color, anchor="w").grid(row=0, column=0, sticky="w", padx=(4, 4))
        return row

    def _make_combo(self, parent, values, default):
        """原生 ttk.Combobox：只可选、不可手输（state=readonly）。"""
        w = ttk.Combobox(parent, values=list(values), state="readonly",
                         width=max(len(str(v)) for v in values) + 1,
                         justify="center")
        w.set(default)
        return w

    # ------------------------------------------------------------ UI 构建

    def _build_ui(self):
        # 左右面板：左=设置区（需要足够宽显示标签+控件），右=功能区
        # 两侧合计需 ~750px；默认 780x600，保证左右都不被截断
        self.minsize(760, 550)
        self.geometry("780x600")

        # 列权重：左 1 / 中 1 / 右 1（等分余量），配合 minsize 保证两侧都不被压
        self.grid_columnconfigure(0, weight=1, minsize=340)
        self.grid_columnconfigure(1, weight=1, minsize=10)
        self.grid_columnconfigure(2, weight=1, minsize=340)
        self.grid_rowconfigure(0, weight=1)

        left = Frame(self, bg=self.COLORS["panel"])
        left.grid(row=0, column=0, sticky="nsew", padx=(10, 2), pady=10)
        left.grid_columnconfigure(0, weight=1)

        right = Frame(self, bg=self.COLORS["panel"])
        right.grid(row=0, column=2, sticky="nsew", padx=(2, 10), pady=10)
        right.grid_columnconfigure(0, weight=1)

        self._build_left(left)
        self._build_right(right)

        default = next(iter(gen.CHIPS), None)
        self._on_pll_changed()
        self._apply_osc_support(gen.CHIPS.get(default))
        self._apply_clk_sources(gen.CHIPS.get(default))
        self._on_chip_changed()

    # ---- 左面板 ----

    def _build_left(self, left):
        chip_box, chip_body = self._section(left, "Model Selection",
                                            self.COLORS["sec_chip"])
        chip_box.pack(fill="x", padx=4, pady=4)
        self.chip_combo = ttk.Combobox(chip_body, values=list(gen.CHIPS.keys()),
                                       state="readonly",
                                       width=max(len(k) for k in gen.CHIPS) + 1,
                                       justify="center")
        self.chip_combo.set(next(iter(gen.CHIPS)))
        self.chip_combo.grid(row=0, column=0, sticky="ew", padx=6, pady=4)
        self.chip_combo.bind("<<ComboboxSelected>>", lambda e: self._on_chip_changed())

        sys_box, sys_body = self._section(left, "System Settings",
                                          self.COLORS["sec_sys"])
        sys_box.pack(fill="x", padx=4, pady=4)

        r = self._row(sys_body, "Clock Source:")
        self.clk_src_combo = self._make_combo(r, gen.ALL_CLOCK_SOURCES,
                                               self._sys_defaults["clk_src"])
        self.clk_src_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))

        r = self._row(sys_body, "PLL:")
        self.pll_combo = self._make_combo(r, ["Enable", "Disable"],
                                          self._sys_defaults["pll"])
        self.pll_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self.pll_combo.bind("<<ComboboxSelected>>", lambda e: self._on_pll_changed())

        r = self._row(sys_body, "PLL Frequency Over Monitor:")
        self.pll_om_combo = self._make_combo(r, ["Enable", "Disable"],
                                             self._sys_defaults["pll_om"])
        self.pll_om_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))

        r = self._row(sys_body, "Wire USB-CDR To PLL Reference:")
        self.cdr_combo = self._make_combo(r, ["Enable", "Disable"],
                                          self._sys_defaults["cdr"])
        self.cdr_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))

        r = self._row(sys_body, "System Clock Frequency (MHz):")
        self.sclk_entry = ttk.Entry(r, width=12)
        self.sclk_entry.insert(0, self._sys_defaults["sclk"])
        self.sclk_entry.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self._restrict_numeric(self.sclk_entry, allow_dot=True)

        r = self._row(sys_body, "XHOSC Frequency (MHz):")
        self.xhosc_entry = ttk.Entry(r, width=12)
        self.xhosc_entry.insert(0, self._sys_defaults["xhosc"])
        self.xhosc_entry.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self._restrict_numeric(self.xhosc_entry, allow_dot=True)

        r = self._row(sys_body, "XLOSC Frequency (KHz):")
        self.xlosc_entry = ttk.Entry(r, width=12)
        self.xlosc_entry.insert(0, self._sys_defaults["xlosc"])
        self.xlosc_entry.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self._restrict_numeric(self.xlosc_entry, allow_dot=True)

        r = self._row(sys_body, "Tick Frequency:")
        self.tick_freq_combo = self._make_combo(r, ["10Hz", "100Hz", "1KHz"],
                                                self._sys_defaults["tick_freq"])
        self.tick_freq_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))

        self._build_left_bottom(left)

    def _build_left_bottom(self, left):
        out_box, out_body = self._section(left, "Output Directory",
                                           self.COLORS["sec_out"])
        out_box.pack(side="bottom", fill="x")
        self.var_outdir = StringVar(value=self.output_dir)
        self.outdir_entry = ttk.Entry(out_body, textvariable=self.var_outdir)
        self.outdir_entry.grid(row=0, column=0, sticky="ew", padx=6, pady=4)
        ttk.Button(out_body, text="Browse...", command=self._browse_outdir
                   ).grid(row=1, column=0, pady=(0, 4))

        btn_bar = Frame(left, bg=self.COLORS["panel"])
        btn_bar.pack(side="bottom", fill="x", padx=4, pady=(2, 6))
        btn_bar.grid_columnconfigure(0, weight=1)
        btn_bar.grid_columnconfigure(1, weight=1)
        btn_bar.grid_columnconfigure(2, weight=1)
        ttk.Button(btn_bar, text="Generate", command=self._generate
                   ).grid(row=0, column=0, sticky="ew", padx=3)
        ttk.Button(btn_bar, text="Reset", command=self._reset_all
                   ).grid(row=0, column=1, sticky="ew", padx=3)
        ttk.Button(btn_bar, text="Open Output", command=self._open_output
                   ).grid(row=0, column=2, sticky="ew", padx=3)

    # ---- 右面板 ----

    def _build_right(self, right):
        uart_box, uart_body = self._section(right, "Debug UART Options",
                                              self.COLORS["sec_uart"])
        uart_box.pack(side="bottom", fill="x")
        self._build_uart_block(uart_body)

        feat_box, feat_body = self._section(right, "Feature Configure",
                                              self.COLORS["sec_feat"])
        feat_box.pack(side="top", fill="both", expand=True)

        # 可滚动的功能模块区：Canvas + 内部 Frame + 右侧滚动条
        self.feature_canvas = Canvas(feat_body, bd=0, highlightthickness=0)
        self.feature_scroll = Frame(self.feature_canvas, bg=self.COLORS["panel"])
        self.feature_canvas_win = self.feature_canvas.create_window(
            (0, 0), window=self.feature_scroll, anchor="nw")
        self.feature_scrollbar = Scrollbar(feat_body, orient="vertical",
                                           command=self.feature_canvas.yview)
        self.feature_canvas.configure(yscrollcommand=self.feature_scrollbar.set)

        # 布局：Canvas 与滚动条并排，Canvas 占满
        self.feature_canvas.grid(row=0, column=0, sticky="nsew")
        self.feature_scrollbar.grid(row=0, column=1, sticky="ns")
        feat_body.grid_rowconfigure(0, weight=1)
        feat_body.grid_columnconfigure(0, weight=1)

        # 内部 Frame 自适应宽度；内容变化时同步滚动区
        self.feature_scroll.bind("<Configure>",
                                 self._on_feature_content_configure)
        self.feature_canvas.bind("<Configure>",
                                 self._on_feature_canvas_configure)
        # 滚轮支持（Linux 需 MouseWheel 事件名不同，这里做通用绑定）
        self._bind_mousewheel(self.feature_canvas)

        self._set_uart_block_state(False)

    def _on_feature_content_configure(self, event=None):
        """内部内容变化时，让 Canvas 的滚动区跟随其实际大小。"""
        self.feature_canvas.configure(scrollregion=self.feature_canvas.bbox("all"))

    def _on_feature_canvas_configure(self, event=None):
        """Canvas 尺寸变化时，同步内部 Frame 宽度（避免左右留白/溢出）。"""
        self.feature_canvas.itemconfigure(self.feature_canvas_win,
                                          width=event.width)

    def _over(self, canvas, event):
        """指针是否落在 canvas 或其任一子孙控件上（命中检测）。

        winfo_containing 返回坐标处的控件；再沿 winfo_parent()（返回的是
        路径名字符串，需 nametowidget 解析）向上走到 canvas 即视为命中。"""
        w = canvas.winfo_containing(event.x_root, event.y_root)
        if w is None:
            return False
        top = canvas.winfo_toplevel()
        # 向上走祖先链，若能到 canvas 本身则命中
        cur = w
        while cur is not None:
            if cur is canvas:
                return True
            pstr = cur.winfo_parent()
            if not pstr:
                break
            try:
                cur = top.nametowidget(pstr)
            except Exception:
                break
        return False

    def _bind_mousewheel(self, canvas):
        """鼠标滚轮滚动：仅当指针在功能模块区上时生效（避免干扰其他区域）。

        Windows 用 <MouseWheel>（delta 为 ±120 的倍数），
        Linux 用 X11 <Button-4>/<Button-5>。用 bind_all + 命中检测限定范围。"""
        def _delta_to_steps(delta):
            # Windows: delta 为 ±120 的整数倍；其他平台兜底 1
            if delta:
                return int(-1 * (delta / 120))
            return -1

        def _win(event):
            if self._over(canvas, event):
                steps = _delta_to_steps(event.delta)
                canvas.yview_scroll(steps, "units")

        def _x11(event):
            if self._over(canvas, event):
                # Button-4=向上滚，Button-5=向下滚
                canvas.yview_scroll(-1 if event.num == 4 else 1, "units")

        try:
            canvas.bind_all("<MouseWheel>", _win)
        except Exception:
            pass
        try:
            canvas.bind_all("<Button-4>", _x11)
            canvas.bind_all("<Button-5>", _x11)
        except Exception:
            pass

    def _build_uart_block(self, uart_body):
        self.uart_block_widgets = []

        r = self._row(uart_body, "Module:")
        self.debug_combo = self._make_combo(r, ["UART0", "UART1"], "UART1")
        self.debug_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self.uart_block_widgets.append((r.winfo_children()[0], self.debug_combo))

        r = self._row(uart_body, "Baud Rate:")
        self.baud_combo = self._make_combo(
            r, ["9600", "14400", "19200", "38400", "57600", "115200",
                "230400", "460800", "921600", "1000000"], "115200")
        self.baud_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self.uart_block_widgets.append((r.winfo_children()[0], self.baud_combo))

        r = self._row(uart_body, "Parity:")
        self.uart_parity_combo = self._make_combo(
            r, ["None", "Odd", "Even", "Always 0", "Always 1"], "None")
        self.uart_parity_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self.uart_block_widgets.append((r.winfo_children()[0], self.uart_parity_combo))

        r = self._row(uart_body, "Duplex:")
        self.uart_duplex_combo = self._make_combo(
            r, ["Full", "Half (use TX pin only)"], "Full")
        self.uart_duplex_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self.uart_block_widgets.append((r.winfo_children()[0], self.uart_duplex_combo))

        r = self._row(uart_body, "Pins:")
        self.uart_pins_combo = self._make_combo(
            r, ["TX0 & RX0", "TX1 & RX1"], "TX0 & RX0")
        self.uart_pins_combo.grid(row=0, column=1, sticky="e", padx=(4, 4))
        self.uart_block_widgets.append((r.winfo_children()[0], self.uart_pins_combo))

    # ------------------------------------------------------------ handlers

    def _set_combo_state(self, combo, enabled):
        combo["state"] = "disabled" if not enabled else "readonly"

    def _set_uart_block_state(self, enabled):
        off_color = self.COLORS["text_off"]
        for lbl, combo in self.uart_block_widgets:
            self._set_combo_state(combo, enabled)
            lbl.configure(fg=off_color if not enabled else self.COLORS["text"])

    def _reset_system_defaults(self):
        d = self._sys_defaults
        for w in (self.pll_om_combo, self.cdr_combo):
            self._set_combo_state(w, True)
        for w in (self.sclk_entry, self.xhosc_entry, self.xlosc_entry):
            w.configure(state="normal")

        self.clk_src_combo["values"] = list(gen.ALL_CLOCK_SOURCES)
        self.clk_src_combo.set(d["clk_src"])
        self.pll_combo.set(d["pll"])
        self.pll_om_combo.set(d["pll_om"])
        self.cdr_combo.set(d["cdr"])
        self.tick_freq_combo.set(d["tick_freq"])

        self.sclk_entry.delete(0, "end"); self.sclk_entry.insert(0, d["sclk"])
        self.xhosc_entry.delete(0, "end"); self.xhosc_entry.insert(0, d["xhosc"])
        self.xlosc_entry.delete(0, "end"); self.xlosc_entry.insert(0, d["xlosc"])

        self.debug_combo.set("UART1")
        self.baud_combo.set("115200")
        self.uart_parity_combo.set("None")
        self.uart_duplex_combo.set("Full")
        self.uart_pins_combo.set("TX0 & RX0")

        self._on_pll_changed()

    def _reset_all(self):
        self._reset_system_defaults()
        # 取消所有可选功能模块勾选（Checkbutton 通过 variable 绑定同步）
        for key, var in self.module_vars.items():
            var.set(False)
        self._apply_clk_sources(self.chip_config)
        self._apply_osc_support(self.chip_config)
        cdr_supported = bool(self.chip_config) and self.chip_config.get("cdr_freq") is not None
        self._set_combo_state(self.cdr_combo, cdr_supported)
        if not cdr_supported:
            self.cdr_combo.set("Disable")
        self._set_uart_block_state(False)
        self._on_pll_changed()

    def _on_pll_changed(self, event=None):
        pll_on = self.pll_combo.get() == "Enable"
        self._set_combo_state(self.pll_om_combo, pll_on)
        if not pll_on:
            self.pll_om_combo.set("Disable")
        self.sclk_entry.configure(state="normal" if pll_on else "disabled")

    def _apply_clk_sources(self, chip_config):
        sources = list(gen.ALL_CLOCK_SOURCES)
        if chip_config and chip_config.get("clk_sources"):
            sources = [s for s in gen.ALL_CLOCK_SOURCES
                       if s in chip_config["clk_sources"]]
        if not sources:
            sources = ["IHOSC"]
        self.clk_src_combo["values"] = sources
        cur = self.clk_src_combo.get()
        if cur not in sources:
            self.clk_src_combo.set(sources[0])

    def _apply_osc_support(self, chip_config):
        if chip_config and chip_config.get("clk_sources"):
            sources = set(chip_config["clk_sources"])
        else:
            sources = set(gen.ALL_CLOCK_SOURCES)
        self.xhosc_entry.configure(state="normal" if "XHOSC" in sources else "disabled")
        self.xlosc_entry.configure(state="normal" if "XLOSC" in sources else "disabled")

    def _on_chip_changed(self, event=None):
        key = self.chip_combo.get()
        new_config = gen.CHIPS.get(key)
        self.chip_config = new_config
        self._reset_system_defaults()

        cdr_supported = bool(new_config) and new_config.get("cdr_freq") is not None
        self._set_combo_state(self.cdr_combo, cdr_supported)
        if not cdr_supported:
            self.cdr_combo.set("Disable")

        self._apply_osc_support(new_config)
        self._apply_clk_sources(new_config)

        for cb in self._feature_checkboxes:
            cb.destroy()
        self._feature_checkboxes = []
        self.module_vars.clear()

        if self.chip_config:
            optional = [m for m in self.chip_config["features"]
                        if not m.get("always", False)]
            for idx, mod in enumerate(optional):
                var = BooleanVar(value=False)
                label = f"{mod['key']}:  {mod.get('description', '')}"
                if mod["key"] == "DEBUG":
                    cb = ttk.Checkbutton(self.feature_scroll, text=label,
                                         variable=var, command=self._toggle_uart_block)
                else:
                    cb = ttk.Checkbutton(self.feature_scroll, text=label, variable=var)
                cb.pack(fill="x", pady=2, anchor="e")
                self._feature_checkboxes.append(cb)
                self.module_vars[f"mod_{idx}"] = var
            self._set_uart_block_state(False)

    def _toggle_uart_block(self):
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

    def _restrict_numeric(self, entry, allow_dot=False):
        """限制输入框只能输入数字（可选允许一个小数点），键入非法字符直接拦截。

        放行编辑类按键（退格/方向键/删除等），只拦"输入非数字字符"的动作。"""
        allowed = "0123456789" + ("." if allow_dot else "")

        def _on_key(event):
            ch = event.char
            key = event.keysym
            # 编辑类按键放行
            if key in ("BackSpace", "Delete", "Left", "Right",
                      "Up", "Down", "Home", "End", "Tab"):
                return "break" if key == "Tab" else None
            # 合法数字/小数点
            if ch and ch in allowed:
                if allow_dot and ch == "." and entry.get().count(".") >= 1:
                    return "break"          # 只允许一个小数点
                return None
            # 其它字符（字母、符号、粘贴非数字）拦截
            return "break"

        entry.bind("<Key>", _on_key)

    # ------------------------------------------------------------ settings

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

    def _validate_numeric_inputs(self):
        """校验三个数字输入框（System Clock / XHOSC / XLOSC）。

        合法返回 None；非法（非数字、非正数、空）返回错误描述。"""
        checks = [
            (self.sclk_entry,  "System Clock Frequency (MHz)", float),
            (self.xhosc_entry, "XHOSC Frequency (MHz)", float),
            (self.xlosc_entry, "XLOSC Frequency (kHz)", float),
        ]
        for entry, name, cast in checks:
            raw = entry.get().strip()
            if raw == "":
                return f"Empty value for {name} — enter a positive number."
            try:
                val = cast(raw)
            except (TypeError, ValueError):
                return f"Invalid value '{raw}' for {name} — must be a number."
            if val <= 0:
                return f"Value for {name} must be positive (got {raw})."
        return None

    def _collect_settings(self):
        base = {
            "xhosc_freq": self._safe_float(self.xhosc_entry, 4),
            "xlosc_freq": self._safe_float(self.xlosc_entry, 32.768),
            "clk_src": self.clk_src_combo.get(),
            "pll": self.pll_combo.get(),
            "pll_om": self.pll_om_combo.get(),
            "cdr_enable": self.cdr_combo.get() == "Enable",
            "sclk_mhz": self._safe_float(self.sclk_entry, 99),
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
                if val:
                    for dep in mod.get("dependencies", []):
                        features[dep] = True
        base["features"] = features
        return base

    # ------------------------------------------------------------ actions

    def _generate(self):
        if not self.chip_config:
            messagebox.showerror("Error", "Please select a chip first")
            return
        # 数字输入校验：非法则报错中止，不再静默回退默认值
        err = self._validate_numeric_inputs()
        if err:
            messagebox.showerror("Invalid Input", err)
            return
        settings = self._collect_settings()
        # 系统时钟超限提示：用户输入超过芯片 sclk_max 时，按上限处理并告知
        applied_hz, clamped, sclk_max = gen.sclk_clamp_info(self.chip_config, settings)
        if clamped:
            messagebox.showwarning(
                "System Clock Clamped",
                f"System Clock input {settings.get('sclk_mhz')} MHz exceeds the chip "
                f"maximum ({sclk_max // 1000000} MHz).\n\n"
                f"It will be clamped to {sclk_max} Hz in the generated config.",
            )
            # 自动把输入框修正为芯片上限，避免下次生成仍用超限值
            self.sclk_entry.delete(0, "end")
            self.sclk_entry.insert(0, str(sclk_max // 1000000))
            settings["sclk_mhz"] = sclk_max // 1000000
        try:
            out = gen.generate_config(self.chip_config, settings, self.var_outdir.get())
        except Exception as exc:
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
