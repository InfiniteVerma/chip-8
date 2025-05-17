#include <cstdint>
#include <fstream>
#include <iostream>
#include <result.h>

constexpr unsigned int MEMORY_SIZE = 4096;
constexpr unsigned int START_ADDRESS = 0x200;

using namespace std;

class Chip8 {
    uint8_t memory[MEMORY_SIZE]{};
public:
    Result<int, string> readROM(const char* romPath);
};

Result<int, string> Chip8::readROM(const char* romPath) {
    ifstream rom(romPath, ios::binary | ios::ate);

    if(!rom) {
        return string("ERROR in opening file\n");
    }

    streamsize size = rom.tellg();
    if(size > (MEMORY_SIZE - START_ADDRESS)) {
        return string("ROM too large to fit in memory\n");
    }

    cout << "Size of rom file: " << size << "\n";

    rom.seekg(0, ios::beg);

    if(!rom.read(reinterpret_cast<char*>(&memory[START_ADDRESS]), size)) {
        return string("Failed to read ROM file into memory\n");
    }

    return size;
}

int main(int argc, char* argv[]) {

    if(argc != 2) {
        cout << "ERROR usage: ./a.out <rom file>\n";
        return 0;
    }

    const char* romPath = argv[1];

    Chip8 chip8;
    Result<int, string> result = chip8.readROM(romPath);

    if(!result.is_ok()) {
        cout << result.error() << "\n";
        return 0;
    }

    cout << "Loaded ROM (" << result.unwrap() << " bytes) into memory at 0x200\n";
    return 0;
}
