#pragma once
#include "ExpOver.h"

class ServerFramework;
class Session;
class GameObject;
class Creature;
class Player;
class Monster;
class Item;

class ServerNetwork
{
public:
	ServerNetwork(ServerFramework* framework);
	~ServerNetwork();

public:
	void Update();
	void ProcessAccept();
	void ProcessDisconnected(int clientIndex);
	void ProcessRecv(int clientIndex, int numByte, ExpOver* expOver);
	void ProcessPacket(std::vector<char>& packet, int clientIndex);
	void ProcessMonsterEvent(int monsterID, ExpOver* expOver);

public:
	// Send
	void SendLoginResultPacket(bool result, const char* message, Session* client);
	void SendAvatarInfoPacket(Player* player, Session* client);
	void SendAddObjectPacket(Creature* creature, Session* client);
	void SendRemoveObjectPacket(GameObject* object, Session* client);
	void SendMoveObjectPacket(GameObject* object, Session* client);
	void SendStatusChangePacket(Creature* creature, int clientIndex);
	void SendAddItemPacket(Item* item, int clientIndex);
	void SendRemoveItemPacket(Item* item, int clientIndex);
	void SendAddItemToInventoryPacket(Item* item, int index, Session* client);
	void SendRemoveItemFromInventoryPacket(int index, Session* client);
	
public:
	// Recv
	void ProcessLoginPacket(C2S_Login packet, int clientIndex);
	void ProcessLogoutPacket(C2S_Logout packet, int clientIndex);
	void ProcessMovePacket(C2S_Move packet, int clientIndex);
	void ProcessAttackPacket(C2S_Attack packet, int clientIndex);
	void ProcessDropItemPacket(C2S_DropItem packet, int clientIndex);
	void ProcessChangeWeaponPacket(C2S_ChageWeapon packet, int clientIndex);

	/*
	void ProcessChangeToolPacket(C_ChangeTool_Packet packet, int clientIndex);
	void ProcessUseToolPacket(C_UseTool_Packet packet, int clientIndex);
	void ProcessSellItemPacket(C_SellItem_Packet packet, int clientIndex);
	void ProcessBuyItemPacket(C_BuyItem_Packet packet, int clientIndex);*/

public:
	HANDLE GetIOCP() { return _iocp; }

private:
	SOCKET _listenSocket{};
	SOCKET _tempSocket{};
	ExpOver _acceptOver{};
	HANDLE _iocp{};
	std::array<class Session*, MAX_PLAYERS> _clients;

	std::array<std::array<Sector, WORLD_HEIGHT / SECTOR_SIZE>, WORLD_WIDTH / SECTOR_SIZE> _sectors;
	ServerFramework* _framework;
};