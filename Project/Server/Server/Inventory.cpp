#include "pch.h"
#include "Inventory.h"

Inventory::Inventory()
{
	for (int i = 0; i < MAX_INVENTORY; ++i)
		_items[i] = -1;
}

Inventory::~Inventory()
{
}

bool Inventory::AddItem(int id)
{
	for (int i = 0; i < MAX_INVENTORY; ++i)
	{
		if (_items[i] == -1)
		{
			_items[i] = id;
			return true;
		}
	}

	return false;
}

bool Inventory::RemoveItem(int id)
{
	for (int i = 0; i < MAX_INVENTORY; ++i)
	{
		if (_items[i] == id)
		{
			_items[i] = -1;
			return true;
		}
	}

	return false;
}

int Inventory::ExistItem(int id)
{
	for (int i = 0; i < MAX_INVENTORY; ++i)
	{
		if (_items[i] == id)
			return i;
	}
	
	return -1;
}