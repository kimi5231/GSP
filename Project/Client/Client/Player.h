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

private:
	TimePoint _lastMoveTime;
};
