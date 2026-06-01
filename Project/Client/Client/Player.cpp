#include "pch.h"
#include "Player.h"

Player::Player()
{
    _texture.loadFromFile("Resource/BlackPawn.bmp");
    _sprite.setTexture(_texture);
    _sprite.setPosition(500, 500);
}

void Player::Render(sf::RenderWindow* window)
{
    window->draw(_sprite);
}