#include "pch.h"
#include "Equipment.h"
#include "Item.h"

Equipment::Equipment()
{
	_pos = { 550, 200 };

	_texture.loadFromFile("Resource/Equipment.png");
	_sprite.setTexture(_texture);
	_sprite.setPosition(_pos.x, _pos.y);

	_currentWeapon = nullptr;
}

void Equipment::Render(sf::RenderWindow* window)
{
	window->draw(_sprite);

	if (_currentWeapon)
	{
		sf::Sprite& sprite = _currentWeapon->GetInInventorySprite();
		if (!_currentWeapon->GetIsClick())
			sprite.setPosition(_pos.x + 10, _pos.y + 10);
		window->draw(sprite);
	}
}