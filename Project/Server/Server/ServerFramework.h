#pragma once
#include "Player.h"

class Monster;

class ServerFramework
{
public:
	ServerFramework();
	~ServerFramework();

public:
	void Update();

public:
	Player* AddPlayer(int clientIndex);
	Monster* AddMonster(Vector pos);
	/*Item* AddItem(bool isTool, ItemType itemType, Vector pos);*/
	void RemoveObject(ObjectType type, int id);

public:
	bool IsCanGo(int x, int y);
	bool IsCanGo(Vector index);

	void AddAliveMonster(int id);
	void RemoveAliveMonster(int id);

public:
	GameObject* GetGameObject(ObjectType type, int id);
	const std::array<Player*, MAX_PLAYERS>& GetPlayers() { return _players; }
	const std::array<Monster*, NUM_NPCS>& GetMonsters() { return _monsters; }

private:
	std::vector<std::vector<short>> _map;
	std::array<Player*, MAX_PLAYERS> _players;
	std::array<Monster*, NUM_NPCS> _monsters;

	std::unordered_map<int, int> _aliveMonsters;
	std::mutex _aliveLock;

	float _sumTime;
};