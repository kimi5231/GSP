#include "pch.h"
#include "Monster.h"
#include "FSM.h"
#include "State.h"
#include "Global.h"

Monster::Monster()
{
	_fsm = new FSM();
	_target = nullptr;

	std::uniform_int_distribution<int> randPos(0, 1999);
	Vector index{ randPos(gen), randPos(gen) };
	_returnPos = { index.x * TILE_SIZE, index.y * TILE_SIZE };
	_pos = _returnPos;

	_sumTime = 0.f;
}

Monster::~Monster()
{
}

void Monster::Update()
{
	_fsm->Update(this);
}

std::deque<Vector> Monster::FindPath(Vector goal)
{
	// 길찾기에 필요한 자료구조 정의
	auto cmp = [](TileNode* a, TileNode* b) { return a->f > b->f; };
	std::priority_queue<TileNode*, std::vector<TileNode*>, decltype(cmp)> openList(cmp);
	std::unordered_map<Vector, TileNode*, VectorHash> openMap;
	std::unordered_set<Vector, VectorHash> closeList;

	std::vector<TileNode*> allocatedNodes;

	// 시작 위치와 목적지의 인덱스 계산
	Vector startIndex = _pos/TILE_SIZE;
	Vector goalIndex = goal / TILE_SIZE;

	if (startIndex == goalIndex)
		return {};

	// openList에 시작 노드 추가
	// 맨해튼 거리로 휴리스틱 계산
	float startH = abs(startIndex.x - goalIndex.x) + abs(startIndex.y - goalIndex.y);
	TileNode* startNode = new TileNode(startIndex, 0, startH, startH, nullptr);
	openList.push(startNode);
	openMap[startIndex] = startNode;
	allocatedNodes.push_back(startNode);

	// 평면 4방향 오프셋 (우, 좌, 상, 하)
	const int dx[4] = { 1, -1, 0, 0 };
	const int dy[4] = { 0, 0, 1, -1 };

	while (!openList.empty())
	{
		// openList에서 f값이 가장 작은 노드 꺼내기
		TileNode* currentNode = openList.top();
		openList.pop();
		openMap.erase(currentNode->index);
		closeList.insert(currentNode->index);

		// 현재 노드와 연결된 노드 확인
		for (int i = 0; i < 4; ++i)
		{
			int nextX = currentNode->index.x + dx[i];
			int nextY = currentNode->index.y + dy[i];

			Vector connectedIndex{ nextX, nextY };

			// 이미 closeList에 있으면 무시
			if (closeList.contains(connectedIndex))
				continue;

			// 목적지 노드인지 확인
			if (connectedIndex.x == goalIndex.x && connectedIndex.y == goalIndex.y)
			{
				std::deque<Vector> path;
				TileNode* node = currentNode;
				while (node != startNode)
				{
					Vector pos = node->index * TILE_SIZE;
					path.push_back(pos);
					node = node->parent;
				}
				std::reverse(path.begin(), path.end());
				_path = path;

				// 메모리 해제 후 경로 반환
				for (TileNode* allocated : allocatedNodes)
					delete allocated;

				return _path;
			}

			if (!g_framework->IsCanGo(connectedIndex))
				continue;

			// 이동 비용 계산
			float g = currentNode->g + 1;
			float h = abs(connectedIndex.x - goalIndex.x) + abs(connectedIndex.y - goalIndex.y);
			float f = g + h;

			// 이미 openList에 있지만 기존 경로가 더 낫다면 무시
			if (openMap.contains(connectedIndex) && openMap[connectedIndex]->f <= f)
				continue;

			// 두 List에 모두 없거나, 새로운 경로가 더 낫다면 openList에 추가
			TileNode* newNode = new TileNode{ connectedIndex, g, h, f, currentNode };
			openList.push(newNode);
			openMap[connectedIndex] = newNode;
			allocatedNodes.push_back(newNode);
		}
	}

	for (TileNode* allocated : allocatedNodes)
		delete allocated;

	return {};
}

void Monster::RandomMove()
{
	std::uniform_int_distribution<int> selectDir(0, 3);
	Dir dir = static_cast<Dir>(selectDir(gen));

	_prevPos = _pos;

	switch (dir)
	{
	case Up:
		_pos.y -= TILE_SIZE;
		break;
	case Right:
		_pos.x += TILE_SIZE;
		break;
	case Down:
		_pos.y += TILE_SIZE;
		break;
	case Left:
		_pos.x -= TILE_SIZE;
		break;
	}

	if (!g_framework->IsCanGo(_pos.x, _pos.y))
		_pos = _prevPos;

	if(!IsHome(_returnPos))
		_pos = _prevPos;
}

void Monster::Move()
{
	if (_path.empty())
		return;

	_prevPos = _pos;
	_pos = _path[0];
	_path.pop_front();

	if (!IsHome(_returnPos) && _state != ObjectState::RETURN)
		SetState(ObjectState::RETURN);
}

void Monster::UpdateNextAttackTime()
{
	auto delay = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<float>(_attackDelay));

	_nextAttackTime = std::chrono::steady_clock::now() + delay;
}

bool Monster::IsAgro(Vector pos)
{
	return abs(_pos.x - pos.x) <= MONSTER_AGRO_RANGE * TILE_SIZE && abs(_pos.y - pos.y) <= MONSTER_AGRO_RANGE * TILE_SIZE;
}

bool Monster::IsNear(Vector pos)
{
	return abs(_pos.x - pos.x) <= MONSTER_NEAR_RANGE * TILE_SIZE && abs(_pos.y - pos.y) <= MONSTER_NEAR_RANGE * TILE_SIZE;
}

bool Monster::IsHome(Vector pos)
{
	return abs(_pos.x - pos.x) <= MONSTER_ROAMING_RANGE * TILE_SIZE && abs(_pos.y - pos.y) <= MONSTER_ROAMING_RANGE * TILE_SIZE;
}

bool Monster::SetState(ObjectState state)
{
	if (!Creature::SetState(state))
		return false;

	switch (state)
	{
	case IDLE:
		_fsm->ChangeState(g_idleState, this);
		break;
	case CHASE:
		_fsm->ChangeState(g_chaseState, this);
		break;
	case RETURN:
		_fsm->ChangeState(g_returnState, this);
		break;
	case ATTACK:
		_fsm->ChangeState(g_attackState, this);
		break;
	case HIT:
		_fsm->ChangeState(g_hitState, this);
		break;
	case DEAD:
		_fsm->ChangeState(g_deadState, this);
		break;
	}

	return true;
}