# space themed 1k intro

A windows 1k intro by mrange & pestis.

## Prerequisites for building

You need Visual Studio (e.g. 2022), installed with [CMake](https://cmake.org/).

Following tools should be in path:

1. [crinkler](https://github.com/runestubbe/Crinkler) Note: As crinkler.exe, not
   link.exe
2. Optionally: [shader_minifier](https://github.com/laurentlb/Shader_Minifier)
3. Optionally: [glslangValidator](https://github.com/KhronosGroup/glslang)

## Build

1. Open the repository folder using Visual Studio
2. Choose the configuration:

| Config name      | Main | Shader-minifier? | Crinkler? | Can debug? | Comments                                         |
|------------------|------|------------------|-----------|------------|--------------------------------------------------|
| cmain-toolmin    | C    | Yes              | No        | Yes        | Used for fast iteration during development       |
| cmain            | C    | No               | No        | Yes        | Used to check the hand-minified shader works     |
| debug            | asm  | No               | No        | Yes        | Used to debug the asm written main               |
| release-toolmin  | asm  | Yes              | Yes       | No         | Used to check roughly the size of the shader     |
| release          | asm  | No               | Yes       | No         | Final release, should produce <= 1k executable   |
| release-comp     | asm  | No               | No        | No         | Same as release, but without compression         |

3. Hit F5 to build & run

CMake should copy the executables to the dist/ folder.
