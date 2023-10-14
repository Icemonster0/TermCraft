# Design notes

### Engine

- Voxel engine
- Render engine
- Character controller
- input processor thread
- render caller thread
    - delta time
- setup and exit

### Voxel engine

- World data
    - 2d vector of chunks
- World generator
- Mesher

### Render engine

- takes copy of world data
- gets called by render thread
- Render pipeline

### Render pipeline

- clear buffers
- vertex shader:
    - model (chunk) transform
    - view transform
    - perspective transform
    - depth division
    - screen transform
- rasterization w/ depth checking
- fragment shader
- framebuffer printing


### Chunk

- 16 x 16 x 256 array of blocks

### Block

- has block type index
