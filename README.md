# voxel-engine

my custom voxel engine written in c++ with opengl. inspired by minecraft.

![](screenshots/world.png)

## features

- infinite world with perlin noise terrain
- chunk-based rendering (16x256x16 chunks)
- water at sea level with sandy beaches
- frustum culling for performance
- texture atlas with grass, dirt, stone, sand and water
- simple fly camera

## building

1. clone the repo
2. open the solution in visual studio
3. build in release mode
4. run

## controls

- wasd - move
- mouse - look around
- space/l_shift - up/down
- esc - quit (menu coming soon)

## tech

- c++20
- opengl 4.6
- glad, glfw3
- glm
- fastnoise
- stb_image

## plans

- main menu and pause menu (imgui)
- block breaking and placing
- trees, caves, biomes
- multiplayer
- proper lighting and shadows

## screenshots

more soon...