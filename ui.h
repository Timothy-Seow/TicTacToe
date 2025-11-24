#ifndef UI_H
#define UI_H

#include "raylib.h"

void InitUI();  // call once to load colors, settings
void DrawButton(Rectangle btn, bool hovered);
void DrawCenteredTextInButton(Rectangle btn, const char* text, int fontSize, Color color);

#endif