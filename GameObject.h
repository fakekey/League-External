#pragma once
#include "AIManagerInfo.h"
#include "BuffInfo.h"
#include "ItemSlot.h"
#include "MemoryLoadable.h"
#include "Missile.h"
#include "Spell.h"
#include "UnitInfo.h"
#include "Vector.h"
#include <map>
#include <string>

class GameObject : MemoryLoadable {
public:
    void LoadFromMem(DWORD64 base, bool deepLoad = true);
    bool IsRanged();
    bool IsAllyTo(const GameObject& other) const;
    bool IsEnemyTo(const GameObject& other) const;
    bool IsHurtable();
    bool HasUnitTags(const std::string& tag) const;
    bool IsEqualTo(const GameObject& other) const;
    bool IsNotEqualTo(const GameObject& other) const;
    double GetCastDelay(double attackSpeed);
    double GetAttackSpeed();
    void LoadChampFromMem(DWORD64 base);
    void LoadBuffFromMem(DWORD64 base);
    void LoadAIMangerFromMem(DWORD64 base);
    void LoadActiveCast(DWORD64 base);
    BuffInfo* GetBuffByName(std::string buffName);

public:
    short objIndex;
    std::string name;
    DWORD32 networkId;
    short team;
    float health;
    float maxHealth;
    Vector3 position;
    bool visible;
    bool targetable;
    bool isAlive;
    float armor;
    float magicRes;
    UnitInfo* info = nullptr;
    DWORD64 address;

    /// players
    short level;
    float attackRange;
    float basicAtk;
    float bonusAtkSpeed;
    float bonusAtk;
    float abilityPower;
    float lethality;
    float armorPen;
    float magicPen;
    float magicResPen;
    ItemSlot itemSlots[7];
    Spell Q = Spell(SpellSlot::Q);
    Spell W = Spell(SpellSlot::W);
    Spell E = Spell(SpellSlot::E);
    Spell R = Spell(SpellSlot::R);
    Spell D = Spell(SpellSlot::D);
    Spell F = Spell(SpellSlot::F);
    std::vector<BuffInfo> buffs;
    Missile activeCast;
    __int64 AIManagerAddress;
    AIManagerInfo AIManager;

private:
    static DWORD64 spellSlotPointerBuffer[7];
    static BYTE buffListBuffer[0x90];

protected:
    static const SIZE_T sizeBuff = 0x5100;
    static BYTE buff[sizeBuff];

public:
    static const char* ZOMBIES[3];
    static float gameTime;

    boost::python::list GetBuffs()
    {
        boost::python::list l;
        for (auto& cbuff : buffs) {
            l.append(boost::ref(cbuff));
        }
        return l;
    }

    boost::python::list GetItemSlots()
    {
        boost::python::list l;
        for (int i = 0; i < 7; ++i) {
            if (!itemSlots[i].isEmpty) {
                l.append(boost::ref(itemSlots[i]));
            }
        }
        return l;
    }

    boost::python::object GetUnitInfo()
    {
        if (info == nullptr) {
            return boost::python::object();
        }
        return boost::python::object(boost::ref(*info));
    }
};