#pragma once
#include "Creature.h"

class Player : public Creature
{
public:
	Player();
	virtual ~Player() {};

public:
	virtual void Update() {};
	virtual void Render(sf::RenderWindow* window);

public:
	bool IsCanMove();
	bool IsCanAttack();
	bool IsCanSkill();

public:
	virtual void SetMaxHP(int maxHP);
	virtual void SetHP(int hp);
	virtual void SetExp(long long exp);
	virtual void SetLevel(int level);

private:
	TimePoint _lastMoveTime;
	TimePoint _lastAttackTime;
	TimePoint _lastSkillTime;
};
