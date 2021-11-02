// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         StructuringElement.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FStructuringElement class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Image/Block.h"

ULIS_NAMESPACE_BEGIN
enum eMorphologicalElementValue {
      MpE_Zero  = 0
    , MpE_One   = 1
    , MpE_Any   = 2
};

/////////////////////////////////////////////////////
/// @class      FStructuringElement
/// @brief      The FStructuringElement class provides a mean of storing and manipulating structuring elements or kernels for morphological operators
/// @details    The structuring element is actually a block in Gray uint8 format.
class ULIS_API FStructuringElement final
    : public FBlock
{
public:
    typedef FStructuringElement tSelf;
    typedef FBlock  tSuperClass;

public:
    // Construction / Destruction
    ~FStructuringElement();
    FStructuringElement( const FVec2I& iSize, eMorphologicalElementValue iValue = MpE_Zero );
    FStructuringElement( const FVec2I& iSize, std::initializer_list< eMorphologicalElementValue > iNums );
    FStructuringElement( const FStructuringElement& );
    FStructuringElement( FStructuringElement&& );
    FStructuringElement& operator=( const FStructuringElement& );
    FStructuringElement& operator=( FStructuringElement&& );

public:
    // Public API
    void  Set( std::initializer_list< eMorphologicalElementValue > iNums );
    eMorphologicalElementValue At( int iX, int iY ) const;
    eMorphologicalElementValue At( const FVec2I& iPoint ) const;
    void SetAt( int iX, int iY, eMorphologicalElementValue iValue );
    void SetAt( const FVec2I& iPoint, eMorphologicalElementValue iValue );
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

ULIS_NAMESPACE_END

