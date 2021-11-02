// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         HasCachedTransform.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the IHasCachedTransform class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Components/TransformInfo.h"
#include "Core/CallbackCapable.h"

ULIS_NAMESPACE_BEGIN
ULIS_DECLARE_SIMPLE_DELEGATE( FOnTransformInfoChanged, void, const FTransformInfo& )
/////////////////////////////////////////////////////
/// @class      IHasCachedTransform
/// @brief      Simple HasCachedTransform class.
class ULIS_API IHasTransform
    : public FOnTransformInfoChanged
{
protected:
    IHasTransform(
          const FVec2F& iPivot = FVec2F( 0.f )
        , const FVec2F& iTranslation = FVec2F( 0.f )
        , ufloat iRotation = 0.f
        , const FVec2F& iScale = FVec2F( 1.f )
        , const FOnTransformInfoChanged& iDelegate = FOnTransformInfoChanged()
    );

public:
    // Pivot setter
    void ResetPivot();
    void SetPivot( const FVec2F& iValues );
    void SetPivot( float iX, float iY );
    void SetPivotX( float iValue );
    void SetPivotY( float iValue );

    void MovePivot( const FVec2F& iDelta );
    void MovePivot( float iDeltaX, float iDeltaY );
    void MovePivotX( float iDelta );
    void MovePivotY( float iDelta );
    void MovePivotRight( float iDelta );
    void MovePivotDown( float iDelta );

    // Translation setter
    void ResetTranslation();
    void SetTranslation( const FVec2F& iValues );
    void SetTranslation( float iX, float iY );
    void SetTranslationX( float iValue );
    void SetTranslationY( float iValue );

    void Translate( const FVec2F& iDelta );
    void Translate( float iDeltaX, float iDeltaY );
    void TranslateX( float iDelta );
    void TranslateY( float iDelta );
    void Move( const FVec2F& iDelta );
    void Move( float iDeltaRight, float iDeltaDown );
    void MoveRight( float iDelta );
    void MoveDown( float iDelta );

    // Rotation setter
    void ResetRotation();
    void SetRotation( float iAngleRad );
    void Rotate( float iAngleRad );

    // Scale setter
    void ResetScale();
    void SetScale( const FVec2F& iValues );
    void SetScale( float iX, float iY );
    void SetScaleuniform( float iValue );
    void SetScaleX( float iValue );
    void SetScaleY( float iValue );

    void Stretch( const FVec2F& iValues );
    void Stretch( float iX, float iY );
    void StretchUniform( float iValue );
    void StretchX( float iValue );
    void StretchY( float iValue );

    void DeltaScale( const FVec2F& iValues );
    void DeltaScale( float iX, float iY );
    void DeltaScaleUniform( float iValue );
    void DeltaScaleX( float iValue );
    void DeltaScaleY( float iValue );

public:
    // Pivot Getter
    const FVec2F& Pivot() const;
    float PivotX() const;
    float PivotY() const;

    // Translation Getter
    const FVec2F& Translation() const;
    float TranslationX() const;
    float TranslationY() const;

    // Rotation Getter
    float Rotation() const;

    // Scale Getter
    const FVec2F& Scale() const;
    float ScaleX() const;
    float ScaleY() const;

public:
    // Advanced Positioning
    void TargetLocal( const FVec2F& iTarget );  // target local from input local point
    void Target( const FVec2F& iTarget );       // target local from input external point

public:
    // Frame of ref API
    FVec2F Right() const;          // Local X( 1, 0 ) expressed in external basis
    FVec2F Down() const;           // Local Y( 0, 1 ) expressed in external basis
    FVec2F LocalXAxis() const;     // Local X( 1, 0 ), expressed in local basis
    FVec2F LocalYAxis() const;     // Local Y( 0, 1 ), expressed in local basis
    FVec2F ExternalXAxis() const;  // External X( 1, 0 ), expressed in local basis
    FVec2F ExternalYAxis() const;  // External Y( 0, 1 ), expressed in local basis

public:
    // Conversion API
    FVec3F ToLocal( const  FVec3F& iHPoint ) const;         // from external to local
    FVec3F FromLocal( const  FVec3F& iHPoint ) const;       // from local to external
    FVec2F PointToLocal( const FVec2F& iPoint ) const;      // from external to local
    FVec2F PointFromLocal( const FVec2F& iPoint ) const;    // from local to external
    FVec2F VecToLocal( const FVec2F& iVec ) const;          // from external to local
    FVec2F VecFromLocal( const FVec2F& iVec ) const;        // from local to external

public:
    // Intermediate Matrix API
    FMat3F PivotMatrix() const;
    FMat3F TranslationMatrix() const;
    FMat3F ScaleMatrix() const;
    FMat3F RotationMatrix() const;

    FMat3F Matrix() const;
    FMat3F InverseMatrix() const;

private:
    FTransformInfo mInfo;
};

ULIS_NAMESPACE_END

