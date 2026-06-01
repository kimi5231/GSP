#include "pch.h"
#include "ServerNetwork.h"
#include "ServerFramework.h"
#include "Session.h"
#include "ExpOver.h"
#include "Global.h"
#include "Player.h"

ServerNetwork::ServerNetwork(ServerFramework* framework)
{
	_framework = framework;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "윈속 초기화 실패" << std::endl;
		return;
	}

	// listenSocket 생성
	_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (_listenSocket == INVALID_SOCKET)
	{
		std::cout << "listenSocket 생성 실패" << std::endl;
		return;
	}

	// bind
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);
	if (bind(_listenSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << "bind 실패" << std::endl;
		return;
	}

	// listen
	if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "listen 실패" << std::endl;
		return;
	}

	// iocp port 생성
	_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	
	// listenSocket 등록
	CreateIoCompletionPort((HANDLE)_listenSocket, _iocp, 0, 0);

	// accept
	_tempSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	_acceptOver._ioType = IOType::Accept;
	AcceptEx(_listenSocket, _tempSocket, _acceptOver._buffer.data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, NULL, &_acceptOver._over);

	for (int i = 0; i < MAX_PLAYERS; ++i)
		_clients[i] = new Session();
}

ServerNetwork::~ServerNetwork()
{
	// socket close
	closesocket(_listenSocket);
	closesocket(_tempSocket);

	// 윈속 종료
	WSACleanup();
}

void ServerNetwork::Update()
{
	DWORD numByte;
	ULONG_PTR key;
	LPOVERLAPPED over;
	GetQueuedCompletionStatus(_iocp, &numByte, &key, &over, 0);
	
	if (over == nullptr)
	{
		if (key == -1)
			exit(-1);
		return;
	}

	ExpOver* expOver = reinterpret_cast<ExpOver*>(over);
	switch (expOver->_ioType)
	{
	case IOType::Accept:
		ProcessAccept();
		break;
	case IOType::Recv:
		ProcessRecv(static_cast<int>(key), numByte, expOver);
		break;
	case IOType::Send:
		delete expOver;
		break;
	default:
		std::cout << "Unknown IO type.\n";
		exit(-1);
		break;
	}
}

void ServerNetwork::ProcessAccept()
{
	// 할당할 수 있는 Client Session 찾기
	int clientIndex = -1;
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (!_clients[i]->_isConnected)
		{
			clientIndex = i;
			break;
		}
	}

	// 할당할 수 있는 Client Session이 없다면 무시
	if (clientIndex == -1)
		return;

	std::cout << "Client[" << clientIndex << "] 접속" << std::endl;

	// clientSocket 등록
	CreateIoCompletionPort((HANDLE)_tempSocket, _iocp, clientIndex, 0);
	_clients[clientIndex]->_clientSocket = _tempSocket;
	_clients[clientIndex]->_id = clientIndex;
	_clients[clientIndex]->_isConnected = true;
	_clients[clientIndex]->_prevRecv = 0;

	_clients[clientIndex]->Recv();

	// accept 다시 걸기
	_tempSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	_acceptOver._ioType = IOType::Accept;
	AcceptEx(_listenSocket, _tempSocket, _acceptOver._buffer.data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, NULL, &_acceptOver._over);
}

void ServerNetwork::ProcessDisconnected(int clientIndex)
{
	std::cout << "client[" << clientIndex << "] 접속 종료\n";
	closesocket(_clients[clientIndex]->_clientSocket);
	_clients[clientIndex]->_clientSocket = INVALID_SOCKET;
	_clients[clientIndex]->_isConnected = false;
	//_clients[clientIndex]->_room->RemoveObject(ObjectType::Player, _clients[clientIndex]->_player->GetID(), true);
}

void ServerNetwork::ProcessRecv(int clientIndex, int numByte, ExpOver* expOver)
{
	// Client 접속 종료
	if (numByte == 0)
	{
		std::cout << "client[" << clientIndex << "] 접속 종료\n";
		_clients[clientIndex]->_isConnected = false;
		closesocket(_clients[clientIndex]->_clientSocket);
		_clients[clientIndex]->_clientSocket = INVALID_SOCKET;
		g_framework->RemoveObject(ObjectType::Player, _clients[clientIndex]->_player->GetID(), true);
		return;
	}

	// 처리해야할 데이터 조합하기
	std::vector<char> packet;
	packet.insert(packet.end(), _clients[clientIndex]->_recvOver._buffer.begin(), _clients[clientIndex]->_recvOver._buffer.begin() + _clients[clientIndex]->_prevRecv);
	packet.insert(packet.end(), expOver->_buffer.begin(), expOver->_buffer.begin() + numByte);
	
	int dataSize = numByte + _clients[clientIndex]->_prevRecv;
	while (dataSize > 0)
	{
		// packetSize 추출
		char packetSize;
		memcpy(&packetSize, packet.data(), sizeof(char));

		// 남은 데이터의 사이즈가 패킷의 사이즈보다 작으면 중단
		if (packetSize > dataSize)
			break;
		
		ProcessPacket(packet, clientIndex);
		
		// 처리한 패킷 길이만큼 제외
		dataSize -= packetSize;
	}

	// 남은 데이터 저장
	if (dataSize > 0)
	{
		memmove(_clients[clientIndex]->_recvOver._buffer.data(), packet.data(), dataSize);
		_clients[clientIndex]->_prevRecv = dataSize;
	}

	_clients[clientIndex]->Recv();
}

void ServerNetwork::ProcessPacket(std::vector<char>& packet, int clientIndex)
{
	// packetID 추출
	PACKET_TYPE id;
	memcpy(&id, packet.data() + sizeof(unsigned char), sizeof(PACKET_TYPE));

	switch (id)
	{
	case C2S_LOGIN:
	{
		unsigned char packetSize;
		memcpy(&packetSize, packet.data(), sizeof(unsigned char));
		packet.erase(packet.begin(), packet.begin() + sizeof(unsigned char) + sizeof(PACKET_TYPE));
		
		C2S_Login loginPacket;
		loginPacket.size = packetSize;
		loginPacket.type = C2S_LOGIN;
		memcpy(&loginPacket.username, packet.data(), sizeof(char) * MAX_NAME_LEN);
		ProcessLoginPacket(loginPacket, clientIndex);
		break;
	}
	case C2S_MOVE:
	{
		C2S_Move movePacket;
		memcpy(&movePacket, packet.data(), sizeof(C2S_Move));
		packet.erase(packet.begin(), packet.begin() + sizeof(C2S_Move));
		ProcessMovePacket(movePacket, clientIndex);
		break;
	}
	case C2S_LOGOUT:
	{
		C2S_Logout logoutPacket;
		memcpy(&logoutPacket, packet.data(), sizeof(C2S_Logout));
		packet.erase(packet.begin(), packet.begin() + sizeof(C2S_Logout));
		ProcessLogoutPacket(logoutPacket, clientIndex);
		break;
	}
	
	/*case C_UpdateObjectState:
	{
		C_UpdateObjectState_Packet updateObjectPacket;
		memcpy(&updateObjectPacket, packet.data(), sizeof(C_UpdateObjectState_Packet));
		packet.erase(packet.begin(), packet.begin() + sizeof(C_UpdateObjectState_Packet));
		ProcessUpdateObjectStatePacket(updateObjectPacket, clientIndex);
		break;
	}
	case C_GetItem:
	{
		C_GetItem_Packet getItemPacket;
		memcpy(&getItemPacket, packet.data(), sizeof(C_GetItem_Packet));
		packet.erase(packet.begin(), packet.begin() + sizeof(C_GetItem_Packet));
		ProcessGetItemPacket(getItemPacket, clientIndex);
		break;
	}
	case C_DropItem:
	{
		C_DropItem_Packet dropItemPacket;
		memcpy(&dropItemPacket, packet.data(), sizeof(C_DropItem_Packet));
		packet.erase(packet.begin(), packet.begin() + sizeof(C_DropItem_Packet));
		ProcessDropItemPacket(dropItemPacket, clientIndex);
		break;
	}
	case C_ChangeTool:
	{
		C_ChangeTool_Packet changeToolPacket;
		memcpy(&changeToolPacket, packet.data(), sizeof(C_ChangeTool_Packet));
		packet.erase(packet.begin(), packet.begin() + sizeof(C_ChangeTool_Packet));
		ProcessChangeToolPacket(changeToolPacket, clientIndex);
		break;
	}
	case C_UseTool:
	{
		C_UseTool_Packet useToolPacket;
		memcpy(&useToolPacket, packet.data(), sizeof(C_UseTool_Packet));
		packet.erase(packet.begin(), packet.begin() + sizeof(C_UseTool_Packet));
		ProcessUseToolPacket(useToolPacket, clientIndex);
		break;
	}
	case C_SellItem:
	{
		C_SellItem_Packet sellItemPacket;
		memcpy(&sellItemPacket, packet.data(), sizeof(C_SellItem_Packet));
		packet.erase(packet.begin(), packet.begin() + sizeof(C_SellItem_Packet));
		ProcessSellItemPacket(sellItemPacket, clientIndex);
		break;
	}
	case C_BuyItem:
	{
		C_BuyItem_Packet buyItemPacket;
		memcpy(&buyItemPacket, packet.data(), sizeof(C_BuyItem_Packet));
		packet.erase(packet.begin(), packet.begin() + sizeof(C_BuyItem_Packet));
		ProcessBuyItemPacket(buyItemPacket, clientIndex);
		break;
	}*/
	}
}

void ServerNetwork::SendLoginResultPacket(bool result, const char* message, Session* client)
{
	S2C_LoginResult packet;
	packet.size = sizeof(S2C_LoginResult);
	packet.type = S2C_LOGIN_RESULT;
	packet.success = result;
	strncpy_s(packet.message, message, sizeof(packet.message) - 1);

	client->Send(packet.size, reinterpret_cast<char*>(&packet));
}

void ServerNetwork::SendAvatarInfoPacket(Player* player, Session* client)
{
	S2C_AvatarInfo packet{ sizeof(S2C_AvatarInfo), S2C_AVATAR_INFO, player->GetID(), 0, player->GetPos().x, player->GetPos().y, player->GetHP(), player->GetMaxHP(), player->GetEXP(),  player->GetLevel() };

	client->Send(packet.size, reinterpret_cast<char*>(&packet));
}

void ServerNetwork::SendAddObjectPacket(Player* player, Session* client)
{
	S2C_AddObject packet;
	packet.size = sizeof(S2C_AddObject);
	packet.type = S2C_ADD_OBJECT;

	client->Send(packet.size, reinterpret_cast<char*>(&packet));
}

//void ServerNetwork::SendRemoveObjectPacket(ObjectType objectType, int objectID, Session* client)
//{
//	// Packet Data 생성
//	S_RemoveObject_Packet packetData{ sizeof(S_RemoveObject_Packet), S_RemoveObject, objectID, objectType };
//
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//
//	client->Send(serializedPacketData);
//}

void ServerNetwork::SendMoveObjectPacket(GameObject* object, Session* client)
{
	// Packet Data 생성
	S2C_MoveObject packet{ sizeof(S2C_MoveObject), S2C_MOVE_OBJECT, object->GetID(), object ->GetPos().x, object->GetPos().y, 0 };

	client->Send(packet.size, reinterpret_cast<char*>(&packet));
}

//void ServerNetwork::SendUpdateObjectStatePacket(GameObject* object, Session* client)
//{
//	// Packet Data 생성
//	S_UpdateObjectState_Packet packetData{ sizeof(S_UpdateObjectState_Packet), S_UpdateObjectState, object->GetID(), object->GetObjectType(), object->GetState() };
//
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//
//	client->Send(serializedPacketData);
//}
//
//void ServerNetwork::SendAddItemToInventoryPacket(Item* item, bool isTool, Session* client)
//{
//	// Packet Data 생성
//	S_AddItemToInventory_Packet packetData{ sizeof(S_AddItemToInventory_Packet), S_AddItemToInventory, item->GetID(), isTool, item->GetItemType(), item->GetWeight() };
//
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//
//	client->Send(serializedPacketData);
//}
//
//void ServerNetwork::SendRemoveItemFromInventoryPacket(Item* item, bool isTool, Session* client)
//{
//	// Packet Data 생성
//	S_RemoveItemFromInventory_Packet packetData{ sizeof(S_RemoveItemFromInventory_Packet), S_RemoveItemFromInventory, item->GetID(), isTool, item->GetItemType() };
//
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//
//	client->Send(serializedPacketData);
//}
//
//void ServerNetwork::SendDropItemPacket(Item* item, int playerID, Vector itemPos, bool isTool, bool isToSellingMachine, Session* client)
//{
//	// Packet Data 생성
//	S_DropItem_Packet packetData{ sizeof(S_DropItem_Packet), S_DropItem, item->GetID(), playerID, isTool, isToSellingMachine, item->GetItemType(), itemPos, item->GetCost(), 0};
//
//	// 아이템이 랜턴이라면 배터리 추가
//	if (item->GetItemType() == ItemType::LANTERN)
//	{
//		Lantern* lantern = dynamic_cast<Lantern*>(item);
//		packetData.laternBattery = lantern->GetCurrentBattery();
//	}
//
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//
//	client->Send(serializedPacketData);
//}
//
//void ServerNetwork::SendUpdateCurrentToolPacket(int itemID, int playerID, ItemType type, Session* client)
//{
//	// Packet Data 생성
//	S_UpdateCurrentTool_Packet packetData{ sizeof(S_UpdateCurrentTool_Packet), S_UpdateCurrentTool, itemID, playerID, type };
//
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//
//	client->Send(serializedPacketData);
//}
//
//void ServerNetwork::SendUseToolPacket(int playerID, ItemType type, Session* client)
//{
//	// Packet Data 생성
//	S_UseTool_Packet packetData{ sizeof(S_UseTool_Packet), S_UseTool, playerID, type };
//
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//
//	client->Send(serializedPacketData);
//}
//
//void ServerNetwork::SendSellItemResultPacket(char playerID, char sellingMachineID, ObjectState sellingMachineState, short remainCredit, short collectCredit, short currentCredit, std::vector<int>& sellItems, Session* client)
//{
//	// Packet Serialize
//	PacketID packetID = S_SellItemResult;
//	std::vector<char> itemIDs = SerializeVector(sellItems);
//	unsigned short packetSize = sizeof(unsigned short) + sizeof(PacketID) + sizeof(char) + sizeof(char) + sizeof(ObjectState) + sizeof(short) + sizeof(short) + sizeof(short) + itemIDs.size();
//	std::vector<char> serializedPacketData(packetSize - itemIDs.size());
//
//	memcpy(serializedPacketData.data(), &packetSize, sizeof(unsigned short));
//	memcpy(serializedPacketData.data() + sizeof(unsigned short), &packetID, sizeof(unsigned char));
//	memcpy(serializedPacketData.data() + sizeof(unsigned short) + sizeof(PacketID), &sellingMachineID, sizeof(unsigned char));
//	memcpy(serializedPacketData.data() + sizeof(unsigned short) + sizeof(PacketID) + sizeof(unsigned char), &playerID, sizeof(unsigned char));
//	memcpy(serializedPacketData.data() + sizeof(unsigned short) + sizeof(PacketID) + sizeof(unsigned char) + sizeof(unsigned char), &remainCredit, sizeof(unsigned short));
//	memcpy(serializedPacketData.data() + sizeof(unsigned short) + sizeof(PacketID) + sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned short), &collectCredit, sizeof(unsigned short));
//	memcpy(serializedPacketData.data() + sizeof(unsigned short) + sizeof(PacketID) + sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned short) + sizeof(unsigned short), &currentCredit, sizeof(unsigned short));
//	memcpy(serializedPacketData.data() + sizeof(unsigned short) + sizeof(PacketID) + sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned short) + sizeof(unsigned short) + sizeof(unsigned short), &sellingMachineState, sizeof(ObjectState));
//	serializedPacketData.insert(serializedPacketData.end(), itemIDs.begin(), itemIDs.end());
//
//	client->Send(serializedPacketData);
//}
//
//void ServerNetwork::SendBuyItemResultPacket(short currentCredit, Session* client)
//{
//	// Packet Data 생성
//	S_BuyItemResult_Packet packetData{ sizeof(S_BuyItemResult_Packet), S_BuyItemResult, currentCredit };
//
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//
//	client->Send(serializedPacketData);
//}
//
//void ServerNetwork::SendUpdateHpPacket(int playerID, int hp, Session* client)
//{
//	// Packet Data 생성
//	S_UpdateHp_Packet packetData{ sizeof(S_UpdateHp_Packet), S_UpdateHp, playerID, hp };
//
//	// Packet Serialize
//	std::vector<char> serializedPacketData = SerializePOD(packetData);
//
//	client->Send(serializedPacketData);
//}

void ServerNetwork::ProcessLoginPacket(C2S_Login packet, int clientIndex)
{
	// 데이터 베이스에 ID가 존재하는지 확인
	// ID가 존재한다면 정보 가져오기

	// ID가 없다면 데이터베이스에 추가 후, 새로운 Player 생성
	_clients[clientIndex]->_player = g_framework->AddPlayer();
	// 삭제 예정
	_clients[clientIndex]->_player->SetClient(_clients[clientIndex]);

	// 새로 접속한 Client에게 자신을 나타낼 Player 정보 전송
	SendAvatarInfoPacket(_clients[clientIndex]->_player, _clients[clientIndex]);

	// 새로 접속한 Client에게 기존에 있던 Object 정보 전송
	// 시야 처리 필요
	for (auto& client : _clients)
	{
		if (!client->_isConnected)
			continue;

		// 자기 자신 제외
		if (_clients[clientIndex]->_id == client->_id)
			continue;

		SendAddObjectPacket(client->_player, _clients[clientIndex]);
	}

	/*for (auto& monster : _clients[clientIndex]->_room->GetMonsters())
	{
		if (monster->GetObjectPoolState() == ObjectPoolState::InWorld)
			SendAddMonsterPacket(monster, _clients[clientIndex]);
	}

	for (auto& item : _clients[clientIndex]->_room->GetItems())
	{
		if (item->GetObjectPoolState() == ObjectPoolState::InWorld)
			SendAddItemPacket(item, dynamic_cast<Tool*>(item), _clients[clientIndex]);
	}*/
}

void ServerNetwork::ProcessLogoutPacket(C2S_Logout packet, int clientIndex)
{
	
}

void ServerNetwork::ProcessMovePacket(C2S_Move packet, int clientIndex)
{
	Player* player = _clients[clientIndex]->_player;

	if (player == nullptr)
		return;

	player->SetPos({packet.x, packet.y});
	
	// 자신을 제외한 모든 Client들에게 알리기
	for (auto& client : _clients)
	{
		if (!client->_isConnected)
			continue;

		// 자기 자신 제외
		if (client == _clients[clientIndex])
			continue;

		SendMoveObjectPacket(player, client);
	}
}

//void ServerNetwork::ProcessGetItemPacket(C_GetItem_Packet packet, int clientIndex)
//{
//	// Player가 요청한 아이템이 얻을 수 있는 것인지 확인
//	Item* item = dynamic_cast<Item*>(_clients[clientIndex]->_room->GetGameObject(ObjectType::Item, packet.itemID));
//	if (item->GetObjectPoolState() != ObjectPoolState::InWorld)
//		return;
//
//	// 아이템을 얻을 수 있는 조건인지 확인(거리)
//	
//	// 아이템이 판매기 안에 있던 거라면 판매기에서 제외
//	const std::vector<SellingMachine*>& sellingMachines = _clients[clientIndex]->_room->GetSellingMachine();
//	for (auto sellingMachine : sellingMachines)
//	{
//		if (sellingMachine->ExistItem(packet.itemID))
//		{
//			sellingMachine->RemoveItem(packet.itemID);
//			break;
//		}
//	}
//
//	// 얻을 수 있는 아이템이라면 Player 인벤토리에 추가
//	Player* player = dynamic_cast<Player*>(_clients[clientIndex]->_room->GetGameObject(ObjectType::Player, packet.playerID));
//	// 아이템이 제대로 추가되었다면
//	if (player->AddItemToInventory(packet.isTool, packet.itemID))
//	{
//		// 획득한 아이템 ObjectPoolState 변경
//		item->SetObjectPoolState(ObjectPoolState::InInventory);
//		// ownerID 설정
//		item->SetOwnerID(player->GetID());
//
//		// 아이템을 획득한 Player에게 인벤토리에 아이템 추가 알림
//		SendAddItemToInventoryPacket(item, packet.isTool, player->GetClient());
//
//		// Broadcast
//		for (auto& p : _clients[clientIndex]->_room->GetPlayers())
//		{
//			if (!p->GetClient())
//				continue;
//
//			// 자기 자신 제외
//			if (p == player)
//				continue;
//
//			SendItemPickupNotifyPacket(item, player->GetID(), packet.isTool, p->GetClient());
//		}
//	}
//}
//
//void ServerNetwork::ProcessDropItemPacket(C_DropItem_Packet packet, int clientIndex)
//{
//	// Player 인벤토리에서 아이템 제거
//	Player* player = dynamic_cast<Player*>(_clients[clientIndex]->_room->GetGameObject(ObjectType::Player, packet.playerID));
//	// 아이템이 제대로 제거되었다면
//	if (player->RemoveItemFromInventory(packet.isTool, packet.itemID))
//	{
//		// 떨어뜨린 아이템 정보 수정
//		Item* item = dynamic_cast<Item*>(_clients[clientIndex]->_room->GetGameObject(ObjectType::Item, packet.itemID));
//		item->SetPos(player->GetPos());
//		item->SetObjectPoolState(ObjectPoolState::InWorld);
//		// ownerID 초기화
//		item->SetOwnerID(-1);
//
//		// Broadcast
//		for (auto& p : _clients[clientIndex]->_room->GetPlayers())
//		{
//			if (!p->GetClient())
//				continue;
//
//			SendDropItemPacket(item, player->GetID(), item->GetPos(), packet.isTool, false,  p->GetClient());
//		}
//	}
//}
//
//void ServerNetwork::ProcessChangeToolPacket(C_ChangeTool_Packet packet, int clientIndex)
//{
//	Player* player = dynamic_cast<Player*>(_clients[clientIndex]->_room->GetGameObject(ObjectType::Player, packet.playerID));
//
//	// toolID가 0이면 도구를 들지 않는 것
//	if(packet.toolID == 0)
//	{
//		player->SetCurrentTool(0);
//
//		// Broadcast
//		for (auto& p : _clients[clientIndex]->_room->GetPlayers())
//		{
//			if (!p->GetClient())
//				continue;
//
//			SendUpdateCurrentToolPacket(packet.toolID, packet.playerID, ItemType::None, p->GetClient());
//		}
//		return;
//	}
//
//	// Player 인벤토리에 해당 도구가 존재하는지 확인
//	if (player->ExistItem(true, packet.toolID))
//	{
//		// 도구가 존재하면 해당 도구를 들도록 설정
//		player->SetCurrentTool(packet.toolID);
//		Item* item = dynamic_cast<Item*>(_clients[clientIndex]->_room->GetGameObject(ObjectType::Item, packet.toolID));
//		
//		// Broadcast
//		for (auto& p : _clients[clientIndex]->_room->GetPlayers())
//		{
//			if (!p->GetClient())
//				continue;
//
//			SendUpdateCurrentToolPacket(packet.toolID, packet.playerID, item->GetItemType(), p->GetClient());
//		}
//	}
//}
//
//void ServerNetwork::ProcessUseToolPacket(C_UseTool_Packet packet, int clientIndex)
//{
//	// 요청된 도구가 Player가 들고 있는 도구가 맞는지 확인
//	Player* player = dynamic_cast<Player*>(_clients[clientIndex]->_room->GetGameObject(ObjectType::Player, packet.playerID));
//	if (player->GetCurrentTool() == packet.toolID)
//	{
//		// 도구 사용 처리
//		player->SetRotation(packet.playerRotation);
//		player->Attack(_clients[clientIndex]->_room);
//
//		// 도구 사용 알리기
//		Tool* tool = dynamic_cast<Tool*>(_clients[clientIndex]->_room->GetGameObject(ObjectType::Item, packet.toolID));
//		
//		// Broadcast
//		for (auto& p : _clients[clientIndex]->_room->GetPlayers())
//		{
//			if (!p->GetClient())
//				continue;
//
//			SendUseToolPacket(packet.playerID, tool->GetItemType(), p->GetClient());
//		}
//	}
//}
//
//void ServerNetwork::ProcessSellItemPacket(C_SellItem_Packet packet, int clientIndex)
//{
//	// 요청한 Player가 판매기와 거리가 되는지 확인 
//	Player* player = dynamic_cast<Player*>(_clients[clientIndex]->_room->GetGameObject(ObjectType::Player, packet.playerID));
//	SellingMachine* sellingMachine = dynamic_cast<SellingMachine*>(_clients[clientIndex]->_room->GetGameObject(ObjectType::SellingMachine, packet.sellingMachineID));
//
//	// 판매기가 활성화 상태인지 확인
//	if (sellingMachine->GetState() == ObjectState::CLOSE)
//		return;
//
//	// 아이템 판매 및 아이템 제거
//	int remainCredit = sellingMachine->SellItem(_clients[clientIndex]->_room);
//	std::vector<int>& sellItems = sellingMachine->GetSellItems();
//	for (auto& itemID : sellItems)
//		_clients[clientIndex]->_room->RemoveObject(ObjectType::Item, itemID, false);
//		
//	// Broadcast
//	for (auto& p : _clients[clientIndex]->_room->GetPlayers())
//	{
//		if (!p->GetClient())
//			continue;
//
//		SendSellItemResultPacket(player->GetID(), sellingMachine->GetID(), sellingMachine->GetState(), remainCredit, _clients[clientIndex]->_room->GetCollectCredit(), _clients[clientIndex]->_room->GetCurrentCredit(), sellItems, p->GetClient());
//	}
//
//	// 데이터 사용을 위해 나중에 초기화
//	sellingMachine->ClearSellItems();
//}
//
//void ServerNetwork::ProcessBuyItemPacket(C_BuyItem_Packet packet, int clientIndex)
//{
//	// 요청한 Player가 아이템을 구매할 수 있는 상태인지 확인
//	Player* player = dynamic_cast<Player*>(_clients[clientIndex]->_room->GetGameObject(ObjectType::Player, packet.playerID));
//
//	// 구매가 가능한 상태라면 크레딧이 충분한지 확인
//	ItemInfo info = g_dataManager->GetItemInfo(packet.itemType);
//	int needCredit = info.cost * packet.itemCount;
//
//	// 크레딧이 충분하다면 구매 완료 및 Base에 아이템 생성
//	if (_clients[clientIndex]->_room->GetCurrentCredit() >= needCredit)
//	{
//		for (int i = 0; i < packet.itemCount; ++i)
//			_clients[clientIndex]->_room->AddItem(true, packet.itemType, {0, 0, 0});
//		
//		// 아이템 가격만큼 크레딧 마이너스
//		_clients[clientIndex]->_room->MinusCredit(needCredit);
//	}
//
//	// 아이템 구매 결과 전송
//	for (auto& p : _clients[clientIndex]->_room->GetPlayers())
//	{
//		if (!p->GetClient())
//			continue;
//
//		SendBuyItemResultPacket(_clients[clientIndex]->_room->GetCurrentCredit(), p->GetClient());
//	}
//}