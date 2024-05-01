#include "GameData.h"
#include "Utils.h"
#include <boost/json.hpp>
#include <filesystem>
#include <fstream>

std::map<int, ItemInfo*> GameData::Items = {};
std::map<std::string, SpellInfo*> GameData::Spells = {};
std::map<std::string, SpellInfo*> GameData::SpellAlts = {};
std::map<std::string, UnitInfo*> GameData::Units = {};

/// Area
/// AreaClamped
/// Cone
/// Direction
/// DragDirection
/// Location
/// LocationClamped
/// Self
/// SelfAoe
/// Target
/// TargetOrLocation

ItemInfo* GameData::GetItemInfoById(int id)
{
    auto it = Items.find(id);
    if (it != Items.end()) {
        return it->second;
    }
    return nullptr;
}

SpellInfo* GameData::GetSpellInfoByName(std::string& name)
{
    auto it = Spells.find(name);
    if (it != Spells.end()) {
        return it->second;
    }
    it = SpellAlts.find(name);
    if (it != SpellAlts.end()) {
        return it->second;
    }
    return nullptr;
}

UnitInfo* GameData::GetUnitInfoByName(std::string& name)
{
    auto it = Units.find(name);
    if (it != Units.end()) {
        return it->second;
    }
    return nullptr;
}

boost::json::value parse_json_file(std::string& path)
{
    std::ifstream inputData;
    inputData.open(path);

    if (!inputData.is_open())
        throw std::runtime_error("Can't open unit data file");

    boost::json::stream_parser p;
    boost::system::error_code ec;
    do {
        char buf[4096];
        inputData.read(buf, sizeof(buf));
        p.write(buf, inputData.gcount(), ec);
    } while (!inputData.eof());
    if (ec)
        throw std::runtime_error("Failed to parse JSON file");
    p.finish(ec);
    if (ec)
        throw std::runtime_error("Failed to parse JSON file");

    boost::json::value jv = p.release();
    return jv;
}

double json_to_double(boost::json::value val)
{
    if (val.is_int64()) {
        return val.as_int64();
    } else {
        return val.as_double();
    }
}

void GameData::Load(std::string& dataFolder)
{
    std::string unitData = dataFolder + "/UnitData.json";
    std::string spellData = dataFolder + "/SpellData.json";
    std::string itemData = dataFolder + "/ItemData.json";
    LoadItemData(itemData);
    LoadSpellData(spellData);
    LoadUnitData(unitData);
}

void GameData::LoadItemData(std::string& path)
{
    boost::json::value jv = parse_json_file(path);
    auto& items = jv.as_array();
    for (auto& itemObj : items) {
        auto& item = itemObj.as_object();
        ItemInfo* info = new ItemInfo();

        info->id = item["id"].as_int64();
        info->name = std::string(item["name"].as_string().c_str());
        info->cost = (double)json_to_double(item["cost"]);
        info->movementSpeed = (double)json_to_double(item["movementSpeed"]);
        info->health = (double)json_to_double(item["health"]);
        info->crit = (double)json_to_double(item["crit"]);
        info->abilityPower = (double)json_to_double(item["abilityPower"]);
        info->mana = (double)json_to_double(item["mana"]);
        info->armour = (double)json_to_double(item["armour"]);
        info->magicResist = (double)json_to_double(item["magicResist"]);
        info->physicalDamage = (double)json_to_double(item["physicalDamage"]);
        info->attackSpeed = (double)json_to_double(item["attackSpeed"]);
        info->lifeSteal = (double)json_to_double(item["lifeSteal"]);
        info->hpRegen = (double)json_to_double(item["hpRegen"]);
        info->movementSpeedPercent = (double)json_to_double(item["movementSpeedPercent"]);

        Items[info->id] = info;
    }
}

void GameData::LoadSpellData(std::string& path)
{
    boost::json::value jv = parse_json_file(path);
    auto& spells = jv.as_array();

    for (auto& spell : spells) {
        auto& spellObj = spell.as_object();
        SpellInfo* info = new SpellInfo();

        info->name = Character::ToLower(std::string(spellObj["name"].as_string().c_str()));
        info->nameAlt = Character::ToLower(std::string(spellObj["nameAlt"].as_string().c_str()));
        info->icon = Character::ToLower(std::string(spellObj["icon"].as_string().c_str()));
        info->flags = spellObj["flags"].as_int64();
        info->delay = (double)json_to_double(spellObj["delay"]);
        info->castRange = (double)json_to_double(spellObj["castRange"]);
        info->castRadius = (double)json_to_double(spellObj["castRadius"]);
        info->width = (double)json_to_double(spellObj["width"]);
        info->height = (double)json_to_double(spellObj["height"]);
        info->speed = (double)json_to_double(spellObj["speed"]);
        info->travelTime = (double)json_to_double(spellObj["travelTime"]);
        info->type = std::string(spellObj["type"].as_string().c_str());
        info->projectDestination = spellObj["projectDestination"].as_bool();

        Spells[info->name] = info;
        SpellAlts[info->nameAlt] = info;
    }
}

void GameData::LoadUnitData(std::string& path)
{
    boost::json::value jv = parse_json_file(path);
    auto& units = jv.get_array();
    for (auto& unit : units) {
        auto& unitObj = unit.get_object();
        UnitInfo* unit = new UnitInfo();

        unit->name = Character::ToLower(std::string(unitObj["name"].as_string().c_str()));
        unit->healthBarHeight = (double)json_to_double(unitObj["healthBarHeight"]);
        unit->baseMoveSpeed = (double)json_to_double(unitObj["baseMoveSpeed"]);
        unit->attackRange = (double)json_to_double(unitObj["attackRange"]);
        unit->attackSpeed = (double)json_to_double(unitObj["attackSpeed"]);
        unit->attackSpeedRatio = (double)json_to_double(unitObj["attackSpeedRatio"]);
        unit->acquisitionRange = (double)json_to_double(unitObj["acquisitionRange"]);
        unit->selectionRadius = (double)json_to_double(unitObj["selectionRadius"]);
        unit->pathingRadius = (double)json_to_double(unitObj["pathingRadius"]);
        unit->gameplayRadius = (double)json_to_double(unitObj["gameplayRadius"]);
        unit->basicAtkMissileSpeed = (double)json_to_double(unitObj["basicAtkMissileSpeed"]);
        unit->basicAtkWindup = (double)json_to_double(unitObj["basicAtkWindup"]);
        unit->basicAtkWindupModify = (double)json_to_double(unitObj["basicAtkWindupModify"]);

        auto& tags = unitObj["tags"].as_array();
        for (auto& tag : tags) {
            unit->SetTag(tag.as_string().c_str());
        }

        Units[unit->name] = unit;
    }
}