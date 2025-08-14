
A 2D platformer written from scratch in C++ using SDL2.  
No Unity. No Unreal. Just raw code, physics, collisions, and tiled maps.

## 🎮 Features
- Smooth pixel-based movement
- Gravity & collision physics
- TMX map loading
- Player animations
- Custom game loop

## 📸 Screenshots

x![ezgif-6d646561b220b6](enemies)

## 🛠️ Built With
- C++
- SDL2
- Tiled (.tmx map support)

## 🚀 Getting Started

To build and run the project, make sure you have the following libraries installed:

* **SDL2**
* **SDL2\_image**
* **tmx** (TMX map parsing)
* **libxml2**
* **zlib**
* **SDL2_ttf**

### 🔧 Compile the Project

```bash
g++ *.cpp -lSDL2 -lSDL2_ttf -lSDL2_image -lxml2 -ltmx -lz -o wiz
```

### ▶️ Run the Game

```bash
./wiz
```

## 📂 Folder Structure
- `levels/` – Tiled maps
- `sprites/` – Player & tileset images
- `./` – Core C++ files
