#include "pch.h"
#include "Monster.h"

Monster::Monster(ObjectType type)
{
	if(type == ObjectType::Agro)
		_texture.loadFromFile("Resource/WhitePawn.png");
	else
		_texture.loadFromFile("Resource/BlackStone.png");

	_sprite.setTexture(_texture);
}

void Monster::Update()
{
}

void Monster::Render(sf::RenderWindow* window)
{
	window->draw(_sprite);
}