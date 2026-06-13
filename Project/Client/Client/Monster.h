#pragma once
#include "Creature.h"

class Monster : public Creature
{
public:
	Monster();
	virtual ~Monster() {};

public:
	virtual void Update();
	virtual void Render(sf::RenderWindow* window);
};

