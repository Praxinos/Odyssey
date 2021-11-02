// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Gradient.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FGradient class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Image/Color.h"
#include "Image/ColorSpace.h"
#include "Image/Format.h"
#include "Image/Pixel.h"
#include "Image/Sample.h"
#include "Math/Geometry/Rectangle.h"
#include "Math/Geometry/Vector.h"
#include "Memory/Array.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TGradientStep
/// @brief      The TGradientStep class provides a mean of storing a gradient step.
template< typename T >
class TGradientStep final
{
public:
    /*! Destroy the gradient step. */
    ~TGradientStep()
    {}

    /*! Construct gradient step. */
    TGradientStep()
        : mParam( 0.f )
        , mValue()
    {}

    /*! Construct gradient step. */
    TGradientStep( ufloat iParam, const T& iValue )
        : mParam( FMath::Clamp( iParam, 0.f, 1.f ) )
        , mValue( iValue )
    {}

    static
    std::shared_ptr< TGradientStep< T > > MakeShared( ufloat iParam, const T& iValue ) {
        return  std::make_shared< TGradientStep< T > >( iParam, iValue );
    }
public:
    /*! Set the step. */
    void Param( ufloat iParam ) {
        mParam = FMath::Clamp( iParam, 0.f, 1.f );
    }

    /*! Get the step. */
    ufloat Param() const {
        return  mParam;
    }

    /*! Get the value, editable. */
    T& Value() {
        return  mValue;
    }

    /*! Get the value. */
    const T& Value() const {
        return  mValue;
    }

    bool operator<( const TGradientStep< T >& iOther ) {
        return  mParam < iOther.mParam;
    }

    bool operator==( const TGradientStep< T >& iOther ) {
        return  mParam == iOther.mParam && mValue == iOther.mValue;
    }

private:
    ufloat mParam;   ///< The gradient step param.
    T mValue;  ///< The gradient step value.
};


typedef TGradientStep< FColor > FColorStep;
typedef TGradientStep< ufloat > FAlphaStep;
template class ULIS_API TGradientStep< FColor >;
template class ULIS_API TGradientStep< ufloat >;
ULIS_NAMESPACE_END

typedef std::shared_ptr< ULIS::FColorStep > FSharedColorStep;
typedef std::shared_ptr< ULIS::FAlphaStep > FSharedAlphaStep;
//template class ULIS_API std::shared_ptr< ULIS::FColorStep >;
//template class ULIS_API std::shared_ptr< ULIS::FAlphaStep >;

ULIS_NAMESPACE_BEGIN
template class ULIS_API TArray< FSharedColorStep >;
template class ULIS_API TArray< FSharedAlphaStep >;
template class ULIS_API TArray< FColorStep >;
template class ULIS_API TArray< FAlphaStep >;


/////////////////////////////////////////////////////
/// @class      FSanitizedGradient
/// @brief      The FSanitizedGradient class provides a mean of storing
///             gradients in various formats, with constraints the help
///             in rasterizing them more efficiently than their non-Sanitized
///             counterparts, which are meant for modification and editing.
/// @details    FSanitizedGradient can have a single or no color.
///             Steps are in [0;1]. Interpolation is linear in the preferred
///             format.
class ULIS_API FSanitizedGradient final
    : public IHasFormat
    , public IHasColorSpace
{
public:
    /*! Destroy the gradient. */
    ~FSanitizedGradient();

    /*! Construct sanitized gradient from regular gradient */
    FSanitizedGradient( eFormat iFormat, const FGradient& iGradient );

    FSanitizedGradient( const FSanitizedGradient& ) = delete;
    FSanitizedGradient& operator=( const FSanitizedGradient& ) = delete;

public:
    /*! Getter for gradient color steps. */
    const TArray< FColorStep >& ColorSteps() const;

    /*! Getter for gradient alpha steps. */
    const TArray< FAlphaStep >& AlphaSteps() const;

    /*! Getter for gradient index lut color. */
    const uint8* IndexLUTColor() const;

    /*! Getter for gradient index lut alpha. */
    const uint8* IndexLUTAlpha() const;

    /*! Change the preferred interpolation format. */
    void ReinterpretInterpolationFormat( eFormat iFormat );

    /*!
        Retrieve index of a color step in the array given an input parameter,
        using fast LUT retrieval.
    */
    uint8 FastColorIndexAtParameter( ufloat iParam ) const;

    /*!
        Retrieve index of a alpha step in the array given an input parameter,
        using fast LUT retrieval.
    */
    uint8 FastAlphaIndexAtParameter( ufloat iParam ) const;

public:
    static constexpr uint8 range = 100;

private:
    TArray< FColorStep > mColorSteps; ///< The gradient color steps.
    TArray< FAlphaStep > mAlphaSteps; ///< The gradient alpha steps.
    uint8 mIndexLUTColor[range]; ///< The gradient color index LUT.
    uint8 mIndexLUTAlpha[range]; ///< The gradient color index LUT.
};

/////////////////////////////////////////////////////
/// @class      FGradient
/// @brief      The FGradient class provides a mean of storing and manipulating
///             gradients in various formats.
/// @details    Gradients can have a single or no color.
///             Steps are in [0;1]. Interpolation is linear in the preferred
///             format.
class ULIS_API FGradient final
    : public IHasFormat
    , public IHasColorSpace
{
public:
    /*! Destroy the gradient. */
    ~FGradient();

    /*!
    Construct gradient with preferred interpolation format.
    By default, gradients are white to black.
    */
    FGradient( eFormat iFormat );

    FGradient( const FGradient& ) = delete;
    FGradient& operator=( const FGradient& ) = delete;

public:
    /*! Getter for gradient color steps. */
    TArray< FSharedColorStep >& ColorSteps();

    /*! Getter for gradient color steps. */
    const TArray< FSharedColorStep >& ColorSteps() const;

    /*! Getter for gradient alpha steps. */
    TArray< FSharedAlphaStep >& AlphaSteps();

    /*! Getter for gradient alpha steps. */
    const TArray< FSharedAlphaStep >& AlphaSteps() const;

    /*! Reset the gradient, keep the same format. */
    void Reset();

    /*! Reset the gradient, change the format. */
    void Reset( eFormat iFormat );

    /*! Change the preferred interpolation format. */
    void ReinterpretInterpolationFormat( eFormat iFormat );

    /*! Add color step. */
    void AddColorStep( ufloat iParam, const ISample& iColor );

    /*! Add alpha step. */
    void AddAlphaStep( ufloat iParam, ufloat iAlpha );

    /*!
        Sort values in both steps containers in case they were changed / moved.
    */
    void Sort();

    /*!
        Return the sanitized version of this gradient.
    */
    FSanitizedGradient Sanitized() const;

    /*!
        Return the sanitized version of this gradient in the specified format.
    */
    FSanitizedGradient Sanitized( eFormat iFormat ) const;

private:
    TArray< FSharedColorStep > mColorSteps; ///< The gradient color steps.
    TArray< FSharedAlphaStep > mAlphaSteps; ///< The gradient alpha steps.
};

ULIS_NAMESPACE_END

