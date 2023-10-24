# Design notes

### Engine

- setup and exit / crashes
- Render engine
- voxel engine (World)
- character controller
- input processor thread
- render caller thread
    - global time, delta time, fps
- window size

---

### Render engine

- takes copy of world mesh
- gets parameters from and is called by render thread

###### Render pipeline

- clear buffers
- vertex shader:
    - view + perspective transform *(programmable)*
    - depth (w) division
    - view clipping
    - screen transform
- rasterization w/ depth checking and vertex interpolation
- fragment shader:
    - process fragments *(programmable)*
    - construct hud
- printing of framebuffer and hud to terminal

---

###  World / Voxel engine

- world data
    - 2d vector of chunks
- world generator
- block updates and interactions
- wold mesher (combines the meshes of all blocks; block transform included)

###### Chunk

- 16 x 16 x 256 array of blocks

###### Block

- stores block type
