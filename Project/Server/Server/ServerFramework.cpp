#include "pch.h"
#include "ServerFramework.h"
#include "Global.h"
#include "Player.h"

ServerFramework::ServerFramework()
{
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

GameObject* ServerFramework::GetGameObject(ObjectType type, int id)
{
	switch (type)
	{
	case ObjectType::Player:
		return _players[id];
	}
}