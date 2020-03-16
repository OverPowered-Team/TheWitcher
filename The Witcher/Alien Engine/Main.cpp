#include <stdlib.h>
#include "Application.h"
#include "Globals.h"

#include "SDL/include/SDL.h"
#pragma comment(lib, "SDL/libx86/SDL2.lib")
#pragma comment(lib, "SDL/libx86/SDL2main.lib")

#include "Optick/include/optick.h"

#ifdef _DEBUG
#pragma comment(lib, "Optick/libx86/debugx86/OptickCore.lib")
#else
#pragma comment(lib, "Optick/libx86/releasex86/OptickCore.lib")
#endif

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};
Application* App = NULL;
int main(int argc, char ** argv)
{

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;
	FILE* fp = fopen("console_log.txt", "w");

	while (state != MAIN_EXIT)
	{
		OPTICK_FRAME("MainThread");

		switch (state)
		{
		case MAIN_CREATION:
			App = new Application();
			LOG_ENGINE("-------------- Application Creation --------------");
			state = MAIN_START;
			App->UpdateLogFile(fp);
			break;

		case MAIN_START:

			LOG_ENGINE("-------------- Application Init --------------");
			if (App->Init() == false)
			{
				LOG_ENGINE("Application Init exits with ERROR");
				state = MAIN_EXIT;
				App->UpdateLogFile(fp);
			}
			else
			{
				state = MAIN_UPDATE;
				LOG_ENGINE("-------------- Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				LOG_ENGINE("Application Update exits with ERROR");
				state = MAIN_EXIT;
				App->UpdateLogFile(fp);
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			LOG_ENGINE("-------------- Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				LOG_ENGINE("Application CleanUp exits with ERROR");
				App->UpdateLogFile(fp);
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}
	App->UpdateLogFile(fp);

	delete App;

	return main_return;
}