#pragma once
#include "Input.h"
#include "MemoryLoadable.h"
#include "SpellInfo.h"
#include "Vector.h"
#include <boost/python.hpp>
#include <map>
#include <string>

enum class SpellSlot {
    Q = 0,
    W,
    E,
    R,
    D,
    F
};

enum SummonerSpellType {
    NONE,
    GHOST,
    HEAL,
    BARRIER,
    EXHAUST,
    CLARITY,
    SNOWBALL,
    FLASH,
    TELEPORT,
    CLEANSE,
    IGNITE,
    SMITE,
    RECALL,
    WARD
};

class Spell : MemoryLoadable {
public:
    Spell(SpellSlot slot)
    {
        this->name = "";
        this->slot = slot;
        this->level = 0;
        this->readyAt = 0.f;
        this->info = nullptr;
    }

    float GetRemainingCooldown(float gameTime);
    const char* GetTypeStr();
    void LoadFromMem(DWORD64 base, HANDLE hProcess, bool deepLoad = true);
    void MoveAndTrigger(const Vector2& pos);
    void MoveAndRelease(const Vector2& pos);
    void Cast(const Vector2& fromPos, const Vector2& toPos);
    void Trigger(bool charge);
    bool IsReady(float gameTime);

public:
    std::string name;
    SpellSlot slot;
    int level;
    float readyAt;
    SpellInfo* info;
    SummonerSpellType summonerSpellType;

    boost::python::object GetSpellInfo()
    {
        if (info == nullptr) {
            return boost::python::object();
        }
        return boost::python::object(boost::ref(*info));
    }

private:
    static BYTE buffer[0x150];
    static const char* spellTypeName[6];
    static const HKey spellSlotKey[6];
    static std::map<std::string, SummonerSpellType> summonerSpellTypeDict;
};