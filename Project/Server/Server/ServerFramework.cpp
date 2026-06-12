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

Player* ServerFramework::AddPlayer()
{
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		// 재사용 가능한 플레이어 찾기
		if (_players[i]->GetObjectPoolState() == ObjectPoolState::Reusable)
		{
			// ObjectPoolState 변경
			_players[i]->Init();

			for (auto& player : _players)
			{
				if (player->GetClient())
					g_network->SendAddObjectPacket(_players[i], player->GetClient());
			}

			return _players[i];
		}
	}

	return nullptr;
}

void ServerFramework::RemoveObject(ObjectType type, int id, bool isSend)
{

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