#pragma once

#include <string>

struct MenuItem
{
    int id;
    int restaurantId;
    std::string name;
    double price;
    std::string category;

    MenuItem(int i = -1, int rId = -1, const std::string &n = "", double p = 0.0, const std::string &cat = "")
    {
        id = i;
        restaurantId = rId;
        name = n;
        price = p;
        category = cat;
    }
};