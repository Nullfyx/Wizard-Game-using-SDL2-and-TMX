
A 2D platformer written from scratch in C++ using SDL2.  
No Unity. No Unreal. Just raw code, physics, collisions, and tiled maps.

## 🎮 Features
- Smooth pixel-based movement
- Gravity & collision physics
- TMX map loading
- Player animations
- Custom game loop

## 📸 Screenshots
<br/>
<img width="987" height="736" alt="image" src="https://github.com/user-attachments/assets/57c22724-4934-46ea-a400-8c808a5f029f" />
<br/>
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

### 🔧 Compile the Project

```bash
g++ *.cpp -lSDL2 -lSDL2_image -lxml2 -ltmx -lz -o wiz
```

### ▶️ Run the Game

```bash
./wiz
```

## 📂 Folder Structure
- `levels/` – Tiled maps
- `sprites/` – Player & tileset images
- `./` – Core C++ files
