#include "JoySlot.h"

int main()
{
	JoySlot app;
	app.Create(600, 400, "Joyslot");
//	app.CreateFullScreen("Joyslot");
	app.Loop();

	return 0;
}