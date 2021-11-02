// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Size2D.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the Size2D utilities.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/Vector.h"
#include "Math/Geometry/Rectangle.h"

ULIS_NAMESPACE_BEGIN
template struct ULIS_API TVector2< uint16 >;
typedef TVector2< uint16 > FVec2UI16;

/////////////////////////////////////////////////////
/// @class      IHasSize2D
/// @brief      The IHasSize2D class provides an interface for accessing components
///             with a 2D Size
class ULIS_API IHasSize2D
{
protected:
    /*! Constructor from eFormat. */
    IHasSize2D( const FVec2UI16& iSize )
        : mSize( iSize )
    {}

public:
    /*! Destructor. */
    ~IHasSize2D()
    {}

    ULIS_FORCEINLINE uint16 Width() const {
        return  mSize.x;
    }

    ULIS_FORCEINLINE uint16 Height() const {
        return  mSize.y;
    }

    ULIS_FORCEINLINE uint32 Area() const {
        return  mSize.x * mSize.y;
    }

    ULIS_FORCEINLINE FVec2I Size() const {
        return  mSize;
    }

    ULIS_FORCEINLINE FRectI Rect() const {
        return  FRectI::FromPositionAndSize( FVec2I( 0 ), mSize );
    }

protected:
    /*!
    Reinterpret the size by changing the value and extrapolating and caching
    the values again.
    */
    ULIS_FORCEINLINE void ReinterpretSize( const FVec2UI16& iSize ) {
        mSize = iSize;
    }

private:
    FVec2UI16 mSize; ///< The Size.
};

ULIS_NAMESPACE_END

