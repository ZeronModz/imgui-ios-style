#include "imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <cstring>
#include <cmath>

static bool show_menu = true;
static int selected_item = -1;
static float anim_progress = 0.0f;
static float menu_alpha = 0.0f;

struct MenuItem {
    const char* icon;
    const char* title;
    const char* subtitle;
    ImVec4 color;
};

static MenuItem menu_items[] = {
    {"Home",     "Dashboard",     "View your overview",     ImVec4(0.04f, 0.52f, 1.00f, 1.0f)},
    {"Profile",  "My Profile",    "Edit your info",         ImVec4(0.20f, 0.78f, 0.35f, 1.0f)},
    {"Settings", "Preferences",   "App settings",           ImVec4(1.00f, 0.58f, 0.00f, 1.0f)},
    {"Messages", "Inbox",         "3 unread messages",      ImVec4(0.69f, 0.32f, 0.87f, 1.0f)},
    {"Photos",   "Gallery",       "128 items",              ImVec4(1.00f, 0.23f, 0.19f, 1.0f)},
    {"Music",    "Player",        "Now playing",            ImVec4(0.19f, 0.69f, 0.50f, 1.0f)},
};

void DrawRoundedRect(ImDrawList* dl, ImVec2 p, ImVec2 s, ImVec4 col, float r) {
    dl->AddRectFilled(p, ImVec2(p.x + s.x, p.y + s.y), ImGui::ColorConvertFloat4ToU32(col), r);
}

void DrawCircle(ImDrawList* dl, ImVec2 c, float r, ImVec4 col) {
    dl->AddCircleFilled(c, r, ImGui::ColorConvertFloat4ToU32(col));
}

void DrawTextCentered(ImDrawList* dl, const char* text, ImVec2 pos, ImVec4 col) {
    ImVec2 ts = ImGui::CalcTextSize(text);
    dl->AddText(ImVec2(pos.x - ts.x * 0.5f, pos.y - ts.y * 0.5f), ImGui::ColorConvertFloat4ToU32(col), text);
}

void RenderMenu() {
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* dl = ImGui::GetBackgroundDrawList();

    float w = io.DisplaySize.x;
    float h = io.DisplaySize.y;

    // Background
    dl->AddRectFilled(ImVec2(0, 0), ImVec2(w, h), IM_COL32(242, 242, 247, 255));

    // Title
    const char* title = "Menu";
    float title_w = ImGui::CalcTextSize(title).x;
    dl->AddText(ImVec2(w * 0.5f - title_w * 0.5f, 50), IM_COL32(0, 0, 0, 217), title);

    // Subtitle
    const char* sub = "Choose an option";
    float sub_w = ImGui::CalcTextSize(sub).x;
    dl->AddText(ImVec2(w * 0.5f - sub_w * 0.5f, 80), IM_COL32(0, 0, 0, 115), sub);

    // Menu card
    float card_x = 30;
    float card_y = 120;
    float card_w = w - 60;
    float card_h = 500;
    float card_r = 20.0f;

    dl->AddRectFilled(
        ImVec2(card_x, card_y),
        ImVec2(card_x + card_w, card_y + card_h),
        IM_COL32(255, 255, 255, 255),
        card_r
    );

    // Menu items
    float item_h = 75.0f;
    float item_pad = 10.0f;

    for (int i = 0; i < 6; i++) {
        float iy = card_y + item_pad + i * item_h;

        // Hover effect
        bool hovered = io.MousePos.x > card_x + 10 && io.MousePos.x < card_x + card_w - 10 &&
                       io.MousePos.y > iy && io.MousePos.y < iy + item_h - 10;

        if (hovered) {
            dl->AddRectFilled(
                ImVec2(card_x + 10, iy),
                ImVec2(card_x + card_w - 10, iy + item_h - 10),
                IM_COL32(0, 0, 0, 15),
                12.0f
            );
        }

        // Icon circle
        float icon_x = card_x + 35;
        float icon_y = iy + (item_h - 10) * 0.5f;
        float icon_r = 20.0f;

        dl->AddCircleFilled(
            ImVec2(icon_x, icon_y),
            icon_r,
            ImGui::ColorConvertFloat4ToU32(menu_items[i].color)
        );

        // Icon text
        ImVec2 icon_ts = ImGui::CalcTextSize(menu_items[i].icon);
        dl->AddText(
            ImVec2(icon_x - icon_ts.x * 0.5f, icon_y - icon_ts.y * 0.5f),
            IM_COL32(255, 255, 255, 255),
            menu_items[i].icon
        );

        // Title
        dl->AddText(
            ImVec2(icon_x + 35, iy + 18),
            IM_COL32(0, 0, 0, 217),
            menu_items[i].title
        );

        // Subtitle
        dl->AddText(
            ImVec2(icon_x + 35, iy + 40),
            IM_COL32(0, 0, 0, 115),
            menu_items[i].subtitle
        );

        // Arrow
        const char* arrow = ">";
        ImVec2 arrow_ts = ImGui::CalcTextSize(arrow);
        dl->AddText(
            ImVec2(card_x + card_w - 30 - arrow_ts.x * 0.5f, iy + (item_h - 10) * 0.5f - arrow_ts.y * 0.5f),
            IM_COL32(0, 0, 0, 80),
            arrow
        );

        // Separator (except last)
        if (i < 5) {
            dl->AddLine(
                ImVec2(card_x + 80, iy + item_h - 10),
                ImVec2(card_x + card_w - 20, iy + item_h - 10),
                IM_COL32(0, 0, 0, 25)
            );
        }

        // Click
        if (hovered && io.MouseDown[0]) {
            selected_item = i;
        }
    }

    // Bottom decoration
    float dot_y = card_y + card_h + 30;
    for (int i = 0; i < 5; i++) {
        float dot_x = w * 0.5f - 40 + i * 20;
        float dot_r = (i == 1) ? 5.0f : 3.0f;
        ImVec4 dot_col = (i == 1) ? ImVec4(0.04f, 0.52f, 1.0f, 1.0f) : ImVec4(0, 0, 0, 60);
        dl->AddCircleFilled(ImVec2(dot_x, dot_y), dot_r, ImGui::ColorConvertFloat4ToU32(dot_col));
    }
}

int main(int, char*[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_Window* window = SDL_CreateWindow("iOS Menu",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        400, 650,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 16;
    s.FrameRounding = 10;
    s.GrabRounding = 8;
    s.ItemSpacing = ImVec2(12, 10);
    s.WindowPadding = ImVec2(16, 16);
    s.Colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.97f, 1.0f);
    s.Colors[ImGuiCol_Text] = ImVec4(0, 0, 0, 0.85f);

    ImGui_ImplSDL2_InitForOpenGL(window, gl);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
                running = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        RenderMenu();

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.95f, 0.95f, 0.97f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
