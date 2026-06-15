#pragma once
class GameNetwork
{
public:
	GameNetwork();
	~GameNetwork();

public:
	void Update();
	
private:
	void ProcessRecv();

public:
	template <class T >
	std::vector<char> SerializePOD(const T& pod);

	template <class T >
	std::vector<char> SerializeVector(const std::vector<T>& vector);

	template <class T>
	std::vector<T> DeserializeVector(std::vector<char>& data);

public:
	void SendLoginPacket(const char* id);
	void SendLogoutPacket();
	void SendMovePacket(PlayerRef player);
	void SendAttackPacket();
	//void SendDropItemPacket(int itemID, bool isTool, int playerID);
	//void SendSellItemPacket(int playerID, int sellingMachineID);
	//void SendBuyItemPacket(int playerID, ItemType itemType, int itemCount);

public:
	void ProcessLoginResultPacket(S2C_LoginResult packet);
	void ProcessAvatarInfoPacket(S2C_AvatarInfo packet);
	void ProcessAddObjectPacket(S2C_AddObject packet);
	void ProcessRemoveObjectPacket(S2C_RemoveObject packet);
	void ProcessMoveObjectPacket(S2C_MoveObject packet);
	void ProcessStatusChangePacket(S2C_StatusChange packet);
	void ProcessAddItemPacket(S2C_AddItem packet);
	void ProcessRemoveItemPacket(S2C_RemoveItem packet);
	void ProcessAddItemToInventoryPacket(S2C_AddItemToInventory packet);

private:
	fd_set _readSet{};
	fd_set _writeSet{};

	SOCKET _clientSocket{};

	std::vector<NetworkEventRef> _sendEvents;
};

template<class T>
inline std::vector<char> GameNetwork::SerializePOD(const T& pod)
{
	std::vector<char> serializedData(sizeof(pod));
	memcpy(serializedData.data(), &pod, sizeof(pod));

	return serializedData;
}

template<class T>
inline std::vector<char> GameNetwork::SerializeVector(const std::vector<T>& vector)
{
	int size = vector.size();

	std::vector<char> serializedData(sizeof(int) + vector.size() * sizeof(T));
	memcpy(serializedData.data(), &size, sizeof(int));
	memcpy(serializedData.data() + sizeof(int), vector.data(), size * sizeof(T));

	return serializedData;
}

template<class T>
inline std::vector<T> GameNetwork::DeserializeVector(std::vector<char>& data)
{
	int size;
	memcpy(&size, data.data(), sizeof(int));
	data.erase(data.begin(), data.begin() + sizeof(int));

	std::vector<T> vector(size);
	memcpy(vector.data(), data.data(), size * sizeof(T));
	data.erase(data.begin(), data.begin() + size * sizeof(T));

	return vector;
}