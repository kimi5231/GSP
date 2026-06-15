#include "pch.h"
#include "Player.h"
#include "Global.h"
#include "Weapon.h"
#include "Inventory.h"

Player::Player()
{
	_type = ObjectType::Player;

	_currentWeapon = nullptr;
	_inventory = new Inventory();

	// 임시
	_maxHP = 100;
	_hp = _maxHP;
	_level = 1;
	_exp = 50;
	_damage = 10;
	_attackDelay = 1.f;
	_sumTime = 0.f;
}

Player::~Player()
{
}

void Player::Init()
{
	_objectPoolState = ObjectPoolState::InWorld;
}

void Player::Update()
{
	_sumTime += g_timer->GetDeltaTime();

	if (_sumTime > 5)
	{
		_sumTime = 0;
		TackHeal(_maxHP / 10);
		g_network->SendStatusChangePacket(this, _id);
	}
}

void Player::AddExp(long long exp)
{
	_exp += exp;

	// 레벨업에 필요한 경험치만큼 모았다면 레벨업
	long long maxExp = pow(2, _level - 1) * 100;
	if (maxExp <= _exp)
	{
		_level++;
		_exp -= maxExp;
		g_network->SendStatusChangePacket(this, _id);
	}
}

bool Player::AddItemToInventory(int id)
{
	return _inventory->AddItem(id);
}

bool Player::RemoveItemFromInventory(int id)
{
	return _inventory->RemoveItem(id);
}

bool Player::IsSkillTack(Vector pos)
{
	return abs(_pos.x - pos.x) <= SKILL_RANGE * TILE_SIZE && abs(_pos.y - pos.y) <= SKILL_RANGE * TILE_SIZE;
}

int Player::GetDamage()
{
	return _damage;
}