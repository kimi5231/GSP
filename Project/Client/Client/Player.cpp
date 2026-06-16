#include "pch.h"
#include "Player.h"
#include "Global.h"
#include "Bar.h"

Player::Player()
{
    _lastMoveTime = std::chrono::high_resolution_clock::now();
	_lastAttackTime = std::chrono::high_resolution_clock::now();
	_lastSkillTime = std::chrono::high_resolution_clock::now();

	_isSkill = false;

    _texture.loadFromFile("Resource/BlackPawn.png");
	_sprite.setTexture(_texture);
	_skillTexture.loadFromFile("Resource/SkillEffect.png");
	_skillSprite.setTexture(_skillTexture);
}

void Player::Render(sf::RenderWindow* window)
{
	if (_isSkill)
	{
		_skillSprite.setPosition(_pos.x - 150, _pos.y - 150);
		window->draw(_skillSprite);
		_isSkill = false;
	}

    window->draw(_sprite);

	sf::Text text;
	text.setFont(g_dataManager->GetFont());            
	text.setString(_name);
	text.setCharacterSize(25); 
	text.setFillColor(sf::Color::Black);
	Vector pos = _pos;
	pos.y += 50;
	text.setPosition(pos.x, pos.y);
	window->draw(text);
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

bool Player::IsCanAttack()
{
	auto duration = std::chrono::high_resolution_clock::now() - _lastAttackTime;

	long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	if (ms > 1000)
	{
		_lastAttackTime = std::chrono::high_resolution_clock::now();
		return true;
	}

	return false;
}

bool Player::IsCanSkill()
{
	auto duration = std::chrono::high_resolution_clock::now() - _lastSkillTime;

	long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	if (ms > 5000)
	{
		_lastSkillTime = std::chrono::high_resolution_clock::now();
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