#pragma once
#include "Creature.h"

class Weapon;

class Player : public Creature
{
public:
	Player();
	virtual ~Player();

public:
	virtual void Init();
	virtual void Update();

public:
	void AddExp(long long exp);

	bool AddItemToInventory(bool isTool, int id);
	bool RemoveItemFromInventory(bool isTool, int id);
	bool ExistItem(bool isTool, int id);

public:
	int GetDamage();

private:
	Weapon* _currentWeapon;
	//Inventory* _inventory{};



	int _damage;
	float _attackDelay;
	float _sumTime;
};

