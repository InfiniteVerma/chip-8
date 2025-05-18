# CHIP-8

A CHIP-8 emulator (rather interpreter) implementation in C++

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

## Demo

### IBM Logo

https://github.com/user-attachments/assets/0998a6a7-6b08-452e-939a-81c5c136cd94

### Chip 8 Logo

https://github.com/user-attachments/assets/2dd38500-cef6-4342-b4b2-d8922322fbaa

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
