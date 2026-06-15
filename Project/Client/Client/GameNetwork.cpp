#include "pch.h"
#include "GameNetwork.h"
#include "Global.h"
#include "Player.h"
#include "Item.h"
#include "Inventory.h"

GameNetwork::GameNetwork()
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	// listenSocket 생성
	_clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_clientSocket == INVALID_SOCKET)
		return;

	// connect
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(PORT);
	if (connect(_clientSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << "connect 실패" << std::endl;
		return;
	}
}

GameNetwork::~GameNetwork()
{
	// clientSocket 종료
	closesocket(_clientSocket);

	// 윈속 종료
	WSACleanup();
}

void GameNetwork::Update()
{
	// socket set 초기화
	FD_ZERO(&_readSet);
 	FD_ZERO(&_writeSet);

	// readSet, writeSet에 clientSocket 등록
	FD_SET(_clientSocket, &_readSet);
	FD_SET(_clientSocket, &_writeSet);

	// select
	if (select(0, &_readSet, &_writeSet, NULL, 0) == SOCKET_ERROR)
		return;

	if (FD_ISSET(_clientSocket, &_readSet))
	{
		ProcessRecv();
	}

	// send가 가능할 때마다 true
	if (FD_ISSET(_clientSocket, &_writeSet))
	{
		for (NetworkEventRef event : _sendEvents)
		{
			send(_clientSocket, event->serializedPacketData.data(), event->serializedPacketData.size(), 0);
			event->isComplete = true;
		}

		_sendEvents.erase(std::remove_if(_sendEvents.begin(), _sendEvents.end(),
			[](NetworkEventRef event) {
				return event->isComplete;
			}), _sendEvents.end());
	}
}

void GameNetwork::ProcessRecv()
{
	char packetSize;
	recv(_clientSocket, (char*)&packetSize, sizeof(char), MSG_WAITALL);
	std::vector<char> packet(BufferSize);
	packet[0] = packetSize;
	recv(_clientSocket, packet.data() + sizeof(char), packetSize - sizeof(char), MSG_WAITALL);
	
	PACKET_TYPE id;
	memcpy(&id, packet.data() + sizeof(char), sizeof(PACKET_TYPE));

	// Data 추출
	switch (id)
	{
	case S2C_LOGIN_RESULT:
	{
		S2C_LoginResult loginResultPacket;
		loginResultPacket.size = packetSize;
		loginResultPacket.type = id;
		memcpy(&loginResultPacket.success, packet.data() + sizeof(char) + sizeof(PACKET_TYPE), sizeof(bool));
		memcpy(&loginResultPacket.message, packet.data() + sizeof(char) + sizeof(PACKET_TYPE) + sizeof(bool), sizeof(char) * 50);
		ProcessLoginResultPacket(loginResultPacket);
		break;
	}
	case S2C_AVATAR_INFO:
	{
		S2C_AvatarInfo avatarInfoPacket;
		memcpy(&avatarInfoPacket, packet.data(), sizeof(S2C_AvatarInfo));
		ProcessAvatarInfoPacket(avatarInfoPacket);
		break;
	}
	case S2C_ADD_OBJECT:
	{
		S2C_AddObject addObjectPacket;
		memcpy(&addObjectPacket, packet.data(), sizeof(S2C_AddObject));
		ProcessAddObjectPacket(addObjectPacket);
		break;
	}
	case S2C_REMOVE_OBJECT:
	{
		S2C_RemoveObject removeObjectPacket;
		memcpy(&removeObjectPacket, packet.data(), sizeof(S2C_RemoveObject));
		ProcessRemoveObjectPacket(removeObjectPacket);
		break;
	}
	case S2C_MOVE_OBJECT:
	{
		S2C_MoveObject moveObjectPacket;
		memcpy(&moveObjectPacket, packet.data(), sizeof(S2C_MoveObject));
		ProcessMoveObjectPacket(moveObjectPacket);
		break;
	}
	case S2C_STATUS_CHANGE:
	{
		S2C_StatusChange statusChangePacket;
		memcpy(&statusChangePacket, packet.data(), sizeof(S2C_StatusChange));
		ProcessStatusChangePacket(statusChangePacket);
		break;
	}
	case S2C_ADD_ITEM:
	{
		S2C_AddItem addItemPacket;
		memcpy(&addItemPacket, packet.data(), sizeof(S2C_AddItem));
		ProcessAddItemPacket(addItemPacket);
		break;
	}
	case S2C_REMOVE_ITEM:
	{
		S2C_RemoveItem removeItemPacket;
		memcpy(&removeItemPacket, packet.data(), sizeof(S2C_RemoveItem));
		ProcessRemoveItemPacket(removeItemPacket);
		break;
	}
	case S2C_ADD_ITEM_TO_INVENTORY:
	{
		S2C_AddItemToInventory addItemToInventoryPacket;
		memcpy(&addItemToInventoryPacket, packet.data(), sizeof(S2C_AddItemToInventory));
		ProcessAddItemToInventoryPacket(addItemToInventoryPacket);
		break;
	}
	case S2C_REMOVE_ITEM_FROM_INVENTORY:
	{
		S2C_RemoveItemFromInventory removeItemFromInventoryPacket;
		memcpy(&removeItemFromInventoryPacket, packet.data(), sizeof(S2C_RemoveItemFromInventory));
		ProcessRemoveItemFromInventoryPacket(removeItemFromInventoryPacket);
		break;
	}
	}
}

void GameNetwork::SendLoginPacket(const char* id)
{
	// Packet Data 생성
	C2S_Login packet;
	packet.size = sizeof(C2S_Login);
	packet.type = C2S_LOGIN;
	memcpy(packet.username, id, MAX_NAME_LEN);
	
	// Packet Serialize
	std::vector<char> serializedPacketData = SerializePOD(packet);

	// SendEvent 생성
	NetworkEventRef event = std::make_shared<NetworkEvent>();
	event->packetID = C2S_LOGIN;
	event->serializedPacketData = serializedPacketData;
	_sendEvents.push_back(event);
}

void GameNetwork::SendLogoutPacket()
{
	// Packet Data 생성
	C2S_Logout packetData{ sizeof(C2S_Logout), C2S_LOGOUT };

	// Packet Serialize
	std::vector<char> serializedPacketData = SerializePOD(packetData);

	// SendEvent 생성
	NetworkEventRef event = std::make_shared<NetworkEvent>();
	event->packetID = C2S_LOGOUT;
	event->serializedPacketData = serializedPacketData;
	_sendEvents.push_back(event);
}

void GameNetwork::SendMovePacket(PlayerRef player)
{
	// Packet Data 생성
	C2S_Move packetData{ sizeof(C2S_Move), C2S_MOVE, player->GetPos().x, player->GetPos().y, 0 };

	// Packet Serialize
	std::vector<char> serializedPacketData = SerializePOD(packetData);

	// SendEvent 생성
	NetworkEventRef event = std::make_shared<NetworkEvent>();
	event->packetID = C2S_MOVE;
	event->serializedPacketData = serializedPacketData;
	_sendEvents.push_back(event);
}

void GameNetwork::SendAttackPacket()
{
	// Packet Data 생성
	C2S_Attack packetData{ sizeof(C2S_Attack), C2S_ATTACK };

	// Packet Serialize
	std::vector<char> serializedPacketData = SerializePOD(packetData);

	// SendEvent 생성
	NetworkEventRef event = std::make_shared<NetworkEvent>();
	event->packetID = C2S_ATTACK;
	event->serializedPacketData = serializedPacketData;
	_sendEvents.push_back(event);
}

void GameNetwork::SendDropItemPacket(int itemID)
{
	// Packet Data 생성
	C2S_DropItem packetData{ sizeof(C2S_DropItem), C2S_DROP_ITEM, itemID };

	// Packet Serialize
	std::vector<char> serializedPacketData = SerializePOD(packetData);

	// SendEvent 생성
	NetworkEventRef event = std::make_shared<NetworkEvent>();
	event->packetID = C2S_DROP_ITEM;
	event->serializedPacketData = serializedPacketData;
	_sendEvents.push_back(event);
}

void GameNetwork::ProcessLoginResultPacket(S2C_LoginResult packet)
{
	if(packet.success)
		g_framework->SetIsInGame(true);
}

void GameNetwork::ProcessAvatarInfoPacket(S2C_AvatarInfo packet)
{
	g_framework->CreateAvatar(packet.playerId, packet.visualId, packet.x, packet.y, packet.hp, packet.max_hp, packet.exp, packet.level);
}

void GameNetwork::ProcessAddObjectPacket(S2C_AddObject packet)
{
	g_framework->AddCreature(packet.object_id, packet.visual_id, packet.obj_name, packet.x, packet.y, packet.hp, packet.max_hp, packet.exp, packet.level);
}

void GameNetwork::ProcessRemoveObjectPacket(S2C_RemoveObject packet)
{
	g_framework->RemoveObject(packet.object_id);
}

void GameNetwork::ProcessMoveObjectPacket(S2C_MoveObject packet)
{
	// ID가 Avatar의 ID와 같다면 Avatar의 좌표 수정
	if (packet.object_id == g_framework->GetAvatar()->GetID())
	{
		g_framework->GetAvatar()->SetPos(packet.x, packet.y);
		return;
	}

	GameObjectRef object = g_framework->GetGameObject(packet.object_id);
	if (!object)
		return;
	object->SetPos(packet.x, packet.y);
}

void GameNetwork::ProcessStatusChangePacket(S2C_StatusChange packet)
{
	if (packet.object_id == g_framework->GetAvatar()->GetID())
	{
		PlayerRef avatar = g_framework->GetAvatar();
		avatar->SetHP(packet.hp);
		avatar->SetMaxHP(packet.max_hp);
		avatar->SetExp(packet.exp);
		avatar->SetLevel(packet.level);
		return;
	}

	CreatureRef creature = dynamic_pointer_cast<Creature>(g_framework->GetGameObject(packet.object_id));
	if (!creature)
		return;
	creature->SetHP(packet.hp);
	creature->SetMaxHP(packet.max_hp);
	creature->SetExp(packet.exp);
	creature->SetLevel(packet.level);
}

void GameNetwork::ProcessAddItemPacket(S2C_AddItem packet)
{
	g_framework->AddItem(packet.id, packet.objectType, packet.pos);
}

void GameNetwork::ProcessRemoveItemPacket(S2C_RemoveItem packet)
{
	g_framework->RemoveObject(packet.id);
}

void GameNetwork::ProcessAddItemToInventoryPacket(S2C_AddItemToInventory packet)
{
	g_framework->GetInventory().AddItem(packet.index, dynamic_pointer_cast<Item>(g_framework->GetGameObject(packet.id)));
}

void GameNetwork::ProcessRemoveItemFromInventoryPacket(S2C_RemoveItemFromInventory packet)
{
	g_framework->GetInventory().RemoveItem(packet.index);
}