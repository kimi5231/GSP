#include "pch.h"
#include "Player.h"

Player::Player()
{
    _texture.loadFromFile("Resource/BlackPawn.png");
    _sprite.setTexture(_texture);
}

void Player::Render(sf::RenderWindow* window)
{
    window->draw(_sprite);
}