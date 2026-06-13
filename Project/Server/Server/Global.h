#pragma once
#include "ServerNetwork.h"
#include "ServerFramework.h"
#include "DataManager.h"
#include "Timer.h"

extern std::random_device rd;
extern std::mt19937 gen;

extern ServerNetwork* g_network;
extern ServerFramework* g_framework;
extern DataManager* g_dataManager;
extern Timer* g_timer;