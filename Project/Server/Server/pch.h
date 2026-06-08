#pragma once
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
#include <tbb/concurrent_unordered_map.h>

#include "Types.h"
#include "protocol_2026.h"

#pragma comment(lib, "MSWSock.lib")
#pragma comment(lib, "ws2_32")

using SessionRef = std::shared_ptr<class Session>;