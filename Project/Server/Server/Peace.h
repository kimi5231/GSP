#pragma once
#include "Monster.h"

class Peace : public Monster
{
public:
	Peace();
	virtual ~Peace();

public:
	virtual void Update();

public:
	virtual bool IsReadyNextState();
};