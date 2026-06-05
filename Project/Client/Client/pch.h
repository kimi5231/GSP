#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <SFML/Graphics.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <filesystem>
#include <unordered_map>

#include "../../Server/Server/Types.h"
#include "../../Server/Server/protocol_2026.h"

#pragma comment(lib, "MSWSock.lib")
#pragma comment(lib, "ws2_32")

struct NetworkEvent
{
	bool isComplete = false;
	PACKET_TYPE packetID;
	std::vector<char> serializedPacketData;
};

using NetworkEventRef = std::shared_ptr<NetworkEvent>;
using PlayerRef = std::shared_ptr<class Player>;