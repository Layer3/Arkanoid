#include "Arkanoid.h"

int main(int argc, char* argv[])
{
	Arkanoid::CArkanoid arkanoid;
	arkanoid.Initialize();
	arkanoid.Run();
	arkanoid.Shutdown();

	return 0;
}
