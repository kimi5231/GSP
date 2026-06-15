#pragma once
class Equipment
{
public:
	Equipment();
	~Equipment() {};

public:
	void Render(sf::RenderWindow* window);

public:
	ItemRef GetCurrentWeapon() { return _currentWeapon; }
	void SetCurrentWeapon(ItemRef item) { _currentWeapon = item; }
	sf::Sprite& GetEquipmentSprite() { return _sprite; }

private:
	sf::Texture _texture;
	sf::Sprite _sprite;

	Vector _pos;

	ItemRef _currentWeapon;
};