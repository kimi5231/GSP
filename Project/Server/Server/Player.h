#pragma once
#include "Creature.h"

class Session;

class Player : public Creature
{
public:
	Player();
	virtual ~Player();

public:
	virtual void Init();
	virtual void Update() {};

public:
	bool AddItemToInventory(bool isTool, int id);
	bool RemoveItemFromInventory(bool isTool, int id);
	bool ExistItem(bool isTool, int id);

	void Attack();

public:

private:
	//Inventory* _inventory{};
};

