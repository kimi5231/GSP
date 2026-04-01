#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(int id, POINT pos)
	:_id(id), _pos(pos)
{
	_pawnBmp = (HBITMAP)LoadImage(NULL, L"Resource\\BlackPawn.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	_size = { 100, 100 };
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
	
}

void GameObject::Render(HDC hdc, HDC backHDC)
{
	HDC srcHDC = CreateCompatibleDC(hdc);
	SelectObject(srcHDC, _pawnBmp);

	TransparentBlt(backHDC, _pos.x * TileSize, _pos.y * TileSize, _size.x, _size.y, srcHDC, 0, 0, _size.x, _size.y, RGB(0, 255, 0));

	DeleteDC(srcHDC);
}