#include "pch.h"
#include "Weapon.h"

void Weapon::SetObjectType(ObjectType type)
{
	GameObject::SetObjectType(type);

	switch (type)
	{
	case ObjectType::Sword:
		_skilCoolTime = 5.f;
		break;
	}
}