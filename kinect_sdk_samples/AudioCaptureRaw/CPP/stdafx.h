/***********************************************************************
*                                                                      *
*   stdafx.h -- Include file for standard system include files, or     *
*               project specific include files that are used           *
*               frequently, but are changed infrequently               *
*                                                                      *
*   Copyright (c) Microsoft Corp. All rights reserved.                 *
*                                                                      *
*  This code is licensed under the terms of the                        *
*  Microsoft Kinect for Windows SDK (Beta) from Microsoft Research     *
*  License Agreement: http://research.microsoft.com/KinectSDK-ToU      *
*                                                                      *
************************************************************************/

#pragma once

#include "targetver.h"
#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 1
#include <new>
#include <windows.h>
#include <strsafe.h>
#pragma warning(push)
#pragma warning(disable : 4201)
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#pragma warning(pop)

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}
