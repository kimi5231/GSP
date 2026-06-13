#include "pch.h"
#include "Peace.h"

Peace::Peace()
{
	_type = ObjectType::Peace;

	// State Table
	_stateTable[RETURN] = IDLE;
	_stateTable[ATTACK] = CHASE;
	_stateTable[HIT] = CHASE;
}

Peace::~Peace()
{
}

void Peace::Update()
{
	Monster::Update();
}

bool Peace::IsReadyNextState()
{
	switch (_state)
	{
	case ObjectState::RETURN:
		return _returnPos == _pos;
	case ObjectState::ATTACK:
		return true;
	case ObjectState::HIT:
		return true;
	}
}