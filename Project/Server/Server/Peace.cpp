#include "pch.h"
#include "Peace.h"

Peace::Peace()
{
	// State Table
	_stateTable[CHASE] = RETURN;
	_stateTable[RETURN] = IDLE;
	_stateTable[ATTACK] = CHASE;
	_stateTable[HIT] = CHASE;
}

Peace::~Peace()
{
}
