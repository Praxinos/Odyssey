// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Drawable.tpp
* @author       Clement Berthaud
* @brief        This file provides the definition for the TDrawable class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Layer/Components/Drawable.h"

ULIS_NAMESPACE_BEGIN
template< class BlockType >
TDrawable< BlockType >::TDrawable()
    : mCacheValid( false )
{}

template< class BlockType >
bool
TDrawable< BlockType >::IsImageCacheValid() const {
    return  mCacheValid;
}

template< class BlockType >
FEvent
TDrawable< BlockType >::RenderImageCache( FContext& iCtx ) {
    if( IsImageCacheValid() )
        return  FEvent::NoOP();

    ValidateImageCache();
    return  FEvent::NoOP();
}

template< class BlockType >
FEvent
TDrawable< BlockType >::RenderImage(
      FContext& iCtx
    , BlockType& ioBlock
    , const FRectI& iRect
    , const FVec2I& iPos
    , const FSchedulePolicy& iPolicy
    , uint32 iNumWait
    , const FEvent* iWaitList
)
{
    RenderImageCache( iCtx );
    return  FEvent::NoOP();
}

template< class BlockType >
void
TDrawable< BlockType >::InvalidImageCache() {
    mCacheValid = false;
}

template< class BlockType >
void
TDrawable< BlockType >::ValidateImageCache() {
    mCacheValid = true;
}


ULIS_NAMESPACE_END

