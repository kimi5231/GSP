#pragma once
class GameObject;
class Player;
class Bar;

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
	void AddCreature(int id, int visualID, const char* name, short x, short y, int hp, int maxHp, long long exp, int level);
	void RemoveObject(int id);

public:
	bool IsCanGo(Vector pos);

public:
	PlayerRef GetAvatar() { return _avatar; }
	GameObjectRef GetGameObject(int id);
	void SetIsInGame(bool isInGame) { _isInGame = isInGame; }
	Bar* GetHpBar() { return _hpBar; }
	Bar* GetExpBar() { return _expBar; }

private:
	sf::RenderWindow* _window;
	sf::View _gameView;
	sf::View _uiView;

	sf::Texture _texture;
	sf::Sprite _sprite;
	
private:
	bool _isInGame;

	sf::RectangleShape _barBackground;
	Bar* _hpBar;
	Bar* _expBar;

	std::vector<std::vector<short>> _map;
	
	char _userName[MAX_NAME_LEN];
	PlayerRef _avatar;
	std::unordered_map<int, GameObjectRef> _objects;
};