# SDL application template
Template readme for template SDL project.


## Build instructions
### Requirements
The engine builds on SDL 2.28.
Linux is actively supported; Windows support is next up.
The C++ compiler must support C++ 20 (namely `filesystem` and `source_location`).

To install all dependencies on Arch Linux:
```
# pacman -S gcc make sdl2 sdl2_ttf
```

### Build instructions
Run `make all` from the project root (check `Makefile` possible build configuration).
This will create a directory `build` which contains a relocatable build of the project.
The default binary name is `a.out`.


## Usage instructions
Running the binary will start the empty application with a frame rate counter enabled by default.
By default, it will look for the resource directory at `<cwd>/rsc`.


## Configuration
See the constructor of `Program` (`program.cpp`) for basic configuration.

If no audio support is needed, remove audio_device from `Program`'s constructor and remove initialization from `main()`.

Classes have their configuration as const members; see their respective header files.
