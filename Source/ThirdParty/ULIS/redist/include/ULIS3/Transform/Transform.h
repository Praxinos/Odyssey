// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Transform.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the transform entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Transform/Methods.h"
#include "Maths/Bezier.h"
#include <vector>

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// TransformAffine
ULIS3_API void TransformAffine( FThreadPool*              iThreadPool
                              , bool                      iBlocking
                              , uint32                    iPerfIntent
                              , const FHostDeviceInfo&    iHostDeviceInfo
                              , bool                      iCallCB
                              , const FBlock*             iSource
                              , FBlock*                   iDestination
                              , const FRect&              iSourceRect
                              , const FTransform2D&       iTransform
                              , eResamplingMethod         iMethod );

/////////////////////////////////////////////////////
// TransformAffineTiled
ULIS3_API void TransformAffineTiled( FThreadPool*              iThreadPool
                                   , bool                      iBlocking
                                   , uint32                    iPerfIntent
                                   , const FHostDeviceInfo&    iHostDeviceInfo
                                   , bool                      iCallCB
                                   , const FBlock*             iSource
                                   , FBlock*                   iDestination
                                   , const FRect&              iSourceRect
                                   , const FRect&              iDestRect
                                   , const FTransform2D&       iTransform
                                   , eResamplingMethod         iMethod );

/////////////////////////////////////////////////////
// TransformPerspective
ULIS3_API void TransformPerspective( FThreadPool*              iThreadPool
                                   , bool                      iBlocking
                                   , uint32                    iPerfIntent
                                   , const FHostDeviceInfo&    iHostDeviceInfo
                                   , bool                      iCallCB
                                   , const FBlock*             iSource
                                   , FBlock*                   iDestination
                                   , const FRect&              iSourceRect
                                   , const FTransform2D&       iTransform
                                   , eResamplingMethod         iMethod );

/////////////////////////////////////////////////////
// TransformBezier
ULIS3_API void TransformBezier( FThreadPool*                                    iThreadPool
                              , bool                                            iBlocking
                              , uint32                                          iPerfIntent
                              , const FHostDeviceInfo&                          iHostDeviceInfo
                              , bool                                            iCallCB
                              , const FBlock*                                   iSource
                              , FBlock*                                         iDestination
                              , const FRect&                                    iSourceRect
                              , const std::vector< FBezierCubicControlPoint >&  iControlPoints
                              , float                                           iThreshold
                              , int                                             iPlotSize
                              , eResamplingMethod                               iMethod );

/////////////////////////////////////////////////////
// Resize
ULIS3_API void Resize( FThreadPool*             iThreadPool
                     , bool                     iBlocking
                     , uint32                   iPerfIntent
                     , const FHostDeviceInfo&   iHostDeviceInfo
                     , bool                     iCallCB
                     , const FBlock*            iSource
                     , FBlock*                  iDestination
                     , const FRect&             iSourceRect
                     , const FVec2F&            iSize
                     , const FVec2F&            iPos
                     , eResamplingMethod        iMethod );

/////////////////////////////////////////////////////
// XResize
ULIS3_API FBlock* XResize( FThreadPool*             iThreadPool
                         , bool                     iBlocking
                         , uint32                   iPerfIntent
                         , const FHostDeviceInfo&   iHostDeviceInfo
                         , bool                     iCallCB
                         , const FBlock*            iSource
                         , const FRect&             iSourceRect
                         , const FVec2F&            iSize
                         , eResamplingMethod        iMethod );

/////////////////////////////////////////////////////
// XTransformAffine
ULIS3_API FBlock* XTransformAffine( FThreadPool*            iThreadPool
                                  , bool                    iBlocking
                                  , uint32                  iPerfIntent
                                  , const FHostDeviceInfo&  iHostDeviceInfo
                                  , bool                    iCallCB
                                  , const FBlock*           iSource
                                  , const FRect&            iSourceRect
                                  , const FTransform2D&     iTransform
                                  , eResamplingMethod       iMethod );

/////////////////////////////////////////////////////
// XTransformAffineTiled
ULIS3_API FBlock* XTransformAffineTiled( FThreadPool*              iThreadPool
                                       , bool                      iBlocking
                                       , uint32                    iPerfIntent
                                       , const FHostDeviceInfo&    iHostDeviceInfo
                                       , bool                      iCallCB
                                       , const FBlock*             iSource
                                       , const FRect&              iSourceRect
                                       , const FRect&              iDestRect
                                       , const FTransform2D&       iTransform
                                       , eResamplingMethod         iMethod );

/////////////////////////////////////////////////////
// XMakeTileableTransformedPattern
ULIS3_API FBlock* XMakeTileableTransformedPattern( FThreadPool*              iThreadPool
                                                 , bool                      iBlocking
                                                 , uint32                    iPerfIntent
                                                 , const FHostDeviceInfo&    iHostDeviceInfo
                                                 , bool                      iCallCB
                                                 , const FBlock*             iSource
                                                 , const FRect&              iSourceRect
                                                 , const FTransform2D&       iTransform
                                                 , eResamplingMethod         iMethod );

/////////////////////////////////////////////////////
// XTransformPerspective
ULIS3_API FBlock* XTransformPerspective( FThreadPool*                   iThreadPool
                                       , bool                           iBlocking
                                       , uint32                         iPerfIntent
                                       , const FHostDeviceInfo&         iHostDeviceInfo
                                       , bool                           iCallCB
                                       , const FBlock*                  iSource
                                       , const FRect&                   iSourceRect
                                       , const std::vector< FVec2F >&   iDestinationPoints
                                       , eResamplingMethod              iMethod );

/////////////////////////////////////////////////////
// TransformAffineMetrics
ULIS3_API FRect TransformAffineMetrics( const FRect&          iSourceRect
                                      , const FTransform2D&   iTransform
                                      , eResamplingMethod     iMethod );

/////////////////////////////////////////////////////
// TransformPerspectiveMetrics
ULIS3_API FRect TransformPerspectiveMetrics( const FRect&          iSourceRect
                                           , const FTransform2D&   iTransform
                                           , eResamplingMethod     iMethod );

/////////////////////////////////////////////////////
// TransformBezierMetrics
ULIS3_API FRect TransformBezierMetrics( const FRect&                                    iSourceRect
                                      , const std::vector< FBezierCubicControlPoint >&  iControlPoints
                                      , eResamplingMethod                               iMethod );

ULIS3_NAMESPACE_END

