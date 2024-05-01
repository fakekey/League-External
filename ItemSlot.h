#pragma once
#include "ItemInfo.h"

struct ItemSlot {
public:
    int slot = 0;
    bool isEmpty = true;
    ItemInfo* info = nullptr;

    boost::python::object GetItemInfo()
    {
        if (info == nullptr) {
            return boost::python::object();
        }
        return boost::python::object(boost::ref(*info));
    }
};