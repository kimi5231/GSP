#include "pch.h"
#include "Global.h"

int main()
{
	g_framework = new ServerFramework();
	g_network = new ServerNetwork(g_framework);

	while(true)
	{
		g_network->Update();
		g_framework->Update();
	}
}