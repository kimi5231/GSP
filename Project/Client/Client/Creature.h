#pragma once
#include "GameObject.h"

class Creature : public GameObject
{
public:
	Creature();
	virtual ~Creature() {};

public:
	virtual void SetMaxHP(int maxHP);
	virtual void SetHP(int hp);
	virtual void SetExp(long long exp);
	virtual void SetLevel(int level);
	void SetName(const char* name) { strncpy_s(_name, name, sizeof(_name) - 1); }

protected:
	int _maxHP;
	int _hp;
	long long _exp;
	int _level;
	char _name[MAX_NAME_LEN];
};