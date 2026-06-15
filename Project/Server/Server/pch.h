#pragma once
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <array>
#include <random>
#include <ranges>
#include <filesystem>
#include <numeric>
#include <utility> 
#include <functional>
#include <chrono>
#include <queue>
#include <cmath>
#include <algorithm>
#include <optional>
#include <MSWSock.h>
#include <limits>
#include <thread>
#include <mutex>
#include <atomic>
#include <tbb/concurrent_unordered_map.h>

#include "Types.h"
#include "protocol_2026.h"

#pragma comment(lib, "MSWSock.lib")
#pragma comment(lib, "ws2_32")

constexpr int VIEW_RANGE = 7;
constexpr int MONSTER_ROAMING_RANGE = 10;
constexpr int MONSTER_AGRO_RANGE = 5;
constexpr int MONSTER_NEAR_RANGE = 1;
constexpr int SKILL_RANGE = 3;
constexpr int SECTOR_SIZE = 20;

struct Sector
{
	std::unordered_set<int> objects;
	std::mutex sectorMutex;
};

using SessionRef = std::shared_ptr<class Session>;