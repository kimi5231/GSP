#include "pch.h"
#include "Global.h"

int main()
{
    // 윈도우창 생성
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "GSP");
    window->setFramerateLimit(60);

    g_dataManager = new DataManager();
    g_framework = new GameFramework(window);
	g_network = new GameNetwork();

    while (window->isOpen())
    {
        g_network->Update();
		g_framework->Update();
		g_framework->Render();
    }

    delete g_network;

    return 0;
}