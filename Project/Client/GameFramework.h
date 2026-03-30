#pragma once
class GameObject;

class GameFramework
{
public:
	GameFramework(HWND hwnd);
	~GameFramework();

public:
	void Update(WPARAM wParam);
	void Render(HDC hdc);

public:
	void ProcessMove(POINT pos);

private:
	HWND _hwnd;

	HBITMAP _boardBmp;
	std::array<std::array<char, 8>, 8> _board;
	
	GameObject* _pawn;
};