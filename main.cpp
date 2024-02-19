#include "Helper/Helper.h"
#include "RdEngine.h"
#include <Windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Helper::WriteToOutput("Hello,GameProj!\n");
	// エンジンを実行する
	gEngine = std::make_unique<RdEngine>();
	gEngine->Run();
	return 0;
}
