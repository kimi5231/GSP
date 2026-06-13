#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
	Player();
	virtual ~Player() {};

public:
	virtual void Update() {};
	virtual void Render(sf::RenderWindow* window);

public:
	bool IsCanMove();

public:
	void SetMaxHP(int maxHP) { _maxHP = maxHP; }
	void SetHP(int hp) { _hp = hp; }
	void SetExp(long long exp) { _exp = exp; }
	void SetLevel(int level) { _level = level; }

private:
	int _maxHP;
	int _hp;
	long long _exp;
	int _level;
	TimePoint _lastMoveTime;
};
