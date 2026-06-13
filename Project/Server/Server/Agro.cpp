#include "pch.h"
#include "Agro.h"
#include "Global.h"
#include "FSM.h"
#include "State.h"

Agro::Agro()
{
	_type = ObjectType::Agro;

	MonsterStat stat = g_dataManager->GetMonsterStat();
	_maxHP = stat.hp;
	_hp = stat.hp;
	_level = stat.level;

	_idleTime = 0.5f;
	_sumTime = 0.f;

	// State Table
	_stateTable[IDLE] = ROAMING;
	_stateTable[ROAMING] = IDLE;
	_stateTable[RETURN] = IDLE;
	_stateTable[ATTACK] = CHASE;
	_stateTable[HIT] = CHASE;
}

Agro::~Agro()
{
}

void Agro::Update()
{
	Monster::Update();
}

bool Agro::IsReadyNextState()
{
	switch (_state)
	{
	case ObjectState::IDLE:
		return _sumTime > _idleTime;
	case ObjectState::ROAMING:
		return _prevPos != _pos;
	case ObjectState::RETURN:
		return IsHome(_returnPos);
	case ObjectState::ATTACK:
		return true;
	case ObjectState::HIT:
		return true;
	}
}

bool Agro::SetState(ObjectState state)
{
	if (!Monster::SetState(state))
		return false;

	switch (state)
	{
	case ROAMING:
		_fsm->ChangeState(g_roamingState, this);
		break;
	}

	return true;
}