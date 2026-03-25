#include "pch.h"
#include "GameFramework.h"
#include "GameObject.h"

GameFramework::GameFramework()
{
	_boardBmp = (HBITMAP)LoadImage(NULL, L"Resource\\ChessBoard.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	// 폰 생성
	_pawn = new GameObject(_board);
}

GameFramework::~GameFramework()
{
}

void GameFramework::Update(HWND hwnd, WPARAM wParam)
{
	_pawn->Update(wParam, _board);

	InvalidateRect(hwnd, NULL, false);
}

void GameFramework::Render(HDC hdc)
{
	HDC srcHDC = CreateCompatibleDC(hdc);
	SelectObject(srcHDC, _boardBmp);

	HDC backHDC = CreateCompatibleDC(hdc);
	HBITMAP bmp = CreateCompatibleBitmap(hdc, WinSize, WinSize);
	SelectObject(backHDC, bmp);

	TransparentBlt(backHDC, 0, 0, WinSize, WinSize, srcHDC, 0, 0, WinSize, WinSize, RGB(0, 255, 0));
	
	_pawn->Render(hdc, backHDC);

	BitBlt(hdc, 0, 0, WinSize, WinSize, backHDC, 0, 0, SRCCOPY);

	DeleteObject(bmp);
	DeleteDC(backHDC);
	DeleteDC(srcHDC);
}