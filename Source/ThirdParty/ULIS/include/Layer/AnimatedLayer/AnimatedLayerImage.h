// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         AnimatedLayerImage.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FAnimatedLayerImage class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Image/Block.h"
#include "Image/ColorSpace.h"
#include "Image/Format.h"
#include "Image/Size2D.h"
#include "Layer/AnimatedLayer/AnimatedLayer.h"
#include "Layer/AnimatedLayer/Sequence.h"
#include "Layer/Components/HasBlendInfo.h"
#include "Animation/AnimatedProperty.h"

ULIS_NAMESPACE_BEGIN
struct ULIS_API FCelBlockFactory
    : public IHasSize2D
    , public IHasFormat
    , public IHasColorSpace
{
    ~FCelBlockFactory() {};

    FCelBlockFactory(
          uint16 iWidth = 0
        , uint16 iHeight = 0
        , eFormat iFormat = Format_RGBA8
        , const FColorSpace* iColorSpace = nullptr
    )
    : IHasSize2D( FVec2UI16( iWidth, iHeight ) )
    , IHasFormat( iFormat )
    , IHasColorSpace( iColorSpace )
    {}

    TCel< FBlock >* MakeBlank( uint32 iExposure = 0 ) {
        return  new TCel< FBlock >( nullptr, iExposure );
    }

    TCel< FBlock >* MakeNew( uint32 iExposure = 0 ) {
        return  new TCel< FBlock >( std::make_shared< FBlock >( Width(), Height(), Format(), ColorSpace() ), iExposure );
    }

    TCel< FBlock >* MakeSharedFrom( TCel< FBlock >* iRefCel, uint32 iExposure = 0 ) {
        ULIS_ASSERT( iRefCel, "Bad input" );
        return  new TCel< FBlock >( iRefCel->Data(), iExposure );
    }
};

/////////////////////////////////////////////////////
/// @class      FAnimatedLayerImage
/// @brief      The FAnimatedLayerImage class provides a class to store an image in a
///             layer stack for painting applications.
class ULIS_API FAnimatedLayerImage final
    : public IAnimatedLayer
    , public IHasBlendInfo
    , public IHasPaintLock
    , public TSequence< FBlock, FCelBlockFactory >
{
    typedef TRoot< IAnimatedLayer > tParent;

public:
    // DTor
    ~FAnimatedLayerImage() override;

    // CTors
    FAnimatedLayerImage(
          const FString& iName = "Untitled Image"
        , bool iLocked = false
        , bool iTimeLocked = false
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

        , const FOnBlendInfoChanged& iOnBlendInfoChanged = FOnBlendInfoChanged()
        , const FOnBoolChanged& iOnPaintLockChanged = FOnBoolChanged()
    );

    // Disable copy
    FAnimatedLayerImage( const FAnimatedLayerImage& ) = delete;
    FAnimatedLayerImage& operator=( const FAnimatedLayerImage& ) = delete;

public:
    // ITypeIdentifiable Interface
    ULIS_OVERRIDE_TYPEID_INTERFACE( "AnimatedImage" );

private:
    // TNode< IAnimatedLayer > Interface
    void InitFromParent( const TRoot< IAnimatedLayer >* iParent ) override;

public:
    // IAnimatedLayer interface
    uint64 GetNumFrames() const override;
};

ULIS_NAMESPACE_END

