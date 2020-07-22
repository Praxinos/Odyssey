// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Blend.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the Blend entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Blend/Modes.h"

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// Blend
ULIS3_API void Blend( FThreadPool*              iThreadPool
                    , bool                      iBlocking
                    , uint32                    iPerfIntent
                    , const FHostDeviceInfo&    iHostDeviceInfo
                    , bool                      iCallCB
                    , const FBlock*             iSource
                    , FBlock*                   iBackdrop
                    , const FRect&              iSourceRect
                    , const FVec2F&             iPosition
                    , bool                      iSubpixelFlag
                    , eBlendingMode             iBlendingMode
                    , eAlphaMode                iAlphaMode
                    , float                     iOpacityValue );

/////////////////////////////////////////////////////
// AlphaBlend
ULIS3_API void AlphaBlend( FThreadPool*             iThreadPool
                         , bool                     iBlocking
                         , uint32                   iPerfIntent
                         , const FHostDeviceInfo&   iHostDeviceInfo
                         , bool                     iCallCB
                         , const FBlock*            iSource
                         , FBlock*                  iBackdrop
                         , const FRect&             iSourceRect
                         , const FVec2F&            iPosition
                         , bool                     iSubpixelFlag
                         , float                    iOpacityValue );

/////////////////////////////////////////////////////
// BlendTiled
ULIS3_API void BlendTiled( FThreadPool*             iThreadPool
                         , bool                     iBlocking
                         , uint32                   iPerfIntent
                         , const FHostDeviceInfo&   iHostDeviceInfo
                         , bool                     iCallCB
                         , const FBlock*            iSource
                         , FBlock*                  iBackdrop
                         , const FRect&             iSourceRect
                         , const FRect&             iDestRect
                         , const FVec2I&            iShift
                         , eBlendingMode            iBlendingMode
                         , eAlphaMode               iAlphaMode
                         , float                    iOpacityValue );

/////////////////////////////////////////////////////
// BlendColor
ULIS3_API void BlendColor( FThreadPool*             iThreadPool
                         , uint32                   iPerfIntent
                         , const FHostDeviceInfo&   iHostDeviceInfo
                         , bool                     iCallCB
                         , const FPixelValue&       iColor
                         , FBlock*                  iBackdrop
                         , const FRect&             iDestRect
                         , eBlendingMode            iBlendingMode
                         , eAlphaMode               iAlphaMode
                         , float                    iOpacityValue );

ULIS3_NAMESPACE_END

