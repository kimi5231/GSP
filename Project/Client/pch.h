#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <array>

#include "../Server/Types.h"

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "Msimg32.lib")

#define WinSize 800
#define TileSize 100
#define BuffSize 256