// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerTiledImage.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FLayerTiledImage class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Common/RegularBlockAllocator.h"
#include "Layer/Components/HasBlendInfo.h"
#include "Layer/Components/HasBlock.h"
#include "Layer/Layer/Layer.h"
#include "Sparse/TiledBlock.h"

ULIS_NAMESPACE_BEGIN
// Forward-declarations
//template< class BlockType, class RendererType, class TSuperStackExtra > class TLayerStack;
//template< class BlockType, class RasterizerType, class RendererType, class BlockAllocatorType, class LayerStackType > class TLayerFolder;

/////////////////////////////////////////////////////
/// @class      FLayerTiledImage
/// @brief      The FLayerTiledImage class provides a class to store an image in a
///             layer stack for painting applications.
class ULIS_API FLayerTiledImage final
    : public ILayer
    , public IHasBlendInfo
    , public IHasPaintLock
{
    typedef TRoot< ILayer > tParent;
    //typedef FLayerTiledImage< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSelf;
    //typedef TLayerFolder< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSiblingFolder;
    //typedef TAbstractLayerDrawable< BlockType > tAbstractLayerDrawable;
    //typedef TRasterizable< tSelf > tRasterizable;
    //typedef THasBlock< BlockType, BlockAllocatorType > tHasBlock;

public:
    // DTor
    ~FLayerTiledImage() override;

    // CTors
    FLayerTiledImage(
          const FString& iName = "Untitled Image"
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

        , const TOnBlockChanged< FBlock >& iOnBlockChanged = TOnBlockChanged< FBlock >()
        , const FOnBlendInfoChanged& iOnBlendInfoChanged = FOnBlendInfoChanged()
        , const FOnBoolChanged& iOnPaintLockChanged = FOnBoolChanged()
    );

    FLayerTiledImage(
          FBlock* iBlock
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

        , const TOnBlockChanged< FBlock >& iOnBlockChanged = TOnBlockChanged< FBlock >()
        , const FOnBlendInfoChanged& iOnBlendInfoChanged = FOnBlendInfoChanged()
        , const FOnBoolChanged& iOnPaintLockChanged = FOnBoolChanged()
    );

    // Disable copy
    FLayerTiledImage( const FLayerTiledImage& ) = delete;
    FLayerTiledImage& operator=( const FLayerTiledImage& ) = delete;

public:
    // ITypeIdentifiable Interface
    ULIS_OVERRIDE_TYPEID_INTERFACE( "Image" );

public:
    FTiledBlock& Block();
    const FTiledBlock& Block() const;

private:
    // TNode< ILayer > Interface
    void InitFromParent( const TRoot< ILayer >* iParent ) override;

private:
    FTiledBlock* mBlock;
};

ULIS_NAMESPACE_END

