# LiDAR Viewer

View LiDAR point clouds and compute a Euclidean MST with Kruskal's algorithm.

## Build

### Windows (MSYS2/MinGW)

Install dependencies in MSYS2 (UCRT64 shell):
```bash
pacman -S mingw-w64-ucrt-x86_64-glfw mingw-w64-ucrt-x86_64-glm
```

Then build:
```bash
make
./lidar_viewer
```

A native Windows file picker (Win32) is used automatically — no extra dependencies needed.

### Linux

Install dependencies:
```bash
# Ubuntu/Debian
sudo apt install libglfw3-dev libglm-dev

# Arch
sudo pacman -S glfw glm

# For the native file browser (optional — falls back to text input if absent)
sudo apt install zenity      # GNOME/GTK
sudo apt install kdialog     # KDE
```

The Makefile auto-detects the OS and switches flags accordingly:
```bash
make
./lidar_viewer
```

## Usage

- Type a path directly or click **Browse...** to open a file picker.
- Load any `.las` file (LAS 1.0–1.4, all point data record formats).
- Camera: **WASD** + **Space** / **LCtrl** to move; **right-mouse drag** to look.
- Set *k neighbours* and click **Build Kruskal MST** to compute the MST.
- Use the **Visible edges** slider to reveal MST edges incrementally.

## LAS / LAZ

LAZ files must be converted to LAS first — e.g. with [PDAL](https://pdal.io/):
```bash
pdal translate input.laz output.las
# or use the pipeline in tests/pipeline.json
```

Source data for Slovenia: <https://gis.arso.gov.si/evode/profile.aspx?id=atlas_voda_Lidar@Arso>

## Project structure

```
src/           — C++ sources
include/       — headers (Vertex.h, PointCloud.h, Graph.h, FileDialog.h, …)
imgui/         — Dear ImGui (vendored)
shaders/       — GLSL shaders
tests/         — PDAL pipeline example
```
