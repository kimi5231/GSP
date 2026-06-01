#pragma once
class GameObject;
class Player;

class GameFramework
{
public:
	GameFramework(sf::RenderWindow* window);
	~GameFramework();

public:
	void Update();
	void Render();

private:
	sf::RenderWindow* _window;

private:
	PlayerRef _avatar;
	std::vector<GameObject*> _gameObjects;
};