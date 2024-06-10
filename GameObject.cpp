#include "GameObject.h"
#include "GameData.h"
#include "Offsets.h"
#include "Utils.h"

BYTE GameObject::buff[GameObject::sizeBuff] = {};
BYTE GameObject::buffListBuffer[0x90] = {};
const char* GameObject::ZOMBIES[3] = { "Sion", "KogMaw", "Karthus" };
DWORD64 GameObject::spellSlotPointerBuffer[7] = {};
float GameObject::gameTime = 0.f;

bool IsZombie(const std::string& name)
{
    for (const char* zombie : GameObject::ZOMBIES) {
        if (name == zombie) {
            return true;
        }
    }
    return false;
}

BuffInfo* GameObject::GetBuffByName(std::string buffName)
{
    float buffEndTime = 0.f;
    BuffInfo* rightBuff = nullptr;
    for (auto cbuff = buffs.begin(); cbuff != buffs.end(); ++cbuff) {
        if (cbuff->name.compare(buffName) == 0 && cbuff->endTime > buffEndTime) {
            buffEndTime = cbuff->endTime;
            rightBuff = cbuff._Ptr;
        }
    }
    return rightBuff;
}

bool GameObject::IsRanged()
{
    return info->attackRange >= 300.f;
}

bool GameObject::IsEnemyTo(const GameObject& other) const
{
    return this->team != other.team;
}

bool GameObject::IsAllyTo(const GameObject& other) const
{
    return this->team == other.team;
}

bool GameObject::IsHurtable()
{
    return visible && isAlive && targetable;
}

bool GameObject::HasUnitTags(const std::string& tag) const
{
    if (info == nullptr) {
        return false;
    }
    return std::find(info->tags.begin(), info->tags.end(), tag) != info->tags.end();
}

bool GameObject::IsEqualTo(const GameObject& other) const
{
    return this->objIndex == other.objIndex;
}

bool GameObject::IsNotEqualTo(const GameObject& other) const
{
    return this->objIndex != other.objIndex;
}

double GameObject::GetCastDelay(double totalAttackSpeed)
{
    auto windup_mod = info->basicAtkWindupModify;
    auto windup_percent = info->basicAtkWindup;
    auto base_attack_speed = info->attackSpeed;
    if (windup_mod) {
        return (1.0 / base_attack_speed) * windup_percent + ((1.0 / totalAttackSpeed * windup_percent) - (1.0 / base_attack_speed) * windup_percent) * windup_mod;
    }
    return (1.0 / base_attack_speed) * windup_percent + ((1.0 / totalAttackSpeed * windup_percent) - (1.0 / base_attack_speed) * windup_percent);
}

double GameObject::GetAttackSpeed()
{
    auto bonusAS = (bonusAtkSpeed * 100) / 100.0;
    // For champions that its base AA is not the same as the Ratio, like for ex: Senna
    if (info->attackSpeedRatio != info->attackSpeed) {
        return std::round((info->attackSpeed + (bonusAS * info->attackSpeedRatio)) * 1000.0) / 1000.0;
    }
    // check if jinx activated Q, then we calculate it's 10% reduction (yes jinx has -10% when Q activated) (you should do the maths here for other champs, if needed)
    if (name.compare("jinx") == 0) {
        bool isJinxQ = false;
        for (auto cbuff : buffs) {
            if (cbuff.name.compare("JinxQ") != 0) {
                continue;
            }
            if (cbuff.countAlt > 0) {
                isJinxQ = true;
                break;
            }
        }
        if (isJinxQ) {
            auto jinxQReduction = info->attackSpeed * (1.0 + bonusAS) / 10;
            return std::round((info->attackSpeed * (1.0 + bonusAS) - jinxQReduction) * 1000.0) / 1000.0;
        }
    }
    //the champ AA base is equal to ratio, so we simply return it's aaspeed
    return info->attackSpeed * (1.0 + bonusAS);
}

void GameObject::LoadFromMem(DWORD64 base, bool deepLoad)
{
    address = base;
    Mem::Read(base, buff, sizeBuff);

    memcpy(&objIndex, &buff[Offsets::ObjIndex], sizeof(short));
    memcpy(&networkId, &buff[Offsets::ObjNetworkID], sizeof(DWORD32));
    memcpy(&team, &buff[Offsets::ObjTeam], sizeof(short));
    memcpy(&health, &buff[Offsets::ObjHealth], sizeof(float));
    memcpy(&maxHealth, &buff[Offsets::ObjMaxHealth], sizeof(float));
    memcpy(&position, &buff[Offsets::ObjPos], sizeof(Vector3));
    memcpy(&visible, &buff[Offsets::ObjVisible], sizeof(bool));
    memcpy(&targetable, &buff[Offsets::ObjTargetable], sizeof(bool));

    if (deepLoad) {
        char nameBuff[50];
        memset(nameBuff, 0, sizeof(nameBuff));
        Mem::Read(Mem::ReadDWORDFromBuffer(buff, Offsets::ObjName), nameBuff, 50);

        name = std::string("");
        if (Character::ContainsOnlyASCII(nameBuff, 50)) {
            name = Character::ToLower(std::string(nameBuff));
        } else {
            name = std::string("");
        }

        if (name == "") {
            char nameBuff2[50];
            memset(nameBuff2, 0, sizeof(nameBuff2));
            Mem::Read(address + Offsets::ObjName, nameBuff2, 50);

            if (Character::ContainsOnlyASCII(nameBuff2, 50)) {
                name = Character::ToLower(std::string(nameBuff2));
            } else {
                name = std::string("");
            }
        }
        info = GameData::GetUnitInfoByName(name);
    }

    // Check if alive
    DWORD32 spawnCount;
    memcpy(&spawnCount, &buff[Offsets::ObjSpawnCount], sizeof(int));
    isAlive = IsZombie(name) ? health > 0 : spawnCount % 2 == 0;

    memcpy(&armor, &buff[Offsets::ObjArmor], sizeof(float));
    memcpy(&magicRes, &buff[Offsets::ObjMagicRes], sizeof(float));
    memcpy(&attackRange, &buff[Offsets::ObjAttackRange], sizeof(float));

    if (HasUnitTags("Unit_Champion")) {
        LoadChampFromMem(base);
        LoadBuffFromMem(base);
        LoadAIMangerFromMem(base);
        LoadActiveCast(base);
    }
}

void GameObject::LoadChampFromMem(DWORD64 base)
{
    memcpy(&spellSlotPointerBuffer, &buff[Offsets::ObjSpellBook], sizeof(DWORD64) * 6);

    Q.LoadFromMem(spellSlotPointerBuffer[0]);
    W.LoadFromMem(spellSlotPointerBuffer[1]);
    E.LoadFromMem(spellSlotPointerBuffer[2]);
    R.LoadFromMem(spellSlotPointerBuffer[3]);
    D.LoadFromMem(spellSlotPointerBuffer[4]);
    F.LoadFromMem(spellSlotPointerBuffer[5]);

    for (int i = 0; i < 7; ++i) {
        itemSlots[i].isEmpty = true;
        itemSlots[i].slot = i;

        DWORD64 slotPtr = 0, itemPtr = 0, itemInfoPtr = 0;
        memcpy(&slotPtr, &buff[Offsets::ObjItemList + i * 0x8], sizeof(DWORD64));
        if (slotPtr == 0)
            continue;

        itemPtr = Mem::ReadDWORD(slotPtr + 0x10);
        if (itemPtr == 0) {
            continue;
        }

        itemInfoPtr = Mem::ReadDWORD(itemPtr + Offsets::ItemInfo);
        if (itemInfoPtr == 0)
            continue;

        int id = Mem::ReadDWORD(itemInfoPtr + Offsets::ItemInfoId);
        itemSlots[i].info = GameData::GetItemInfoById(id);
        if (itemSlots[i].info != nullptr) {
            itemSlots[i].isEmpty = false;
        }
    }

    memcpy(&level, &buff[Offsets::ObjLevel], sizeof(short));
    memcpy(&basicAtk, &buff[Offsets::ObjBaseAttack], sizeof(float));
    memcpy(&bonusAtkSpeed, &buff[Offsets::ObjBonusAttackSpeed], sizeof(float));
    memcpy(&bonusAtk, &buff[Offsets::ObjBonusAttack], sizeof(float));
    memcpy(&abilityPower, &buff[Offsets::ObjMagicDamage], sizeof(float));
    memcpy(&lethality, &buff[Offsets::ObjLethality], sizeof(float));
    memcpy(&armorPen, &buff[Offsets::ObjArmorPen], sizeof(float));
    memcpy(&magicPen, &buff[Offsets::ObjMagicPen], sizeof(float));
    memcpy(&magicResPen, &buff[Offsets::ObjMagicResPen], sizeof(float));
}

void GameObject::LoadBuffFromMem(DWORD64 base)
{
    DWORD64 buffArrayBgn, buffArrayEnd;
    Mem::Read(address + Offsets::ObjBuffManager + Offsets::BuffArrayBegin, &buffArrayBgn, sizeof(DWORD64));
    Mem::Read(address + Offsets::ObjBuffManager + Offsets::BuffArrayEnd, &buffArrayEnd, sizeof(DWORD64));

    int i = 0;
    buffs.clear();
    buffs.reserve(i);
    char buffnamebuffer[240];

    for (DWORD64 pBuffPtr = buffArrayBgn; pBuffPtr != buffArrayEnd; pBuffPtr += 0x8) {
        i++;
        DWORD64 buffInstance = Mem::ReadDWORD(pBuffPtr);
        Mem::Read(buffInstance, buffListBuffer, 0x90);

        DWORD64 buffInfo, buffNamePtr;
        memcpy(&buffInfo, &buffListBuffer[Offsets::BuffEntryBuff], sizeof(DWORD64));
        if (buffInfo <= 0x1000) {
            continue;
        }
        Mem::Read(buffInfo + Offsets::BuffName, &buffNamePtr, sizeof(DWORD64));
        memset(buffnamebuffer, 0, sizeof(buffnamebuffer));

        Mem::Read(buffNamePtr, buffnamebuffer, 240);
        if (std::string(buffnamebuffer) == "" || !Character::ContainsOnlyASCII(buffnamebuffer, 240)) {
            continue;
        }

        bool isAlive = false;
        float buffStartTime;
        float buffEndTime;
        UINT8 buffCount;
        UINT8 buffCountAlt;
        UINT8 buffCountAlt2;
        UINT8 bufftype;

        memcpy(&buffStartTime, &buffListBuffer[Offsets::BuffEntryBuffStartTime], sizeof(float));
        memcpy(&buffEndTime, &buffListBuffer[Offsets::BuffEntryBuffEndTime], sizeof(float));
        memcpy(&buffCount, &buffListBuffer[Offsets::BuffEntryBuffCount], sizeof(UINT8));
        memcpy(&buffCountAlt, &buffListBuffer[Offsets::BuffEntryBuffCountAlt], sizeof(UINT8));
        memcpy(&buffCountAlt2, &buffListBuffer[Offsets::BuffEntryBuffCountAlt2], sizeof(UINT8));
        memcpy(&bufftype, &buffListBuffer[Offsets::BuffType], sizeof(UINT8));

        if (buffEndTime > gameTime) {
            isAlive = true;
        }

        buffs.push_back(BuffInfo(buffnamebuffer, buffCount, buffCountAlt, buffCountAlt2, bufftype, buffStartTime, buffEndTime, isAlive));
    }
}

void GameObject::LoadAIMangerFromMem(DWORD64 base)
{
    if (!AIMangerAddress) {
        AIMangerAddress = Mem::GetAIMangerAddress(address);
    }
    if (AIMangerAddress) {
        bool isDashing = false;
        DWORD64 targetPosRoot;
        short posCount = 0;

        Mem::Read(AIMangerAddress + Offsets::AIManagerStartPath, &AIManager.startPath, sizeof(Vector3));
        Mem::Read(AIMangerAddress + Offsets::AIManagerEndPath, &AIManager.endPath, sizeof(Vector3));
        Mem::Read(AIMangerAddress + Offsets::AIManagerIsMoving, &AIManager.isMoving, sizeof(bool));
        Mem::Read(AIMangerAddress + Offsets::AIManagerIsDashing, &isDashing, sizeof(bool));
        Mem::Read(AIMangerAddress + Offsets::AIManagerTargetPos, &targetPosRoot, sizeof(DWORD64));
        Mem::Read(AIMangerAddress + Offsets::AIManagerSegmentCount, &posCount, sizeof(short));
        Mem::Read(AIMangerAddress + Offsets::Velocity, &AIManager.velocity, sizeof(Vector3));
        Mem::Read(AIMangerAddress + Offsets::ServerPos, &AIManager.serverPos, sizeof(Vector3));
        Mem::Read(AIMangerAddress + Offsets::AIManagerMoveSpeed, &AIManager.moveSpeed, sizeof(float));
        Mem::Read(AIMangerAddress + Offsets::AIManagerDashSpeed, &AIManager.dashSpeed, sizeof(float));
        AIManager.isDashing = isDashing > 0.f && AIManager.isMoving;

        int j = 0;
        AIManager.targetPath.clear();
        AIManager.targetPath.reserve(j);
        for (int i = 0; i < posCount; i++) {
            j++;
            Vector3 tPos;
            Mem::Read(targetPosRoot + i * 0xC, &tPos, sizeof(Vector3));
            AIManager.targetPath.push_back(tPos);
        }
    }
}

void GameObject::LoadActiveCast(DWORD64 base)
{
    DWORD64 activeCastRoot;
    Mem::Read(address + Offsets::ActiveSpellRoot, &activeCastRoot, sizeof(DWORD64));
    Mem::Read(activeCastRoot + Offsets::ActiveSpellStartPos, &activeCast.startPos, sizeof(Vector3));
    Mem::Read(activeCastRoot + Offsets::ActiveSpellEndPos, &activeCast.endPos, sizeof(Vector3));

    DWORD64 spellInfoPtr;
    Mem::Read(activeCastRoot + Offsets::ActiveSpellInfo, &spellInfoPtr, sizeof(DWORD64));

    DWORD64 spellDataPtr = Mem::ReadDWORD(spellInfoPtr + Offsets::SpellInfoSpellData);
    DWORD64 spellNamePtr = Mem::ReadDWORD(spellDataPtr + Offsets::SpellDataSpellName);

    char spellNameBuff[50];
    Mem::Read(spellNamePtr, spellNameBuff, 50);
    if (Character::ContainsOnlyASCII(spellNameBuff, 50)) {
        activeCast.name = Character::ToLower(std::string(spellNameBuff));
    } else {
        activeCast.name = std::string("");
    }
    activeCast.info = GameData::GetSpellInfoByName(activeCast.name);
}