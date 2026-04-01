#include "pch.h"
#include "GameFramework.h"
#include "GameObject.h"
#include "GameNetwork.h"
#include "Global.h"

GameFramework::GameFramework(HWND hwnd)
{
	_hwnd = hwnd;

	_boardBmp = (HBITMAP)LoadImage(NULL, L"Resource\\ChessBoard.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

GameFramework::~GameFramework()
{
}

void GameFramework::Update()
{
	InvalidateRect(_hwnd, NULL, false);
}

void GameFramework::Render(HDC hdc)
{
	HDC srcHDC = CreateCompatibleDC(hdc);
	SelectObject(srcHDC, _boardBmp);

	HDC backHDC = CreateCompatibleDC(hdc);
	HBITMAP bmp = CreateCompatibleBitmap(hdc, WinSize, WinSize);
	SelectObject(backHDC, bmp);

	TransparentBlt(backHDC, 0, 0, WinSize, WinSize, srcHDC, 0, 0, WinSize, WinSize, RGB(0, 255, 0));
	
	for (auto& item : _players)
		item.second.Render(hdc, backHDC);

	BitBlt(hdc, 0, 0, WinSize, WinSize, backHDC, 0, 0, SRCCOPY);

	DeleteObject(bmp);
	DeleteDC(backHDC);
	DeleteDC(srcHDC);
}

void GameFramework::ProcessInput(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_UP:
	{
		Dir dir = UP;
		g_gameNetwork->SendMovePacket(dir);
		break;
	}
	case VK_RIGHT:
	{
		Dir dir = RIGHT;
		g_gameNetwork->SendMovePacket(dir);
		break;
	}
	case VK_DOWN:
	{
		Dir dir = DOWN;
		g_gameNetwork->SendMovePacket(dir);
		break;
	}
	case VK_LEFT:
	{
		Dir dir = LEFT;
		g_gameNetwork->SendMovePacket(dir);
		break;
	}
	}
}

void GameFramework::ProcessAddObjectPacket(S_AddObject_Packet packet)
{
	if (_players.size() == 0)
		_myID = packet.id;
	_players.try_emplace(packet.id, packet.id, POINT{ packet.pos.x, packet.pos.y });
}

void GameFramework::ProcessMovePacket(S_Move_Packet packet)
{
	_players[packet.id].SetPos(POINT{ packet.pos.x, packet.pos.y });
}