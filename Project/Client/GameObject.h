#pragma once
class GameObject
{
public:
	GameObject() {};
	GameObject(int id, POINT pos);
	~GameObject();

public:
	void Update();
	void Render(HDC hdc, HDC backHDC);

public:
	POINT GetPos() { return _pos; }
	void SetPos(POINT pos) { _pos = pos; }

private:
	HBITMAP _pawnBmp;
	int _id;
	POINT _pos;
	POINT _size;
};