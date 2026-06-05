#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

void GameObject::SetPos(short x, short y)
{
	_pos.x = x;
	_pos.y = y;

	_sprite.setPosition(x, y);
}

void GameObject::SetPos(Vector pos)
{
	_pos = pos;

	_sprite.setPosition(pos.x, pos.y);
}