#pragma once
class GameObject
{
public:
	GameObject() {};
	virtual ~GameObject() {};

public:
	virtual void Update() {};
	virtual void Render(sf::RenderWindow* window) {};
};

