#pragma once
#include "Item.h"

class Weapon : public Item
{
public:
	Weapon() {};
	virtual ~Weapon() {};

public:
	virtual void SetObjectType(ObjectType type);

private:
	float _skilCoolTime;
};