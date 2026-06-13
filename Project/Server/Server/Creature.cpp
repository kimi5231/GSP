#include "pch.h"
#include "Creature.h"

Creature::Creature()
{
	_lastMoveTime = std::chrono::high_resolution_clock::now();
}

Creature::~Creature()
{
}

bool Creature::TackDamage(int damage)
{
	_hp -= damage;

	if (_hp <= 0)
		SetState(ObjectState::DEAD);
	else
		SetState(ObjectState::HIT);

	return true;
}

bool Creature::TackHeal(int heal)
{
	_hp += heal;
	if (_hp > _maxHP)
		_hp = _maxHP;

	return true;
}

bool Creature::IsCanMove()
{
	auto duration = std::chrono::high_resolution_clock::now() - _lastMoveTime;

	long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	if (ms > 500)
	{
		_lastMoveTime = std::chrono::high_resolution_clock::now();
		return true;
	}
		
	return false;
}

bool Creature::SetState(ObjectState state)
{
	if (_state == state)
		return false;

	_state = state;

	return true;
}