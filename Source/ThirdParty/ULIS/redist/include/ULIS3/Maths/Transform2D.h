// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Transform2D.h
* @author       Clement Berthaud
* @brief        This file provides the Transform2D class declaration.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Maths/Geometry.h"

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// FTransform2D
class ULIS3_API FTransform2D
{
public:
    class FTransform2D_imp;

public:
    // Construction / Destruction
    ~FTransform2D();
    FTransform2D();
    FTransform2D( const FTransform2D& iOther );
    FTransform2D( FTransform2D&& iOther );
    FTransform2D& operator=( const FTransform2D& iOther );

public:
    // Public API
    const FTransform2D_imp& GetImp() const;
    const tByte* Ptr() const;
    FTransform2D Inverse() const;

private:
    // Private API
    FTransform2D( FTransform2D_imp* iVal );

public:
    // Static API
    static FTransform2D MakeFromMatrix( float iM00, float iM10, float iM20, float iM01, float iM11, float iM21, float iM02, float iM12, float iM22 );
    static FTransform2D MakeIdentityTransform();
    static FTransform2D MakeRotationTransform( float iAngleRag );
    static FTransform2D MakeScaleTransform( float iX, float iY );
    static FTransform2D MakeShearTransform( float iX, float iY );
    static FTransform2D MakeTranslationTransform( float iX, float iY );
    static FTransform2D ComposeTransforms( const FTransform2D& iA, const FTransform2D& iB );
    static void DecomposeTransform( const FTransform2D& iTransform, float* oTx, float* oTy, float* oRotation, float* oScaleX, float* oScaleY, float* oSkewX, float* oSkewY );
    static FTransform2D GetPerspectiveTransform( const FVec2F iSrc[], const FVec2F iDst[] );
    static FVec2F DoHomographyTransform( const FVec2F& iPoint, const FTransform2D& iTransform );

private:
    // Private Data Members
    FTransform2D_imp* mImp;
};

ULIS3_NAMESPACE_END

