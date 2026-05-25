#include "pch.h"
#include "ServerObject.h"

ServerObject::ServerObject(int id) 
	: _id(id)
{
	_pos = { 0, 0 };
}

ServerObject::~ServerObject()
{
}

Vector ServerObject::Move(Dir dir)
{
	Vector pos = _pos;

	switch (dir)
	{
	case UP:
		pos.y--;
		break;
	case RIGHT:
		pos.x++;
		break;
	case DOWN:
		pos.y++;
		break;
	case LEFT:
		pos.x--;
		break;
	}

	if (0 <= pos.x && pos.x < BoadSize && 0 <= pos.y && pos.y < BoadSize)
		_pos = pos;
	
	return _pos;
}