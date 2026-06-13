#pragma once
class DataManager
{
public:
	DataManager();
	~DataManager() {}

public:
	void LoadTilemaps();
	void LoadItemInfos();
	void LoadMonsterStat();

public:
	const std::vector<std::vector<short>>& GetTilemap() { return _tilemap; }
	//const ItemInfo& GetItemInfo(ItemType type) { return _itemInfos[type]; }
	const MonsterStat& GetMonsterStat() { return _monsterStat; }

private:
	std::filesystem::path _dataPath;

	std::vector<std::vector<short>> _tilemap;
	//std::unordered_map<ItemType, ItemInfo> _itemInfos;

	MonsterStat _monsterStat;
};