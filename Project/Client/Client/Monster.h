#pragma once
#include "Creature.h"

class Bar;

class Monster : public Creature
{
public:
	Monster(ObjectType type);
	virtual ~Monster() {};

public:
	virtual void Update();
	virtual void Render(sf::RenderWindow* window);

public:
	virtual void SetMaxHP(int maxHP);
	virtual void SetHP(int hp);
	Bar* GetHpBar() { return _hpBar; }

private:
	Bar* _hpBar;
};