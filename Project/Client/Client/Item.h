#pragma once
#include "GameObject.h"

class Item : public GameObject
{
public:
	Item(ObjectType type);
	virtual ~Item() {};

public:
	virtual void Render(sf::RenderWindow* window);

public:
	sf::Sprite& GetInInventoryTexture() { return _spriteInventory; }

private:
	sf::Texture _textureInventory;
	sf::Sprite _spriteInventory;

	bool _isInInventoryOpen;
};