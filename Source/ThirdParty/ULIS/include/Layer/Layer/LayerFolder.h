// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerFolder.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FLayerFolder class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Components/HasBlendInfo.h"
#include "Layer/Components/HasBlock.h"
#include "Layer/Components/HasCollapse.h"
#include "Layer/Components/Searchable.h"
#include "Layer/Layer/Layer.h"

ULIS_NAMESPACE_BEGIN
// Forward-declarations
//template< class BlockType, class RendererType, class SuperStackExtra > class TLayerStack;
//template< class BlockType, class RasterizerType, class RendererType, class BlockAllocatorType, class LayerStackType > class TLayerImage;

/////////////////////////////////////////////////////
/// @class      FLayerFolder
/// @brief      The FLayerFolder class provides a class to store a folder of
///             layers in a layer stack for painting applications.
class ULIS_API FLayerFolder
    : public ILayer
    , public TRoot< ILayer >
    , public TSearchable< TRoot< ILayer > >
    , public IHasBlendInfo
    , public IHasCollapse
{
    typedef TRoot< ILayer > tParent;
    //typedef TLayerImage< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSiblingImage;
    //typedef FLayerFolder< BlockType, RasterizerType, RendererType, BlockAllocatorType, LayerStackType > tSelf;
    //typedef TAbstractLayerDrawable< BlockType > tAbstractLayerDrawable;
    //typedef TRasterizable< tSiblingImage > tRasterizable;
    //typedef THasBlock< BlockType, BlockAllocatorType > tHasBlock;

public:
    // DTor
    ~FLayerFolder() override;

    // CTors
    FLayerFolder(
          const FString& iName = "Untitled Folder"
        , bool iLocked = false
        , bool iVisible = true
        , const FColor& iPrettyColor = FColor::Transparent
        , eBlendMode iBlendMode = eBlendMode::Blend_Normal
        , eAlphaMode iAlphaMode = eAlphaMode::Alpha_Normal
        , ufloat iOpacity = 1.f
        , bool iCollapsed = false
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
        , const FOnNodeAdded& iOnLayerAdded = FOnNodeAdded()
        , const FOnNodeRemoved& iOnLayerRemoved = FOnNodeRemoved()

        , const FOnBlendInfoChanged& iOnBlendInfoChanged = FOnBlendInfoChanged()
        , const FOnBoolChanged& iOnCollapseChanged = FOnBoolChanged()
    );

    // Disable copy
    FLayerFolder( const FLayerFolder& ) = delete;
    FLayerFolder& operator=( const FLayerFolder& ) = delete;

public:
    // ITypeIdentifiable Interface
    ULIS_OVERRIDE_TYPEID_INTERFACE( "Folder" );

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

    // TRoot Overload Shadow Interface
    // TSearchable Overload Shadow Interface
    using TRoot< ILayer >::operator[];
    using TSearchable< TRoot< ILayer > >::operator[];

private:
    // TNode< ILayer > Interface
    void InitFromParent( const TRoot< ILayer >* iParent ) override;
};

ULIS_NAMESPACE_END

