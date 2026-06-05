#pragma once
class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

public:
	virtual void Update() {};
	virtual void Render(sf::RenderWindow* window) {};

public:
	Vector GetPos() { return _pos; }
	void SetPos(short x, short y);
	void SetPos(Vector pos);

protected:
	sf::Texture _texture;
	sf::Sprite _sprite;

	int _id;
	Vector _pos;
};

