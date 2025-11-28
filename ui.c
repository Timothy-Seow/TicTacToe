#include "raylib.h"
#include "ui.h"

// Internal UI settings and colors
static Color btnColor;
static Color glowColorBase;
static Color glowColorHover;

static float baseGlow;
static float hoverGlow;
static float roundness;
static int segments;


void InitUI()
{
    // Neon yellow button
    btnColor       = (Color){ 255, 255, 0, 255 };

    // Neon glow colors
    glowColorBase  = (Color){ 255, 255, 120, 80 };
    glowColorHover = (Color){ 255, 255, 180, 160 };

    // Glow settings
    baseGlow  = 12.0f;
    hoverGlow = 22.0f;

    // Shape
    roundness = 0.4f;
    segments  = 16;
}

void DrawButton(Rectangle btn, bool hovered)
{
    float glow = hovered ? hoverGlow : baseGlow;
    Color glowColor = hovered ? glowColorHover : glowColorBase;

    // Glow layer
    DrawRectangleRounded(
        (Rectangle){
            btn.x - glow/2,
            btn.y - glow/2,
            btn.width  + glow,
            btn.height + glow
        },
        roundness,
        segments,
        glowColor
    );

    // Main button
    DrawRectangleRounded(btn, roundness, segments, btnColor);
}

void DrawCenteredTextInButton(Rectangle btn, const char* text, int fontSize, Color color)
{
    int textWidth = MeasureText(text, fontSize);

    int x = btn.x + (btn.width  - textWidth) / 2;
    int y = btn.y + (btn.height - fontSize) / 2;

    DrawText(text, x, y, fontSize, color);
}