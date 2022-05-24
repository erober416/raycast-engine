# raycast-engine
![Screenshot from 2022-05-24 06-34-50](https://user-images.githubusercontent.com/61103027/170012421-0a859a8c-b6a4-48f8-a7b8-5e9996d864eb.png)
# What is this
This is a fully-featured pseudo-3D fast real-time rendered raycast engine. It is written in C++ using SDL2 as a hardware compatibility layer and was inspired by the id Tech 0 engine by id Software.
# Features
* Texture mapping
* Floor and ceiling casting
* Axis based shading
* Keyboard controls
* Tile editing
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
* Use <kbd>→</kbd> to turn clockwise
* Use <kbd>←</kbd> to turn counter-clockwise
* Use <kbd>↑</kbd> to move forwards along line of sight
* Use <kbd>↓</kbd> to move backwards along line of sight
* Hold <kbd>tab</kbd> to view minimap
# Tile editing
The default map file location is build/map1. Below is an example of a possible map configuration.
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
The first line first specifies the length and width of the map. The rest of the file represents what is stored in each of these tiles with a character each.<br>
* <kbd>0</kbd>: Empty space
* <kbd>1</kbd>-<kbd>9</kbd>: Wall. The value of the digit specifies the texture to be mapped to this wall tile
* <kbd>P</kbd>: Default location of camera
# Texture specification
The default texture specification file location is build/textures. Below is an example of this kind of file.
```
3
sample.bmp
snail.bmp
money.bmp
```
The first line specifies the number of textures available (1-9). The rest of the file specifies the relative path of these texture files. This project **only** supports bitmap files without alpha channel.
