#include "ModuleTaskManager.h"


void ModuleTaskManager::threadMain()
{

	while (true)
	{
		// TODO 3:
		// - Wait for new tasks to arrive
		std::unique_lock<std::mutex> lock(mtx);
		if (!scheduledTasks.empty())
		{
			// - Retrieve a task from scheduledTasks
			Task* currentTask = scheduledTasks.front();
			scheduledTasks.pop();

			// - Execute it
			currentTask->execute();

			// - Insert it into finishedTasks
			finishedTasks.push(currentTask);
		}
		else
		{
			myevent.wait(lock);
		}

		if (exitFlag)
		{
			break;
		}

	}
}

bool ModuleTaskManager::init()
{
	// TODO 1: Create threads (they have to execute threadMain())

	for (int i = 0; i < MAX_THREADS; i++)
	{
		threads[i] = std::thread(&ModuleTaskManager::threadMain,this);
	}

	return true;
}

bool ModuleTaskManager::update()
{
	// TODO 4: Dispatch all finished tasks to their owner module (use Module::onTaskFinished() callback)
	while (!finishedTasks.empty())
	{
		Task* finishedTask = finishedTasks.front();
		finishedTasks.pop();

		finishedTask->owner->onTaskFinished(finishedTask);
	}
	
	return true;
}

bool ModuleTaskManager::cleanUp()
{
	// TODO 5: Notify all threads to finish and join them
	myevent.notify_all();
	exitFlag = true;

	for (int i = 0; i < MAX_THREADS; i++)
	{
		threads[i].join();
	}

	return true;
}

void ModuleTaskManager::scheduleTask(Task *task, Module *owner)
{
	task->owner = owner;

	// TODO 2: Insert the task into scheduledTasks so it is executed by some thread
	{
		std::unique_lock<std::mutex> lock(mtx);
		scheduledTasks.push(task);
	}
	myevent.notify_one();
}

/*
		IDEA
--------------------------
1. Creem els threads i els assignem la funci�. Entenc que la forma es std::thread(&ModuleTaskManager::threadMain,this); per a referir-se a la instancia.
2. Al principi tots els threads estaran en wait ja que no hi haura cap tasca.
3. Al introduir una tasca el que hem de fer es treure UN thread de la espera. Per aixo cridem el myevent.
4. Per esborrar tots els threads el que primer es cridar-los a tots amb el myevent i despres fer el join() per obtenir els resultats.
*/