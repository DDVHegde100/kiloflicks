// imgui_main.cpp
// Minimal ImGui GUI for Thousand Flicks
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include "bmp.h"
#include "lsb.h"

static std::string status;
static std::string message;
static std::string input_path, output_path;

void encode_image() {
    try {
        BMPImage img = load_bmp(input_path);
        std::vector<uint8_t> msg(message.begin(), message.end());
        lsb_encode(img, msg);
        write_bmp(output_path, img);
        status = "[OK] Encoded and saved: " + output_path;
    } catch (const std::exception& e) {
        status = std::string("[ERR] ") + e.what();
    }
}

void decode_image() {
    try {
        BMPImage img = load_bmp(input_path);
        auto msg = lsb_decode(img, 8192);
        message = std::string(msg.begin(), msg.end());
        status = "[OK] Decoded message.";
    } catch (const std::exception& e) {
        status = std::string("[ERR] ") + e.what();
    }
}

int main(int, char**) {
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
    SDL_Window* window = SDL_CreateWindow("Thousand Flicks (ImGui)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 150");
    bool show = true;
    while (show) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) show = false;
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        ImGui::Begin("Thousand Flicks");
        ImGui::InputText("Input BMP", &input_path);
        ImGui::InputText("Output BMP", &output_path);
        ImGui::InputTextMultiline("Message", &message);
        if (ImGui::Button("Encode")) encode_image();
        ImGui::SameLine();
        if (ImGui::Button("Decode")) decode_image();
        ImGui::TextWrapped("%s", status.c_str());
        ImGui::End();
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
