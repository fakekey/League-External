#pragma once
#include "ItemInfo.h"
#include "SpellInfo.h"
#include "UnitInfo.h"
#include <map>

class GameData {
public:
    static void Load(std::string& dataFolder);
    static ItemInfo* GetItemInfoById(int id);
    static SpellInfo* GetSpellInfoByName(std::string& name);
    static UnitInfo* GetUnitInfoByName(std::string& name);

private:
    static void LoadItemData(std::string& path);
    static void LoadSpellData(std::string& path);
    static void LoadUnitData(std::string& path);

public:
    static std::map<int, ItemInfo*> Items;
    static std::map<std::string, SpellInfo*> Spells;
    static std::map<std::string, SpellInfo*> SpellAlts;
    static std::map<std::string, UnitInfo*> Units;
};