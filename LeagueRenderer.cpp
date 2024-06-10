#include "LeagueRenderer.h"
#include "Offsets.h"
#include "Utils.h"

void LeagueRenderer::LoadFromMem(DWORD_PTR moduleBase)
{
    char buff[128];

    Mem::Read(moduleBase + Offsets::ViewProjMatrix, buff, 128);
    memcpy(viewMatrix, buff, 16 * sizeof(float));
    memcpy(projMatrix, &buff[16 * sizeof(float)], 16 * sizeof(float));
    MultiplyMatrices(viewProjMatrix, viewMatrix, 4, 4, projMatrix, 4, 4);
}

void LeagueRenderer::MultiplyMatrices(float* out, float* a, int row1, int col1, float* b, int row2, int col2)
{
    int size = row1 * col2;
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < col2; j++) {
            float sum = 0.f;
            for (int k = 0; k < col1; k++) {
                sum = sum + a[i * col1 + k] * b[k * col2 + j];
            }
            out[i * col2 + j] = sum;
        }
    }
}

Vector2 LeagueRenderer::WorldToScreen(const Vector3& pos) const
{

    Vector2 out = { 0.f, 0.f };
    Vector2 screen = { (float)width, (float)height };
    static Vector4 clipCoords;
    clipCoords.x = pos.x * viewProjMatrix[0] + pos.y * viewProjMatrix[4] + pos.z * viewProjMatrix[8] + viewProjMatrix[12];
    clipCoords.y = pos.x * viewProjMatrix[1] + pos.y * viewProjMatrix[5] + pos.z * viewProjMatrix[9] + viewProjMatrix[13];
    clipCoords.z = pos.x * viewProjMatrix[2] + pos.y * viewProjMatrix[6] + pos.z * viewProjMatrix[10] + viewProjMatrix[14];
    clipCoords.w = pos.x * viewProjMatrix[3] + pos.y * viewProjMatrix[7] + pos.z * viewProjMatrix[11] + viewProjMatrix[15];

    if (clipCoords.w < 1.0f)
        clipCoords.w = 1.f;

    Vector2 M;
    M.x = clipCoords.x / clipCoords.w;
    M.y = clipCoords.y / clipCoords.w;

    out.x = (screen.x / 2.f * M.x) + (M.x + screen.x / 2.f);
    out.y = -(screen.y / 2.f * M.y) + (M.y + screen.y / 2.f);

    return out;
}

bool LeagueRenderer::IsScreenPointOnScreen(const Vector2& point, float offsetX, float offsetY) const
{
    return point.x > -offsetX && point.x < (width + offsetX) && point.y > -offsetY && point.y < (height + offsetY);
}

bool LeagueRenderer::IsWorldPointOnScreen(const Vector3& point, float offsetX, float offsetY) const
{
    return IsScreenPointOnScreen(WorldToScreen(point), offsetX, offsetY);
}

float LeagueRenderer::sinValues[100] = {};
float LeagueRenderer::cosValues[100] = {};

void LeagueRenderer::InitRenderer()
{
    float step = 6.2831f / 100;
    float theta = 0.f;
    for (int i = 0; i < 100; i++, theta += step) {
        sinValues[i] = sin(theta);
        cosValues[i] = cos(theta);
    }
}

void LeagueRenderer::DrawCircleAt(ImDrawList* canvas, const Vector3& worldPos, float radius, bool filled, ImColor color, float thickness) const
{
    static ImVec2 points[100];

    for (int i = 0; i < 100; i++) {
        Vector3 worldSpace = { worldPos.x + radius * cosValues[i], worldPos.y, worldPos.z - radius * sinValues[i] };
        Vector2 screenSpace = WorldToScreen(worldSpace);

        points[i].x = screenSpace.x;
        points[i].y = screenSpace.y;
    }

    if (filled) {
        canvas->AddConvexPolyFilled(points, 100, color);
    } else {
        canvas->AddPolyline(points, 100, color, true, thickness);
    }
}