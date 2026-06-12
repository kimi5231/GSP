#pragma once
class GameObject;
class Player;

class GameFramework
{
public:
	GameFramework(sf::RenderWindow* window);
	~GameFramework();

public:
	void Update();
	void Render();

public:
	void LoadTile();

public:
	void CreateAvatar(int id, int visualID, short x, short y, int hp, int maxHp, long long exp, int level);
	void AddPlayer(int id, int visualID, const char* name, short x, short y, int hp, int maxHp, long long exp, int level);

public:
	bool IsCanGo(Vector pos);

public:
	PlayerRef GetAvatar() { return _avatar; }
	GameObjectRef GetGameObject(ObjectType type, int id);

private:
	sf::RenderWindow* _window;
	sf::View _view;

	sf::Texture _texture;
	sf::Sprite _sprite;

private:
	std::vector<std::vector<short>> _map;
	PlayerRef _avatar;
	std::unordered_map<int, PlayerRef> _players;
};