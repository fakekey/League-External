#pragma once
#include <string>
#include <vector>

struct UnitInfo {
public:
    std::string name;
    float healthBarHeight;
    float baseMoveSpeed;
    float attackRange;
    double attackSpeed;
    double attackSpeedRatio;
    float acquisitionRange;
    float selectionRadius;
    float pathingRadius;
    float gameplayRadius;
    float basicAtkMissileSpeed;
    double basicAtkWindup;
    float basicAtkWindupModify;
    std::vector<std::string> tags;

public:
    void SetTag(const char* tag);
};