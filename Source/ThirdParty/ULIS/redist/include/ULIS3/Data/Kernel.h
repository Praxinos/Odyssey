// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Kernel.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FKernel class.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Data/Block.h"
#include "Maths/Geometry.h"

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FConvolutionKernel
/// @brief      The FKernel class provides a mean of storing and manipulating convolution kernels
/// @details    The kernel is actually a block in Gray floating point format.
/// @details    kernel data lives on CPU, kernel can also be used as matrix.
class ULIS3_API FKernel : public FBlock
{
public:
    typedef FKernel tSelf;
    typedef FBlock  tSuperClass;

public:
    // Construction / Destruction
    virtual ~FKernel();
    FKernel( const FVec2I& iSize, float iValue = 0.f );
    FKernel( const FVec2I& iSize, std::initializer_list< float > iNums );
    FKernel( const FKernel& );
    FKernel( FKernel&& );
    FKernel& operator=( const FKernel& );
    FKernel& operator=( FKernel&& );

public:
    // Public API
    void  Set( std::initializer_list< float > iNums );
    float At( int iX, int iY ) const;
    float At( FVec2I iPoint ) const;
    void SetAt( int iX, int iY, float iValue );
    void SetAt( FVec2I iPoint, float iValue );
    void Clear();
    void Fill( float iValue );
    void SetZeroes();
    void SetOnes();
    float Sum() const;
    void Add( float iValue );
    void Mul( float iValue );
    void Normalize();
    bool IsNormalized() const;
    void FlipX();
    void FlipY();
    void Rotate90CW();
    void Rotate90CCW();
    void Rotate180();
    FKernel Normalized() const;
    FKernel FlippedX() const;
    FKernel FlippedY() const;
    FKernel Rotated90CW() const;
    FKernel Rotated90CCW() const;
    FKernel Rotated180() const;
    const FVec2I Size() const;
    const FVec2I& Pivot() const;
    void SetPivot( const FVec2I& iPivot );

private:
    // Private Data Members
    FVec2I mPivot;
};

ULIS3_NAMESPACE_END

