#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	_pawnBmp = (HBITMAP)LoadImage(NULL, L"Resource\\BlackPawn.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	_pos = {0, 0};
	_size = {100, 100};
}

GameObject::~GameObject()
{
}

void GameObject::Update(WPARAM wParam, std::array<std::array<short, 8>, 8>& board)
{
	POINT prevPos = _pos;

	switch (wParam)
	{
	case VK_UP:
		_pos.y -= 100;
		break;
	case VK_RIGHT:
		_pos.x += 100;
		break;
	case VK_DOWN:
		_pos.y += 100;
		break;
	case VK_LEFT:
		_pos.x -= 100;
		break;
	}

	if (_pos.x / 100 < 0 || _pos.y / 100 < 0 || _pos.x / 100 > board.size() - 1 || _pos.y / 100 > board.size() - 1)
		_pos = prevPos;
}

void GameObject::Render(HDC hdc, HDC backHDC)
{
	HDC srcHDC = CreateCompatibleDC(hdc);
	SelectObject(srcHDC, _pawnBmp);

	TransparentBlt(backHDC, _pos.x, _pos.y, _size.x, _size.y, srcHDC, 0, 0, 100, 100, RGB(0, 255, 0));

	DeleteDC(srcHDC);
}