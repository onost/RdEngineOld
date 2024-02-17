#pragma once
#include <cassert>
#include "Helper.h"
// 自作Assert
#ifdef NDEBUG
#define MyAssert(e) ((void)0)
#else
#define MyAssert(e) (void)(\
            (!!(e)) ||\
            (_wassert(_CRT_WIDE(#e), Helper::ConvertToWstr(Helper::GetFileName(__FILE__)).c_str(), (unsigned)(__LINE__)), 0)\
        )
#endif
