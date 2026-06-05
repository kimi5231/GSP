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

public:
	int GetMaxHP() { return _maxHP; }
	int GetHP() { return _hp; }
	long long GetEXP() { return _exp; }
	char GetLevel() { return _level; }
	const char* GetName() { return _name; }
	void SetName(const char* name) { strncpy_s(_name, name, sizeof(_name) - 1); }

protected:
	int _maxHP;
	int _hp;
	long long _exp;
	char _level;
	char _name[MAX_NAME_LEN];
};