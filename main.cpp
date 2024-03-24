#include "RdEngine.h"
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// エンジンを実行
	gEngine = std::make_unique<RdEngine>();
	gEngine->Run();
	return 0;
}
