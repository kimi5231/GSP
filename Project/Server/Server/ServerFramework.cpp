#include "pch.h"
#include "ServerFramework.h"
#include "Global.h"
#include "Player.h"

ServerFramework::ServerFramework()
{
	_map = g_dataManager->GetTilemap();

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		_players[i] = new Player();
		_players[i]->SetID(i);
		_players[i]->SetObjectPoolState(ObjectPoolState::Reusable);
	}
}

ServerFramework::~ServerFramework()
{
	
}

void ServerFramework::Update()
{
	
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

GameObject* ServerFramework::GetGameObject(ObjectType type, int id)
{
	switch (type)
	{
	case ObjectType::Player:
		return _players[id];
	}
}