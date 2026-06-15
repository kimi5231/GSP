#pragma once
#include "GameObject.h"

class Item : public GameObject
{
public:
	Item(ObjectType type);
	virtual ~Item() {};

public:
	virtual void Render(sf::RenderWindow* window);
};