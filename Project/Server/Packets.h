#pragma once
#include "Types.h"

enum PacketID
{
	S_AddObject,
	S_Move,

	C_Move,
};

struct Header
{
	PacketID id;
	int dataSize;
};

struct S_AddObject_Packet
{
	int id;
	Vector pos;
};

struct S_Move_Packet
{
	int id;
	Vector pos;
};

struct C_Move_Packet
{
	Dir dir;
};