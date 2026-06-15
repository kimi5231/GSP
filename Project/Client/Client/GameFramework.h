#pragma once
#include "Inventory.h"

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
	void AddItem(int id, ObjectType type, Vector pos);
	void RemoveObject(int id);

public:
	bool IsCanGo(Vector pos);

public:
	PlayerRef GetAvatar() { return _avatar; }
	GameObjectRef GetGameObject(int id);
	void SetIsInGame(bool isInGame) { _isInGame = isInGame; }
	Bar* GetHpBar() { return _hpBar; }
	Bar* GetExpBar() { return _expBar; }
	Inventory& GetInventory() { return _inventory; }

private:
	sf::RenderWindow* _window;
	sf::View _gameView;
	sf::View _uiView;

	sf::Texture _texture;
	sf::Sprite _sprite;
	
private:
	bool _isInGame;
	bool _isOpenInventory;

	ItemRef _dragItem;

	sf::RectangleShape _barBackground;
	Bar* _hpBar;
	Bar* _expBar;

	std::vector<std::vector<short>> _map;
	
	char _userName[MAX_NAME_LEN];
	PlayerRef _avatar;
	Inventory _inventory;
	std::unordered_map<int, GameObjectRef> _objects;
};