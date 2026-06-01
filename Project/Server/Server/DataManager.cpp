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
    /*for (const auto& tilemap : data["tilemaps"])
    {
        CubeType type = tilemap["type"];
        Vector tileCount{ tilemap["tileCount"][0], tilemap["tileCount"][1], tilemap["tileCount"][2] };

        std::vector<std::vector<std::vector<short>>> tilemap3D;
        for (const auto& layer : tilemap["tilemap"])
        {
            std::vector<std::vector<short>> tilemap2D;
            for (const auto& matrix : layer["layer"])
            {
                std::vector<short> tilemap1D;
                for (const auto& pattern : matrix["pattern"])
                {
                    for (int i = 0; i < pattern[1]; i++)
                        tilemap1D.push_back(pattern[0]);
                }

                for (int i = 0; i < matrix["repeatY"]; i++)
                    tilemap2D.push_back(tilemap1D);
            }

            for (int i = 0; i < layer["repeatZ"]; i++)
                tilemap3D.push_back(tilemap2D);
        }

        _cubeTilemaps[type] = tilemap3D;
    }*/
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