// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         ColorSpace.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the ColorSpace utilities.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class  FColorSpace
class ULIS_API FColorSpace
{
public:
    ~FColorSpace();
    FColorSpace();
};

/////////////////////////////////////////////////////
/// @class  IHasColorSpace
class ULIS_API IHasColorSpace
{
protected:
    /*! Construct a IHasColorSpace interface in a derived class */
    IHasColorSpace( const FColorSpace* iColorSpace = nullptr )
        : mColorSpace( iColorSpace )
    {}

public:
    /*! Destructor, the colorspace lifetime is not managed */
    ~IHasColorSpace()
    {}

    /*!
    Assign a new optional color-space.

    This functions does not perform any kind of conversion and doesn't modify
    the data at all, it just means the recipient will be interpreted in the
    given colorspace where needed, such as in conversion functions.

    The \a iColorSpace parameter is optional and can be nullptr, in which case
    the object that implements this interface will have no colorspace and will
    fallback to a default colorspace for the color-model default ( e.g: sRGB
    for RGB ) where needed.

    \warning IHasColorSpace doesn't own nor manage lifetime of its color-space.

    \sa ColorSpace()
    */
    ULIS_FORCEINLINE
    void AssignColorSpace( const FColorSpace* iColorSpace )
    {
        mColorSpace = iColorSpace;
    }

    /*!
    Get a pointer to the color-space.

    The returned value can be nullptr.

    \warning IHasColorSpace doesn't own nor manage lifetime of its color-space.

    \sa AssignColorSpace()
    */
    ULIS_FORCEINLINE
    const FColorSpace* ColorSpace() const
    {
        return  mColorSpace;
    }

private:
    const FColorSpace* mColorSpace; ///< The colorspace, lifetime is not managed.
};

ULIS_NAMESPACE_END

