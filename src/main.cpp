#include <stdio.h>
#include <queue>
#include "raylib.h"
#include "imgui.h"
#include "physics.h"
#include "rlImGui.h"

#define PLAYER_WIDTH 8
#define PLAYER_HEIGHT 64
#define PLAYER_COLOR RAYWHITE

#define BALL_RADIUS 16
#define BALL_COLOR RAYWHITE
#define BALL_DEFAULT_VELOCITY 1

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define FONT_COLOR RAYWHITE
#define FONT GetFontDefault()
#define FONT_SIZE 24

constexpr double TICKRATE = 256;
constexpr double TICKDELAY_S = 1000 / TICKRATE / 1000;

typedef struct {
    int y = 0;
    int score = 0;
} Player;

typedef struct {
    int x = WINDOW_WIDTH / 2;
    int y = WINDOW_HEIGHT / 2;
    int velX = BALL_DEFAULT_VELOCITY;
    int velY = BALL_DEFAULT_VELOCITY;
} Ball;

struct {
    bool debugMenu = false;
    Player p1;
    Player p2;
    Ball ball;

    double tickTimer;

    std::queue<int> keyQueue;
} gameState;

static RenderTexture2D gameTexture2D;
static RenderTexture2D debugTexture2D;

static Vector2 debugGameWindowSize = {WINDOW_WIDTH/1.5,WINDOW_WIDTH/1.5};

void UpdateState();

void Draw();

void Reset();

void HandleKeyQueue();
void embraceTheDarkness()
{
  ImVec4* colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
  colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
  colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
  colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
  colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
  colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
  colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowPadding                     = ImVec2(8.00f, 8.00f);
  style.FramePadding                      = ImVec2(5.00f, 2.00f);
  style.CellPadding                       = ImVec2(6.00f, 6.00f);
  style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
  style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
  style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
  style.IndentSpacing                     = 25;
  style.ScrollbarSize                     = 15;
  style.GrabMinSize                       = 10;
  style.WindowBorderSize                  = 1;
  style.ChildBorderSize                   = 1;
  style.PopupBorderSize                   = 1;
  style.FrameBorderSize                   = 1;
  style.TabBorderSize                     = 1;
  style.WindowRounding                    = 7;
  style.ChildRounding                     = 4;
  style.FrameRounding                     = 3;
  style.PopupRounding                     = 4;
  style.ScrollbarRounding                 = 9;
  style.GrabRounding                      = 3;
  style.LogSliderDeadzone                 = 4;
  style.TabRounding                       = 4;
}
int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "kal game engine");

    gameTexture2D = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    debugTexture2D = LoadRenderTexture(WINDOW_WIDTH / 1.5, WINDOW_HEIGHT / 1.5);

    rlImGuiSetup(true);
    embraceTheDarkness();

    printf("%f", TICKDELAY_S);
    while (!WindowShouldClose()) {
        HandleKeyQueue();
        if (GetTime() - gameState.tickTimer > TICKDELAY_S) {
            gameState.tickTimer = GetTime();
            UpdateState();
        }
        Draw();
    }

    return 0;
}

void UpdateState() {
    SetWindowTitle(TextFormat("kal game engine | %d FPS", GetFPS()));

    for (; !gameState.keyQueue.empty(); gameState.keyQueue.pop()) {
        int key = gameState.keyQueue.front();

        if (key == KEY_F9) {
            gameState.debugMenu = !gameState.debugMenu;
        }
    }

    if (IsKeyDown(KEY_DOWN)) {
        if (gameState.p2.y + 1 <= gameTexture2D.texture.height - PLAYER_HEIGHT) {
            gameState.p2.y++;
        }
    }

    if (IsKeyDown(KEY_UP)) {
        if (gameState.p2.y - 1 >= 0) {
            gameState.p2.y--;
        }
    }

    if (IsKeyDown(KEY_S)) {
        if (gameState.p1.y + 1 <= gameTexture2D.texture.height - PLAYER_HEIGHT) {
            gameState.p1.y++;
        }
    }

    if (IsKeyDown(KEY_W)) {
        if (gameState.p1.y - 1 >= 0) {
            gameState.p1.y--;
        }
    }
    if (circleRectIntersection(
            CircleType{gameState.ball.x, gameState.ball.y, BALL_RADIUS},
            RectType{0, gameState.p1.y, PLAYER_WIDTH, PLAYER_HEIGHT}
        )
    ) {
        gameState.ball.velX = -gameState.ball.velX;

        printf("intersectp1\n");
    } else if (circleRectIntersection(
            CircleType{gameState.ball.x, gameState.ball.y, BALL_RADIUS},
            RectType{WINDOW_WIDTH - PLAYER_WIDTH, gameState.p2.y, PLAYER_WIDTH, PLAYER_HEIGHT}
        )
    ) {
        printf("intersectp2\n");
        gameState.ball.velX = -gameState.ball.velX;
    } else if (gameState.ball.y - BALL_RADIUS < 0 ||
               gameState.ball.y + BALL_RADIUS > WINDOW_HEIGHT) {
        gameState.ball.velY = -gameState.ball.velY;
    } else if (
        gameState.ball.x < 0
    ) {
        gameState.p2.score++;
        Reset();
    } else if (gameState.ball.x > WINDOW_WIDTH - BALL_RADIUS) {
        gameState.p1.score++;
        Reset();
    }
    gameState.ball.x += gameState.ball.velX;
    gameState.ball.y += gameState.ball.velY;
}

void DebugDrawPlayerData(Player* player, const char* name) {
    ImGui::Text(TextFormat("> Player %s", name));
    ImGui::SliderInt(TextFormat("> Player Y##%s", name), &player->y, 0, WINDOW_HEIGHT - PLAYER_HEIGHT);
    ImGui::Text(TextFormat("> Player Score: %d", player->score));
}

void DebugDrawBallData(Ball* ball) {
    ImGui::Text("> Ball Info");
    ImGui::SliderInt("> Ball X", &ball->x, 0, WINDOW_WIDTH);
    ImGui::SliderInt("> Ball Y", &ball->y, 0, WINDOW_HEIGHT);
    ImGui::SliderInt("> Ball velX", &ball->velX, -128, 128);
    ImGui::SliderInt("> Ball velY", &ball->velY, -128, 128);
}

void Draw() {
    BeginTextureMode(gameTexture2D);
    ClearBackground(BLACK);
    DrawRectangle(0, gameState.p1.y, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_COLOR);
    DrawRectangle(gameTexture2D.texture.width - PLAYER_WIDTH, gameState.p2.y, PLAYER_WIDTH, PLAYER_HEIGHT,
                  PLAYER_COLOR);
    DrawCircle(gameState.ball.x, gameState.ball.y, BALL_RADIUS, BALL_COLOR);
    const char* p1ScoreText = TextFormat("P1: %d", gameState.p1.score);
    const char* p2ScoreText = TextFormat("P2: %d", gameState.p2.score);

    DrawText(p1ScoreText, WINDOW_WIDTH / 2 - MeasureText(p1ScoreText, FONT_SIZE) / 2 - 15, FONT_SIZE + 5, FONT_SIZE,
             FONT_COLOR);
    DrawText(p2ScoreText, WINDOW_WIDTH / 2 + MeasureText(p2ScoreText, FONT_SIZE) / 2 + 15, FONT_SIZE + 5, FONT_SIZE,
             FONT_COLOR);
    EndTextureMode();


    if (gameState.debugMenu) {
        BeginTextureMode(debugTexture2D);
        DrawTexturePro(gameTexture2D.texture, Rectangle{0, 0, WINDOW_WIDTH, -WINDOW_HEIGHT},
                       Rectangle{0, 0, WINDOW_WIDTH/1.5, WINDOW_HEIGHT/1.5}, Vector2{0, 0}, 0.0f, WHITE);
        EndTextureMode();
    }

    BeginDrawing();
    rlImGuiBegin();
    ClearBackground(BLACK);
    if (gameState.debugMenu) {
        ImGui::Begin("Debug Menu", &gameState.debugMenu);
        ImGui::Text("KAL game engine");
        ImGui::Separator();
        DebugDrawPlayerData(&gameState.p1, "1");
        ImGui::Separator();
        DebugDrawPlayerData(&gameState.p2, "2");
        ImGui::Separator();
        DebugDrawBallData(&gameState.ball);
        ImGui::Separator();
        ImGui::End();

        ImGui::Begin("Game", &gameState.debugMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
        rlImGuiImageRenderTexture(&debugTexture2D);
        ImGui::End();
    } else {
        DrawTexturePro(gameTexture2D.texture, Rectangle{0, 0, WINDOW_WIDTH, -WINDOW_HEIGHT},
                       Rectangle{0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, Vector2{0, 0}, 0.0f, WHITE);
    }
    rlImGuiEnd();
    EndDrawing();
}

void HandleKeyQueue() {
    gameState.keyQueue.push(GetKeyPressed());
}

void Reset() {
    gameState.p1.y = 0;
    gameState.p2.y = 0;
    gameState.ball.x = WINDOW_WIDTH / 2;
    gameState.ball.y = WINDOW_HEIGHT / 2;
    gameState.ball.velX = BALL_DEFAULT_VELOCITY;
    gameState.ball.velY = BALL_DEFAULT_VELOCITY;
}
