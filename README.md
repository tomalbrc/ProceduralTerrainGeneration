# Procedural Terrain Generation
Procedural Terrain Generation in irrlicht using the C++ STL for multithreading

[Check out release 0.9](https://github.com/tomalbrc/ProceduralTerrainGeneration/releases/tag/0.9)

Controls - Press:
 - WASD or Arrow Keys to move
 - SHIFT for accelerated movement
 - P to spawn enemy
 - SPACE to jump
 - R to reload
 - M to increase view distance
 - N to decrease view distance
 - F1 to reset position
 - F2 to toggle fly mode
 - F4 to reload terrain
 - ESCAPE to leave

 - Left click triggers a raycast from the player and colors the triangle at the collision point for debug purposes

### Dependencies

This project relies on the following open source libraries:

- irrBullet (irrlicht compatible bullet wrapper)
- bullet3 (physics engine)
- irrlicht 3D (3d graphics engine)
- PortAudio (audio engine)

### Building

To build the project on windows, just open the ProcMapGeneration.sln in 
Visual Studio 2017 and build


On macOS you can use the ProcMapGeneration-macOS.xcodeproj Xcode 9 
compatible project

Future versions may use CMake

### Notes

Raycasting currently does not work (since bullet3 intergration)
but will be added again in future revisions

### Screenshots

![v0.9.1](https://github.com/tomalbrc/ProceduralTerrainGeneration/raw/master/screenshots/screenshot-0.9.1.png "v091")
![v0.9](https://github.com/tomalbrc/ProceduralTerrainGeneration/raw/master/screenshots/screenshot-0.9.png "v0.9")
![Early screenshot](https://github.com/tomalbrc/ProceduralTerrainGeneration/raw/master/screenshots/screenshot-0.8.png "Early Screenshot")

### Assets:
[Clouds, trees & bushes](https://opengameart.org/content/4-trees-3-clouds-and-2-bushes)
 (CC BY-SA 3.0 https://creativecommons.org/licenses/by-sa/3.0/)

[Low Poly Knight](https://opengameart.org/content/low-poly-knight)

[Low Poly Vegetation Pack](https://opengameart.org/content/low-poly-vegetation-pack)

[UI Elements](https://opengameart.org/content/fantasy-ui-elements-by-ravenmore)

### License of this Project?
MIT! Free software, hell yeah!

