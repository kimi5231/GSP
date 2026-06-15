#pragma once
class Inventory
{
public:
	Inventory();
	~Inventory();

public:
	void Render(sf::RenderWindow* window);

public:
	void AddItem(int index, ItemRef item);
	void RemoveItem(int index);

public:
	std::array<ItemRef, MAX_INVENTORY>& GetItems() { return _items; }
	sf::Sprite& GetInventorySprite() { return _sprite; }

private:
	sf::Texture _texture;
	sf::Sprite _sprite;

	Vector _pos;

	std::array<ItemRef, MAX_INVENTORY> _items;
};