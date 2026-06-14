#include "pch.h"
#include "Creature.h"

Creature::Creature()
{
}

void Creature::SetMaxHP(int maxHP)
{
	_maxHP = maxHP;
}

void Creature::SetHP(int hp)
{
	_hp = hp;
}

void Creature::SetExp(long long exp)
{
	_exp = exp;
}

void Creature::SetLevel(int level)
{
	_level = level;
}