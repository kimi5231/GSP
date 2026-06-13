#include "pch.h"
#include "Monster.h"

Monster::Monster()
{
	_texture.loadFromFile("Resource/WhitePawn.png");
	_sprite.setTexture(_texture);
}

void Monster::Update()
{
}

void Monster::Render(sf::RenderWindow* window)
{
	window->draw(_sprite);
}