#pragma once
class DataManager
{
public:
	DataManager();
	~DataManager() {}

public:
	void LoadTilemaps();

public:
	sf::Font& GetFont() { return _font; }
	const std::vector<std::vector<short>>& GetTilemap() { return _tilemap; }

private:
	std::filesystem::path _dataPath;

	sf::Font _font;
	std::vector<std::vector<short>> _tilemap;

};