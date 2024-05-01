#pragma once
#include "GameData.h"
#include "MemSnapShot.h"
#include "imgui.h"
#include <boost/python.hpp>
#include <map>

using namespace boost::python;

/// Interface used by python scripts for game related stuff
class PyGame {
public:
    MemSnapShot* ms;
    ImDrawList* overlay;

public:
    PyGame() { }

    // Exposed Fields
    boost::python::list champs, minions, jungles, missiles, others;
    float gameTime;
    bool isChatOpen;
    int ping;

    GameObject* localChampion;

    object GetLocalChampion()
    {
        if (localChampion == nullptr) {
            return object();
        }
        return object(boost::ref(*localChampion));
    };

    //Exposed methods
    Vector2 WorldToScreen(const Vector3& pos)
    {
        return ms->renderer->WorldToScreen(pos);
    }

    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(IsScreenPointOnScreenOverloads, IsScreenPointOnScreen, 1, 3);
    bool IsScreenPointOnScreen(const Vector2& point, float offsetX = 0.f, float offsetY = 0.f)
    {
        return ms->renderer->IsScreenPointOnScreen(point, offsetX, offsetY);
    }

    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(IsWorldPointOnScreenOverloads, IsWorldPointOnScreen, 1, 3);
    bool IsWorldPointOnScreen(const Vector3& point, float offsetX = 0.f, float offsetY = 0.f)
    {
        return ms->renderer->IsWorldPointOnScreen(point, offsetX, offsetY);
    }

    void DrawCircle(const Vector2& center, float radius, int numPoints, float thickness, const ImVec4& color)
    {
        overlay->AddCircle(ImVec2(center.x, center.y), radius, ImColor(color), numPoints, thickness);
    }

    void DrawCircleFilled(const Vector2& center, float radius, int numPoints, const ImVec4& color)
    {
        overlay->AddCircleFilled(ImVec2(center.x, center.y), radius, ImColor(color), numPoints);
    }

    void DrawTxt(const Vector2& pos, const char* text, const ImVec4& color)
    {
        overlay->AddText(ImVec2(pos.x, pos.y), ImColor(color), text);
    }

    void MoveCursor(const Vector2& pos)
    {
        Input::Move(pos.x, pos.y);
    }

    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(DrawRectOverloads, DrawRect, 2, 4);
    void DrawRect(const Vector4& box, const ImVec4& color, float rounding = 0, float thickness = 1.0)
    {
        overlay->AddRect(ImVec2(box.x, box.y), ImVec2(box.z, box.w), ImColor(color), rounding, 15, thickness);
    }

    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(DrawRectFilledOverloads, DrawRectFilled, 2, 3);
    void DrawRectFilled(const Vector4& box, const ImVec4& color, float rounding = 0)
    {
        overlay->AddRectFilled(ImVec2(box.x, box.y), ImVec2(box.z, box.w), ImColor(color), rounding);
    }

    void DrawRectWorld(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& p4, float thickness, const ImVec4& color)
    {
        static Vector2 points[4];
        points[0] = ms->renderer->WorldToScreen(p1);
        points[1] = ms->renderer->WorldToScreen(p2);
        points[2] = ms->renderer->WorldToScreen(p3);
        points[3] = ms->renderer->WorldToScreen(p4);

        overlay->AddPolyline((ImVec2*)points, 4, ImColor(color), true, thickness);
    }

    void DrawRectFromPos(const Vector3& startPos, const Vector3& endPos, float radius, float thickness, const ImVec4& color)
    {
        Vector3 dir = Vector3(endPos.x - startPos.x, 0.f, endPos.z - startPos.z).normalize();
        if (std::isnan(dir.x)) {
            return;
        }
        Vector3 leftDir = dir.rotate_y(90.f).scale(radius);
        Vector3 rightDir = dir.rotate_y(-90.f).scale(radius);
        Vector3 p1 = Vector3(startPos.x + leftDir.x, startPos.y + leftDir.y, startPos.z + leftDir.z);
        Vector3 p2 = Vector3(endPos.x + leftDir.x, endPos.y + leftDir.y, endPos.z + leftDir.z);
        Vector3 p3 = Vector3(endPos.x + rightDir.x, endPos.y + rightDir.y, endPos.z + rightDir.z);
        Vector3 p4 = Vector3(startPos.x + rightDir.x, startPos.y + rightDir.y, startPos.z + rightDir.z);

        DrawRectWorld(p1, p2, p3, p4, thickness, color);
    }

    void DrawTriangleWorld(const Vector3& p1, const Vector3& p2, const Vector3& p3, float thickness, const ImVec4& color)
    {
        overlay->AddTriangle(
            (ImVec2&)ms->renderer->WorldToScreen(p1),
            (ImVec2&)ms->renderer->WorldToScreen(p2),
            (ImVec2&)ms->renderer->WorldToScreen(p3), ImColor(color), thickness);
    }

    void DrawTriangleWorldFilled(const Vector3& p1, const Vector3& p2, const Vector3& p3, const ImVec4& color)
    {
        overlay->AddTriangleFilled(
            (ImVec2&)ms->renderer->WorldToScreen(p1),
            (ImVec2&)ms->renderer->WorldToScreen(p2),
            (ImVec2&)ms->renderer->WorldToScreen(p3), ImColor(color));
    }

    void DrawCircleWorld(const Vector3& center, float radius, float thickness, const ImVec4& color)
    {
        ms->renderer->DrawCircleAt(overlay, center, radius, false, ImColor(color), thickness);
    }

    void DrawCircleWorldFilled(const Vector3& center, float radius, const ImVec4& color)
    {
        ms->renderer->DrawCircleAt(overlay, center, radius, true, ImColor(color));
    }

    void DrawLine(const Vector2& start, const Vector2& end, float thickness, const ImVec4& color)
    {
        overlay->AddLine((const ImVec2&)start, (const ImVec2&)end, ImColor(color), thickness);
    }

    void DrawLineWorld(const Vector3& start, const Vector3& end, float thickness, const ImVec4& color)
    {
        overlay->AddLine(
            (const ImVec2&)ms->renderer->WorldToScreen(start),
            (const ImVec2&)ms->renderer->WorldToScreen(end), ImColor(color), thickness);
    }

    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(DrawButtonOverloads, DrawButton, 4, 5);
    void DrawButton(const Vector2& p, const char* text, ImVec4& colorButton, ImVec4& colorText, float rounding = 0)
    {
        int txtSize = strlen(text);
        overlay->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + txtSize * 7.2f + 5, p.y + 17), ImColor(colorButton), rounding);
        overlay->AddText(ImVec2(p.x + 5, p.y + 2), ImColor(colorText), text);
    }

    Vector3 clamp_norm_2d(const Vector3& v, float n_max)
    {
        float vx = v.x, vy = v.y, vz = v.z;
        float n = sqrt(pow(vx, 2.f) + pow(vz, 2.f));
        float f = min(n, n_max) / n;

        return Vector3(f * vx, vy, f * vz);
    }

    bool isLeft(const Vector2& a, const Vector2& b, const Vector2& c)
    {
        return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)) > 0;
    }

    bool PointOnLineSegment(const Vector2& pt1, const Vector2& pt2, const Vector2& pt, double epsilon = 0.001)
    {
        if (pt.x - max(pt1.x, pt2.x) > epsilon || min(pt1.x, pt2.x) - pt.x > epsilon || pt.y - max(pt1.y, pt2.y) > epsilon || min(pt1.y, pt2.y) - pt.y > epsilon)
            return false;

        if (abs(pt2.x - pt1.x) < epsilon)
            return abs(pt1.x - pt.x) < epsilon || abs(pt2.x - pt.x) < epsilon;
        if (abs(pt2.y - pt1.y) < epsilon)
            return abs(pt1.y - pt.y) < epsilon || abs(pt2.y - pt.y) < epsilon;

        double x = pt1.x + (pt.y - pt1.y) * (pt2.x - pt1.x) / (pt2.y - pt1.y);
        double y = pt1.y + (pt.x - pt1.x) * (pt2.y - pt1.y) / (pt2.x - pt1.x);

        return abs(pt.x - x) < epsilon || abs(pt.y - y) < epsilon;
    }

    bool isPointOnLineSegment(const Vector2& point, const Vector2& start, const Vector2& end)
    {
        if (max(start.x, end.x) > point.x && point.x > min(start.x, end.x)
            && max(start.y, end.y) > point.y && point.y > min(start.y, end.y)) {
            return true;
        }

        return false;
    }

    int compareTo(int a, int b)
    {
        if (a < b)
            return -1;
        else if (a == b)
            return 0;
        else
            return 1;
    }

    void PressKey(int key)
    {
        return Input::PressKey((HKey)key);
    }

    void KeyDown(int key)
    {
        return Input::KeyDown((HKey)key);
    }

    void KeyUp(int key)
    {
        return Input::KeyUp((HKey)key);
    }

    bool WasKeyPressed(int key)
    {
        return Input::WasKeyPressed((HKey)key);
    }

    void PressLeftClick()
    {
        return Input::PressLeftClick();
    }

    void PressRightClick()
    {
        return Input::PressRightClick();
    }

    void MouseRightDown()
    {
        return Input::MouseRightDown();
    }

    void MouseRightUp()
    {
        return Input::MouseRightUp();
    }

    void MouseLeftDown()
    {
        return Input::MouseLeftDown();
    }

    void MouseLeftUp()
    {
        return Input::MouseLeftUp();
    }

    void ClickAt(bool leftClick, const Vector2& pos)
    {
        return Input::ClickAt(leftClick, pos.x, pos.y);
    }

    bool IsKeyDown(int key)
    {
        return Input::IsKeyDown((HKey)key);
    }

    Vector2 GetCursor()
    {
        return Input::GetCursorPosition();
    }

    SpellInfo* GetSpellInfo(const char* spellName)
    {
        std::string name(spellName);
        return GameData::GetSpellInfoByName(name);
    }

    GameObject* GetBestTarget(const std::string& unitType, float range)
    {
        GameObject* champion = nullptr;
        float lastHealth = FLT_MAX;

        if (!range || range == 0) {
            range = localChampion->attackRange + localChampion->info->gameplayRadius;
        }
        for (auto& champ :
            (unitType == "Unit_Champion"             ? ms->champions
                    : unitType == "Unit_Minion_Lane" ? ms->minions
                    : unitType == "Unit_Monster"     ? ms->jungles
                                                     : ms->champions)) {
            if (champ->HasUnitTags("Unit_Champion_Clone")) {
                continue;
            }
            if (champ->isAlive && champ->visible && champ->targetable && champ->IsEnemyTo(*localChampion) && lastHealth >= champ->health && champ->position.distance(localChampion->position) <= range + (champ->info->gameplayRadius / 2)) {
                lastHealth = champ->health;
                champion = champ.get();
            }
        };
        return champion;
    }

    GameObject* GetObjectByIndex(short index)
    {
        auto it = ms->indexToNetId.find(index);
        if (it == ms->indexToNetId.end())
            return nullptr;

        auto it2 = ms->objectMap.find(it->second);
        if (it2 == ms->objectMap.end())
            return nullptr;

        return it2->second.get();
    }

    GameObject* GetObjectByNetId(int net_id)
    {
        auto it = ms->objectMap.find(net_id);
        return (it != ms->objectMap.end()) ? it->second.get() : nullptr;
    }

    static PyGame ConstructFromMemSnapShot(MemSnapShot& snapshot)
    {
        PyGame gs;

        gs.ms = &snapshot;
        gs.gameTime = snapshot.gameTime;
        gs.isChatOpen = snapshot.isChatOpen;
        gs.ping = snapshot.ping;
        gs.localChampion = snapshot.player.get();

        for (auto it = snapshot.champions.begin(); it != snapshot.champions.end(); ++it) {
            if ((*it).get() != nullptr) {
                gs.champs.append(boost::ref(**it));
            }
        }
        for (auto it = snapshot.minions.begin(); it != snapshot.minions.end(); ++it) {
            if ((*it).get() != nullptr) {
                gs.minions.append(boost::ref(**it));
            }
        }
        for (auto it = snapshot.jungles.begin(); it != snapshot.jungles.end(); ++it) {
            if ((*it).get() != nullptr) {
                gs.jungles.append(boost::ref(**it));
            }
        }
        for (auto it = snapshot.missiles.begin(); it != snapshot.missiles.end(); ++it) {
            if ((*it).get() != nullptr) {
                gs.missiles.append(boost::ref(**it));
            }
        }
        for (auto it = snapshot.others.begin(); it != snapshot.others.end(); ++it) {
            if ((*it).get() != nullptr) {
                gs.others.append(boost::ref(**it));
            }
        }
        return gs;
    }
};