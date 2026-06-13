#pragma once
#include "Monster.h"

class Agro : public Monster
{
public:
	Agro();
	virtual ~Agro();

public:
	virtual void Update();

public:
	virtual bool IsReadyNextState();

public:
	virtual bool SetState(ObjectState state);

private:
	
};