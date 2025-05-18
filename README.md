# CHIP-8

A CHIP-8 emulator (rather interpreter) implementation in C++ using SDL3

## Build

```
cmake -B build
cmake --build build -j4
```

## Run

```
cd build/bin
./chip8 ../../roms/<pass rom file>
```

## Demos

| IBM Logo  | CHIP-8 Logo |
| ------------- | ------------- |
| ![ibm-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/e32b2717-2dd6-4d54-8e10-2b53ca732069)  | ![chip8_logo-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/05a46165-e08a-4402-9368-afa47c6fab98)  |
| Space Invaders | Octojam Title |
| ![2025-05-1813-59-26-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/57775ad2-52c1-4120-8fab-df9c73b17a36) | ![2025-05-1814-02-45-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/178f3be1-6aef-4be6-802b-a8bdf05de49e) |

## Pending Stuff

 - [ ] Pass tests from chip8-test-suite
 - [ ] Keypad actions
 - [ ] Add beep sound
 - [ ] Use only the CHIP-8 ram memory for stack/registers

## Resources

 - https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
 - http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
 - https://github.com/Timendus/chip8-test-suite
 - https://wiki.libsdl.org/SDL3
