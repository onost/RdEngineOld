#pragma once
#include "Helper.h"
#include <cassert>

// 自作アサーション
// ファイルパスではなくファイル名を表示
#ifdef NDEBUG
#define MY_ASSERT(expression) ((void)0)
#else
#define MY_ASSERT(expression) (void)(\
            (!!(expression)) ||\
            (_wassert(_CRT_WIDE(#expression), Helper::ConvertToWstr(Helper::ExtractFileName(__FILE__)).c_str(), (unsigned)(__LINE__)), 0)\
        )
#endif
