#include "ModuleTaskManager.h"


void ModuleTaskManager::threadMain()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(mtx);
		// TODO 3:
		// - Wait for new tasks to arrive
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
	scheduledTasks.push(task);
}
