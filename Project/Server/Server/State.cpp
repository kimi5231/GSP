#include "pch.h"
#include "State.h"
#include "Global.h"
#include "Agro.h"
#include "Peace.h"

void State::Tick(Monster* monster)
{
	if (monster->IsReadyNextState())
		monster->SetState(monster->GetStateTable().at(monster->GetState()));
}

IdleState* g_idleState = new IdleState();
RoamingState* g_roamingState = new RoamingState();
ChaseState* g_chaseState = new ChaseState();
ReturnState* g_returnState = new ReturnState();
AttackState* g_attackState = new AttackState();
HitState* g_hitState = new HitState();
DeadState* g_deadState = new DeadState();

//--------------Idle--------------
void IdleState::Tick(Monster* monster)
{
	State::Tick(monster);

	monster->AddDeltaTime(g_timer->GetDeltaTime());
}

void IdleState::Exit(Monster* monster)
{
	monster->InitSumTime();
}

//--------------Roaming--------------
void RoamingState::Enter(Monster* monster)
{
	// 랜덤한 방향으로 이동
	Vector prevPos = monster->GetPos();
	monster->RandomMove();

	if (prevPos != monster->GetPos())
	{
		ExpOver* over = new ExpOver(IOType::Monster);
		over->_monsterEventType = MonsterEventType::Move;
		PostQueuedCompletionStatus(g_network->GetIOCP(), 0, static_cast<ULONG_PTR>(monster->GetID()), &over->_over);
	}
}

void RoamingState::Tick(Monster* monster)
{
	State::Tick(monster);
}

//--------------Chase--------------
void ChaseState::Tick(Monster* monster)
{
	Vector prevPos = monster->GetPos();
	monster->FindPath(monster->GetTarget()->GetPos());
	monster->Move();
	
	if (prevPos != monster->GetPos())
	{
		ExpOver* over = new ExpOver(IOType::Monster);
		over->_monsterEventType = MonsterEventType::Move;
		PostQueuedCompletionStatus(g_network->GetIOCP(), 0, static_cast<ULONG_PTR>(monster->GetID()), &over->_over);
	}
}

void ChaseState::Exit(Monster* monster)
{
	monster->ClearPath();
}

//--------------Return----------------
void ReturnState::Enter(Monster* monster)
{
	std::deque<Vector> path = monster->FindPath(monster->GetReturnPos());
	if (path.empty())
		monster->SetObjectState(ObjectState::IDLE);
}

void ReturnState::Tick(Monster* monster)
{
	State::Tick(monster);
	
	Vector prevPos = monster->GetPos();
	monster->Move();

	if (monster->IsNear(monster->GetReturnPos()))
		monster->SetPos(monster->GetReturnPos());

	if (prevPos != monster->GetPos())
	{
		ExpOver* over = new ExpOver(IOType::Monster);
		over->_monsterEventType = MonsterEventType::Move;
		PostQueuedCompletionStatus(g_network->GetIOCP(), 0, static_cast<ULONG_PTR>(monster->GetID()), &over->_over);
	}
}

void ReturnState::Exit(Monster* monster)
{
	monster->ClearPath();
	monster->SetTarget(nullptr);
}

//--------------Attack--------------
void AttackState::Enter(Monster* monster)
{
	monster->GetTarget()->TackDamage(monster->GetDamage());
}

void AttackState::Tick(Monster* monster)
{
	State::Tick(monster);
}

void AttackState::Exit(Monster* monster)
{
	monster->UpdateNextAttackTime();
}

//--------------Hit--------------
void HitState::Tick(Monster* monster)
{
	State::Tick(monster);
}
 
//--------------Dead--------------
void DeadState::Enter(Monster* monster)
{
	// 30초 뒤 부활
}