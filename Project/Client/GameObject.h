#pragma once
class GameObject
{
public:
	GameObject(std::array<std::array<char, 8>, 8>& board);
	~GameObject();

public:
	void Update();
	void Render(HDC hdc, HDC backHDC);

public:
	POINT GetPos() { return _pos; }
	void SetPos(POINT pos) { _pos = pos; }

private:
	HBITMAP _pawnBmp;
	POINT _pos;
	POINT _size;
};