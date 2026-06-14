#pragma once
class UI
{
public:
	UI(Vector pos, Vector size);
	virtual ~UI() {};

public:
	virtual void Render(sf::RenderWindow* window) {};

public:
	virtual void SetPos(Vector pos) { _pos = pos; }

protected:
	Vector _pos;
	Vector _size;
};