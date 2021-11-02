// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerText.tpp
* @author       Clement Berthaud
* @brief        This file provides the definition for the TLayerText class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Layer/Layer/LayerText.h"

// Template Macro Utility
#define TEMPLATE template< class BlockType, class RasterizerType, class RendererType, class BlockAllocatorType, class LayerStackType >
#define CLASS TLayerText< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType >

ULIS_NAMESPACE_BEGIN
TEMPLATE
CLASS::~TLayerText() {
    ULIS_DEBUG_PRINTF( "TLayerText Destroyed" )
}

TEMPLATE
CLASS::TLayerText(
      const FString& iName
    , bool iLocked
    , bool iVisible
    , const FColor& iPrettyColor
    , uint16 iWidth
    , uint16 iHeight
    , eFormat iFormat
    , const FColorSpace* iColorSpace
    , eBlendMode iBlendMode
    , eAlphaMode iAlphaMode
    , ufloat iOpacity
    , const FWString& iText
    , const FFont& iFont
    , uint32 iFontSize
    , const ISample& iTextColor
    , bool iAntiAliased
    , const FTransformInfo& iTransform
    , const TRoot< ILayer >* iParent

    , const FOnNameChanged& iOnNameChanged
    , const FOnBoolChanged& iOnLockChanged
    , const FOnBoolChanged& iOnVisibleChanged
    , const FOnColorChanged& iOnColorChanged
    , const FOnUserDataAdded& iOnUserDataAdded
    , const FOnUserDataChanged& iOnUserDataChanged
    , const FOnUserDataRemoved& iOnUserDataRemoved
    , const FOnParentChanged& iOnParentChanged
    , const FOnSelfChanged& iOnSelfChanged

    , const TOnBlockChanged< BlockType >& iOnBlockChanged
    , const FOnBlendInfoChanged& iOnBlendInfoChanged
    , const FOnTextInfoChanged& iOnTextInfoChanged
    , const FOnTransformInfoChanged& iOnTransformInfoChanged
)
    : TNode< ILayer >(
          iParent
        , iOnParentChanged
        , iOnSelfChanged
    )
    , ILayer(
          iName
        , iLocked
        , iVisible
        , iPrettyColor
        , iParent

        , iOnNameChanged
        , iOnLockChanged
        , iOnVisibleChanged
        , iOnColorChanged
        , iOnUserDataAdded
        , iOnUserDataChanged
        , iOnUserDataRemoved
        , iOnParentChanged
        , iOnSelfChanged
    )
    , tAbstractLayerDrawable(
          iName
        , iLocked
        , iVisible
        , iPrettyColor
        , iParent

        , iOnNameChanged
        , iOnLockChanged
        , iOnVisibleChanged
        , iOnColorChanged
        , iOnUserDataAdded
        , iOnUserDataChanged
        , iOnUserDataRemoved
        , iOnParentChanged
        , iOnSelfChanged
    )
    , tRasterizable()
    , tHasBlock(
          iWidth
        , iHeight
        , iFormat
        , iColorSpace
        , iOnBlockChanged
    )
    , IHasBlendInfo(
          iBlendMode
        , iAlphaMode
        , iOpacity
        , iOnBlendInfoChanged
    )
    , IHasText(
          iText
        , iFont
        , iFontSize
        , iTextColor
        , iAntiAliased
        , iOnTextInfoChanged
    )
    , IHasTransform(
          FVec2F( 0 )
        , FVec2F( 0 )
        , 0
        , FVec2F( 1 )
        , iOnTransformInfoChanged
    )
{
    ULIS_DEBUG_PRINTF( "TLayerText Created" )
}

TEMPLATE
CLASS::TLayerText(
      BlockType* iBlock
    , const FString& iName
    , bool iLocked
    , bool iVisible
    , const FColor& iPrettyColor
    , eBlendMode iBlendMode
    , eAlphaMode iAlphaMode
    , ufloat iOpacity
    , const FWString& iText
    , const FFont& iFont
    , uint32 iFontSize
    , const ISample& iTextColor
    , bool iAntiAliased
    , const FTransformInfo& iTransform
    , const TRoot< ILayer >* iParent

    , const FOnNameChanged& iOnNameChanged
    , const FOnBoolChanged& iOnLockChanged
    , const FOnBoolChanged& iOnVisibleChanged
    , const FOnColorChanged& iOnColorChanged
    , const FOnUserDataAdded& iOnUserDataAdded
    , const FOnUserDataChanged& iOnUserDataChanged
    , const FOnUserDataRemoved& iOnUserDataRemoved
    , const FOnParentChanged& iOnParentChanged
    , const FOnSelfChanged& iOnSelfChanged

    , const TOnBlockChanged< BlockType >& iOnBlockChanged
    , const FOnBlendInfoChanged& iOnBlendInfoChanged
    , const FOnTextInfoChanged& iOnTextInfoChanged
    , const FOnTransformInfoChanged& iOnTransformInfoChanged
)
    : TNode< ILayer >(
          iParent
        , iOnParentChanged
        , iOnSelfChanged
    )
    , ILayer(
          iName
        , iLocked
        , iVisible
        , iPrettyColor
        , iParent

        , iOnNameChanged
        , iOnLockChanged
        , iOnVisibleChanged
        , iOnColorChanged
        , iOnUserDataAdded
        , iOnUserDataChanged
        , iOnUserDataRemoved
        , iOnParentChanged
        , iOnSelfChanged
    )
    , tAbstractLayerDrawable(
          iName
        , iLocked
        , iVisible
        , iPrettyColor
        , iParent

        , iOnNameChanged
        , iOnLockChanged
        , iOnVisibleChanged
        , iOnColorChanged
        , iOnUserDataAdded
        , iOnUserDataChanged
        , iOnUserDataRemoved
        , iOnParentChanged
        , iOnSelfChanged
    )
    , tRasterizable()
    , tHasBlock(
          iBlock
        , iOnBlockChanged
    )
    , IHasBlendInfo(
          iBlendMode
        , iAlphaMode
        , iOpacity
        , iOnBlendInfoChanged
    )
    , IHasText(
          iText
        , iFont
        , iFontSize
        , iTextColor
        , iAntiAliased
        , iOnTextInfoChanged
    )
    , IHasTransform(
          FVec2F( 0 )
        , FVec2F( 0 )
        , 0
        , FVec2F( 1 )
        , iOnTransformInfoChanged
    )
{
    ULIS_DEBUG_PRINTF( "TLayerText Created" )
}



// TDrawable Interface
TEMPLATE
FEvent
CLASS::RenderImageCache( FContext& iCtx ) // override
{
    if( tSelf::IsImageCacheValid() )
        return  FEvent::NoOP();

    FEvent eventClear;
    iCtx.Clear( *tSelf::Block(), FRectI::Auto, FSchedulePolicy::CacheEfficient, 0, nullptr, &eventClear );
    FEvent ev;
    if( IsAntiAliased() ) {
        iCtx.RasterTextAA( *tSelf::Block(), Text(), Font(), FontSize(), Matrix(), TextColor(), FSchedulePolicy::MonoChunk, 1, &eventClear, &ev );
    } else {
        iCtx.RasterText( *tSelf::Block(), Text(), Font(), FontSize(), Matrix(), TextColor(), FSchedulePolicy::MonoChunk, 1, &eventClear, &ev );
    }

    tSelf::ValidateImageCache();
    return  ev;
}

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
    ulError err = iCtx.Blend(
          *tSelf::Block()
        , ioBlock
        , iRect
        , iPos
        , BlendMode()
        , AlphaMode()
        , Opacity()
        , iPolicy
        , iNumWait
        , iWaitList
        , &ev
    );
    ULIS_ASSERT( !err, "Error during layer image blend" );
    return  ev;
}

// TRasterizable Interface
TEMPLATE
typename CLASS::tSelf*
CLASS::Rasterize( FContext& iCtx, FEvent* oEvent ) // override
{
    const BlockType* ref = tSelf::Block();
    if( !ref )
        return  nullptr;

    // Actual Deep Copy with Event.
    tSelf* rasterized = new tSelf(
          tSelf::Name()
        , tSelf::IsLocked()
        , tSelf::IsVisible()
        , tSelf::PrettyColor()
        , ref->Width()
        , ref->Height()
        , ref->Format()
        , ref->ColorSpace()
        , BlendMode()
        , AlphaMode()
        , Opacity()
        , tSelf::IsPaintLocked()
        , nullptr

        , FOnNameChanged::GetDelegate()
        , FOnBoolChanged::GetDelegate()
        , FOnBoolChanged::GetDelegate()
        , FOnColorChanged::GetDelegate()
        , FOnUserDataAdded::GetDelegate()
        , FOnUserDataChanged::GetDelegate()
        , FOnUserDataRemoved::GetDelegate()
        , FOnParentChanged::GetDelegate()
        , FOnSelfChanged::GetDelegate()

        , TOnBlockChanged< BlockType >::GetDelegate()
        , FOnBlendInfoChanged::GetDelegate()
        , FOnBoolChanged::GetDelegate()
    );

    iCtx.Copy( *tSelf::Block(), *(rasterized->Block()), FRectI::Auto, FVec2I( 0 ), FSchedulePolicy::CacheEfficient, 0, nullptr, oEvent );
    return  rasterized;
}

// TNode< ILayer > Interface
TEMPLATE
void
CLASS::InitFromParent( const TRoot< ILayer >* iParent ) // override
{
    ULIS_ASSERT( iParent == Parent(), "Inconsistent Parent" );
    const tParent* topLevel = iParent->TopLevelParent();
    if( !topLevel )
        return;

    if( !tSelf::Block() ) {
        //const ILayer* layer = dynamic_cast< const ILayer* >( topLevel );
        const ILayer* layer = (const ILayer*)( topLevel );
        ULIS_ASSERT( layer, "Parent cannot be cast to ILayer, there's something wrong with the class hierarchy !" );
        switch( layer->TypeID() ) {
            case LayerStackType::StaticTypeID(): {
                const LayerStackType* stack = dynamic_cast< const LayerStackType* >( layer );
                ULIS_ASSERT( stack, "Parent cannot be cast to stack, this is inconsistent with the StaticTypeID !" );
                Realloc( stack->Width(), stack->Height(), stack->Format(), stack->ColorSpace() );
                break;
            }
            case tSiblingFolder::StaticTypeID(): {
                const tSiblingFolder* folder = dynamic_cast< const tSiblingFolder* >( layer );
                ULIS_ASSERT( folder, "Parent cannot be cast to folder, this is inconsistent with the StaticTypeID !" );
                const BlockType* ref = folder->Block();
                if( !ref )
                    break;
                Realloc( ref->Width(), ref->Height(), ref->Format(), ref->ColorSpace() );
                break;
            }
        }
    }
}

ULIS_NAMESPACE_END

// Template Macro Utility
#undef TEMPLATE
#undef CLASS

