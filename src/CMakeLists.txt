add_executable(chip8
    main.cpp
    chip8.cpp
    display.cpp
)

target_include_directories(chip8 PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(chip8 PRIVATE SDL3::SDL3)
