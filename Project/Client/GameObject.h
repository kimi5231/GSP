#pragma once
class GameObject
{
public:
	GameObject() {};
	GameObject(int id, POINT pos);
	~GameObject();

public:
	void Update();
	void Render(HDC hdc, HDC backHDC, bool isMy);

public:
	POINT GetPos() { return _pos; }
	void SetPos(POINT pos) { _pos = pos; }
	int GetID() { return _id; }

private:
	HBITMAP _blackPawnBmp;
	HBITMAP _whitePawnBmp;
	int _id;
	POINT _pos;
	POINT _size;
};