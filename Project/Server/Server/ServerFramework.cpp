#include "pch.h"
#include "ServerFramework.h"
#include "Global.h"
#include "Player.h"
#include "Agro.h"
#include "Peace.h"
#include "Weapon.h"

ServerFramework::ServerFramework()
{
	_map = g_dataManager->GetTilemap();

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		_players[i] = new Player();
		_players[i]->SetID(i);
		_players[i]->SetObjectPoolState(ObjectPoolState::Reusable);
	}

	for (int i = 0; i < NUM_NPCS / 2; ++i)
	{
		_monsters[i] = new Agro();
		_monsters[i]->SetID(i + MONSTER_ID);
		_monsters[i]->SetObjectPoolState(ObjectPoolState::InWorld);
	}

	for (int i = NUM_NPCS / 2; i < NUM_NPCS; ++i)
	{
		_monsters[i] = new Peace();
		_monsters[i]->SetID(i + MONSTER_ID);
		_monsters[i]->SetObjectPoolState(ObjectPoolState::InWorld);
	}

	for (int i = 0; i < MAX_ITEMS / 2; ++i)
	{
		_items[i] = new Item();
		_items[i]->SetID(i + ITEM_ID);
		_items[i]->SetObjectPoolState(ObjectPoolState::Reusable);
	}

	for (int i = MAX_ITEMS / 2; i < MAX_ITEMS; ++i)
	{
		_items[i] = new Weapon();
		_items[i]->SetID(i + ITEM_ID);
		_items[i]->SetObjectPoolState(ObjectPoolState::Reusable);
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

	_aliveMonsterLock.lock();
	for (auto& [id, count] : _aliveMonsters)
		_monsters[id - MONSTER_ID]->Update();
	_aliveMonsterLock.unlock();

	_alivePlayerLock.lock();
	for (auto& id : _alivePlayers)
		_players[id]->Update();
	_alivePlayerLock.unlock();
}

Player* ServerFramework::AddPlayer(int clientIndex)
{
	// 해당 ID의 Player가 재사용 가능한지 확인
	if (_players[clientIndex]->GetObjectPoolState() == ObjectPoolState::Reusable)
	{
		// 초기화
		_players[clientIndex]->Init();
		AddAlivePlayer(clientIndex);
		return _players[clientIndex];
	}

	return nullptr;
}

Item* ServerFramework::AddItem(ObjectType type, Vector pos)
{
	for (int i = 0; i < MAX_ITEMS; ++i)
	{
		if (_items[i]->GetObjectPoolState() == ObjectPoolState::Reusable)
		{
			_items[i]->SetObjectPoolState(ObjectPoolState::InWorld);
			_items[i]->SetObjectType(type);
			_items[i]->SetPos(pos);
			return _items[i];
		}
	}

	return nullptr;
}

void ServerFramework::RemoveObject(ObjectType type, int id)
{
	switch (type)
	{
	case ObjectType::Player:
		_players[id]->SetObjectPoolState(ObjectPoolState::Reusable);
		RemoveAlivePlayer(id);
		break;
	case ObjectType::Sword:
		_items[id - ITEM_ID]->SetObjectPoolState(ObjectPoolState::Reusable);
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

	_aliveMonsterLock.lock();
	if (!_aliveMonsters.count(id))
		_aliveMonsters[id] = 1;
	else
		_aliveMonsters[id]++;
	_aliveMonsterLock.unlock();
}

void ServerFramework::RemoveAliveMonster(int id)
{
	if (!_aliveMonsters.count(id))
		return;

	_aliveMonsterLock.lock();
	_aliveMonsters[id]--;

	if (_aliveMonsters[id] == 0)
		_aliveMonsters.erase(id);
	_aliveMonsterLock.unlock();
}

void ServerFramework::AddAlivePlayer(int id)
{
	if (id >= MONSTER_ID)
		return;

	if (_alivePlayers.count(id))
		return;

	_alivePlayerLock.lock();
	_alivePlayers.insert(id);
	_alivePlayerLock.unlock();
}

void ServerFramework::RemoveAlivePlayer(int id)
{
	if (!_alivePlayers.count(id))
		return;

	_alivePlayerLock.lock();
	_alivePlayers.erase(id);
	_alivePlayerLock.unlock();
}

void ServerFramework::AddCanGetItem(int ownerID, int id)
{
	if (ownerID >= MONSTER_ID || id < ITEM_ID)
		return;

	if (_canGetItems.count(id))
		return;

	_canGetItemLock.lock();
	_canGetItems[id] = ownerID;
	_canGetItemLock.unlock();
}

void ServerFramework::RemoveCanGetItem(int ownerID, int id)
{
	if (!_canGetItems.count(id))
		return;

	_canGetItemLock.lock();
	_canGetItems.erase(id);
	_canGetItemLock.unlock();
}

GameObject* ServerFramework::GetGameObject(ObjectType type, int id)
{
	switch (type)
	{
	case ObjectType::Player:
		return _players[id];
	case ObjectType::Agro:
	case ObjectType::Peace:
		return _monsters[id - MONSTER_ID];
	case ObjectType::Sword:
		return _items[id - ITEM_ID];
	}
}

std::unordered_set<int> ServerFramework::GetCanGetItems(int ownerID)
{
	std::unordered_set<int> canGetItems;

	_canGetItemLock.lock();
	for (auto& [ItemID, playerID] : _canGetItems)
	{
		if (playerID == ownerID)
			canGetItems.insert(ItemID);
	}
	_canGetItemLock.unlock();

	return canGetItems;
}