#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <SFML/Graphics.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <filesystem>
#include <unordered_map>
#include <algorithm>

#include "../../Server/Server/Types.h"
#include "../../Server/Server/protocol_2026.h"

#pragma comment(lib, "MSWSock.lib")
#pragma comment(lib, "ws2_32")

constexpr int WINDOW_WIDTH = 1000;
constexpr int WINDOW_HEIGHT = 1000;
constexpr int InventoryTileSize = 30;

enum class UIType
{
	HpBar,
	ExpBar,
};

struct NetworkEvent
{
	bool isComplete = false;
	PACKET_TYPE packetID;
	std::vector<char> serializedPacketData;
};

using NetworkEventRef = std::shared_ptr<NetworkEvent>;
using GameObjectRef = std::shared_ptr<class GameObject>;
using CreatureRef = std::shared_ptr<class Creature>;
using PlayerRef = std::shared_ptr<class Player>;
using ItemRef = std::shared_ptr<class Item>;