# space themed 1k intro

A windows 1k intro by mrange & pestis.

## Prerequisites for building

You need Visual Studio (e.g. 2022), installed with [CMake](https://cmake.org/).

Following tools should be in path:

1. [nasm](https://www.nasm.us/)
2. [shader_minifier](https://github.com/laurentlb/Shader_Minifier)
3. [crinkler](https://github.com/runestubbe/Crinkler) Note: As crinkler.exe, not
   link.exe
4. Optionally: [glslangValidator](https://github.com/KhronosGroup/glslang)

## Build

1. Open the repository folder using Visual Studio
2. Choose the configuration:
   - heavy/light has main written in asm and compressed with crinkler
   - uncompressed has main written in asm, but is not compressed
   - debug version has a main function written in C for easier understanding
3. Hit F5 to build & run

CMake should copy the executables to the dist/ folder.
