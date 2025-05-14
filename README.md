# I acknowledge that this is a late submission.

# 💡 Computer Graphics HW6: Shading with Custom Rasterizer

A C++ project that implements a software rasterizer with flat, Gouraud, and Phong shading. 
OpenGL is used only for displaying the final image — all transformation, depth buffering, and shading are done manually.

---

## ✅ Description

This project demonstrates the full pipeline of a custom triangle rasterizer without using GPU hardware rasterization. 
Three different shading models are implemented over a unit sphere: 

- Flat Shading
- Gouraud Shading
- Phong Shading

Each shading stage handles lighting, gamma correction, and depth comparison with a manually built z-buffer.

---

## 🛠️ Compilation & Run Instructions

1. Open the solution file (`RasterizerHW6.sln`) in **Visual Studio 2019** or later.
2. Build the project (`Ctrl + Shift + B`)
3. Run the executable (`F5`)
4. Ensure the following dependencies are properly linked:
   - **GLEW**
   - **GLFW**
   - **GLM**

---

## 📦 Dependencies

| Library | Usage         |
|---------|---------------|
| GLEW    | OpenGL extension handling |
| GLFW    | Window creation and context |
| GLM     | Vector and matrix math |

---

## 🧠 Implemented Features

| 항목 | 내용 |
|------|------|
| Q1   | Flat shading using triangle centroid normal |
| Q2   | Gouraud shading with per-vertex lighting and color interpolation |
| Q3   | Phong shading with interpolated normals and per-fragment lighting |
| 공통 | 수동 구현된 변환 파이프라인 (모델링 → 투시 → 뷰포트) |
|      | 바운딩 박스 기반 삼각형 래스터라이징 및 뎁스 테스트 |
|      | 감마 보정 (`gamma = 2.2`) 포함 |

---

## 🌐 Light & Material Settings

- Point light at `(-4, 4, -3)`
- White light, no attenuation
- Material:
  - Ambient: `ka = (0, 1, 0)`
  - Diffuse: `kd = (0, 0.5, 0)`
  - Specular: `ks = (0.5, 0.5, 0.5)`
  - Shininess: `p = 32`
- Ambient light intensity `Ia = 0.2`

---

## 🖼️ Output Screenshots

| Flat Shading | Gouraud Shading | Phong Shading |
|--------------|------------------|----------------|
| ![Q1](./q1.png) | ![Q2](./q2.png)   | ![Q3](./q3.png)   |

---

## 📁 File Structure

```
├── Q1_flat_shading.cpp
├── Q2_gouraud_shading.cpp
├── Q3_phong_shading.cpp
├── include/
│   └── glm/, glfw/, glew/
├── lib/
│   └── glew32.lib, glfw3.lib, opengl32.lib
└── result/
    ├── q1.png
    ├── q2.png
    └── q3.png
```

---
