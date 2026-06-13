#pragma once
#include "Creature.h"
#include "Player.h"

class FSM;

class Monster : public Creature
{
public:
	Monster();
	virtual ~Monster();

public:
	virtual void Update();

public:
	virtual bool IsReadyNextState() { return false; };

	std::deque<Vector> FindPath(Vector goal);
	void ClearPath() { _path.clear(); }

	void RandomMove();
	void Move();

	void AddDeltaTime(float deltaTime) { _sumTime += deltaTime; }
	void InitSumTime() { _sumTime = 0.f; }

	void UpdateNextAttackTime();

	bool IsAgro(Vector pos);
	bool IsAttack(Vector pos);
	bool IsHome(Vector pos);

public:
	ObjectState GetState() { return _state; }
	virtual bool SetState(ObjectState state);
	const std::unordered_map<ObjectState, ObjectState>& GetStateTable() { return _stateTable; }
	Player* GetTarget() { return _target; }
	void SetTarget(Player* target) { _target = target; }
	Vector GetReturnPos() { return _returnPos; }
	Vector GetPrevPos() { return _prevPos; }
	int GetDamage() { return _damage; }

protected:
	FSM* _fsm;
	
	std::unordered_map<ObjectState, ObjectState> _stateTable;

	Player* _target;
	Vector _prevPos;
	Vector _returnPos;
	float _sumTime;
	std::chrono::steady_clock::time_point _nextAttackTime;
	float _attackDelay;

	std::deque<Vector> _path;

	float _idleTime;
	int _damage;
};