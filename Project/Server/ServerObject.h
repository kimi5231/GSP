#pragma once
class ServerObject
{
public:
	ServerObject() {};
	ServerObject(int id);
	~ServerObject();

public:
	Vector Move(Dir dir);

public:
	int GetID() { return _id; }
	void SetID(int id) { _id = id; }
	Vector GetPos() { return _pos; }

private:
	int _id;
	Vector _pos;
};