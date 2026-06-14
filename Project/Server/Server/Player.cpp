#include "pch.h"
#include "Player.h"

Player::Player()
{
	_type = ObjectType::Player;

	// юс╫ц
	_maxHP = 100;
	_hp = _maxHP;
	_level = 1;
	_exp = 50;
	_damage = 10;
}

Player::~Player()
{
}

void Player::Init()
{
	_objectPoolState = ObjectPoolState::InWorld;
}

int Player::GetDamage()
{
	return _damage;
}