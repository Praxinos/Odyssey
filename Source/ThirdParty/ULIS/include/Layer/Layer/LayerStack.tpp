// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerStack.tpp
* @author       Clement Berthaud
* @brief        This file provides the definition for the TLayerStack class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Layer/Layer/LayerStack.h"

// Template Macro Utility
#define TEMPLATE template< class BlockType, class RendererType, class TSuperStackExtra >
#define CLASS TLayerStack< BlockType, RendererType, TSuperStackExtra >

ULIS_NAMESPACE_BEGIN
// DTor
TEMPLATE
CLASS::~TLayerStack()
{
    ULIS_DEBUG_PRINTF( "TLayerStack Destroyed" )
}

// CTor
TEMPLATE
template< typename ... Args >
CLASS::TLayerStack(
      uint16 iWidth
    , uint16 iHeight
    , eFormat iFormat
    , const FColorSpace* iColorSpace
    , const FOnSelfChanged& iOnSelfChanged
    , const FOnNodeAdded& iOnLayerAdded
    , const FOnNodeRemoved& iOnLayerRemoved
    , const FOnUserDataAdded& iOnUserDataAdded
    , const FOnUserDataChanged& iOnUserDataChanged
    , const FOnUserDataRemoved& iOnUserDataRemoved
    , Args ... args
)
    : TNode< ILayer >(
          nullptr
        , FOnParentChanged()
        , iOnSelfChanged
    )
    , ILayer(
          "LayerStack"
        , false
        , false
        , FColor::Transparent
        , nullptr
        , FOnNameChanged()
        , FOnBoolChanged()
        , FOnBoolChanged()
        , FOnColorChanged()
        , iOnUserDataAdded
        , iOnUserDataChanged
        , iOnUserDataRemoved
        , FOnParentChanged()
        , iOnSelfChanged
    )
    , TAbstractLayerDrawable< BlockType >(
          "LayerStack"
        , false
        , false
        , FColor::Transparent
        , nullptr
        , FOnNameChanged()
        , FOnBoolChanged()
        , FOnBoolChanged()
        , FOnColorChanged()
        , iOnUserDataAdded
        , iOnUserDataChanged
        , iOnUserDataRemoved
        , FOnParentChanged()
    )
    , TRoot< ILayer > (
          nullptr
        , FOnParentChanged()
        , iOnSelfChanged
        , iOnLayerAdded
        , iOnLayerRemoved
    )
    , TSearchable< TRoot< ILayer > >()
    , IHasSize2D( FVec2UI16( iWidth, iHeight ) )
    , IHasFormat( iFormat )
    , IHasColorSpace( iColorSpace )
    , TSuperStackExtra( args ... )
{
    ULIS_DEBUG_PRINTF( "TLayerStack Created" )
}

// TLayerStack Interface
TEMPLATE
template< typename ... Args >
void
CLASS::Reset(
      uint16 iWidth
    , uint16 iHeight
    , eFormat iFormat
    , const FColorSpace* iColorSpace
    , const FOnSelfChanged& iOnSelfChanged
    , const FOnNodeAdded& iOnLayerAdded
    , const FOnNodeRemoved& iOnLayerRemoved
    , const FOnUserDataAdded& iOnUserDataAdded
    , const FOnUserDataChanged& iOnUserDataChanged
    , const FOnUserDataRemoved& iOnUserDataRemoved
    , Args ... args
)
{
    ReinterpretFormat( iFormat );
    AssignColorSpace( iColorSpace );
    ReinterpretSize( FVec2UI16( iWidth, iHeight ) );
    this->TRoot< ILayer >::Reset(
          nullptr
        , FOnParentChanged()
        , iOnLayerAdded
        , iOnLayerRemoved
    );
    this->FOnSelfChanged::SetDelegate( iOnSelfChanged );
    this->FOnUserDataAdded::SetDelegate( iOnUserDataAdded );
    this->FOnUserDataChanged::SetDelegate( iOnUserDataChanged );
    this->FOnUserDataRemoved::SetDelegate( iOnUserDataRemoved );
}

// TDrawable Interface
TEMPLATE
FEvent
CLASS::RenderImage(
      FContext& iCtx
    , BlockType& ioBlock
    , const FRectI& iRect
    , const FVec2I& iPos
    , const FSchedulePolicy& iPolicy
    , uint32 iNumWait
    , const FEvent* iWaitList
) // override
{
    FEvent ev;
    iCtx.Clear( ioBlock, FRectI::Auto, FSchedulePolicy::CacheEfficient, iNumWait, iWaitList, &ev );
    const int max = static_cast< int >( Children().Size() ) - 1;
    bool bFirst = true;
    for( int i = max; i >= 0; --i ) {
        typedef TAbstractLayerDrawable< BlockType > tDrawable;
        tDrawable* drawable = dynamic_cast< tDrawable* >( &( Children()[i]->Self() ) );
        if( !drawable )
            continue;

        ev = drawable->RenderImage(
              iCtx
            , ioBlock
            , iRect
            , iPos
            , iPolicy
            , 1
            , &ev
        );
    }
    return  ev;
}

ULIS_NAMESPACE_END

// Template Macro Utility
#undef TEMPLATE
#undef CLASS

