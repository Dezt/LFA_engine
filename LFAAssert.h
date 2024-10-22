#pragma once
#ifndef LFAASSERT_H
#define LFAASSERT_H


namespace LFA
{
#ifdef _DEBUG
	#include <cassert>
	#define LFA_Assert assert
#else
	#define LFA_Assert(exp) static_cast<void>(0)
#endif
}

#endif

