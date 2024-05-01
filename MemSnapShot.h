#pragma once
#include "GameObject.h"
#include "LeagueRenderer.h"
#include <memory>
#include <set>
#include <vector>

struct MemSnapShot {
    std::vector<std::shared_ptr<GameObject>> champions;
    std::vector<std::shared_ptr<GameObject>> minions;
    std::vector<std::shared_ptr<GameObject>> jungles;
    std::vector<std::shared_ptr<GameObject>> others;
    std::vector<std::shared_ptr<Missile>> missiles;

    /* The champion of the player running the app */
    std::shared_ptr<GameObject> player = nullptr;

    /* Game renderer info */
    std::unique_ptr<LeagueRenderer> renderer = std::unique_ptr<LeagueRenderer>(new LeagueRenderer());

    /* A map between the network id of the object and the object itself */
    std::map<unsigned int, std::shared_ptr<GameObject>> objectMap;
    std::map<short, unsigned int> indexToNetId;

    /* Used to clear objectMap for objects that are no longer in game */
    std::set<unsigned int> updatedThisFrame;

    float gameTime = 0.f;
    int ping = 24;
    bool isChatOpen = false;
};