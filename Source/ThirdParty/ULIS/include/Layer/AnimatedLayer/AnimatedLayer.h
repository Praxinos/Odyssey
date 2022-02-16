// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         AnimatedLayer.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the IAnimatedLayer class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Memory/Tree.h"
#include "Core/Callback.h"
#include "Layer/Components/HasLock.h"
#include "Layer/Components/HasName.h"
#include "Layer/Components/HasPrettyColor.h"
#include "Layer/Components/HasUserData.h"
#include "Layer/Components/HasVisibility.h"
#include "Layer/Components/TypeIdentifiable.h"

ULIS_NAMESPACE_BEGIN
// Forward Declarations
class IAnimatedLayer;

// Exports
template class ULIS_API TLambdaCallback< void, const TNode< IAnimatedLayer >*, const TRoot< IAnimatedLayer >* >;
template class ULIS_API TLambdaCallback< void, const TRoot< IAnimatedLayer >*, const TNode< IAnimatedLayer >* >;
template class ULIS_API TLambdaCallback< void, const TRoot< IAnimatedLayer >*, const TNode< IAnimatedLayer >*, bool >;
template class ULIS_API TLambdaCallback< void, const TNode< IAnimatedLayer >* >;
template class ULIS_API TCallbackCapable< TLambdaCallback< void, const TRoot< IAnimatedLayer >*, const TNode< IAnimatedLayer >* >, 0 >;
template class ULIS_API TCallbackCapable< TLambdaCallback< void, const TRoot< IAnimatedLayer >*, const TNode< IAnimatedLayer >*, bool >, 1 >;
template class ULIS_API TCallbackCapable< TLambdaCallback< void, const TNode< IAnimatedLayer >*, const TRoot< IAnimatedLayer >* >, 2 >;
template class ULIS_API TCallbackCapable< TLambdaCallback< void, const TNode< IAnimatedLayer >* >, 3 >;

#ifdef ULIS_WIN
template class ULIS_API TNode< IAnimatedLayer >;
template class ULIS_API TArray< TNode< IAnimatedLayer >* >;
template class ULIS_API TRoot< IAnimatedLayer >;
#endif

typedef FOn_bool_Changed FOnBoolChanged;
typedef TOnParentChanged< IAnimatedLayer > FOnAnimatedParentChanged;
typedef TOnSelfChanged< IAnimatedLayer > FOnAnimatedSelfChanged;
typedef TOnNodeAdded< IAnimatedLayer > FOnAnimatedNodeAdded;
typedef TOnNodeRemoved< IAnimatedLayer > FOnAnimatedNodeRemoved;

/////////////////////////////////////////////////////
/// @class      IAnimatedLayer
/// @brief      The IAnimatedLayer class provides a base abstract class to
///             store an animated layer in an animated layer stack for
///             animation applications.
class ULIS_API IAnimatedLayer
    : public virtual TNode< IAnimatedLayer >
    , public ITypeIdentifiable
    , public IHasUserData
    , public IHasName
    , public IHasLock
    , public IHasTimeLock
    , public IHasVisibility
    , public IHasPrettyColor
{
public:
    // DTor
    virtual ~IAnimatedLayer() override = 0;

    // CTor
    IAnimatedLayer(
          const FString& iName = "Untitled"
        , bool iLocked = false
        , bool iTimeLocked = false
        , bool iVisible = true
        , const FColor& iPrettyColor = FColor::Transparent
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
    );

public:
    virtual uint64 GetNumFrames() const = 0;
};

ULIS_NAMESPACE_END

