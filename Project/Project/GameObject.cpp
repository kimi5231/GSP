#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(std::array<std::array<char, 8>, 8>& board)
{
	_pawnBmp = (HBITMAP)LoadImage(NULL, L"Resource\\BlackPawn.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	_pos = {0, 0};
	_size = { 100, 100 };

	// 보드에 기록
	board[_pos.x][_pos.y] = 1;
}

GameObject::~GameObject()
{
}

void GameObject::Update(WPARAM wParam, std::array<std::array<char, 8>, 8>& board)
{
	POINT pos = _pos;

	switch (wParam)
	{
	case VK_UP:
		pos.y--;
		break;
	case VK_RIGHT:
		pos.x++;
		break;
	case VK_DOWN:
		pos.y++;
		break;
	case VK_LEFT:
		pos.x--;
		break;
	}

	if (0 <= pos.x && pos.x < board.size() && 0 <= pos.y && pos.y < board.size())
	{
		board[_pos.x][_pos.y] = 0;
		_pos = pos;
		board[_pos.x][_pos.y] = 1;
	}
}

void GameObject::Render(HDC hdc, HDC backHDC)
{
	HDC srcHDC = CreateCompatibleDC(hdc);
	SelectObject(srcHDC, _pawnBmp);

	TransparentBlt(backHDC, _pos.x * TileSize, _pos.y * TileSize, _size.x, _size.y, srcHDC, 0, 0, _size.x, _size.y, RGB(0, 255, 0));

	DeleteDC(srcHDC);
}