# 3D Voxel Engine (Minecraft Clone)
<img width="789" height="595" alt="voxel engine demo" src="https://github.com/user-attachments/assets/a5d2141d-21e6-4d90-8e49-d74319f91bb1" />

My 3D Voxel Engine is a reiterated version of Beta Minecraft, featuring random terrain generation, random tree generation, water transparency, and a global lighting system. To optimize, 
I used a greedy meshing system that doesn't render faces of a block if they are not exposed to air and to have chunks loaded if a player is within 5 chunks or so. 

## Installation 
1. Install:
   - MinGW-w64
   - GLFW
   - OpenGL drivers

2. Compile:

```bash
g++ src/*.cpp \
    -Iinclude -Ivendor \
    -lglfw3 -lopengl32 -lgdi32 \
    -std=c++17 \
    -o voxel.exe
```
