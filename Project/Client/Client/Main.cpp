#include <SFML/Graphics.hpp>

int main()
{
    // 1. 800x600 크기의 윈도우 창 생성
    sf::RenderWindow window(sf::VideoMode(800, 600), "GSP Test Client");

    // 2. 간단한 캐릭터 대용 원(Circle) 도형 만들기
    sf::CircleShape player(20.f); // 반지름 20짜리 원
    player.setFillColor(sf::Color::Green); // 초록색 캐릭터
    player.setPosition(400.f, 300.f);     // 초기 좌표 (x, y)

    // 3. 메인 게임/렌더링 루프
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // 창 닫기 버튼 누르면 종료
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // 키보드 입력 받아서 캐릭터 좌표 이동 테스트
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) player.move(-0.1f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) player.move(0.1f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) player.move(0.f, -0.1f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) player.move(0.f, 0.1f);

        // 4. 화면 그리기 처리
        window.clear();          // 이전 프레임 지우기 (도화지 초기화)
        window.draw(player);     // 플레이어 그리기
        window.display();        // 화면에 표출
    }

    return 0;
}