#include "pch.h"
#include "Inventory.h"
#include "Item.h"

Inventory::Inventory()
{
	_pos = { 600, 200 };

	_texture.loadFromFile("Resource/Inventory.png");
	_sprite.setTexture(_texture);
	_sprite.setPosition(_pos.x, _pos.y);

	for (int i = 0; i < MAX_INVENTORY; ++i)
		_items[i] = nullptr;
}

Inventory::~Inventory()
{
}

void Inventory::Render(sf::RenderWindow* window)
{
	window->draw(_sprite);

	for (int i = 0; i < MAX_INVENTORY; ++i)
	{
		if (_items[i])
		{
			sf::Sprite sprite = _items[i]->GetInInventoryTexture();
			sprite.setPosition(_pos.x + (i%3) * InventoryTileSize, _pos.y + (i / 3) * InventoryTileSize);
			window->draw(sprite);
		}
	}
}

void Inventory::AddItem(int index, ItemRef item)
{
	_items[index] = item;
}

void Inventory::RemoveItem(int index)
{
	_items[index] = nullptr;
}