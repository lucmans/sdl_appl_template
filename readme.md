# SDL application template
Template readme for template SDL project.


## Build instructions
### Requirements
The engine builds on SDL 2.
Both Linux and Windows are supported.
The C++ compiler must support C++ 20.

#### Windows
Mingw-w64 is used to compile on Windows. Download and install [MSYS2](https://www.msys2.org/) and launch an UCRT64 shell (optionally, update all packages using `pacman -Syuu`; possibly twice if updating will close all MSYS2 processes). To install Mingw-w64 along with make, run the following command and answer the default option to every question:
```
$ pacman -S mingw-w64-ucrt-x86_64-toolchain make
```

Then, retrieve the mingw binary release of [SDL 2.x](https://github.com/libsdl-org/SDL/releases) and [SDL_tff 2.x](https://github.com/libsdl-org/SDL_ttf/releases), extract for each the `x86_64-w64-mingw32` build somewhere (e.g. `dependencies/sdl2` and `dependencies/sdl2_ttf` respectively).

Finally, copy over all `.dll` files the program depends on to the build directory (TODO: automate). Don't forget `libstdc++.dll`, `libgcc_s_seh-1.dll` and `libwinpthread-1.dll` in `<MSYS2_ROOT>/uctr64/bin/`.

#### Linux
To install the dependencies on Arch Linux, run:
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

Classes have their configuration as const members; see their respective header files.
