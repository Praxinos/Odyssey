// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         AnimatedLayerFolder.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FAnimatedLayerFolder class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/AnimatedLayer/AnimatedLayer.h"
#include "Layer/Components/HasBlendInfo.h"
#include "Layer/Components/HasCollapse.h"
#include "Layer/Components/Searchable.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FAnimatedLayerFolder
/// @brief      The FAnimatedLayerFolder class provides a class to store a folder of
///             layers in a layer stack for painting applications.
class ULIS_API FAnimatedLayerFolder final
    : public IAnimatedLayer
    , public TRoot< IAnimatedLayer >
    , public TSearchable< TRoot< IAnimatedLayer > >
    , public IHasBlendInfo
    , public IHasCollapse
{
    typedef TRoot< IAnimatedLayer > tParent;

public:
    // DTor
    ~FAnimatedLayerFolder() override;

    // CTors
    FAnimatedLayerFolder(
          const FString& iName = "Untitled Folder"
        , bool iLocked = false
        , bool iTimeLocked = false
        , bool iVisible = true
        , const FColor& iPrettyColor = FColor::Transparent
        , eBlendMode iBlendMode = eBlendMode::Blend_Normal
        , eAlphaMode iAlphaMode = eAlphaMode::Alpha_Normal
        , ufloat iOpacity = 1.f
        , bool iCollapsed = false
        , const TRoot< IAnimatedLayer >* iParent = nullptr

        , const FOnNameChanged& iOnNameChanged = FOnNameChanged()
        , const FOnBoolChanged& iOnLockChanged = FOnBoolChanged()
        , const FOnBoolChanged& iOnTimeLockChanged = FOnBoolChanged()
        , const FOnBoolChanged& iOnVisibleChanged = FOnBoolChanged()
        , const FOnColorChanged& iOnColorChanged = FOnColorChanged()
        , const FOnUserDataAdded& iOnUserDataAdded = FOnUserDataAdded()
        , const FOnUserDataChanged& iOnUserDataChanged = FOnUserDataChanged()
        , const FOnUserDataRemoved& iOnUserDataRemoved = FOnUserDataRemoved()
        , const FOnAnimatedParentChanged& iOnParentChanged = FOnAnimatedParentChanged()
        , const FOnAnimatedSelfChanged& iOnSelfChanged = FOnAnimatedSelfChanged()
        , const FOnAnimatedNodeAdded& iOnLayerAdded = FOnAnimatedNodeAdded()
        , const FOnAnimatedNodeRemoved& iOnLayerRemoved = FOnAnimatedNodeRemoved()

        , const FOnBlendInfoChanged& iOnBlendInfoChanged = FOnBlendInfoChanged()
        , const FOnBoolChanged& iOnCollapseChanged = FOnBoolChanged()
    );

    // Disable copy
    FAnimatedLayerFolder( const FAnimatedLayerFolder& ) = delete;
    FAnimatedLayerFolder& operator=( const FAnimatedLayerFolder& ) = delete;

public:
    // ITypeIdentifiable Interface
    ULIS_OVERRIDE_TYPEID_INTERFACE( "AnimatedFolder" );

    // TRoot Overload Shadow Interface
    // TSearchable Overload Shadow Interface
    using TRoot< IAnimatedLayer >::operator[];
    using TSearchable< TRoot< IAnimatedLayer > >::operator[];

private:
    // TNode< IAnimatedLayer > Interface
    void InitFromParent( const TRoot< IAnimatedLayer >* iParent ) override;
};

ULIS_NAMESPACE_END

