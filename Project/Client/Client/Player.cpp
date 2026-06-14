#include "pch.h"
#include "Player.h"
#include "Global.h"
#include "Bar.h"

Player::Player()
{
    _lastMoveTime = std::chrono::high_resolution_clock::now();
    _texture.loadFromFile("Resource/BlackPawn.png");
    _sprite.setTexture(_texture);
}

void Player::Render(sf::RenderWindow* window)
{
    window->draw(_sprite);
}

bool Player::IsCanMove()
{
	auto duration = std::chrono::high_resolution_clock::now() - _lastMoveTime;

	long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	if (ms > 500)
	{
		_lastMoveTime = std::chrono::high_resolution_clock::now();
		return true;
	}

	return false;
}

void Player::SetMaxHP(int maxHP)
{
	Creature::SetMaxHP(maxHP);
	g_framework->GetHpBar()->SetMaxValue(maxHP);
}

void Player::SetHP(int hp)
{
	Creature::SetHP(hp);
	g_framework->GetHpBar()->SetCurrentValue(hp);
}

void Player::SetExp(long long exp)
{
	Creature::SetExp(exp);
	g_framework->GetExpBar()->SetCurrentValue(exp);
}

void Player::SetLevel(int level)
{
	Creature::SetLevel(level);
	g_framework->GetExpBar()->SetMaxValue(pow(2, level - 1) * 100);
}