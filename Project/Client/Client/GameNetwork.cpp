#include "pch.h"
#include "GameNetwork.h"
#include "Global.h"
#include "Player.h"

GameNetwork::GameNetwork()
{
	// 扩加 檬扁拳
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	// listenSocket 积己
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
		std::cout << "connect 角菩" << std::endl;
		return;
	}
}

GameNetwork::~GameNetwork()
{
	// clientSocket 辆丰
	closesocket(_clientSocket);

	// 扩加 辆丰
	WSACleanup();
}

void GameNetwork::Update()
{
	// socket set 檬扁拳
	FD_ZERO(&_readSet);
 	FD_ZERO(&_writeSet);

	// readSet, writeSet俊 clientSocket 殿废
	FD_SET(_clientSocket, &_readSet);
	FD_SET(_clientSocket, &_writeSet);

	// select
	if (select(0, &_readSet, &_writeSet, NULL, 0) == SOCKET_ERROR)
		return;

	if (FD_ISSET(_clientSocket, &_readSet))
	{
		ProcessRecv();
	}

	// send啊 啊瓷且 锭付促 true
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
	std::vector<char> packet(packetSize);
	packet[0] = packetSize;
	recv(_clientSocket, packet.data() + sizeof(char), packetSize - sizeof(char), MSG_WAITALL);
	
	PACKET_TYPE id;
	memcpy(&id, packet.data() + sizeof(char), sizeof(PACKET_TYPE));

	// Data 眠免
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
	}
}

void GameNetwork::SendLoginPacket(const char* id)
{
	// Packet Data 积己
	C2S_Login packet;
	packet.size = sizeof(C2S_Login);
	packet.type = C2S_LOGIN;
	memcpy(packet.username, id, MAX_NAME_LEN);
	
	// Packet Serialize
	std::vector<char> serializedPacketData = SerializePOD(packet);

	// SendEvent 积己
	NetworkEventRef event = std::make_shared<NetworkEvent>();
	event->packetID = C2S_LOGIN;
	event->serializedPacketData = serializedPacketData;
	_sendEvents.push_back(event);
}

void GameNetwork::SendLogoutPacket()
{
	// Packet Data 积己
	C2S_Logout packetData{ sizeof(C2S_Logout), C2S_LOGOUT };

	// Packet Serialize
	std::vector<char> serializedPacketData = SerializePOD(packetData);

	// SendEvent 积己
	NetworkEventRef event = std::make_shared<NetworkEvent>();
	event->packetID = C2S_LOGOUT;
	event->serializedPacketData = serializedPacketData;
	_sendEvents.push_back(event);
}

void GameNetwork::SendMovePacket(PlayerRef player)
{
	// Packet Data 积己
	C2S_Move packetData{ sizeof(C2S_Move), C2S_MOVE, player->GetPos().x, player->GetPos().y, 0 };

	// Packet Serialize
	std::vector<char> serializedPacketData = SerializePOD(packetData);

	// SendEvent 积己
	NetworkEventRef event = std::make_shared<NetworkEvent>();
	event->packetID = C2S_MOVE;
	event->serializedPacketData = serializedPacketData;
	_sendEvents.push_back(event);
}

void GameNetwork::SendAttackPacket()
{
	// Packet Data 积己
	C2S_Attack packetData{ sizeof(C2S_Attack), C2S_ATTACK };

	// Packet Serialize
	std::vector<char> serializedPacketData = SerializePOD(packetData);

	// SendEvent 积己
	NetworkEventRef event = std::make_shared<NetworkEvent>();
	event->packetID = C2S_ATTACK;
	event->serializedPacketData = serializedPacketData;
	_sendEvents.push_back(event);
}

//void GameNetwork::SendGetItemPacket(int itemID, bool isTool, int playerID)
//{
//	// Packet Data 积己
//	C_GetItem_Packet packetData{ sizeof(C_GetItem_Packet), C_GetItem, itemID, playerID, isTool };
//
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//
//	// SendEvent 积己
//	NetworkEventRef event = std::make_shared<NetworkEvent>();
//	event->packetID = C_GetItem;
//	event->serializedPacketData = serializedPacketData;
//	std::lock_guard<std::mutex> lock(_sendMutex);
//	_sendEvents.push_back(event);
//}
//
//void GameNetwork::SendDropItemPacket(int itemID, bool isTool, int playerID)
//{
//	// Packet Data 积己
//	C_DropItem_Packet packetData{ sizeof(C_DropItem_Packet), C_DropItem, itemID, playerID, isTool };
//
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//
//	// SendEvent 积己
//	NetworkEventRef event = std::make_shared<NetworkEvent>();
//	event->packetID = C_DropItem;
//	event->serializedPacketData = serializedPacketData;
//	std::lock_guard<std::mutex> lock(_sendMutex);
//	_sendEvents.push_back(event);
//}
//
//void GameNetwork::SendSellItemPacket(int playerID, int sellingMachineID)
//{
//	// Packet Data 积己
//	C_SellItem_Packet packetData{ sizeof(C_SellItem_Packet), C_SellItem, sellingMachineID, playerID };
//
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//
//	// SendEvent 积己
//	NetworkEventRef event = std::make_shared<NetworkEvent>();
//	event->packetID = C_SellItem;
//	event->serializedPacketData = serializedPacketData;
//	std::lock_guard<std::mutex> lock(_sendMutex);
//	_sendEvents.push_back(event);
//}
//
//void GameNetwork::SendBuyItemPacket(int playerID, ItemType itemType, int itemCount)
//{
//	// Packet Data 积己
//	C_BuyItem_Packet packetData{ sizeof(C_BuyItem_Packet), C_BuyItem, playerID, itemType, itemCount };
//	
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//	
//	// SendEvent 积己
//	NetworkEventRef event = std::make_shared<NetworkEvent>();
//	event->packetID = C_BuyItem;
//	event->serializedPacketData = serializedPacketData;
//	std::lock_guard<std::mutex> lock(_sendMutex);
//	_sendEvents.push_back(event);
//}

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
	// ID啊 Avatar狼 ID客 鞍促搁 Avatar狼 谅钎 荐沥
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