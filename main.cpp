#include "Helper/Helper.h"
#include "RdEngine.h"
#ifdef _DEBUG
int main()
{
#else
#include <Windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif
	Helper::WriteToOutput("Hello,GameProj!\n");
	// エンジンを実行する
	gEngine = std::make_unique<RdEngine>();
	gEngine->Run();
	return 0;
}
