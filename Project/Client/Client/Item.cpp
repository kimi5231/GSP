#include "pch.h"
#include "Item.h"

Item::Item(ObjectType type)
{
	switch(type)
	{
	case ObjectType::Sword:
		_texture.loadFromFile("Resource/Sword.png");
		break;
	}

	_sprite.setTexture(_texture);
}

void Item::Render(sf::RenderWindow* window)
{
	window->draw(_sprite);
}