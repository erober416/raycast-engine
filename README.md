# raycast-engine
![Screenshot from 2022-05-24 06-34-50](https://user-images.githubusercontent.com/61103027/170012421-0a859a8c-b6a4-48f8-a7b8-5e9996d864eb.png)
# Features
This is a fully-featured pseudo-3D real-time raycast engine including texture mapping, floor and ceiling casting, axis-based shading, keyboard controls, and map editing. Written in C++ using SDL2 as a hardware compatibility layer and inspired by id Tech 0 by id Software.
# How to build
This is an example of how to build using **Linux**. This software requires SDL2 to be installed on a system before compiling, and CMake is the default tool used to build.
```shell
# Step 1: Get sources from GitHub
$ git clone https://github.com/erober416/raycast-engine.git
$ cd raycast-engine/raycast
```
```shell
# Step 2: Create makefile by calling cmake
$ cmake -H. -Bbuild
```
```shell
# Step 3: Compile sources using makefile
$ cd build
$ make
```
```shell
# Step 4: Launching executable file
$ ./untitled
```
# Controls
Use <kbd>→</kbd> to turn clockwise<br>
Use <kbd>←</kbd> to turn counter-clockwise<br>
Use <kbd>↑</kbd> to move forwards along line of sight<br>
Use <kbd>↓</kbd> to move backwards along line of sight<br>
Hold <kbd>tab</kbd> to view minimap
# Tile editing
The default map file location is build/map1. Below is an example of a possible map configuration
```
8 8
1 2 1 2 1 2 1 2
2 P 0 0 0 0 0 1
1 0 0 0 0 0 0 2
2 0 0 0 0 0 0 1
1 0 0 0 0 0 0 2
2 0 0 0 0 0 0 1
1 0 0 0 0 0 0 2
2 1 2 1 2 1 2 1
```
