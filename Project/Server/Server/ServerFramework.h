#pragma once
#include "Player.h"

class Monster;
class Item;

class ServerFramework
{
public:
	ServerFramework();
	~ServerFramework();

public:
	void Update();

public:
	Player* AddPlayer(int clientIndex);
	Item* AddItem(ObjectType type, Vector pos);
	void RemoveObject(ObjectType type, int id);

public:
	bool IsCanGo(int x, int y);
	bool IsCanGo(Vector index);

	void AddAliveMonster(int id);
	void RemoveAliveMonster(int id);
	void AddAlivePlayer(int id);
	void RemoveAlivePlayer(int id);

public:
	GameObject* GetGameObject(ObjectType type, int id);
	const std::array<Player*, MAX_PLAYERS>& GetPlayers() { return _players; }
	const std::array<Monster*, NUM_NPCS>& GetMonsters() { return _monsters; }
	const std::array<Item*, MAX_ITEMS>& GetItems() { return _items; }

private:
	std::vector<std::vector<short>> _map;
	std::array<Player*, MAX_PLAYERS> _players;
	std::array<Monster*, NUM_NPCS> _monsters;
	std::array<Item*, MAX_ITEMS> _items;

	std::unordered_set<int> _alivePlayers;
	std::unordered_map<int, int> _aliveMonsters;
	std::mutex _alivePlayerLock;
	std::mutex _aliveMonsterLock;

	float _sumTime;
};