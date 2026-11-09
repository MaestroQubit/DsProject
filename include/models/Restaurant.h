#pragma once

#include <string>

struct Restaurant
{
    int id;
    std::string name;
    int locationId;

    Restaurant(int i = -1, const std::string &n = "", int loc = -1) : id(i), name(n), locationId(loc) {}
};