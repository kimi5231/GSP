#pragma once
class GameFramework
{
public:
	GameFramework();
	~GameFramework();

public:
	void Update();
	void Render(HDC hdc);

private:
	HBITMAP _board;
};