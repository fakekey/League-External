#pragma once
#include "windows.h"
#include <string>

enum BuffType {
    Internal = 0,
    Aura = 1,
    CombatEnchancer = 2,
    CombatDehancer = 3,
    SpellShield = 4,
    Stun = 5,
    Invisibility = 6,
    Silence = 7,
    Taunt = 8,
    Berserk = 9,
    Polymorph = 10,
    Slow = 11,
    Snare = 12,
    Damage = 13,
    Heal = 14,
    Haste = 15,
    SpellImmunity = 16,
    PhysicalImmunity = 17,
    Invulnerability = 18,
    AttackSpeedSlow = 19,
    NearSight = 20,
    Fear = 22,
    Charm = 23,
    Poison = 24,
    Suppression = 25,
    Blind = 26,
    Counter = 27,
    Currency = 21,
    Shred = 28,
    Flee = 29,
    Knockup = 30,
    Knockback = 31,
    Disarm = 32,
    Grounded = 33,
    Drowsy = 34,
    Asleep = 35,
    Obscured = 36,
    ClickProofToEnemies = 37,
    Unkillable = 38
};

struct BuffInfo {
public:
    BuffInfo() {};
    BuffInfo(std::string name, UINT8 count, UINT8 countAlt, UINT8 countAlt2, UINT8 type, float startTime, float endTime, bool isAlive)
    {
        this->name = name;
        this->count = count;
        this->countAlt = countAlt;
        this->countAlt2 = countAlt2;
        this->type = type;
        this->startTime = startTime;
        this->endTime = endTime;
        this->isAlive = isAlive;
    }
    std::string name;
    int count = 0;
    int countAlt = 0;
    int countAlt2 = 0;
    int type = -1;
    float startTime = 0.f;
    float endTime = 0.f;
    bool isAlive = false;
};