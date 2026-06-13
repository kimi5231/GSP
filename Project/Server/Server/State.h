#pragma once
class Monster;

class State
{
public:
	State() {};
	virtual ~State() {};

public:
	virtual void Enter(Monster* monster) = 0;
	virtual void Tick(Monster* monster);
	virtual void Exit(Monster* monster) = 0;
};

extern class IdleState* g_idleState;
extern class RoamingState* g_roamingState;
extern class ChaseState* g_chaseState;
extern class ReturnState* g_returnState;
extern class AttackState* g_attackState;
extern class HitState* g_hitState;
extern class DeadState* g_deadState;

class IdleState : public State
{
public:
	IdleState() {};
	virtual ~IdleState() {};

public:
	virtual void Enter(Monster* monster) override {};
	virtual void Tick(Monster* monster) override;
	virtual void Exit(Monster* monster) override;
};

class RoamingState : public State
{
public:
	RoamingState() {};
	virtual ~RoamingState() {};

public:
	virtual void Enter(Monster* monster) override;
	virtual void Tick(Monster* monster) override;
	virtual void Exit(Monster* monster) override {};
};

class ChaseState : public State
{
public:
	ChaseState() {};
	virtual ~ChaseState() {};

public:
	virtual void Enter(Monster* monster) override {};
	virtual void Tick(Monster* monster) override;
	virtual void Exit(Monster* monster) override;
};

class ReturnState : public State
{
public:
	ReturnState() {};
	virtual ~ReturnState() {};

public:
	virtual void Enter(Monster* monster) override;
	virtual void Tick(Monster* monster) override;
	virtual void Exit(Monster* monster) override;
};

class AttackState : public State
{
public:
	AttackState() {};
	virtual ~AttackState() {};

public:
	virtual void Enter(Monster* monster) override;
	virtual void Tick(Monster* monster) override;
	virtual void Exit(Monster* monster) override;
};

class HitState : public State
{
public:
	HitState() {};
	virtual ~HitState() {};

public:
	virtual void Enter(Monster* monster) override {};
	virtual void Tick(Monster* monster) override;
	virtual void Exit(Monster* monster) override {};
};

class DeadState : public State
{
public:
	DeadState() {};
	virtual ~DeadState() {};

public:
	virtual void Enter(Monster* monster) override;
	virtual void Tick(Monster* monster) override {};
	virtual void Exit(Monster* monster) override {};
};