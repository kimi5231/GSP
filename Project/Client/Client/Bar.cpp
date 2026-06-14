#include "pch.h"
#include "Bar.h"

Bar::Bar(Vector pos, Vector size, UIType type)
	: UI(pos, size)
{
	_background.setPosition(pos.x, pos.y);
	_background.setSize(sf::Vector2f(size.x, size.y));
	_background.setFillColor(sf::Color(80, 80, 80));
	_gage.setPosition(pos.x + 5, pos.y + 5);
	_gage.setSize(sf::Vector2f(size.x - 10, size.y - 10));

	switch (type)
	{
	case UIType::HpBar:
		_gage.setFillColor(sf::Color::Red);
		break;
	case UIType::ExpBar:
		_gage.setFillColor(sf::Color::Green);
		break;
	}
}

void Bar::Render(sf::RenderWindow* window)
{
	window->draw(_background);
	_gage.setSize(sf::Vector2f((_size.x - 10) * (_currentValue / _maxValue), _size.y - 10));
	window->draw(_gage);
}