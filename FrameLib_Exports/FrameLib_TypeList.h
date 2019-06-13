
#ifndef FRAMELIB_TYPELIST_H
#define FRAMELIB_TYPELIST_H

#include "FrameLib_Objects.h"
#include <utility>

template<typename...Ts>
struct FrameLib_Typelist
{
    template<template<typename> class F, typename...Args>
    static void execute(Args...args)
    {
        (void)std::initializer_list<int>{(F<Ts>{}(std::forward<Args>(args)...),0)...};
    }
};

template<typename T>
struct FrameLib_ObjectName
{
    const char* name()
    {
        constexpr bool falseTest = T::getType() == kScheduler && 0;
        static_assert(falseTest, "Object has not been assigned a name");
        return "none";
    }
};

#endif /* FrameLib_TypeList_h */
