// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         StructuringElement.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the StructuringElement class.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Data/Block.h"
#include "Maths/Geometry.h"

ULIS3_NAMESPACE_BEGIN

enum eMorphologicalElementValue {
      MP_ZERO   = 0
    , MP_ONE    = 1
    , MP_ANY    = 2
};

/////////////////////////////////////////////////////
/// @class      FStructuringElement
/// @brief      The FStructuringElement class provides a mean of storing and manipulating structuring elements or kernels for morphological operators
/// @details    The structuring element is actually a block in Gray uint8 format.
class ULIS3_API FStructuringElement : public FBlock
{
public:
    typedef FStructuringElement tSelf;
    typedef FBlock  tSuperClass;

public:
    // Construction / Destruction
    virtual ~FStructuringElement();
    FStructuringElement( const FVec2I& iSize, eMorphologicalElementValue iValue = MP_ZERO );
    FStructuringElement( const FVec2I& iSize, std::initializer_list< eMorphologicalElementValue > iNums );
    FStructuringElement( const FStructuringElement& );
    FStructuringElement( FStructuringElement&& );
    FStructuringElement& operator=( const FStructuringElement& );
    FStructuringElement& operator=( FStructuringElement&& );

public:
    // Public API
    void  Set( std::initializer_list< eMorphologicalElementValue > iNums );
    eMorphologicalElementValue At( int iX, int iY ) const;
    eMorphologicalElementValue At( FVec2I iPoint ) const;
    void SetAt( int iX, int iY, eMorphologicalElementValue iValue );
    void SetAt( FVec2I iPoint, eMorphologicalElementValue iValue );
    void Clear();
    void Fill( eMorphologicalElementValue iValue );
    void SetZeroes();
    void SetOnes();
    void FlipX();
    void FlipY();
    void Rotate90CW();
    void Rotate90CCW();
    void Rotate180();
    FStructuringElement FlippedX() const;
    FStructuringElement FlippedY() const;
    FStructuringElement Rotated90CW() const;
    FStructuringElement Rotated90CCW() const;
    FStructuringElement Rotated180() const;
    const FVec2I Size() const;
    const FVec2I& Pivot() const;
    void SetPivot( const FVec2I& iPivot );

private:
    // Private Data Members
    FVec2I mPivot;
};

ULIS3_NAMESPACE_END

