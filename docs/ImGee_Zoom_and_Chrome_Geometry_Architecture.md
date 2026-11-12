# ImGee Zoom & Chrome Geometry Architecture
*Mathematical Mechanics, Pre- vs. Post-Scaled Coordinates, and Turnkey Scaling Contracts for X-Plane ImGui Plugins*

---

## 1. Executive Summary & Design Principles

Designing resizable, zoomable cockpit interfaces in X-Plane via Dear ImGui and `ImgWindow` requires reconciling two distinct coordinate systems:
1. **The Pre-Scaled Design Space ($1.0\times$ Baseline):** The theoretical canvas dimensions and widget geometries defined at standard $100\%$ scale ($F_A(x) = x$).
2. **The Post-Scaled Physical Space:** The real screen pixels (boxels) rendered on the monitor after font scaling, OS HiDPI scaling, and window geometry transforms are applied.

When composite windows combine **fixed, invariant operational chrome** (e.g., zoom sliders, reset buttons, close buttons) with a **dynamically scaled content canvas** (avionics dials, status tables, scratchpads), a naive approach to window sizing limits will create catastrophic aspect ratio distortion, premature clamping, or spontaneous scrollbars.

---

## 2. Pre-Scaled vs. Post-Scaled Dimensions

### A. The Scaling Function $F_A(x)$
In Steve's architectural formulation:
$$F_A(x) = x \times \left(\frac{\text{target\_scale}}{\text{base\_scale}}\right)$$

- When $\text{target\_scale} = 1.0$, $F_A(x) = x$ (pure design boxels).
- When $\text{target\_scale} = 0.5$, $F_A(x) = 0.5x$ (half physical boxels).

### B. The API Contract Breakdown

| API | Domain | Coordinate Space | Behavior & Purpose |
| :--- | :---: | :---: | :--- |
| `SetBaselineWindowSize(w, h)` | `ImgWindow` | **Pre-scaled ($1.0\times$)** | Canonical baseline reference dimensions of the window when zoom is $100\%$. |
| `SetUnscaledChrome(w, h)` | `ImgWindow` | **Post-scaled Physical** | Height and width of invariant UI elements rendered at fixed $1.0\times$ scale that must *not* be multiplied by the zoom ratio. |
| `SetWindowResizingLimits(min_w, min_h, max_w, max_h)` | X-Plane SDK / OS | **Post-scaled Physical** | Hard physical clamping floor and ceiling enforced by the window manager during mouse drags and screen boundary clamping. |
| `ScaleWindowGeometry(from_z, to_z)` | `ImgWindow` | **Dynamic Transform** | Multiplies the scalable portion of the window by $(to\_z / from\_z)$, adding back the unscaled chrome. |
| `ResizeToScale(target_z)` | `ImgWindow` | **Absolute Transform** | Computes absolute dimensions anchored directly to $1.0\times$ baseline: $\text{Dim} = \text{Chrome} + (\text{Baseline} - \text{Chrome}) \times target\_z$. |

---

## 3. The Anatomy of UI Rows: Invariant Chrome vs. Scaled Canvas

Composite flight deck windows consist of three distinct vertical zones:

```
┌────────────────────────────────────────────────────────┐
│ Zone 1: Top Invariant Chrome (1.0x Scale)               │  <- Row 1: Title & Zoom Suite
├────────────────────────────────────────────────────────┤
│ Zone 2: Dynamically Scaled Canvas (_fontAdjust Scale)   │  <- Row 2: Environment / Theme Suite
│   ┌──────────────────────────────────────────────────┐ │  <- Tab Bar & Tab Headers
│   │                                                  │ │  <- Tab Content (Knobs, Sliders, Tables)
│   └──────────────────────────────────────────────────┘ │
├────────────────────────────────────────────────────────┤
│ Zone 3: Bottom Invariant Chrome (1.0x Scale)            │  <- Close & Quick Status Footers
└────────────────────────────────────────────────────────┘
```

### The Rule of Invariance:
- **Lock to $1.0\times$ Chrome ONLY what the user must click to operate window scaling:**
  - Row 1 holds the Zoom Steppers, Zoom Slider, Reset Button, and Fit Button. If Row 1 shrunk during zooming, the controls would twitch and move under the user's mouse while dragging or clicking!
  - The bottom footer holds the fixed `Close` trigger.
  - Total invariant chrome = $\text{Row 1} (\sim 36\text{px}) + \text{Footer} (\sim 28\text{px}) + \text{Window Padding} (\sim 16\text{px}) = \mathbf{80\text{px}}$.
- **Allow secondary feature suites (Row 2) to scale dynamically:**
  - Putting Row 2 (`[Opacity] [Transparent] Theme: [Charcoal ▾]`) *after* `ImGui::SetWindowFontScale(_fontAdjust)` allows its fonts, buttons, sliders, and pulldowns to shrink proportionally with the content.
  - This eliminates Row 2 from `SetUnscaledChrome`, completely removing it as an obstacle to window contraction.

---

## 4. The "Premature Physical Clamping" Trap

### Why Hardcoded Limits Break Zoom Scaling
Suppose a window has a $100\%$ baseline of $680 \times 596\text{px}$, and a developer sets:
```cpp
SetWindowResizingLimits(580, 378, 2560, 1440);
```
thinking: *"580x378 is the minimum comfortable size at 100% font scale."*

When the user zooms down toward $50\%$:
1. **The Width Knee at 85.3% Zoom:**
   $$\text{Target Width} = 680 \times 0.853 = \mathbf{580\text{px}}$$
   At $85.3\%$ zoom, the width hits the `minWidth = 580` physical wall. Width freezes solid!
2. **The Height Disparity:**
   The height is allowed to continue shrinking down toward $378\text{px}$ ($55.3\%$ zoom).
3. **The Distortion:**
   Between $85\%$ and $55\%$ zoom, width is frozen while height is shrinking. The window mutates from a natural landscape box into a tall, distorted column!
4. **The Dead Zone Below 55%:**
   Below $55\%$ zoom, height hits $378\text{px}$ and freezes too. The window sits completely dead at $580 \times 378$, while the zoom slider moves from $55\%$ to $50\%$ with zero window movement.

### The Correct Physics of Resizing Limits
`SetWindowResizingLimits` must accommodate the **lowest legitimate zoom level** supported by the UI (typically $50\%$):
$$\text{minWidth} = \text{Baseline Width} \times \text{MinZoom} = 680 \times 0.50 = \mathbf{340\text{px}}$$
$$\text{minHeight} = \text{Chrome} + (\text{Baseline Height} - \text{Chrome}) \times \text{MinZoom} = 80 + (596 - 80) \times 0.50 = \mathbf{338\text{px}}$$

Setting `SetWindowResizingLimits(340, 320, ...)` ensures that both axes scale isotropically and continuously all the way down to $50\%$ without hitting a premature physical wall.

---

## 5. Responsive Chrome Headers on Compact Zoom

When a window contracts to $340\text{px}$ at $50\%$ zoom, an invariant $1.0\times$ header row containing a long title like `"ImGee UX & Modern Cockpit Suite"` ($315\text{px}$) plus a full zoom button suite ($229\text{px}$) would require $560\text{px}$, clipping off-screen.

To preserve a single-line invariant toolbar at all scales, implement **responsive header title shortening**:
```cpp
const char* title_md = (ImGui::GetWindowWidth() > 540.0f) ? "# ImGee UX & Modern Cockpit Suite"
                     : (ImGui::GetWindowWidth() > 400.0f) ? "# ImGee Showcase"
                     : "# ImGee";
ImGui::TextMD(title_md);
```
- At $100\%$ scale ($680\text{px}$): Full descriptive suite title is displayed.
- At $70\%$ scale ($476\text{px}$): Gracefully steps down to `"ImGee Showcase"`.
- At $50\%$ scale ($340\text{px}$): Compact `"ImGee"` header prevents toolbar overflow.

---

## 6. Upstream Platform Recommendations for Roger (`ImgWindow`)

To make dynamic window scaling turnkey and prevent developers from falling into the physical clamping trap:

1. **Decouple Zoom Limits from Manual Resizing Limits:**
   In `ImgWindow::ScaleWindowGeometry`, boundary clamping should not indiscriminately use `minWidth` and `minHeight` if they represent manual drag limits. Provide an internal `minZoomScale` (default $0.5$) that computes legitimate zoom clamping floors dynamically:
   $$\text{min\_zoom\_w} = \text{chrome\_w} + (\text{base\_w} - \text{chrome\_w}) \times \text{min\_scale}$$
2. **Turnkey `SetZoomLimits(min_scale, max_scale)` API:**
   Let developers declare the operational zoom bounds in scale units (e.g. $0.50\text{f} \dots 1.50\text{f}$), allowing `ImgWindow` to derive all internal geometry constraints automatically without manual arithmetic.
