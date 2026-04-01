#pragma once
#include "../Server/Packets.h"

class GameObject;

class GameFramework
{
public:
	GameFramework(HWND hwnd);
	~GameFramework();

public:
	void Update();
	void Render(HDC hdc);

	void ProcessInput(WPARAM wParam);

public:
	void ProcessAddObjectPacket(S_AddObject_Packet packet);
	void ProcessRemoveObjectPacket(S_RemoveObject_Packet packet);
	void ProcessMovePacket(S_Move_Packet packet);

private:
	HWND _hwnd;

	HBITMAP _boardBmp;
	std::array<std::array<char, 8>, 8> _board;
	
	std::unordered_map<int, GameObject> _players;
	int _myID;
};