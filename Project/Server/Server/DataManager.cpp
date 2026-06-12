#include "pch.h"
#include "DataManager.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

DataManager::DataManager()
{
    _dataPath = std::filesystem::current_path().parent_path() / "Data";

    LoadTilemaps();
    LoadItemInfos();
}

void DataManager::LoadTilemaps()
{
    std::ifstream file(_dataPath / "Tilemap.json");
    json data = json::parse(file);

    // Tilemap 추출
    Vector tileCount{ data["tileCounts"][0],  data["tileCounts"][1] };

    std::vector<std::vector<short>> tilemap;

    std::vector<short> x;
    for (const auto& tiles : data["tiles"])
    {
        for (const auto& pattern : tiles["pattern"])
        {
            for (int i = 0; i < pattern[1]; i++)
                x.push_back(pattern[0]);
        }

        for (int i = 0; i < tiles["repeatY"]; i++)
            tilemap.push_back(x);
    }
}

void DataManager::LoadItemInfos()
{
    std::ifstream file(_dataPath / "ItemInfos.json");
    json data = json::parse(file);

    // ItemInfo 추출
    /*for (const auto& item : data["items"])
    {
        ItemInfo info;

        info.type = item["type"];
        info.size.x = item["size"][0];
        info.size.y = item["size"][1];
        info.weight = item["weight"];
        info.cost = item["cost"];

        _itemInfos[info.type] = info;
    }*/
}