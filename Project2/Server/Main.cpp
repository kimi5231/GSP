#include "pch.h"
#include "Global.h"
#include "ServerFramework.h"

int main()
{
	g_serverFramework = new ServerFramework();

	while(true)
	{
		g_serverFramework->Update();
	}
}