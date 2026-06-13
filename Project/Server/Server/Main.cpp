#include "pch.h"
#include "Global.h"

void WorkerThread()
{
	while (true)
	{
		g_framework->Update();
		g_network->Update();
		g_timer->Update();
	}
}

int main()
{
	g_dataManager = new DataManager();
	g_framework = new ServerFramework();
	g_network = new ServerNetwork(g_framework);
	g_timer = new Timer();
	
	std::vector<std::thread> workerThreads;
	int threadCount = std::thread::hardware_concurrency();

	for (int i = 0; i < threadCount; ++i)
		workerThreads.emplace_back(WorkerThread);
	for (auto& thread : workerThreads)
		thread.join();
}