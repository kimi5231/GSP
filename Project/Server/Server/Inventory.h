#pragma once
class Inventory
{
public:
	Inventory();
	~Inventory();

public:
	bool AddItem(int id);
	bool RemoveItem(int id);
	int ExistItem(int id);

private:
	std::array<int, MAX_INVENTORY> _items;
};