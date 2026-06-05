#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	_objectPoolState = ObjectPoolState::Reusable;
}

GameObject::~GameObject()
{
}

void GameObject::Init()
{
}

void GameObject::Update()
{
}

bool GameObject::SetObjectState(ObjectState state, bool isSend)
{
	return false;
}

void GameObject::SetObjectPoolState(ObjectPoolState objectPoolState)
{
}