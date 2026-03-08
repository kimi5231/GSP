#pragma once
class GameObject
{
public:
	GameObject();
	~GameObject();

public:
	void Update(WPARAM wParam, std::array<std::array<short, 8>, 8>& board);
	void Render(HDC hdc, HDC backHDC);

public:
	POINT GetPos() { return _pos; }

private:
	HBITMAP _pawnBmp;
	POINT _pos;
	POINT _size;
};