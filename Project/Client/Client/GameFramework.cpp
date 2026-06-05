#include "pch.h"
#include "GameFramework.h"
#include "Global.h"
#include "Player.h"

GameFramework::GameFramework(sf::RenderWindow* window)
{	
	// 윈도우창 생성
	_window = window;
    _avatar = nullptr;
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
		Vector pos = _avatar->GetPos();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            _avatar->SetPos({ pos.x - 50, pos.y });
			g_network->SendMovePacket(_avatar);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            _avatar->SetPos({ pos.x + 50, pos.y });
            g_network->SendMovePacket(_avatar);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            _avatar->SetPos({ pos.x, pos.y - 50 });
            g_network->SendMovePacket(_avatar);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            _avatar->SetPos({ pos.x, pos.y + 50 });
            g_network->SendMovePacket(_avatar); 
        }
    }
}

void GameFramework::Render()
{
    _window->clear(); 
    if (_avatar)
	    _avatar->Render(_window);
    for (auto& [id, player] : _players)
    {
        player->Render(_window);
    }
    _window->display();
}

void GameFramework::CreateAvatar(int playerId, int visualId, short x, short y, int hp, int maxHp, long long exp, int level)
{
    _avatar = std::make_shared<Player>();
    _avatar->SetPos(x, y);
    _avatar->SetHP(hp);
    _avatar->SetMaxHP(maxHp);
    _avatar->SetExp(exp);
	_avatar->SetLevel(level);
}

void GameFramework::AddPlayer(int id, int visualID, const char* name, short x, short y, int hp, int maxHp, long long exp, int level)
{
    PlayerRef player = std::make_shared<Player>();
    player->SetPos(x, y);
    player->SetHP(hp);
    player->SetMaxHP(maxHp);
    player->SetExp(exp);
    player->SetLevel(level);
    _players[id] = player;
}

GameObjectRef GameFramework::GetGameObject(ObjectType type, int id)
{
    switch (type)
    {
    case ObjectType::Player:
        return _players[id];
    }
}