#pragma once
#include "Vector.h"
#include <boost/python.hpp>
#include <vector>

struct AIManagerInfo {
public:
    AIManagerInfo() {};
    Vector3 startPath;
    Vector3 endPath;
    bool isMoving = false;
    bool isDashing = false;
    std::vector<Vector3> targetPath;
    Vector3 velocity;
    Vector3 serverPos;
    float moveSpeed = 0.f;
    float dashSpeed = 0.f;

    boost::python::list GetTargetPath()
    {
        boost::python::list l;
        for (auto& path : targetPath) {
            l.append(boost::ref(path));
        }
        return l;
    }
};