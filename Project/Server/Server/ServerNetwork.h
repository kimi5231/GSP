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
	void ProcessMonster(int monsterID, ExpOver* expOver);

public:
	// Send
	void SendLoginResultPacket(bool result, const char* message, Session* client);
	void SendAvatarInfoPacket(Player* player, Session* client);
	void SendAddObjectPacket(Creature* creature, Session* client);
	void SendRemoveObjectPacket(GameObject* object, Session* client);
	void SendMoveObjectPacket(GameObject* object, Session* client);
	/*void SendUpdateObjectStatePacket(GameObject* object, Session* client);
	void SendAddItemToInventoryPacket(Item* item, bool isTool, Session* client);
	void SendRemoveItemFromInventoryPacket(Item* item, bool isTool, Session* client);
	void SendDropItemPacket(Item* item, int playerID, Vector itemPos, bool isTool, bool isToSellingMachine, Session* client);
	void SendUpdateCurrentToolPacket(int itemID, int playerID, ItemType type, Session* client);
	void SendUseToolPacket(int playerID, ItemType type, Session* client);
	void SendSellItemResultPacket(char playerID, char sellingMachineID, ObjectState sellingMachineState, short remainCredit, short collectCredit, short currentCredit, std::vector<int>& itemIDs, Session* client);
	void SendBuyItemResultPacket(short currentCredit, Session* client);
	void SendUpdateHpPacket(int playerID, int hp, Session* client);*/
	
public:
	// Recv
	void ProcessLoginPacket(C2S_Login packet, int clientIndex);
	void ProcessLogoutPacket(C2S_Logout packet, int clientIndex);
	void ProcessMovePacket(C2S_Move packet, int clientIndex);

	/*void ProcessGetItemPacket(C_GetItem_Packet packet, int clientIndex);
	void ProcessDropItemPacket(C_DropItem_Packet packet, int clientIndex);
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