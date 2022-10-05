// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Pixel.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FPixel class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Image/Sample.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FPixel
/// @brief      The FPixel class provides a mean of storing and manipulating
///             colors in various formats.
/// @details    A FPixel does not allocates its own storage space and is not
///             responsible for its memory.
///
///             \sa ISample
///             \sa FColor
class ULIS_API FPixel final
    : public ISample
{
public:
    /*! Destructor, doesn't manage lifetime of internal buffer. */
    ~FPixel();

    /*! Constructor, from external data, format and optional colorspace. */
    FPixel( uint8* iData, eFormat iFormat, const FColorSpace* iColorSpace = nullptr, uint64 iPlaneSize = 0 );

    /*! Constructor, from const external data, format and optional colorspace. */
    FPixel( const uint8* iData, eFormat iFormat, const FColorSpace* iColorSpace = nullptr, uint64 iPlaneSize = 0 );

    /*! Copy Constructor */
    FPixel( const FPixel& iValue );

    /*! Constructor, from any sample, such as color. */
    FPixel( const ISample& iSample );

    /*! Move Constructor */
    FPixel( FPixel&& iValue );

    /*! Assignment Operator. */
    FPixel& operator=( const FPixel& iValue );

    /*!
    Move the underlying pointer to a compatible signal, usually obtained with
    PixelBits() from the FBlock this FPixel object was initially obtained from.

    \sa FBlock
    */
    void SetPointer( uint8* iPtr );

    /*!
    Move the underlying signal data pointer to the next sample.
    */
    ULIS_FORCEINLINE void Next() const { mSignal += BytesPerPixel(); }

    /*!
    Move the underlying signal data pointer to the prev sample.
    */
    ULIS_FORCEINLINE void Prev() const { mSignal -= BytesPerPixel(); }
};

ULIS_NAMESPACE_END

