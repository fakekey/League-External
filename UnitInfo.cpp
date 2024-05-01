#include "UnitInfo.h"

void UnitInfo::SetTag(const char* tag)
{
    if (tag == nullptr) {
        return;
    }
    std::string strTag(tag);
    tags.push_back(strTag);
}