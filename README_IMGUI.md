# ImGui and SDL2/OpenGL3 integration for Thousand Flicks

## Dependencies
- Dear ImGui (https://github.com/ocornut/imgui)
- SDL2 (`brew install sdl2`)
- OpenGL (macOS provides this)

## Setup
1. Download ImGui source files (imgui.cpp, imgui_draw.cpp, imgui_tables.cpp, imgui_widgets.cpp, imgui_demo.cpp, imgui.h, imgui_internal.h, imconfig.h, imstb_rectpack.h, imstb_textedit.h, imstb_truetype.h) and place them in `thirdparty/imgui/`.
2. Download the SDL2+OpenGL3 backend files (imgui_impl_sdl.cpp, imgui_impl_sdl.h, imgui_impl_opengl3.cpp, imgui_impl_opengl3.h) and place them in `thirdparty/imgui/`.
3. Install SDL2: `brew install sdl2`

## Build Example
```
g++ -std=c++17 -Ithirdparty/imgui -I/usr/local/include -I/opt/homebrew/include \
    src/main.cpp src/bmp.cpp src/lsb.cpp src/imgui_main.cpp \
    thirdparty/imgui/imgui*.cpp thirdparty/imgui/imgui_impl_*.cpp \
    -L/usr/local/lib -L/opt/homebrew/lib -lSDL2 -framework OpenGL -o thousandflicks
```

## Usage
Run `./thousandflicks` to launch the ImGui GUI.
