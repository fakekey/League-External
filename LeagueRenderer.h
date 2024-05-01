#pragma once
#include "Vector.h"
#include "imgui.h"
#include "windows.h"
#include <numbers>

class LeagueRenderer {
public:
    int width = 1920;
    int height = 1080;

    float viewMatrix[16];
    float projMatrix[16];
    float viewProjMatrix[16];

    void LoadFromMem(DWORD_PTR moduleBase, HANDLE hProcess);

    /// Converts world coordinates to screen coordinates
    Vector2 WorldToScreen(const Vector3& pos) const;

    /// Draws a circle at the given coordinate. Coordinates and radius must be in world space
    void DrawCircleAt(ImDrawList* canvas, const Vector3& worldPos, float radius, bool filled, ImColor color, float thickness = 3.f) const;

    /// Used to determine if a screen space point is on screen
    bool IsScreenPointOnScreen(const Vector2& point, float offsetX = 0.f, float offsetY = 0.f) const;

    /// Used to determine if a world space point is on screen
    bool IsWorldPointOnScreen(const Vector3& point, float offsetX = 0.f, float offsetY = 0.f) const;

private:
    void MultiplyMatrices(float* out, float* a, int row1, int col1, float* b, int row2, int col2);

public:
    static float sinValues[100];
    static float cosValues[100];
    static void InitRenderer();
};