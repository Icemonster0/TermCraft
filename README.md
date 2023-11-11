# TermCraft - WIP
A 3D Minecraft clone running in the terminal.

### Work in progress
This is an early version of the project. [Many features](doc/TODO.md) are still missing.

![Screenshot](doc/Screenshot.png)

---

### Index
- [Build Instructions](#build-instructions)
- [Keybinds](#keybinds)
- [Troubleshooting](#troubleshooting)
    - [Error Codes](#error-codes)
    - [Performance](#performance)
- [Command Line Parameters](#command-line-parameters)
    - [Settings](#settings)
    - [Flags](#flags)

---

### Build Instructions
Compile the project with cmake:  
```
mkdir build
cd build
cmake path/to/termcraft
make
```
The executable must be run in a command line!

---

### Keybinds
It is recommended to turn off the delay on repeat keys in the system settings (probably under the keyboard or accessibility section). Try to quit using `q` rather than `ctrl-c` whenever possible to ensure a clean exit.

`w a s d`: Movement  
`i j k l` or `8 4 2 6`: Look/Turn  
`space`: fly up  
`c`: fly down  
`q`: quit

---

### Troubleshooting

##### Error Codes
When TermCraft exits or crashes in a controlled manner, it will print the following message:  
`Engine exited with return value x`.  
Here are all possible values for `x` and the associated errors.

| Code | Error | Possible Solution |
| ---- | ----- | -------- |
| 0 | Success | N/A |
| 1 | Failed to create temporary directory (command: `mkdir -p tmp.term_craft`) | `--fixed-window-size` |
| 2 | Failed to get terminal size and/or write to temporary file (command: `tput cols >> tmp.term_craft/term-size.tmp` or `tput lines >> tmp.term_craft/term-size.tmp`) | `--fixed-window-size` |
| 3 | Failed to remove temporary directory (command: `rm -r tmp.term_craft`) | `--fixed-window-size` |
| 4 | Failed to set cursor to position zero (command: `tput cup 0 0`) |  |
| 5 | Failed to make cursor invisible (command: `tput civis`) | `--cursor-visible` |
| 6 | Input setup failed (command: `stty -echo cbreak`) |  |
| 7 | Failed to clear the terminal window (command: `tput clear`) |  |
| 8 | Failed to reset cursor to normal mode (command: `tput cnorm`) | `--cursor-visible` |
| 9 | Failed to reset input to normal mode (command: `stty echo -cbreak`) |  |
| 10 | Failed to clear temporary file (command: `> tmp.term_craft/term-size.tmp`) | `--fixed-window-size` |
| 11 | Failed to open temporary file `tmp.term_craft/term-size.tmp` using `std::ifstream` |  |

If TermCraft crashes by printing `Killed`, the system likely ran out of memory and you should set the world size smaller (parameter `world-size`).  

In any other case, feel free to open an issue.

##### Performance
You can see useful info (fps, estimated memory usage etc.) by running with the `--debug--info` flag.
The biggest factors affecting performance are:

- Render distance (parameter `render-distance`)
- Viewport size (parameters `--fixed-window-size`, `width`, `height`)
- World size (parameter `world-size`)

---

### Command Line Parameters
Here are all parameters you can pass to TermCraft in the command line. Note that a *setting* must be followed by a value while a *flag* must not.

##### Settings
| name | type | default value | description |
| ---- | ---- | ------------- | ----------- |
| `color-mode` | string | `FULL` | Can be one of: `FULL` (full rgb color); `COMPAT` (grayscale with low dynamic range) |
| `fog` | float | `0.5` | Fog factor between 0.0 and 1.0 (0.0 = no fog; 1.0 = fog gradient comes up to camera) |
| `fov` | float | `70` | Field of view in degrees |
| `fps` | int | `24` | Target fps / fps cap |
| `height` | int | `24` | Height of viewport in pixels, if `--fixed-window-size` is set |
| `render-distance` | float | `100` | Render distance in blocks |
| `sky-color` | hex | `0x7ce1ff` | Color of the sky and fog (note the `0x` instead of `#`) |
| `width` | int | `80` | Width of viewport in pixels, if `--fixed-window-size` is set |
| `world-size` | int | `10` | World width in both X and Z directions in chunks (`world-size`*16 blocks) |

##### Flags
| name | description |
| ---- | ----------- |
| `--bad-normals` | Show frontfacing triangles in blue, backfacing in red (similar to Blender); Turn backface culling off |
| `--cursor-visible` | Make the terminal cursor visible (hidden by default); Fix error codes 5 and 8 |
| `--debug-info` | Show useful info as part of the HUD |
| `--disable-textures` | Use flat colors instead of textures |
| `--fixed-window-size` | Enable the `width` and `height` settings (if not set: automatic window size) |
| `--help` | Display a similar help message to these tables and exit |
