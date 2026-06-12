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

bool GameObject::IsVisiable(Vector pos)
{
	return abs(_pos.x - pos.x) <= VIEW_RANGE * TILE_SIZE && abs(_pos.y - pos.y) <= VIEW_RANGE * TILE_SIZE;
}

bool GameObject::SetObjectState(ObjectState state, bool isSend)
{
	return false;
}

void GameObject::SetObjectPoolState(ObjectPoolState objectPoolState)
{
	_objectPoolState = objectPoolState;
}