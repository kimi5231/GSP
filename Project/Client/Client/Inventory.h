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

private:
	sf::Texture _texture;
	sf::Sprite _sprite;

	Vector _pos;

	std::array<ItemRef, MAX_INVENTORY> _items;
};