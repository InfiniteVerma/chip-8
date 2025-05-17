#include <chip8.h>
#include <constants.h>
#include <result.h>
#include <unistd.h>

#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "ERROR usage: ./a.out <rom file>\n";
        return 0;
    }

    const char* romPath = argv[1];

    Chip8 chip8;
    Result<int, string> result = chip8.readROM(romPath);

    if (!result.is_ok()) {
        cout << result.error() << "\n";
        return 0;
    }

    cout << "Loaded ROM (" << result.unwrap() << " bytes) into memory at 0x200\n";
    chip8.run();
    return 0;
}
