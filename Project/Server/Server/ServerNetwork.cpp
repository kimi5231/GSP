#include "pch.h"
#include "ServerNetwork.h"
#include "ServerFramework.h"
#include "Session.h"
#include "ExpOver.h"
#include "Global.h"
#include "Player.h"
#include "Monster.h"
#include "Weapon.h"

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

	// Monster Sector 등록
	const std::array<Monster*, NUM_NPCS>& monsters = _framework->GetMonsters();
	for (const auto& monster : monsters)
	{
		Vector sectorIndex{ monster->GetPos().x / (SECTOR_SIZE * TILE_SIZE), monster->GetPos().y / (SECTOR_SIZE * TILE_SIZE) };

		Vector start{ std::max(0, sectorIndex.x - 1), std::max(0, sectorIndex.y - 1) };
		Vector end{ std::min(WORLD_WIDTH / SECTOR_SIZE, sectorIndex.x + 1), std::min(WORLD_HEIGHT / SECTOR_SIZE, sectorIndex.y + 1) };

		for (int x = start.x; x < end.x; ++x)
		{
			for (int y = start.y; y < end.y; ++y)
			{
				_sectors[x][y].sectorMutex.lock();
				_sectors[x][y].objects.insert(monster->GetID());
				_sectors[x][y].sectorMutex.unlock();
			}
		}
	}
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
	GetQueuedCompletionStatus(_iocp, &numByte, &key, &over, INFINITE);
	
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
	case IOType::MonsterEvent:
		ProcessMonsterEvent(static_cast<int>(key), expOver);
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
	{
		// accept 다시 걸기
		_tempSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		_acceptOver._ioType = IOType::Accept;
		AcceptEx(_listenSocket, _tempSocket, _acceptOver._buffer.data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, NULL, &_acceptOver._over);
		return;
	}
	
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
	if (_clients[clientIndex]->_state == SessionState::Play)
	{
		// 시야 처리
		{
			const std::array<Player*, MAX_PLAYERS>& players = _framework->GetPlayers();
			Vector sectorIndex{ players[clientIndex]->GetPos().x / (SECTOR_SIZE * TILE_SIZE), players[clientIndex]->GetPos().y / (SECTOR_SIZE * TILE_SIZE)};
			_sectors[sectorIndex.x][sectorIndex.y].sectorMutex.lock();
			_sectors[sectorIndex.x][sectorIndex.y].objects.erase(clientIndex);
			_sectors[sectorIndex.x][sectorIndex.y].sectorMutex.unlock();

			Vector start{ std::max(0, sectorIndex.x - 1), std::max(0, sectorIndex.y - 1) };
			Vector end{ std::min(WORLD_WIDTH / SECTOR_SIZE, sectorIndex.x + 1), std::min(WORLD_HEIGHT / SECTOR_SIZE, sectorIndex.y + 1) };

			const std::array<Monster*, NUM_NPCS>& monsters = _framework->GetMonsters();
			std::unordered_set<int> currentView;
			for (int x = start.x; x < end.x; ++x)
			{
				for (int y = start.y; y < end.y; ++y)
				{
					_sectors[x][y].sectorMutex.lock();
					for (int id : _sectors[x][y].objects)
					{
						// Monster
						if (id >= MONSTER_ID)
						{
							if (players[clientIndex]->IsVisiable(monsters[id - MONSTER_ID]->GetPos()))
								_framework->RemoveAliveMonster(id);
							continue;
						}

						if (_clients[id]->_state == SessionState::Play && players[clientIndex]->IsVisiable(players[id]->GetPos()))
							currentView.insert(id);
					}
					_sectors[x][y].sectorMutex.unlock();
				}
			}

			// 시야에 있는 Player에게 삭제 알림
			for (int id : currentView)
				SendRemoveObjectPacket(players[clientIndex], _clients[id]);

			// View List 업데이트
			_clients[clientIndex]->_viewLock.lock();
			_clients[clientIndex]->_viewList.clear();
			_clients[clientIndex]->_viewLock.unlock();
		}

		_framework->RemoveObject(ObjectType::Player, clientIndex);
	}
	_clients[clientIndex]->_state = SessionState::Title;
}

void ServerNetwork::ProcessRecv(int clientIndex, int numByte, ExpOver* expOver)
{
	// Client 접속 종료
	if (numByte == 0)
	{
		ProcessDisconnected(clientIndex);
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
		C2S_Login loginPacket;
		memcpy(&loginPacket, packet.data(), sizeof(C2S_Login));
		packet.erase(packet.begin(), packet.begin() + sizeof(C2S_Login));
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
	case C2S_ATTACK:
	{
		C2S_Attack attackPacket;
		memcpy(&attackPacket, packet.data(), sizeof(C2S_Attack));
		packet.erase(packet.begin(), packet.begin() + sizeof(C2S_Attack));
		ProcessAttackPacket(attackPacket, clientIndex);
		break;
	}
	case C2S_SKILL:
	{
		C2S_Skill skillPacket;
		memcpy(&skillPacket, packet.data(), sizeof(C2S_Skill));
		packet.erase(packet.begin(), packet.begin() + sizeof(C2S_Skill));
		ProcessSkillPacket(skillPacket, clientIndex);
		break;
	}
	case C2S_DROP_ITEM:
	{
		C2S_DropItem dropItemPacket;
		memcpy(&dropItemPacket, packet.data(), sizeof(C2S_DropItem));
		packet.erase(packet.begin(), packet.begin() + sizeof(C2S_DropItem));
		ProcessDropItemPacket(dropItemPacket, clientIndex);
		break;
	}
	case C2S_CHANGE_WEAPON:
	{
		C2S_ChageWeapon changeWeaponPacket;
		memcpy(&changeWeaponPacket, packet.data(), sizeof(C2S_ChageWeapon));
		packet.erase(packet.begin(), packet.begin() + sizeof(C2S_ChageWeapon));
		ProcessChangeWeaponPacket(changeWeaponPacket, clientIndex);
		break;
	}
	case C2S_TELEPORT:
	{
		C2S_Teleport teleportPacket;
		memcpy(&teleportPacket, packet.data(), sizeof(C2S_Teleport));
		packet.erase(packet.begin(), packet.begin() + sizeof(C2S_Teleport));
		ProcessTeleportPacket(teleportPacket, clientIndex);
		break;
	}
	}
}

void ServerNetwork::ProcessMonsterEvent(int monsterID, ExpOver* expOver)
{
	switch (expOver->_monsterEventType)
	{
	case MonsterEventType::Move:
	{
		const std::array<Monster*, NUM_NPCS>& monsters = _framework->GetMonsters();
		Monster* monster = monsters[monsterID - MONSTER_ID];

		if (monster->GetObjectState() == ObjectState::DEAD)
		{
			delete expOver;
			return;
		}

		Vector oldIndex{ monster->GetPrevPos().x / (SECTOR_SIZE * TILE_SIZE), monster->GetPrevPos().y / (SECTOR_SIZE * TILE_SIZE)};
		Vector newIndex{ monster->GetPos().x / (SECTOR_SIZE * TILE_SIZE), monster->GetPos().y / (SECTOR_SIZE * TILE_SIZE) };

		// 섹터 옮기기 
		if (oldIndex != newIndex)
		{
			_sectors[oldIndex.x][oldIndex.y].sectorMutex.lock();
			_sectors[oldIndex.x][oldIndex.y].objects.erase(monsterID);
			_sectors[oldIndex.x][oldIndex.y].sectorMutex.unlock();

			_sectors[newIndex.x][newIndex.y].sectorMutex.lock();
			_sectors[newIndex.x][newIndex.y].objects.insert(monsterID);
			_sectors[newIndex.x][newIndex.y].sectorMutex.unlock();
		}

		Vector newStart{ std::max(0, newIndex.x - 1), std::max(0, newIndex.y - 1) };
		Vector newEnd{ std::min(WORLD_WIDTH / SECTOR_SIZE - 1, newIndex.x + 1), std::min(WORLD_HEIGHT / SECTOR_SIZE - 1, newIndex.y + 1) };

		std::unordered_set<int> newNearPlayers;
		for (int x = newStart.x; x <= newEnd.x; ++x)
		{
			for (int y = newStart.y; y <= newEnd.y; ++y)
			{
				_sectors[x][y].sectorMutex.lock();
				for (int id : _sectors[x][y].objects)
				{
					if (id >= MONSTER_ID)
						continue;
					
					newNearPlayers.insert(id);
				}
				_sectors[x][y].sectorMutex.unlock();
			}
		}

		Vector oldStart{ std::max(0, oldIndex.x - 1), std::max(0, oldIndex.y - 1) };
		Vector oldEnd{ std::min(WORLD_WIDTH / SECTOR_SIZE - 1, oldIndex.x + 1), std::min(WORLD_HEIGHT / SECTOR_SIZE - 1, oldIndex.y + 1) };

		std::unordered_set<int> oldNearPlayers;
		for (int x = oldStart.x; x <= oldEnd.x; ++x)
		{
			for (int y = oldStart.y; y <= oldEnd.y; ++y)
			{
				_sectors[x][y].sectorMutex.lock();
				for (int id : _sectors[x][y].objects)
				{
					if (id >= MONSTER_ID)
						continue;

					oldNearPlayers.insert(id);
				}
				_sectors[x][y].sectorMutex.unlock();
			}
		}

		const std::array<Player*, MAX_PLAYERS>& players = _framework->GetPlayers();
		for (int id : newNearPlayers)
		{
			if (!oldNearPlayers.count(id))
				SendAddObjectPacket(monster, _clients[id]);
			else
				SendMoveObjectPacket(monster, _clients[id]);

			if (monster->IsAgro(players[id]->GetPos()) && monster->GetObjectType() == ObjectType::Agro && !monster->GetTarget())
			{
				monster->SetTarget(players[id]);
				monster->SetState(ObjectState::CHASE);
			}

			if (monster->IsNear(players[id]->GetPos()) && monster->GetObjectType() == ObjectType::Agro)
			{
				if(!monster->GetTarget())
					monster->SetTarget(players[id]);
				monster->SetState(ObjectState::ATTACK);
				SendStatusChangePacket(players[id], id);
			}	
		}

		for (int id : oldNearPlayers)
		{
			if (!newNearPlayers.count(id))
				SendRemoveObjectPacket(monster, _clients[id]);
		}
		delete expOver;
		break;
	}
	case MonsterEventType::UpdateStat:
	{
		const std::array<Monster*, NUM_NPCS>& monsters = _framework->GetMonsters();
		Monster* monster = monsters[monsterID - MONSTER_ID];

		if (monster->GetObjectState() == ObjectState::DEAD)
		{
			delete expOver;
			return;
		}

		Vector sectorIndex{ monster->GetPos().x / (SECTOR_SIZE * TILE_SIZE), monster->GetPos().y / (SECTOR_SIZE * TILE_SIZE) };

		Vector start{ std::max(0, sectorIndex.x - 1), std::max(0, sectorIndex.y - 1) };
		Vector end{ std::min(WORLD_WIDTH / SECTOR_SIZE - 1, sectorIndex.x + 1), std::min(WORLD_HEIGHT / SECTOR_SIZE - 1, sectorIndex.y + 1) };

		std::unordered_set<int> newNearPlayers;
		for (int x = start.x; x <= end.x; ++x)
		{
			for (int y = start.y; y <= end.y; ++y)
			{
				_sectors[x][y].sectorMutex.lock();
				for (int id : _sectors[x][y].objects)
				{
					if (id >= MONSTER_ID)
						continue;

					newNearPlayers.insert(id);
				}
				_sectors[x][y].sectorMutex.unlock();
			}
		}

		for (int id : newNearPlayers)
			SendStatusChangePacket(monster, id);
		
		delete expOver;
		break;
	}
	case MonsterEventType::Dead:
	{
		const std::array<Monster*, NUM_NPCS>& monsters = _framework->GetMonsters();
		Monster* monster = monsters[monsterID - MONSTER_ID];

		Vector sectorIndex{ monster->GetPos().x / (SECTOR_SIZE * TILE_SIZE), monster->GetPos().y / (SECTOR_SIZE * TILE_SIZE) };

		Vector start{ std::max(0, sectorIndex.x - 1), std::max(0, sectorIndex.y - 1) };
		Vector end{ std::min(WORLD_WIDTH / SECTOR_SIZE - 1, sectorIndex.x + 1), std::min(WORLD_HEIGHT / SECTOR_SIZE - 1, sectorIndex.y + 1) };

		std::unordered_set<int> newNearPlayers;
		for (int x = start.x; x <= end.x; ++x)
		{
			for (int y = start.y; y <= end.y; ++y)
			{
				_sectors[x][y].sectorMutex.lock();
				for (int id : _sectors[x][y].objects)
				{
					if (id >= MONSTER_ID)
						continue;

					newNearPlayers.insert(id);
				}
				_sectors[x][y].sectorMutex.unlock();
			}
		}

		for (int id : newNearPlayers)
			SendRemoveObjectPacket(monster, _clients[id]);

		delete expOver;
		break;
	}
	case MonsterEventType::Respawn:
	{
		const std::array<Monster*, NUM_NPCS>& monsters = _framework->GetMonsters();
		Monster* monster = monsters[monsterID - MONSTER_ID];

		Vector sectorIndex{ monster->GetPos().x / (SECTOR_SIZE * TILE_SIZE), monster->GetPos().y / (SECTOR_SIZE * TILE_SIZE) };

		Vector start{ std::max(0, sectorIndex.x - 1), std::max(0, sectorIndex.y - 1) };
		Vector end{ std::min(WORLD_WIDTH / SECTOR_SIZE - 1, sectorIndex.x + 1), std::min(WORLD_HEIGHT / SECTOR_SIZE - 1, sectorIndex.y + 1) };

		std::unordered_set<int> newNearPlayers;
		for (int x = start.x; x <= end.x; ++x)
		{
			for (int y = start.y; y <= end.y; ++y)
			{
				_sectors[x][y].sectorMutex.lock();
				for (int id : _sectors[x][y].objects)
				{
					if (id >= MONSTER_ID)
						continue;

					newNearPlayers.insert(id);
				}
				_sectors[x][y].sectorMutex.unlock();
			}
		}

		for (int id : newNearPlayers)
			SendAddObjectPacket(monster, _clients[id]);

		delete expOver;
		break;
	}
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

void ServerNetwork::SendAddObjectPacket(Creature* creature, Session* client)
{
	S2C_AddObject packet;
	packet.size = sizeof(S2C_AddObject);
	packet.type = S2C_ADD_OBJECT;
	packet.object_id = creature->GetID();
	packet.visual_id = static_cast<int>(creature->GetObjectType());
	strncpy_s(packet.obj_name, client->_userName, sizeof(packet.obj_name) - 1);
	packet.x = creature->GetPos().x;
	packet.y = creature->GetPos().y;
	packet.hp = creature->GetHP();
	packet.max_hp = creature->GetMaxHP();
	packet.exp = creature->GetEXP();
	packet.level = creature->GetLevel();

	client->Send(packet.size, reinterpret_cast<char*>(&packet));
}

void ServerNetwork::SendRemoveObjectPacket(GameObject* object, Session* client)
{
	// Packet Data 생성
	S2C_RemoveObject packet{ sizeof(S2C_RemoveObject), S2C_REMOVE_OBJECT, object->GetID()};

	client->Send(packet.size, reinterpret_cast<char*>(&packet));
}

void ServerNetwork::SendMoveObjectPacket(GameObject* object, Session* client)
{
	// Packet Data 생성
	S2C_MoveObject packet{ sizeof(S2C_MoveObject), S2C_MOVE_OBJECT, object->GetID(), object ->GetPos().x, object->GetPos().y, 0 };

	client->Send(packet.size, reinterpret_cast<char*>(&packet));
}

void ServerNetwork::SendStatusChangePacket(Creature* creature, int clientIndex)
{
	// Packet Data 생성
	S2C_StatusChange packet{ sizeof(S2C_StatusChange), S2C_STATUS_CHANGE, creature->GetID(), creature->GetHP(), creature->GetMaxHP(), creature->GetEXP(), creature->GetLevel() };

	_clients[clientIndex]->Send(packet.size, reinterpret_cast<char*>(&packet));
}

void ServerNetwork::SendAddItemPacket(Item* item, int clientIndex)
{
	// Packet Data 생성
	S2C_AddItem packet{ sizeof(S2C_AddItem), S2C_ADD_ITEM, item->GetID(), item->GetObjectType(), item->GetPos() };

	_clients[clientIndex]->Send(packet.size, reinterpret_cast<char*>(&packet));
}

void ServerNetwork::SendRemoveItemPacket(Item* item, int clientIndex)
{
	// Packet Data 생성
	S2C_RemoveItem packet{ sizeof(S2C_RemoveItem), S2C_REMOVE_ITEM, item->GetID() };

	_clients[clientIndex]->Send(packet.size, reinterpret_cast<char*>(&packet));
}

void ServerNetwork::SendAddItemToInventoryPacket(Item* item, int index, Session* client)
{
	// Packet Data 생성
	S2C_AddItemToInventory packet{ sizeof(S2C_AddItemToInventory), S2C_ADD_ITEM_TO_INVENTORY, item->GetID(), item->GetObjectType(), index };

	client->Send(packet.size, reinterpret_cast<char*>(&packet));
}

void ServerNetwork::SendRemoveItemFromInventoryPacket(int index, Session* client)
{
	// Packet Data 생성
	S2C_RemoveItemFromInventory packet{ sizeof(S2C_RemoveItemFromInventory), S2C_REMOVE_ITEM_FROM_INVENTORY, index };

	client->Send(packet.size, reinterpret_cast<char*>(&packet));
}

void ServerNetwork::ProcessLoginPacket(C2S_Login packet, int clientIndex)
{
	// 현재 접속하고 있는 ID라면 로그인 실패
	if (_clients[clientIndex]->_state == SessionState::Play)
	{
		SendLoginResultPacket(false, "이미 접속한 ID입니다.", _clients[clientIndex]);
		return;
	}
		
	// 로그인 성공
	SendLoginResultPacket(true, "로그인 성공", _clients[clientIndex]);

	// 데이터 베이스에 ID가 존재하는지 확인
	// ID가 존재한다면 정보 가져오기

	// ID가 없다면 데이터베이스에 추가 후, 새로운 Player 생성
	Player* player = g_framework->AddPlayer(clientIndex);

	_clients[clientIndex]->_state = SessionState::Play;

	// 새로 접속한 Client에게 자신을 나타낼 Player 정보 전송
	SendAvatarInfoPacket(player, _clients[clientIndex]);

	// 시야처리
	{
		Vector sectorIndex{ player->GetPos().x / (SECTOR_SIZE * TILE_SIZE), player->GetPos().y / (SECTOR_SIZE * TILE_SIZE) };
		_sectors[sectorIndex.x][sectorIndex.y].sectorMutex.lock();
		_sectors[sectorIndex.x][sectorIndex.y].objects.insert(clientIndex);
		_sectors[sectorIndex.x][sectorIndex.y].sectorMutex.unlock();

		Vector start{ std::max(0, sectorIndex.x - 1), std::max(0, sectorIndex.y - 1) };
		Vector end{ std::min(WORLD_WIDTH / SECTOR_SIZE, sectorIndex.x + 1), std::min(WORLD_HEIGHT / SECTOR_SIZE, sectorIndex.y + 1) };

		const std::array<Player*, MAX_PLAYERS>& players = _framework->GetPlayers();
		const std::array<Monster*, NUM_NPCS>& monsters = _framework->GetMonsters();
		std::unordered_set<int> currentView;
		for (int x = start.x; x < end.x; ++x)
		{
			for (int y = start.y; y < end.y; ++y)
			{
				_sectors[x][y].sectorMutex.lock();
				for (int id : _sectors[x][y].objects)
				{
					if (player->GetID() == id)
						continue;

					// Monster
					if (id >= MONSTER_ID)
					{
						if (players[clientIndex]->IsVisiable(monsters[id - MONSTER_ID]->GetPos()))
						{
							_framework->AddAliveMonster(id);
							currentView.insert(id);
						}
						continue;
					}

					if(_clients[id]->_state == SessionState::Play && player->IsVisiable(players[id]->GetPos()))
						currentView.insert(id);
				}
				_sectors[x][y].sectorMutex.unlock();
			}
		}

		for (int id : currentView)
		{
			// Monster
			if (id >= MONSTER_ID)
			{
				if(monsters[id - MONSTER_ID]->GetObjectState() != ObjectState::DEAD)
					SendAddObjectPacket(monsters[id - MONSTER_ID], _clients[clientIndex]);
				continue;
			}

			// 시야에 있는 Player 추가 및 접속 알림
			SendAddObjectPacket(players[id], _clients[clientIndex]);
			SendAddObjectPacket(player, _clients[id]);
		}

		// View List 업데이트
		_clients[clientIndex]->_viewLock.lock();
		_clients[clientIndex]->_viewList = std::move(currentView);
		_clients[clientIndex]->_viewLock.unlock();
	}
}

void ServerNetwork::ProcessLogoutPacket(C2S_Logout packet, int clientIndex)
{
	_clients[clientIndex]->_state = SessionState::Title;

	// 시야 처리
	{
		const std::array<Player*, MAX_PLAYERS>& players = _framework->GetPlayers();
		Vector sectorIndex{ players[clientIndex]->GetPos().x / (SECTOR_SIZE * TILE_SIZE), players[clientIndex]->GetPos().y / (SECTOR_SIZE * TILE_SIZE) };
		_sectors[sectorIndex.x][sectorIndex.y].sectorMutex.lock();
		_sectors[sectorIndex.x][sectorIndex.y].objects.erase(clientIndex);
		_sectors[sectorIndex.x][sectorIndex.y].sectorMutex.unlock();

		Vector start{ std::max(0, sectorIndex.x - 1), std::max(0, sectorIndex.y - 1) };
		Vector end{ std::min(WORLD_WIDTH / SECTOR_SIZE, sectorIndex.x + 1), std::min(WORLD_HEIGHT / SECTOR_SIZE, sectorIndex.y + 1) };

		const std::array<Monster*, NUM_NPCS>& monsters = _framework->GetMonsters();
		std::unordered_set<int> currentView;
		for (int x = start.x; x < end.x; ++x)
		{
			for (int y = start.y; y < end.y; ++y)
			{
				_sectors[x][y].sectorMutex.lock();
				for (int id : _sectors[x][y].objects)
				{
					// Monster
					if (id >= MONSTER_ID)
					{
						if(players[clientIndex]->IsVisiable(monsters[id - MONSTER_ID]->GetPos()))
							_framework->RemoveAliveMonster(id);
						continue;
					}

					if (_clients[id]->_state == SessionState::Play && players[clientIndex]->IsVisiable(players[id]->GetPos()))
						currentView.insert(id);
				}
				_sectors[x][y].sectorMutex.unlock();
			}
		}

		// 시야에 있는 Player에게 삭제 알림
		for (int id : currentView)
			SendRemoveObjectPacket(players[id], _clients[id]);

		// View List 업데이트
		_clients[clientIndex]->_viewLock.lock();
		_clients[clientIndex]->_viewList.clear();
		_clients[clientIndex]->_viewLock.unlock();
	}

	_framework->RemoveObject(ObjectType::Player, clientIndex);
}

void ServerNetwork::ProcessMovePacket(C2S_Move packet, int clientIndex)
{
	const std::array<Player*, MAX_PLAYERS>& players = _framework->GetPlayers();
	Player* player = players[clientIndex];

	if (!player->IsCanMove())
	{
		SendMoveObjectPacket(player, _clients[clientIndex]);
		return;
	}

	Vector prevPos = player->GetPos();

	// 갈 수 없는 곳이면 이전 좌표로 되돌리기
	if (!g_framework->IsCanGo(packet.x, packet.y))
	{
		SendMoveObjectPacket(player, _clients[clientIndex]);
		return;
	}
		
	player->SetPos({packet.x, packet.y});

	// 획득할 수 있는 Item이 있는지 확인
	const std::array<Item*, MAX_ITEMS>& items = _framework->GetItems();
	for (auto& id : _framework->GetCanGetItems(clientIndex))
	{
		if (items[id - ITEM_ID]->GetPos() == player->GetPos())
		{
			player->AddItemToInventory(id);
			int index = player->ExistItem(id);
			if (index != -1)
			{
				SendAddItemToInventoryPacket(items[id - ITEM_ID], index, _clients[clientIndex]);
				SendRemoveItemPacket(items[id - ITEM_ID], clientIndex);
				_framework->RemoveCanGetItem(player->GetID(), id);
			}
				
		}
	}

	// 시야처리
	{
		Vector oldIndex{ prevPos.x / (SECTOR_SIZE * TILE_SIZE), prevPos.y / (SECTOR_SIZE * TILE_SIZE) };
		Vector newIndex{ packet.x / (SECTOR_SIZE * TILE_SIZE), packet.y / (SECTOR_SIZE * TILE_SIZE) };

		// 섹터 옮기기
		if (oldIndex != newIndex)
		{
			_sectors[oldIndex.x][oldIndex.y].sectorMutex.lock();
			_sectors[oldIndex.x][oldIndex.y].objects.erase(player->GetID());
			_sectors[oldIndex.x][oldIndex.y].sectorMutex.unlock();

			_sectors[newIndex.x][newIndex.y].sectorMutex.lock();
			_sectors[newIndex.x][newIndex.y].objects.insert(player->GetID());
			_sectors[newIndex.x][newIndex.y].sectorMutex.unlock();
		}

		Vector start{ std::max(0, newIndex.x - 1), std::max(0, newIndex.y - 1) };
		Vector end{ std::min(WORLD_WIDTH / SECTOR_SIZE, newIndex.x + 1), std::min(WORLD_HEIGHT / SECTOR_SIZE, newIndex.y + 1) };

		const std::array<Monster*, NUM_NPCS>& monsters = _framework->GetMonsters();
		std::unordered_set<int> newView;
		for (int x = start.x; x < end.x; ++x)
		{
			for (int y = start.y; y < end.y; ++y)
			{
				_sectors[x][y].sectorMutex.lock();
				for (int id : _sectors[x][y].objects)
				{
					if (player->GetID() == id)
						continue;

					// Monster
					if (id >= MONSTER_ID)
					{
						if (player->IsVisiable(monsters[id - MONSTER_ID]->GetPos()))
							newView.insert(id);
						continue;
					}

					if (_clients[id]->_state == SessionState::Play && player->IsVisiable(players[id]->GetPos()))
						newView.insert(id);
				}
				_sectors[x][y].sectorMutex.unlock();
			}
		}

		_clients[clientIndex]->_viewLock.lock();
		for (int id : newView)
		{
			// 새로 시야에 들어온 Player 추가
			if (!_clients[clientIndex]->_viewList.count(id))
			{
				// Monster
				if (id >= MONSTER_ID)
				{
					if(monsters[id - MONSTER_ID]->GetObjectState() == ObjectState::DEAD)
						continue;
						
					_framework->AddAliveMonster(id);
					SendAddObjectPacket(monsters[id - MONSTER_ID], _clients[clientIndex]);
					continue;
				}

				SendAddObjectPacket(players[id], _clients[clientIndex]);
				SendAddObjectPacket(player, _clients[id]);
			}
			else
			{
				// 계속 시야에 있는 Player들에게 이동 알림
				if (id < MONSTER_ID)
					SendMoveObjectPacket(player, _clients[id]);
			}
		}

		// 시야에서 사라진 Player 삭제
		for (int id : _clients[clientIndex]->_viewList)
		{
			if (!newView.count(id))
			{
				// Monster
				if (id >= MONSTER_ID)
				{
					_framework->RemoveAliveMonster(id);
					SendRemoveObjectPacket(monsters[id - MONSTER_ID], _clients[clientIndex]);
					continue;
				}

				SendRemoveObjectPacket(players[id], _clients[clientIndex]);
				SendRemoveObjectPacket(player, _clients[id]);
			}
		}

		// View List 업데이트
		_clients[clientIndex]->_viewList = std::move(newView);
		_clients[clientIndex]->_viewLock.unlock();
	}
}

void ServerNetwork::ProcessAttackPacket(C2S_Attack packet, int clientIndex)
{
	const std::array<Player*, MAX_PLAYERS>& players = _framework->GetPlayers();
	const std::array<Monster*, NUM_NPCS>& monsters = _framework->GetMonsters();
	Player* player = players[clientIndex];

	Vector sectorIndex{ player->GetPos().x / (SECTOR_SIZE * TILE_SIZE), player->GetPos().y / (SECTOR_SIZE * TILE_SIZE) };
	
	Vector start{ std::max(0, sectorIndex.x - 1), std::max(0, sectorIndex.y - 1) };
	Vector end{ std::min(WORLD_WIDTH / SECTOR_SIZE - 1, sectorIndex.x + 1), std::min(WORLD_HEIGHT / SECTOR_SIZE - 1, sectorIndex.y + 1) };

	std::unordered_set<int> nearMonsters;
	for (int x = start.x; x <= end.x; ++x)
	{
		for (int y = start.y; y <= end.y; ++y)
		{
			_sectors[x][y].sectorMutex.lock();
			for (int id : _sectors[x][y].objects)
			{
				if (id < MONSTER_ID)
					continue;

				nearMonsters.insert(id);
			}
			_sectors[x][y].sectorMutex.unlock();
		}
	}

	for (int id : nearMonsters)
	{
		Monster* monster = monsters[id - MONSTER_ID];
		if (player->IsNear(monster->GetPos()))
		{
			if(!monster->GetTarget())
				monsters[id - MONSTER_ID]->SetTarget(player);
			monster->TackDamage(player->GetDamage());
			
			ExpOver* over = new ExpOver(IOType::MonsterEvent);
			over->_monsterEventType = MonsterEventType::UpdateStat;
			PostQueuedCompletionStatus(g_network->GetIOCP(), 0, static_cast<ULONG_PTR>(id), &over->_over);
		}
	}
}

void ServerNetwork::ProcessSkillPacket(C2S_Skill packet, int clientIndex)
{
	const std::array<Player*, MAX_PLAYERS>& players = _framework->GetPlayers();
	const std::array<Monster*, NUM_NPCS>& monsters = _framework->GetMonsters();
	Player* player = players[clientIndex];

	Vector sectorIndex{ player->GetPos().x / (SECTOR_SIZE * TILE_SIZE), player->GetPos().y / (SECTOR_SIZE * TILE_SIZE) };

	Vector start{ std::max(0, sectorIndex.x - 1), std::max(0, sectorIndex.y - 1) };
	Vector end{ std::min(WORLD_WIDTH / SECTOR_SIZE - 1, sectorIndex.x + 1), std::min(WORLD_HEIGHT / SECTOR_SIZE - 1, sectorIndex.y + 1) };

	std::unordered_set<int> nearMonsters;
	for (int x = start.x; x <= end.x; ++x)
	{
		for (int y = start.y; y <= end.y; ++y)
		{
			_sectors[x][y].sectorMutex.lock();
			for (int id : _sectors[x][y].objects)
			{
				if (id < MONSTER_ID)
					continue;

				nearMonsters.insert(id);
			}
			_sectors[x][y].sectorMutex.unlock();
		}
	}

	for (int id : nearMonsters)
	{
		Monster* monster = monsters[id - MONSTER_ID];
		if (player->IsSkillTack(monster->GetPos()))
		{
			if (!monster->GetTarget())
				monsters[id - MONSTER_ID]->SetTarget(player);
			monster->TackDamage(player->GetDamage());

			ExpOver* over = new ExpOver(IOType::MonsterEvent);
			over->_monsterEventType = MonsterEventType::UpdateStat;
			PostQueuedCompletionStatus(g_network->GetIOCP(), 0, static_cast<ULONG_PTR>(id), &over->_over);
		}
	}
}

void ServerNetwork::ProcessDropItemPacket(C2S_DropItem packet, int clientIndex)
{
	// Player 인벤토리에 진짜로 아이템이 존하는지 확인
	Player* player = dynamic_cast<Player*>(_framework->GetGameObject(ObjectType::Player, clientIndex));
	int index = player->ExistItem(packet.id);

	// 진짜로 존재하면 아이템 삭제 및 알림
	if (index != -1)
	{
		player->RemoveItemFromInventory(packet.id);

		Item* item = dynamic_cast<Item*>(_framework->GetGameObject(ObjectType::Sword, packet.id));
		item->SetObjectPoolState(ObjectPoolState::Reusable);
	
		SendRemoveItemFromInventoryPacket(index, _clients[clientIndex]);
	}
}

void ServerNetwork::ProcessChangeWeaponPacket(C2S_ChageWeapon packet, int clientIndex)
{
	Player* player = dynamic_cast<Player*>(_framework->GetGameObject(ObjectType::Player, clientIndex));
	if (packet.id == -1)
	{
		player->SetCurrentWeapon(nullptr);
		return;
	}
	
	Weapon* item = dynamic_cast<Weapon*>(_framework->GetGameObject(ObjectType::Sword, packet.id));
	player->SetCurrentWeapon(item);
}

void ServerNetwork::ProcessTeleportPacket(C2S_Teleport packet, int clientIndex)
{
	GameObject* object = _framework->GetGameObject(ObjectType::Player, clientIndex);
	object->SetPos({ packet.x, packet.y });
}