#pragma once
class DataManager
{
public:
	DataManager();
	~DataManager() {}

public:
	void LoadTilemaps();

public:
	const std::vector<std::vector<short>>& GetTilemap() { return _tilemap; }

private:
	std::filesystem::path _dataPath;

	std::vector<std::vector<short>> _tilemap;

};