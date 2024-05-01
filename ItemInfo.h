#pragma once
#include <string>

struct ItemInfo {
public:
    int id;
    std::string name;
    float cost;
    float movementSpeed;
    float health;
    float crit;
    float abilityPower;
    float mana;
    float armour;
    float magicResist;
    float physicalDamage;
    float attackSpeed;
    float lifeSteal;
    float hpRegen;
    float movementSpeedPercent;
};