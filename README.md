# Raycaster-SDL2

A retro-style 3D raycaster engine built from scratch with C++ and SDL2, inspired by Wolfenstein 3D.

## Features

- Textured walls, floors, and ceilings
- Sprite rendering (barrels, pillars, lights)
- Point lighting with warm ambient glow
- Volumetric light shafts
- Distance-based fog
- Real-time minimap overlay
- Collision detection with wall sliding

## Controls

| Key | Action |
|---|---|
| `W` / `↑` | Move forward |
| `S` / `↓` | Move backward |
| `A` / `←` | Turn left |
| `D` / `→` | Turn right |
| `Shift` + `A`/`D` | Strafe left/right |
| `Alt` + `C` | Toggle floor/ceiling casting |
| `F11` | Toggle fullscreen |

## Building

Requires **SDL2** and **SDL2_image**.

```bash
g++ main.cpp -o raycaster -lSDL2 -lSDL2_image -O3
./raycaster
```

## Credits

Raycasting technique based on [Lodev's Computer Graphics Tutorial](https://lodev.org/cgtutor/raycasting.html).
