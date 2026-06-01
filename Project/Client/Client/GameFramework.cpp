#include "pch.h"
#include "GameFramework.h"
#include "Player.h"

GameFramework::GameFramework(sf::RenderWindow* window)
{	
	// 윈도우창 생성
	_window = window;
    _avatar = std::make_shared<Player>();
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

    //// 키보드 입력 받아서 캐릭터 좌표 이동 테스트
    //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { player.move(-0.1f, 0.f); }
    //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { player.move(0.1f, 0.f); }
    //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { player.move(0.f, -0.1f); }
    //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { player.move(0.f, 0.1f); }
}

void GameFramework::Render()
{
    _window->clear(); 
	_avatar->Render(_window);
    _window->display();
}