// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerImage.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FLayerImage class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Common/RegularBlockAllocator.h"
#include "Layer/Components/HasBlendInfo.h"
#include "Layer/Components/HasBlock.h"
#include "Layer/Layer/Layer.h"

ULIS_NAMESPACE_BEGIN
// Forward-declarations
//template< class BlockType, class RendererType, class TSuperStackExtra > class TLayerStack;
//template< class BlockType, class RasterizerType, class RendererType, class BlockAllocatorType, class LayerStackType > class TLayerFolder;

/////////////////////////////////////////////////////
/// @class      FLayerImage
/// @brief      The FLayerImage class provides a class to store an image in a
///             layer stack for painting applications.
class ULIS_API FLayerImage final
    : public ILayer
    , public THasBlock< FBlock, FRegularBlockAllocator >
    , public IHasBlendInfo
    , public IHasPaintLock
{
    typedef TRoot< ILayer > tParent;
    //typedef FLayerImage< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSelf;
    //typedef TLayerFolder< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSiblingFolder;
    //typedef TAbstractLayerDrawable< BlockType > tAbstractLayerDrawable;
    //typedef TRasterizable< tSelf > tRasterizable;
    //typedef THasBlock< BlockType, BlockAllocatorType > tHasBlock;

public:
    // DTor
    ~FLayerImage() override;

    // CTors
    FLayerImage(
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

        , const TOnBlockChanged< FBlock >& iOnBlockChanged = TOnBlockChanged< FBlock >()
        , const FOnBlendInfoChanged& iOnBlendInfoChanged = FOnBlendInfoChanged()
        , const FOnBoolChanged& iOnPaintLockChanged = FOnBoolChanged()
    );

    FLayerImage(
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
    FLayerImage( const FLayerImage& ) = delete;
    FLayerImage& operator=( const FLayerImage& ) = delete;

public:
    // ITypeIdentifiable Interface
    ULIS_OVERRIDE_TYPEID_INTERFACE( "Image" );

    /*
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
    */

private:
    // TNode< ILayer > Interface
    void InitFromParent( const TRoot< ILayer >* iParent ) override;
};

ULIS_NAMESPACE_END

