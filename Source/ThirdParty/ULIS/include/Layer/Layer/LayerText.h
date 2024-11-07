// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerText.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FLayerText class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Font/Font.h"
#include "Layer/Components/HasBlendInfo.h"
#include "Layer/Components/HasBlock.h"
#include "Layer/Components/HasText.h"
#include "Layer/Components/HasTransform.h"
#include "Layer/Layer/Layer.h"
#include "String/WString.h"

ULIS_NAMESPACE_BEGIN
// Forward-declarations
//template< class BlockType, class RendererType, class TSuperStackExtra > class TLayerStack;
//template< class BlockType, class RasterizerType, class RendererType, class BlockAllocatorType, class LayerStackType > class TLayerFolder;
//template< class BlockType, class RasterizerType, class RendererType, class BlockAllocatorType, class LayerStackType > class TLayerImage;

/////////////////////////////////////////////////////
/// @class      FLayerText
/// @brief      The FLayerText class provides a class to store text in a layer
///             of a layer stack for painting applications.
class ULIS_API FLayerText final
    : public ILayer
    , public IHasBlendInfo
    , public IHasText
    , public IHasTransform
{
    typedef TRoot< ILayer > tParent;
    //typedef FLayerText< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSelf;
    //typedef TLayerFolder< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSiblingFolder;
    //typedef TLayerImage< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSiblingImage;
    //typedef TAbstractLayerDrawable< BlockType > tAbstractLayerDrawable;
    //typedef TRasterizable< tSiblingImage > tRasterizable;
    //typedef THasBlock< BlockType, BlockAllocatorType > tHasBlock;

public:
    // DTor
    ~FLayerText() override;

    // CTors
    FLayerText(
          const FString& iName = "Untitled Text"
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

        //, const TOnBlockChanged< BlockType >& iOnBlockChanged = TOnBlockChanged< BlockType >()
        , const FOnBlendInfoChanged& iOnBlendInfoChanged = FOnBlendInfoChanged()
        , const FOnTextInfoChanged& iOnTextInfoChanged = FOnTextInfoChanged()
        , const FOnTransformInfoChanged& iOnTransformInfoChanged = FOnTransformInfoChanged()
    );

    // Disable copy
    FLayerText( const FLayerText& ) = delete;
    FLayerText& operator=( const FLayerText& ) = delete;

public:
    // ITypeIdentifiable Interface
    ULIS_OVERRIDE_TYPEID_INTERFACE( "Text" );

    /*
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
    tSiblingImage* Rasterize( FContext& iCtx, FEvent* oEvent = nullptr ) override;
    */

private:
    // TNode< ILayer > Interface
    void InitFromParent( const TRoot< ILayer >* iParent ) override;
};

ULIS_NAMESPACE_END

