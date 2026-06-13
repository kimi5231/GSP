#include "pch.h"
#include "GameFramework.h"
#include "Global.h"
#include "Player.h"
#include "Monster.h"

GameFramework::GameFramework(sf::RenderWindow* window)
{	
	// 윈도우창 생성
	_window = window;
    _avatar = nullptr;

    _map = g_dataManager->GetTilemap();

    _texture.loadFromFile("Resource/Tile0.png");
    _sprite.setTexture(_texture);
}

GameFramework::~GameFramework()
{
}

void GameFramework::Update()
{
    sf::Event event;
    while (_window->pollEvent(event))
    {
        // 창 닫기 버튼 누르면 종료
        if (event.type == sf::Event::Closed)
            _window->close();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) && !_avatar)
        g_network->SendLoginPacket("abc");
   
    if (_avatar && _window->hasFocus())
    {
       if (!_avatar->IsCanMove())
            return;

		Vector pos = _avatar->GetPos();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            pos.x -= TILE_SIZE;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            pos.x += TILE_SIZE;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            pos.y -= TILE_SIZE;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            pos.y += TILE_SIZE;
        else
            return;

        if (!IsCanGo(pos))
            return;

        _avatar->SetPos(pos);
        g_network->SendMovePacket(_avatar);
    }

    if (_avatar)
    {
        Vector cameraPos = _avatar->GetPos();

        cameraPos.x = std::max(WINDOW_WIDTH / 2, std::min(cameraPos.x, WORLD_WIDTH * TILE_SIZE - WINDOW_WIDTH / 2));
        cameraPos.y = std::max(WINDOW_HEIGHT / 2, std::min(cameraPos.y, WORLD_HEIGHT * TILE_SIZE - WINDOW_HEIGHT / 2));

        _view.setCenter(cameraPos.x, cameraPos.y);
        _window->setView(_view);
    }
}

void GameFramework::Render()
{
    _window->clear(); 

    sf::Vector2f viewPos = _view.getCenter();
    Vector index{ static_cast<int>(viewPos.x) / TILE_SIZE, static_cast<int>(viewPos.y) / TILE_SIZE };
   
    int viewRange = 12;
    Vector start{ std::max(0, index.x - viewRange), std::max(0, index.y - viewRange) };
    Vector end{ std::min(WORLD_WIDTH - 1, index.x + viewRange), std::min(WORLD_HEIGHT - 1, index.y + viewRange) };
  
    for (int y = start.y; y <= end.y; ++y)
    {
        for (int x = start.x; x <= end.x; ++x)
        {
            _sprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
            _window->draw(_sprite);
        }
    }

    if (_avatar)
	    _avatar->Render(_window);

    for (auto& [id, object] : _objects)
        object->Render(_window);

    _window->display();
}

void GameFramework::LoadTile()
{

}

void GameFramework::CreateAvatar(int playerId, int visualId, short x, short y, int hp, int maxHp, long long exp, int level)
{
    _avatar = std::make_shared<Player>();
    _avatar->SetID(playerId);
    _avatar->SetPos(x, y);
    _avatar->SetHP(hp);
    _avatar->SetMaxHP(maxHp);
    _avatar->SetExp(exp);
	_avatar->SetLevel(level);
}

void GameFramework::AddCreature(int id, int visualID, const char* name, short x, short y, int hp, int maxHp, long long exp, int level)
{
    CreatureRef creture;
    
    switch (static_cast<ObjectType>(visualID))
    {
    case ObjectType::Player:
        creture = std::make_shared<Player>();
        creture->SetName(name);
        break;
    case ObjectType::Monster:
        creture = std::make_shared<Monster>();
        break;
    }

    creture->SetID(id);
    creture->SetPos(x, y);
    creture->SetHP(hp);
    creture->SetMaxHP(maxHp);
    creture->SetExp(exp);
    creture->SetLevel(level);

    _objects[id] = creture;
}

void GameFramework::RemoveObject(int id)
{
    _objects.erase(id);
}

bool GameFramework::IsCanGo(Vector pos)
{
    Vector index = pos / TILE_SIZE;
    Vector max{ static_cast<int>(_map[0].size()), static_cast<int>(_map.size()) };

    if (index < Vector{ 0, 0 } || index >= max)
        return false;

    if (_map[index.x][index.y] != 0)
        return false;

    return true;
}

GameObjectRef GameFramework::GetGameObject(int id)
{
    if(_objects.count(id))
        return _objects[id];

    return nullptr;
}