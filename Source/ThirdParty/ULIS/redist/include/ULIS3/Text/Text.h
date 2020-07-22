// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Text.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the Text entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Maths/Geometry.h"
#include "Maths/Transform2D.h"

ULIS3_NAMESPACE_BEGIN
ULIS3_API void RenderText( FThreadPool*             iThreadPool
                         , bool                     iBlocking
                         , uint32                   iPerfIntent
                         , const FHostDeviceInfo&   iHostDeviceInfo
                         , bool                     iCallCB
                         , FBlock*                  iDestination
                         , const std::wstring       iText
                         , const FFont&             iFont
                         , int                      iSize
                         , const IPixel&            iColor
                         , const FTransform2D&      iTransform
                         , bool                     iAntialiasing );

ULIS3_API FRect TextMetrics( std::wstring           iText
                           , const FFont&           iFont
                           , int                    iSize
                           , const FTransform2D&    iTransform );
ULIS3_NAMESPACE_END

