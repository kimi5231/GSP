#include "pch.h"
#include "Monster.h"
#include "Bar.h"

Monster::Monster(ObjectType type)
{
	if(type == ObjectType::Agro)
		_texture.loadFromFile("Resource/WhitePawn.png");
	else
		_texture.loadFromFile("Resource/BlackStone.png");

	_sprite.setTexture(_texture);

	_hpBar = new Bar({ 0, 0 }, { 50, 15 }, UIType::HpBar);
}

void Monster::Update()
{
}

void Monster::Render(sf::RenderWindow* window)
{
	window->draw(_sprite);

	Vector barPos = _pos;
	barPos.y -= 7;
	_hpBar->SetPos(barPos);
	_hpBar->Render(window);
}

void Monster::SetMaxHP(int maxHP)
{
	Creature::SetMaxHP(maxHP);
	_hpBar->SetMaxValue(maxHP);
}

void Monster::SetHP(int hp)
{
	Creature::SetHP(hp);
	_hpBar->SetCurrentValue(hp);
}