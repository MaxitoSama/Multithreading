#include "ModuleResources.h"
#include "ModuleTextures.h"
#include "ModuleTaskManager.h"
#include "Application.h"
#include "Log.h"

#if defined(USE_TASK_MANAGER)
class TaskLoadTexture : public Task
{
public:

	const char *filename = nullptr;
	Texture *texture = nullptr;

	void execute() override
	{
		texture = App->modTextures->loadTexture(filename);
	}
};
#endif


bool ModuleResources::init()
{
	// load a texture
	loading = App->modTextures->loadTexture("loading_screen.png");
	yellow = App->modTextures->loadTexture("yellow_color.png");

#if !defined(USE_TASK_MANAGER)
	mario = App->modTextures->loadTexture("mario.jpg");
	space = App->modTextures->loadTexture("space_background.jpg");
	asteroid1 = App->modTextures->loadTexture("asteroid1.png");
	asteroid2 = App->modTextures->loadTexture("asteroid2.png");
	spacecraft1 = App->modTextures->loadTexture("spacecraft1.png");
	spacecraft2 = App->modTextures->loadTexture("spacecraft2.png");
	spacecraft3 = App->modTextures->loadTexture("spacecraft3.png");
	loadingFinished = true;
	completionRatio = 1.0f;
#else
	const char *texturesToLoad[] = {
		"mario.jpg",
		"space_background.jpg",
		"asteroid1.png",
		"asteroid2.png",
		"spacecraft1.png",
		"spacecraft2.png",
		"spacecraft3.png",
		"test_0.jpg",
		"test_1.jpg",
		"test_2.jpg",
		"test_3.jpg",
		"test_4.jpg",
		"test_5.jpg",
		"test_6.jpg",
		"test_7.jpg",
		"test_8.jpg",
		"test_9.jpg",
		"test_10.png",
		"test_11.png",
		"test_12.png",
		"test_13.png",
		"test_14.png",
		"test_15.png",
		"test_16.png",
		"test_17.png",
		"test_18.png",
		"test_19.png",
		"test_20.png",
		"test_0 - copia.jpg",
		"test_1 - copia.jpg",
		"test_2 - copia.jpg",
		"test_3 - copia.jpg",
		"test_4 - copia.jpg",
		"test_5 - copia.jpg",
		"test_6 - copia.jpg",
		"test_7 - copia.jpg",
		"test_8 - copia.jpg",
		"test_9 - copia.jpg",
		"test_10 - copia.jpg",
		"test_11 - copia.png",
		"test_12 - copia.png",
		"test_13 - copia.png",
		"test_14 - copia.png",
		"test_15 - copia.png",
		"test_16 - copia.png",
		"test_17 - copia.png",
		"test_18 - copia.png",
		"test_19 - copia.png",
		"test_20 - copia.png"
	};
	for (auto filename : texturesToLoad)
	{
		loadTextureAsync(filename);
	}
#endif

	return true;
}

#if defined(USE_TASK_MANAGER)

void ModuleResources::loadTextureAsync(const char * filename)
{
	TaskLoadTexture *task = new TaskLoadTexture;
	task->owner = this;
	task->filename = filename;
	App->modTaskManager->scheduleTask(task, this);
}

void ModuleResources::onTaskFinished(Task * task)
{
	TaskLoadTexture *taskLoadTexture = dynamic_cast<TaskLoadTexture*>(task);
	if (taskLoadTexture != nullptr)
	{
		if      (taskLoadTexture->filename == "mario.jpg") mario = taskLoadTexture->texture;
		else if (taskLoadTexture->filename == "space_background.jpg") space = taskLoadTexture->texture;
		else if (taskLoadTexture->filename == "asteroid1.png") asteroid1 = taskLoadTexture->texture;
		else if (taskLoadTexture->filename == "asteroid2.png") asteroid2 = taskLoadTexture->texture;
		else if (taskLoadTexture->filename == "spacecraft1.png") spacecraft1 = taskLoadTexture->texture;
		else if (taskLoadTexture->filename == "spacecraft2.png") spacecraft2 = taskLoadTexture->texture;
		else if (taskLoadTexture->filename == "spacecraft3.png") spacecraft3 = taskLoadTexture->texture;

		int numLoaded = 0;
		numLoaded += (int)(mario != nullptr);
		numLoaded += (int)(space != nullptr);
		numLoaded += (int)(asteroid1 != nullptr);
		numLoaded += (int)(asteroid2 != nullptr);
		numLoaded += (int)(spacecraft1 != nullptr);
		numLoaded += (int)(spacecraft2 != nullptr);
		numLoaded += (int)(spacecraft3 != nullptr);

		loadingFinished = numLoaded >= 7;
		completionRatio = (float)numLoaded / 7.0f;
	}

	delete task;
}

#endif
