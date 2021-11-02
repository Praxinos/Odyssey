// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerImage.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the TLayerImage class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Components/HasBlendInfo.h"
#include "Layer/Components/HasBlock.h"
#include "Layer/Components/Rasterizable.h"
#include "Layer/Layer/AbstractLayerDrawable.h"

ULIS_NAMESPACE_BEGIN
// Forward-declarations
template< class BlockType, class RendererType, class TSuperStackExtra > class TLayerStack;
template< class BlockType, class RasterizerType, class RendererType, class BlockAllocatorType, class LayerStackType > class TLayerFolder;

/////////////////////////////////////////////////////
/// @class      TLayerImage
/// @brief      The TLayerImage class provides a class to store an image in a
///             layer stack for painting applications.
template<
      class BlockType
    , class RasterizerType
    , class RendererType
    , class BlockAllocatorType
    , class LayerStackType
>
class TLayerImage final
    : public TAbstractLayerDrawable< BlockType >
    , public TRasterizable< TLayerImage< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > >
    , public THasBlock< BlockType, BlockAllocatorType >
    , public IHasBlendInfo
    , public IHasPaintLock
{
    typedef TRoot< ILayer > tParent;
    typedef TLayerImage< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSelf;
    typedef TLayerFolder< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSiblingFolder;
    typedef TAbstractLayerDrawable< BlockType > tAbstractLayerDrawable;
    typedef TRasterizable< tSelf > tRasterizable;
    typedef THasBlock< BlockType, BlockAllocatorType > tHasBlock;

public:
    // DTor
    virtual ~TLayerImage() override;

    // CTors
    TLayerImage(
          const FString& iName = "Untitled Image"
        , bool iLocked = false
        , bool iVisible = true
        , const FColor& iPrettyColor = FColor::Transparent
        , uint16 iWidth = 0
        , uint16 iHeight = 0
        , eFormat iFormat = Format_RGBA8
        , const FColorSpace* iColorSpace = nullptr
        , eBlendMode iBlendMode = eBlendMode::Blend_Normal
        , eAlphaMode iAlphaMode = eAlphaMode::Alpha_Normal
        , ufloat iOpacity = 1.f
        , bool iAlphaLocked = false
        , const TRoot< ILayer >* iParent = nullptr

        , const FOnNameChanged& iOnNameChanged = FOnNameChanged()
        , const FOnBoolChanged& iOnLockChanged = FOnBoolChanged()
        , const FOnBoolChanged& iOnVisibleChanged = FOnBoolChanged()
        , const FOnColorChanged& iOnColorChanged = FOnColorChanged()
        , const FOnUserDataAdded& iOnUserDataAdded = FOnUserDataAdded()
        , const FOnUserDataChanged& iOnUserDataChanged = FOnUserDataChanged()
        , const FOnUserDataRemoved& iOnUserDataRemoved = FOnUserDataRemoved()
        , const FOnParentChanged& iOnParentChanged = FOnParentChanged()
        , const FOnSelfChanged& iOnSelfChanged = FOnSelfChanged()

        , const TOnBlockChanged< BlockType >& iOnBlockChanged = TOnBlockChanged< BlockType >()
        , const FOnBlendInfoChanged& iOnBlendInfoChanged = FOnBlendInfoChanged()
        , const FOnBoolChanged& iOnPaintLockChanged = FOnBoolChanged()
    );

    TLayerImage(
          BlockType* iBlock
        , const FString& iName = "Untitled Image"
        , bool iLocked = false
        , bool iVisible = true
        , const FColor& iPrettyColor = FColor::Transparent
        , eBlendMode iBlendMode = eBlendMode::Blend_Normal
        , eAlphaMode iAlphaMode = eAlphaMode::Alpha_Normal
        , ufloat iOpacity = 1.f
        , bool iAlphaLocked = false
        , const TRoot< ILayer >* iParent = nullptr

        , const FOnNameChanged& iOnNameChanged = FOnNameChanged()
        , const FOnBoolChanged& iOnLockChanged = FOnBoolChanged()
        , const FOnBoolChanged& iOnVisibleChanged = FOnBoolChanged()
        , const FOnColorChanged& iOnColorChanged = FOnColorChanged()
        , const FOnUserDataAdded& iOnUserDataAdded = FOnUserDataAdded()
        , const FOnUserDataChanged& iOnUserDataChanged = FOnUserDataChanged()
        , const FOnUserDataRemoved& iOnUserDataRemoved = FOnUserDataRemoved()
        , const FOnParentChanged& iOnParentChanged = FOnParentChanged()
        , const FOnSelfChanged& iOnSelfChanged = FOnSelfChanged()

        , const TOnBlockChanged< BlockType >& iOnBlockChanged = TOnBlockChanged< BlockType >()
        , const FOnBlendInfoChanged& iOnBlendInfoChanged = FOnBlendInfoChanged()
        , const FOnBoolChanged& iOnPaintLockChanged = FOnBoolChanged()
    );

    // Disable copy
    TLayerImage( const TLayerImage& ) = delete;
    TLayerImage& operator=( const TLayerImage& ) = delete;

public:
    // ITypeIdentifiable Interface
    ULIS_OVERRIDE_TYPEID_INTERFACE( "Image" );

    // TDrawable Interface
    FEvent RenderImage(
          FContext& iCtx
        , BlockType& ioBlock
        , const FRectI& iRect = FRectI::Auto
        , const FVec2I& iPos = FVec2I( 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy()
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
    ) override;

    // TRasterizable Interface
    tSelf* Rasterize( FContext& iCtx, FEvent* oEvent = nullptr ) override;

private:
    // TNode< ILayer > Interface
    void InitFromParent( const TRoot< ILayer >* iParent ) override;
};

ULIS_NAMESPACE_END

#include "Layer/Layer/LayerImage.tpp"

