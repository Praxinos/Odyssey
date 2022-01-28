// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         AnimatedLayerStack.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FAnimatedLayerStack class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/AnimatedLayer/AnimatedLayer.h"
#include "Image/ColorSpace.h"
#include "Image/Format.h"
#include "Image/Size2D.h"
#include "Layer/Components/Searchable.h"
#include "Layer/Components/HasTimeSettings.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FAnimatedLayerStack
/// @brief      The FAnimatedLayerStack class provides a class to store a layer stack
///             for painting applications.
class FAnimatedLayerStack final

    : public IAnimatedLayer
    , public TRoot< IAnimatedLayer >
    , public TSearchable< TRoot< IAnimatedLayer > >
    , public IHasSize2D
    , public IHasFormat
    , public IHasColorSpace
    , public IHasTimeSettings
{
public:
    // DTor
    ~FAnimatedLayerStack() override;

    // CTor
    FAnimatedLayerStack(
          uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat = eFormat::Format_RGBA8
        , const FColorSpace* iColorSpace = nullptr
        , uint16 iFps = 24
        , int64 iBeginFrame = 0
        , int64 iEndFrame = 0
        , int64 iCurrentFrame = 0
        , const FOnTimeSettingsChanged& iOnTimeSettingsChanged = FOnTimeSettingsChanged()
        , const FOnAnimatedSelfChanged& iOnSelfChanged = FOnAnimatedSelfChanged()
        , const FOnAnimatedNodeAdded& iOnLayerAdded = FOnAnimatedNodeAdded()
        , const FOnAnimatedNodeRemoved& iOnLayerRemoved = FOnAnimatedNodeRemoved()
        , const FOnUserDataAdded& iOnUserDataAdded = FOnUserDataAdded()
        , const FOnUserDataChanged& iOnUserDataChanged = FOnUserDataChanged()
        , const FOnUserDataRemoved& iOnUserDataRemoved = FOnUserDataRemoved()
    );

    // Disable copy
    FAnimatedLayerStack( const FAnimatedLayerStack& ) = delete;
    FAnimatedLayerStack& operator=( const FAnimatedLayerStack& ) = delete;

public:
    // FLayerStack Interface
    void Reset(
          uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat = eFormat::Format_RGBA8
        , const FColorSpace* iColorSpace = nullptr
        , uint16 iFps = 24
        , int64 iBeginFrame = 0
        , int64 iEndFrame = 0
        , int64 iCurrentFrame = 0
        , const FOnTimeSettingsChanged& iOnTimeSettingsChanged = FOnTimeSettingsChanged()
        , const FOnAnimatedSelfChanged& iOnSelfChanged = FOnAnimatedSelfChanged()
        , const FOnAnimatedNodeAdded& iOnLayerAdded = FOnAnimatedNodeAdded()
        , const FOnAnimatedNodeRemoved& iOnLayerRemoved = FOnAnimatedNodeRemoved()
        , const FOnUserDataAdded& iOnUserDataAdded = FOnUserDataAdded()
        , const FOnUserDataChanged& iOnUserDataChanged = FOnUserDataChanged()
        , const FOnUserDataRemoved& iOnUserDataRemoved = FOnUserDataRemoved()
    );

    // ITypeIdentifiable Interface
    ULIS_OVERRIDE_TYPEID_INTERFACE( "AnimatedStack" );

    // TRoot Overload Shadow Interface
    // TSearchable Overload Shadow Interface
    using TRoot< IAnimatedLayer >::operator[];
    using TSearchable< TRoot< IAnimatedLayer > >::operator[];
};

ULIS_NAMESPACE_END

