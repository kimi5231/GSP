#pragma once
class GameObject;
class GameNetwork;

class GameFramework
{
public:
	GameFramework();
	~GameFramework();

public:
	void Update(HWND hwnd, WPARAM wParam);
	void Render(HDC hdc);

private:
	HBITMAP _boardBmp;
	std::array<std::array<char, 8>, 8> _board;
	
	GameObject* _pawn;

	GameNetwork* _gameNetwork;
};