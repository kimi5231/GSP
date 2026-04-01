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
	Vector GetPos() { return _pos; }

private:
	int _id;
	Vector _pos;
};