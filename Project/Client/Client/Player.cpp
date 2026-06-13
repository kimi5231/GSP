#include "pch.h"
#include "Player.h"

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