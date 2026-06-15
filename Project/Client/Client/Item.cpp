#include "pch.h"
#include "Item.h"

Item::Item(ObjectType type)
{
	switch(type)
	{
	case ObjectType::Sword:
		_texture.loadFromFile("Resource/Sword.png");
		_textureInventory.loadFromFile("Resource/SwordInventory.png");
		break;
	}

	_sprite.setTexture(_texture);
	_spriteInventory.setTexture(_textureInventory);

	_isClick = false;
}

void Item::Render(sf::RenderWindow* window)
{
	window->draw(_sprite);
}