#include "Spell.h"
#include "GameData.h"
#include "Offsets.h"
#include "Utils.h"

BYTE Spell::buffer[0x150];
const char* Spell::spellTypeName[6] = { "Q", "W", "E", "R", "D", "F" };
const HKey Spell::spellSlotKey[6] = { HKey::Q, HKey::W, HKey::E, HKey::R, HKey::D, HKey::F };

std::map<std::string, SummonerSpellType> Spell::summonerSpellTypeDict = {
    { std::string("summonerhaste"), SummonerSpellType::GHOST },
    { std::string("summonerheal"), SummonerSpellType::HEAL },
    { std::string("summonerbarrier"), SummonerSpellType::BARRIER },
    { std::string("summonerexhaust"), SummonerSpellType::EXHAUST },
    { std::string("summonermana"), SummonerSpellType::CLARITY },
    { std::string("summonermark"), SummonerSpellType::SNOWBALL },
    { std::string("summonerflash"), SummonerSpellType::FLASH },
    { std::string("summonerteleport"), SummonerSpellType::TELEPORT },
    { std::string("s12_summonerteleport"), SummonerSpellType::TELEPORT },
    { std::string("s12_summonerteleportupgrade"), SummonerSpellType::TELEPORT },
    { std::string("summonerboost"), SummonerSpellType::CLEANSE },
    { std::string("summonerdot"), SummonerSpellType::IGNITE },
    { std::string("summonersmite"), SummonerSpellType::SMITE },
    { std::string("summonerrecall"), SummonerSpellType::RECALL },
    { std::string("s12_summonersmiteplayerganker"), SummonerSpellType::SMITE },
    { std::string("s12_summonersmiteduel"), SummonerSpellType::SMITE },
};

float Spell::GetRemainingCooldown(float gameTime)
{
    return (readyAt > gameTime ? readyAt - gameTime : 0.f);
}

const char* Spell::GetTypeStr()
{
    return spellTypeName[(int)slot];
}

void Spell::MoveAndTrigger(const Vector2& pos)
{
    return Input::MoveAndPress(spellSlotKey[(int)slot], pos.x, pos.y);
}

void Spell::Trigger(bool charge)
{
    if (charge == true) {
        return Input::KeyDown(spellSlotKey[(int)slot]);
    } else {
        return Input::PressKey(spellSlotKey[(int)slot]);
    }
}

void Spell::MoveAndRelease(const Vector2& pos)
{
    return Input::MoveAndRelease(spellSlotKey[(int)slot], pos.x, pos.y);
}

void Spell::Cast(const Vector2& fromPos, const Vector2& toPos)
{
    return Input::MoveWhilePress(spellSlotKey[(int)slot], fromPos.x, fromPos.y, toPos.x, toPos.y);
}

bool Spell::IsReady(float gameTime)
{
    return level > 0 && GetRemainingCooldown(gameTime) == 0.f;
}

void Spell::LoadFromMem(DWORD64 base, HANDLE hProcess, bool deepLoad)
{
    Mem::Read(hProcess, base, buffer, 0x150);

    memcpy(&readyAt, buffer + Offsets::SpellSlotTime, sizeof(float));
    memcpy(&level, buffer + Offsets::SpellSlotLevel, sizeof(int));

    DWORD64 spellInfoPtr;
    memcpy(&spellInfoPtr, buffer + Offsets::SpellSlotSpellInfo, sizeof(DWORD64));

    DWORD64 spellDataPtr = Mem::ReadDWORD(hProcess, spellInfoPtr + Offsets::SpellInfoSpellData);
    DWORD64 spellNamePtr = Mem::ReadDWORD(hProcess, spellDataPtr + Offsets::SpellDataSpellName);

    char buff[50];
    Mem::Read(hProcess, spellNamePtr, buff, 50);
    if (Character::ContainsOnlyASCII(buff, 50)) {
        name = Character::ToLower(std::string(buff));
    } else {
        name = std::string("");
    }

    auto it = summonerSpellTypeDict.find(name.c_str());
    if (it != summonerSpellTypeDict.end())
        summonerSpellType = it->second;

    info = GameData::GetSpellInfoByName(name);
}