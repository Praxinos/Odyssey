// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Kernel.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FKernel class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Image/Block.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FKernel
/// @brief      The FKernel class provides a mean of storing and manipulating convolution kernels
/// @details    The kernel is actually a block in Gray floating point format.
///             kernel data lives on CPU, kernel can also be used as matrix.
class ULIS_API FKernel final
    : public FBlock
{
public:
    typedef FKernel tSelf;
    typedef FBlock  tSuperClass;

public:
    // Construction / Destruction
    ~FKernel();
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
    float At( const FVec2I& iPoint ) const;
    void SetAt( int iX, int iY, float iValue );
    void SetAt( const FVec2I& iPoint, float iValue );
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

public:
    // Static common kernels
    static const FKernel Identity;
    static const FKernel Edge4;
    static const FKernel Edge8;
    static const FKernel Sharpen;
    static const FKernel BoxBlur;
    static const FKernel GaussianBlur;
    static const FKernel UnsharpMask;

private:
    // Private Data Members
    FVec2I mPivot;
};

ULIS_NAMESPACE_END

