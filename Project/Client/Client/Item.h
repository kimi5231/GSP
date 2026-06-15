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
	sf::Sprite& GetInInventorySprite() { return _spriteInventory; }
	void SetInInventorySpritePos(sf::Vector2f pos) { _spriteInventory.setPosition(pos.x, pos.y); }
	void SetIsClick(bool isClick) { _isClick = isClick; }
	bool GetIsClick() { return _isClick; }

private:
	sf::Texture _textureInventory;
	sf::Sprite _spriteInventory;

	bool _isClick;
};