# Modern Shields.io & Native ImGee Badges

This note demonstrates local disk image loading alongside asynchronous online remote images (Tier 3) and zero-texture native ImGee vector badges and pills.

---

### 1. Local Disk Image (Tier 1 stb_image Loader)
![X-Plane Cockpit Reference](Resources/plugins/imgui4xp/imgui_demo.jpg)

---

### 2. Online Remote Images & Shields (Tier 3 Async HTTP Download & GPU Upload)
![X-Plane](https://img.shields.io/badge/X--Plane-12.00+-blue.png)
![imgui4xp](https://img.shields.io/badge/imgui4xp-v1.14-success.png)
![ImgWindow](https://img.shields.io/badge/ImgWindow-v2.3.0-orange.png)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen.png)

*Remote Web Images (PNG & JPEG Multi-Format Test):*
![Remote JPEG Photo](https://httpbin.org/image/jpeg)
![Remote PNG Icon](https://httpbin.org/image/png)

---

### 3. Zero-Texture Native ImGee Badges & Pills
* Status Capsules: <pill color=#2ecc71>NOMINAL</pill>   <pill color=#e67e22>CAUTION</pill>   <pill color=#e74c3c>WARNING</pill>
* Release Chips: <badge>v1.14</badge>   <badge>v2.3.0</badge>   <badge>macOS Universal</badge>
* Keycaps: <kbd>AP DISC</kbd>   <kbd>Alt</kbd>+<kbd>F4</kbd>   <kbd>Shift</kbd>+<kbd>Z</kbd>

---

### Comparison Matrix
| Format | Texture Required | Network Required | Crisp at High DPI |
| :--- | :---: | :---: | :---: |
| **Shields.io Image** | Yes (downloaded) | Yes | Rasterized |
| **ImGee Native Vector** | No (zero VRAM) | No (offline) | Perfect Sub-pixel |
<!-- table-preset: width=520 sort="Format" asc -->
