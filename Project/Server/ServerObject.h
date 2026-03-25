#pragma once
#include "Types.h"
class ServerObject
{
public:
	ServerObject();
	~ServerObject();

public:
	Vector Move(Dir dir);

private:
	Vector _pos;
};