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

private:
	sf::Texture _texture;
	sf::Sprite _sprite;
};

