#pragma once
#include "GameObject.h"

class Creature : public GameObject
{
public:
	Creature();
	virtual ~Creature();

public:
	bool TackDamage(int damage);
	bool TackHeal(int heal);
	bool IsCanMove();

public:
	int GetMaxHP() { return _maxHP; }
	int GetHP() { return _hp; }
	long long GetEXP() { return _exp; }
	char GetLevel() { return _level; }
	
protected:
	int _maxHP;
	int _hp;
	long long _exp;
	char _level;
	TimePoint _lastMoveTime;
};