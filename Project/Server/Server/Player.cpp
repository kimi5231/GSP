#include "pch.h"
#include "Player.h"

Player::Player()
{
	_maxHP = 100;
	_hp = _maxHP;
	_exp = 0;
	_level = 1;
}

Player::~Player()
{
}

void Player::Init()
{
	_objectPoolState = ObjectPoolState::InWorld;
	_client = nullptr;
}