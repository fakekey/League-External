#include "LeagueMemReader.h"
#include "GameData.h"
#include "GameObject.h"
#include "Offsets.h"
#include "Utils.h"
#include "psapi.h"
#include <deque>
#include <set>
#include <string>

LeagueMemReader::LeagueMemReader()
{
    // Some trash object not worth reading
    blacklistedObjectNames.insert("testcube");
    blacklistedObjectNames.insert("testcuberender");
    blacklistedObjectNames.insert("testcuberender10vision");
    blacklistedObjectNames.insert("s5test_wardcorpse");
    blacklistedObjectNames.insert("sru_camprespawnmarker");
    blacklistedObjectNames.insert("sru_plantrespawnmarker");
    blacklistedObjectNames.insert("preseason_turret_shield");
}

bool LeagueMemReader::IsLeagueWindowActive()
{
    HWND handle = GetForegroundWindow();
    DWORD h;
    GetWindowThreadProcessId(handle, &h);
    return pid == h;
}

bool LeagueMemReader::IsHookedToProcess()
{
    return LeagueProcess::IsProcessRunning(pid);
}

void LeagueMemReader::HookToProcess()
{
    // Find the window
    hWindow = FindWindowA("RiotWindowClass", NULL);
    if (hWindow == NULL) {
        throw WinApiException("League window not found");
    }

    // Get the process ID
    GetWindowThreadProcessId(hWindow, &pid);
    if (pid == NULL) {
        throw WinApiException("Couldn't retrieve league process id");
    }

    // Open the process
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        throw WinApiException("Couldn't open league process");
    }

    // Check architecture
    if (0 == IsWow64Process(hProcess, &is64Bit)) {
        throw WinApiException("Failed to identify if process has 32 or 64 bit architecture");
    }

    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        moduleBaseAddr = (DWORD_PTR)hMods[0];
    } else {
        throw WinApiException("Couldn't retrieve league base address");
    }

    blacklistedObjects.clear();
}

void LeagueMemReader::ReadRenderer(MemSnapShot& ms)
{
    ms.renderer->LoadFromMem(moduleBaseAddr, hProcess);
}

void LeagueMemReader::ReadChampions(MemSnapShot& ms)
{
    ms.champions.clear();
    ms.others.clear();

    auto HeroList = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::ChampionList);
    auto pList = Mem::ReadDWORD(hProcess, HeroList + 0x8);
    UINT pSize = Mem::ReadDWORD(hProcess, HeroList + 0x10);

    // Read objects from the pointers we just read
    for (unsigned int i = 0; i < pSize; ++i) {
        auto champObject = Mem::ReadDWORD(hProcess, pList + (0x8 * i));

        std::shared_ptr<GameObject> obj;
        obj = std::shared_ptr<GameObject>(new GameObject());
        obj->LoadFromMem(champObject, hProcess, true);
        ms.objectMap[obj->networkId] = obj;
        ms.indexToNetId[obj->objIndex] = obj->networkId;
        ms.updatedThisFrame.insert(obj->networkId);

        if (obj->name.size() <= 2 || blacklistedObjectNames.find(obj->name) != blacklistedObjectNames.end()) {
            blacklistedObjects.insert(obj->networkId);
        } else if (obj->HasUnitTags("Unit_Champion")) {
            ms.champions.push_back(obj);
        } else {
            ms.others.push_back(obj);
        }
    }
}

void LeagueMemReader::ReadMinions(MemSnapShot& ms)
{
    ms.minions.clear();
    ms.jungles.clear();

    auto MinionList = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::MinionList);
    auto pList = Mem::ReadDWORD(hProcess, MinionList + 0x8);
    UINT pSize = Mem::ReadDWORD(hProcess, MinionList + 0x10);

    // Read objects from the pointers we just read
    for (unsigned int i = 0; i < pSize; ++i) {
        auto champObject = Mem::ReadDWORD(hProcess, pList + (0x8 * i));

        std::shared_ptr<GameObject> obj;
        obj = std::shared_ptr<GameObject>(new GameObject());
        obj->LoadFromMem(champObject, hProcess, true);
        ms.objectMap[obj->networkId] = obj;
        ms.indexToNetId[obj->objIndex] = obj->networkId;
        ms.updatedThisFrame.insert(obj->networkId);

        if (obj->name.size() <= 2 || blacklistedObjectNames.find(obj->name) != blacklistedObjectNames.end()) {
            blacklistedObjects.insert(obj->networkId);
        } else if (obj->HasUnitTags("Unit_Minion_Lane")) {
            ms.minions.push_back(obj);
        } else if (obj->HasUnitTags("Unit_Monster")) {
            ms.jungles.push_back(obj);
        } else if (!obj->HasUnitTags("Unit_Plant")) {
            ms.others.push_back(obj);
        }
    }
}

void LeagueMemReader::ReadMissiles(MemSnapShot& ms)
{
    ms.missiles.clear();

    DWORD64 missileMapPtr = 0;
    Mem::Read(hProcess, moduleBaseAddr + Offsets::MissileList, &missileMapPtr, sizeof(DWORD64));
    UINT16 numMissiles = 0;
    DWORD64 rootNode = 0;
    Mem::Read(hProcess, missileMapPtr + Offsets::MissileCount, &numMissiles, sizeof(UINT16));
    Mem::Read(hProcess, missileMapPtr + Offsets::MissileRoot, &rootNode, sizeof(DWORD64));

    std::deque<DWORD64> nodesToVisit;
    std::set<DWORD64> visitedNodes;
    nodesToVisit.push_back(rootNode);
    DWORD64 childNode1 = 0, childNode2 = 0, childNode3 = 0, node = 0;
    char missileNameBuff[50];

    while (nodesToVisit.size() > 0 && visitedNodes.size() < numMissiles * 2) {
        node = nodesToVisit.front();
        nodesToVisit.pop_front();
        visitedNodes.insert(node);

        Mem::Read(hProcess, node, &childNode1, sizeof(DWORD64));
        Mem::Read(hProcess, node + 0x8, &childNode2, sizeof(DWORD64));
        Mem::Read(hProcess, node + 0x10, &childNode3, sizeof(DWORD64));

        if (visitedNodes.find(childNode1) == visitedNodes.end()) {
            if (std::find(nodesToVisit.begin(), nodesToVisit.end(), childNode1) == nodesToVisit.end()) {
                nodesToVisit.push_back(childNode1);
            }
        }
        if (visitedNodes.find(childNode2) == visitedNodes.end()) {
            if (std::find(nodesToVisit.begin(), nodesToVisit.end(), childNode2) == nodesToVisit.end()) {
                nodesToVisit.push_back(childNode2);
            }
        }
        if (visitedNodes.find(childNode3) == visitedNodes.end()) {
            if (std::find(nodesToVisit.begin(), nodesToVisit.end(), childNode3) == nodesToVisit.end()) {
                nodesToVisit.push_back(childNode3);
            }
        }

        DWORD32 netId = 0;
        Mem::Read(hProcess, node + Offsets::MissileKey, &netId, sizeof(DWORD32));
        if (netId - 0x40000000 > 0x100000) {
            continue;
        }

        DWORD64 addr = 0;
        Mem::Read(hProcess, node + Offsets::MissileValue, &addr, sizeof(DWORD64));
        if (addr == 0) {
            continue;
        }

        DWORD64 missileInfoPtr, missileNamePtr;
        Mem::Read(hProcess, addr + Offsets::MissileInfo, &missileInfoPtr, sizeof(DWORD64));
        Mem::Read(hProcess, missileInfoPtr + Offsets::MissileValue, &missileNamePtr, sizeof(DWORD64));
        memset(missileNameBuff, 0, sizeof(missileNameBuff));

        Mem::Read(hProcess, missileNamePtr, missileNameBuff, 50);
        if (std::string(missileNameBuff) == "" || !Character::ContainsOnlyASCII(missileNameBuff, 50)) {
            continue;
        }
        std::string missileName = Character::ToLower(std::string(missileNameBuff));
        if (missileName.find("basicattack") != std::string::npos) {
            continue;
        }
        auto info = GameData::GetSpellInfoByName(missileName);
        if (info == nullptr || info->flags == 0) {
            continue;
        }

        std::shared_ptr<Missile> obj;
        obj = std::shared_ptr<Missile>(new Missile());
        obj->name = missileName;
        Mem::Read(hProcess, addr + Offsets::MissileSrcIdx, &obj->srcIdx, sizeof(short));
        Mem::Read(hProcess, addr + Offsets::ObjTeam, &obj->team, sizeof(short));
        Mem::Read(hProcess, addr + Offsets::ObjPos, &obj->pos, sizeof(Vector3));
        Mem::Read(hProcess, addr + Offsets::MissileStartPos, &obj->startPos, sizeof(Vector3));
        Mem::Read(hProcess, addr + Offsets::MissileEndPos, &obj->endPos, sizeof(Vector3));
        obj->info = info;

        ms.missiles.push_back(obj);
    }
}

void LeagueMemReader::FindLocalPlayer(MemSnapShot& ms)
{
    DWORD32 netId = 0;
    Mem::Read(hProcess, Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::LocalPlayer) + Offsets::ObjNetworkID, &netId, sizeof(DWORD32));
    auto it = ms.objectMap.find(netId);
    if (it != ms.objectMap.end()) {
        ms.player = it->second;
    } else {
        ms.player = (ms.champions.size() > 0 ? ms.champions[0] : nullptr);
    }
}

void LeagueMemReader::ClearMissingObjects(MemSnapShot& ms)
{
    auto it = ms.objectMap.begin();
    while (it != ms.objectMap.end()) {
        if (ms.updatedThisFrame.find(it->first) == ms.updatedThisFrame.end()) {
            it = ms.objectMap.erase(it);
        } else {
            ++it;
        }
    }

    auto it2 = ms.indexToNetId.begin();
    while (it2 != ms.indexToNetId.end()) {
        if (ms.updatedThisFrame.find(it2->second) == ms.updatedThisFrame.end()) {
            it2 = ms.indexToNetId.erase(it2);
        } else {
            ++it2;
        }
    }
}

void LeagueMemReader::MakeSnapShot(MemSnapShot& ms)
{
    Mem::Read(hProcess, moduleBaseAddr + Offsets::GameTime, &ms.gameTime, sizeof(float));
    // Checking chat
    DWORD64 chatInstance = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::ChatClient);
    Mem::Read(hProcess, chatInstance + Offsets::ChatIsOpen, &ms.isChatOpen, sizeof(bool));

    if (ms.gameTime > 2.f) {
        ms.updatedThisFrame.clear();
        GameObject::gameTime = ms.gameTime;
        ReadRenderer(ms);
        ReadChampions(ms);
        ReadMinions(ms);
        ReadMissiles(ms);
        ClearMissingObjects(ms);
        FindLocalPlayer(ms);
    }
}