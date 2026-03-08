#include "pch.h"
#include "GameFramework.h"

GameFramework::GameFramework()
{
	// Load Bitmap
	_board = (HBITMAP)LoadImage(NULL, L"Resource\\ChessBoard.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); 
}

GameFramework::~GameFramework()
{
}

void GameFramework::Update()
{
}

void GameFramework::Render(HDC hdc)
{
	HDC srcHDC = CreateCompatibleDC(hdc);
	SelectObject(srcHDC, _board);

	HDC backHDC = CreateCompatibleDC(hdc);
	HBITMAP bmp = CreateCompatibleBitmap(hdc, WinSize, WinSize);
	SelectObject(backHDC, bmp);

	TransparentBlt(backHDC, 0, 0, WinSize, WinSize, srcHDC, 0, 0, WinSize, WinSize, RGB(0, 255, 0));

	BitBlt(hdc, 0, 0, WinSize, WinSize, backHDC, 0, 0, SRCCOPY);

	DeleteObject(bmp);
	DeleteDC(backHDC);
	DeleteDC(srcHDC);
}