// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerText.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the TLayerText class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Components/HasBlendInfo.h"
#include "Layer/Components/HasBlock.h"
#include "Layer/Components/Rasterizable.h"
#include "Layer/Components/HasText.h"
#include "Layer/Components/HasTransform.h"
#include "Layer/Layer/AbstractLayerDrawable.h"

ULIS_NAMESPACE_BEGIN
// Forward-declarations
template< class BlockType, class RendererType, class TSuperStackExtra > class TLayerStack;
template< class BlockType, class RasterizerType, class RendererType, class BlockAllocatorType, class LayerStackType > class TLayerFolder;
template< class BlockType, class RasterizerType, class RendererType, class BlockAllocatorType, class LayerStackType > class TLayerImage;
/////////////////////////////////////////////////////
/// @class      TLayerText
/// @brief      The TLayerText class provides a class to store text in a layer
///             of a layer stack for painting applications.
template<
      class BlockType
    , class RasterizerType
    , class RendererType
    , class BlockAllocatorType
    , class LayerStackType
>
class TLayerText final
    : public TAbstractLayerDrawable< BlockType >
    , public TRasterizable< TLayerImage< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > >
    , public THasBlock< BlockType, BlockAllocatorType >
    , public IHasBlendInfo
    , public IHasText
    , public IHasTransform
{
    typedef TRoot< ILayer > tParent;
    typedef TLayerText< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSelf;
    typedef TLayerFolder< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSiblingFolder;
    typedef TLayerImage< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSiblingImage;
    typedef TAbstractLayerDrawable< BlockType > tAbstractLayerDrawable;
    typedef TRasterizable< tSiblingImage > tRasterizable;
    typedef THasBlock< BlockType, BlockAllocatorType > tHasBlock;

public:
    // DTor
    virtual ~TLayerText() override;

    // CTors
    TLayerText(
          const FString& iName = "Untitled Text"
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
        , const FWString& iText = L"Lorem ipsum dolor sit amet..."
        , const FFont& iFont = FFont::DefaultFont
        , uint32 iFontSize = 12
        , const ISample& iTextColor = FColor::Black
        , bool iAntiAliased = true
        , const FTransformInfo& iTransform = {}
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
        , const FOnTextInfoChanged& iOnTextInfoChanged = FOnTextInfoChanged()
        , const FOnTransformInfoChanged& iOnTransformInfoChanged = FOnTransformInfoChanged()
    );

    TLayerText(
          BlockType* iBlock
        , const FString& iName = "Untitled Text"
        , bool iLocked = false
        , bool iVisible = true
        , const FColor& iPrettyColor = FColor::Transparent
        , eBlendMode iBlendMode = eBlendMode::Blend_Normal
        , eAlphaMode iAlphaMode = eAlphaMode::Alpha_Normal
        , ufloat iOpacity = 1.f
        , const FWString& iText = L"Lorem ipsum dolor sit amet..."
        , const FFont& iFont = FFont::DefaultFont
        , uint32 iFontSize = 12
        , const ISample& iTextColor = FColor::Black
        , bool iAntiAliased = true
        , const FTransformInfo& iTransform = {}
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
        , const FOnTextInfoChanged& iOnTextInfoChanged = FOnTextInfoChanged()
        , const FOnTransformInfoChanged& iOnTransformInfoChanged = FOnTransformInfoChanged()
    );

    // Disable copy
    TLayerText( const TLayerText& ) = delete;
    TLayerText& operator=( const TLayerText& ) = delete;

public:
    // ITypeIdentifiable Interface
    ULIS_OVERRIDE_TYPEID_INTERFACE( "Text" );

    // TDrawable Interface
    FEvent RenderImageCache( FContext& iCtx ) override;
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

#include "Layer/Layer/LayerText.tpp"

