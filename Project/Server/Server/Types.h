#pragma once

using TimePoint = std::chrono::steady_clock::time_point;

//------------------Enums------------------
enum class IOType
{
	Send,
	Recv,
	Accept,

	MonsterEvent,
};

enum class MonsterEventType
{
	Move,
	UpdateStat,
};

enum class SessionState
{
	Title,
	Play,
};

enum class LoginResult
{
	Sucess,
};

enum class ObjectType : char
{
	Player,
	Peace,
	Agro,
	Item,
};

enum ObjectState : char
{
	IDLE,
	
	// Etc
	HIT,
	DEAD,

	// Structure
	LOCK,
	OPEN,
	CLOSE,

	// Monster
	ROAMING,
	CHASE,
	ATTACK,
	RETURN,
};

enum class ObjectPoolState 
{
	InWorld, 
	InInventory,
	Reusable,
};

enum Dir
{
	Up,
	Right,
	Down,
	Left,

	DirCount
};

//------------------Struct------------------
struct Vector
{
	int x;
	int y;

	inline Vector() : x(0), y(0) {}
	inline Vector(int X, int Y) : x(X), y(Y) {}

	Vector operator+(const Vector& other) const
	{
		return {other.x + x, other.y + y };
	}

	Vector operator-(const Vector& other) const
	{
		return { x - other.x, y - other.y };
	}

	Vector operator/(const int value) const
	{
		return { x/ value, y/ value };
	}

	Vector operator*(const int value) const
	{
		return { x * value, y * value };
	}

	void operator*=(const int value)
	{
		x*= value;
		y*= value;
	}

	bool operator<(const Vector& other) const
	{
		return (x < other.x || y < other.y);
	}

	bool operator>(const Vector& other) const
	{
		return (x > other.x || y > other.y);
	}

	bool operator>=(const Vector& other) const
	{
		return (x >= other.x || y >= other.y );
	}

	bool operator==(const Vector& other) const
	{
		return (x == other.x && y == other.y );
	}
};

// unordered map에 Vector를 쓰기 위해 필요한 구조체
struct VectorHash
{
	size_t operator()(const Vector& v) const noexcept
	{
		// 두 값을 섞어서 Hash 생성
		std::size_t h1 = std::hash<int>()(v.x);
		std::size_t h2 = std::hash<int>()(v.y);
		return h1 ^ (h2 << 1);
	}
};

struct TileNode
{
	Vector index;
	float g, h, f;
	TileNode* parent;
};

struct ItemInfo
{
	Vector size;
	float weight;
	int cost;
};

struct MonsterStat
{
	int hp;
	int level;
	int damage;
	Vector minPos;
	Vector maxPos;
};

struct PeaceStat
{
	int hp;
	int level;
	Vector minPos;
	Vector maxPos;
	int damage;
};