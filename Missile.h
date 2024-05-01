#pragma once
#include "SpellInfo.h"
#include "Vector.h"
#include <string>

struct Missile {
public:
    Missile() {};
    std::string name;
    short srcIdx;
    short team;
    Vector3 pos;
    Vector3 startPos;
    Vector3 endPos;
    SpellInfo* info = nullptr;

    boost::python::object GetSpellInfo()
    {
        if (info == nullptr) {
            return boost::python::object();
        }
        return boost::python::object(boost::ref(*info));
    }
};