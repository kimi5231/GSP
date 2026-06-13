#include "pch.h"
#include "ServerFramework.h"
#include "Global.h"
#include "Player.h"
#include "Agro.h"

ServerFramework::ServerFramework()
{
	_map = g_dataManager->GetTilemap();

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		_players[i] = new Player();
		_players[i]->SetID(i);
		_players[i]->SetObjectPoolState(ObjectPoolState::Reusable);
	}

	for (int i = 0; i < NUM_NPCS; ++i)
	{
		_monsters[i] = new Agro();
		_monsters[i]->SetID(i + MONSTER_ID);
		_monsters[i]->SetObjectPoolState(ObjectPoolState::Reusable);
	}

	_sumTime = 0.f;
}

ServerFramework::~ServerFramework()
{
	
}

void ServerFramework::Update()
{
	_sumTime += g_timer->GetDeltaTime();
	if (_sumTime < 0.5)
		return;
	_sumTime = 0;

	_aliveLock.lock();
	for (auto& [id, count] : _aliveMonsters)
		_monsters[id - MONSTER_ID]->Update();
	_aliveLock.unlock();
}

Player* ServerFramework::AddPlayer(int clientIndex)
{
	// 해당 ID의 Player가 재사용 가능한지 확인
	if (_players[clientIndex]->GetObjectPoolState() == ObjectPoolState::Reusable)
	{
		// 초기화
		_players[clientIndex]->Init();
		return _players[clientIndex];
	}

	return nullptr;
}

Monster* ServerFramework::AddMonster(Vector pos)
{
	return nullptr;
}

void ServerFramework::RemoveObject(ObjectType type, int id)
{
	switch (type)
	{
	case ObjectType::Player:
		_players[id]->SetObjectPoolState(ObjectPoolState::Reusable);
		break;
	}
}

bool ServerFramework::IsCanGo(int x, int y)
{
	Vector index{ x / TILE_SIZE, y / TILE_SIZE };
	Vector max{ static_cast<int>(_map[0].size()), static_cast<int>(_map.size()) };

	if (index < Vector{0, 0} || index >= max)
		return false;

	if (_map[index.x][index.y] != 0)
		return false;

	return true;
}

bool ServerFramework::IsCanGo(Vector index)
{
	Vector max{ static_cast<int>(_map[0].size()), static_cast<int>(_map.size()) };

	if (index < Vector{ 0, 0 } || index >= max)
		return false;

	if (_map[index.x][index.y] != 0)
		return false;

	return true;
}

void ServerFramework::AddAliveMonster(int id)
{
	if (id < MONSTER_ID)
		return;

	_aliveLock.lock();
	if (!_aliveMonsters.count(id))
		_aliveMonsters[id] = 1;
	else
		_aliveMonsters[id]++;
	_aliveLock.unlock();
}

void ServerFramework::RemoveAliveMonster(int id)
{
	if (!_aliveMonsters.count(id))
		return;

	_aliveLock.lock();
	_aliveMonsters[id]--;

	if (_aliveMonsters[id] == 0)
		_aliveMonsters.erase(id);
	_aliveLock.unlock();
}

GameObject* ServerFramework::GetGameObject(ObjectType type, int id)
{
	switch (type)
	{
	case ObjectType::Player:
		return _players[id];
	}
}