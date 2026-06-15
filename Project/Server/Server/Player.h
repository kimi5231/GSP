#pragma once
#include "Creature.h"
#include "Inventory.h"

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

	bool AddItemToInventory(int id);
	bool RemoveItemFromInventory(int id);
	int ExistItem(int id) { return _inventory->ExistItem(id); }

public:
	int GetDamage();
	Weapon* GetCurrentWeapon() { return _currentWeapon; }
	void SetCurrentWeapon(Weapon* weapon) { _currentWeapon = weapon; }

private:
	Weapon* _currentWeapon;
	Inventory* _inventory;

	int _damage;
	float _attackDelay;
	float _sumTime;
};

