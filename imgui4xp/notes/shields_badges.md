# Modern Shields.io & Native ImGee Badges

This note demonstrates local disk image loading alongside asynchronous online remote images (Tier 3) and zero-texture native ImGee vector badges and pills.

---

### 1. Local Disk Image with Pandoc Sizing & Borders
* **Default Auto-Constraint (max-width: 100%):**
![Full Panel Auto-Fit](Resources/plugins/imgui4xp/imgui_demo.jpg)

* **Proportional 50% Panel Width with Avionics Blue Border (`=50% border=#3498db`):**
![Cockpit Half Width](<Resources/plugins/imgui4xp/imgui_demo.jpg =50% border=#3498db>)

* **Explicit Sizing with Gold Border (`=220x120 border=gold`):**
![Cockpit 220px](<Resources/plugins/imgui4xp/imgui_demo.jpg =220x120 border=gold>)

* **HTML `<img />` Tag (`width="220" border="#2ecc71"`):**
<img src="Resources/plugins/imgui4xp/imgui_demo.jpg" width="220" border="#2ecc71" />

---

### 2. Online Remote Badges with Size Modifiers & Borders
* **Auto-sized Shields.io Badges:**
![X-Plane](https://img.shields.io/badge/X--Plane-12.00+-blue.png)
![imgui4xp](https://img.shields.io/badge/imgui4xp-v1.14-success.png)
![ImgWindow](https://img.shields.io/badge/ImgWindow-v2.3.0-orange.png)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen.png)

* **Pandoc Angle-Bracket Modifiers (`=160x border=#e74c3c`):**
![X-Plane Red Border](<https://img.shields.io/badge/X--Plane-12.00+-blue.png =160x border=#e74c3c>)
![Build Green Border](<https://img.shields.io/badge/Build-Passing-brightgreen.png =140x border=#2ecc71>)

* **HTML `<img />` Remote Badges:**
<img src="https://img.shields.io/badge/Vulkan-Metal-brightgreen.png" width="160" height="28" border="#25d366" />
<img src="https://img.shields.io/badge/Cockpit-Glass%20Suite-blue.png" width="180" height="28" border="#3498db" />

---

### 3. Zero-Texture Native ImGee Badges & Pills
* Status Capsules: <pill color=#2ecc71>NOMINAL</pill>   <pill color=#e67e22>CAUTION</pill>   <pill color=#e74c3c>WARNING</pill>
* Release Chips: <badge>v1.14</badge>   <badge>v2.3.0</badge>   <badge>macOS Universal</badge>
* Keycaps: <kbd>AP DISC</kbd>   <kbd>Alt</kbd>+<kbd>F4</kbd>   <kbd>Shift</kbd>+<kbd>Z</kbd>

---

### 4. In-Memory Named Images (`ImgWindow::RegisterMemoryImage`)
* **Standard CommonMark Link (`![Beacon](avionics_beacon)`):**
![Beacon](avionics_beacon)

* **Explicit Sizing with Emerald Border (`<avionics_beacon =32x32 border=#2ecc71>`):**
![Beacon](<avionics_beacon =32x32 border=#2ecc71>)

* **HTML `<img />` Tag (`<img src="avionics_beacon" width="32" height="32" border="#2ecc71" />`):**
<img src="avionics_beacon" width="32" height="32" border="#2ecc71" />

* **Native Icon Capsules & Annunciators:**
  * Status Pill with Icon: <pill icon=avionics_beacon color=#2ecc71>ACTIVE</pill>
  * Release Badge with Icon: <badge icon=avionics_beacon>v1.14</badge>
  * Unary Annunciator Capsules: <pill icon=avionics_beacon color=#e74c3c /> (Warning)   <pill icon=avionics_beacon color=#f39c12 /> (Caution)   <badge icon=avionics_beacon /> (Standby)
  * Sized Typography Tags: <tiny color=#888888>10px Tiny</tiny>   <small color=#3498db>12px Small</small>   <regular>14px Regular</regular>   <large color=#e67e22>16px Large</large>   <huge color=#2ecc71>20px Huge</huge>

---

### Comparison Matrix
| Format | Texture Required | Network Required | Crisp at High DPI |
| :--- | :---: | :---: | :---: |
| **Shields.io Image** | Yes (downloaded) | Yes | Rasterized |
| **ImGee Native Vector** | No (zero VRAM) | No (offline) | Perfect Sub-pixel |
<!-- table-preset: width=520 sort="Format" asc -->
