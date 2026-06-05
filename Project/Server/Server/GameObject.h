#pragma once
class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

public:
	virtual void Init();
	virtual void Update();

public:
	int GetID() { return _id; }
	void SetID(int id) { _id = id; }
	ObjectType GetObjectType() { return _type; }
	void SetObjectType(ObjectType type) { _type = type; }
	Vector GetPos() { return _pos; }
	void SetPos(Vector pos) { _pos = pos; };
	ObjectState GetObjectState() { return _state; }
	virtual bool SetObjectState(ObjectState state, bool isSend = true);
	ObjectPoolState GetObjectPoolState() { return _objectPoolState; }
	void SetObjectPoolState(ObjectPoolState objectPoolState);
	
protected:
	int _id;
	int _visualID;
	ObjectType _type;
	Vector _pos;
	ObjectState _state;
	ObjectPoolState _objectPoolState;
};