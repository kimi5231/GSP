#pragma once
#include "UI.h"

class Bar : public UI
{
public:
	Bar(Vector pos, Vector size, UIType type);
	virtual ~Bar() {};

public:
	virtual void Render(sf::RenderWindow* window);

public:
	void SetMaxValue(int value) { _maxValue = value; }
	void SetCurrentValue(int value) { _currentValue = value; }

private:
	float _maxValue;
	float _currentValue;
	sf::RectangleShape _background;
	sf::RectangleShape _gage;
};