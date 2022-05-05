// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         BezierInterpolation.h
* @author       Thomas Schmitt
* @brief        This file provides declaration for the bezier interpolation
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Animation/Interpolation/AbstractInterpolation.h"
#include "Math/Interpolation/Bezier.h"

ULIS_NAMESPACE_BEGIN

template< typename T >
class TBezierInterpolation : public TAbstractInterpolation< T >
{
public:
    TBezierInterpolation<T>();
    virtual TBezierInterpolation<T>* Clone() const;

public:
    virtual T Interpolate( ufloat iFrame, const TKey<T>& iLeftKey, const TKey<T>& iRightKey ) const override;

    void SetRightTangent( FVec2F iRightTangent );
    void SetLeftTangent(FVec2F iLeftTangent);

private:
    /** Represents the right tangent of the left key */
    FVec2F mRightTangent;

    /** Represents the left tangent of the right key */
    FVec2F mLeftTangent;
};

template< typename T >
TBezierInterpolation<T>::TBezierInterpolation() :
    TAbstractInterpolation<T>(),
    mRightTangent( FVec2I( 1.f, 0.f )),
    mLeftTangent( FVec2I( -1.f, 0.f ))
{

}

template< typename T >
TBezierInterpolation<T>* TBezierInterpolation<T>::Clone() const
{
    TBezierInterpolation<T>* bezierInterp = new TBezierInterpolation<T>();
    bezierInterp->mLeftTangent = mLeftTangent;
    bezierInterp->mRightTangent = mRightTangent;
    return bezierInterp;
}

template< typename T >
T TBezierInterpolation<T>::Interpolate( ufloat iFrame, const TKey<T>& iLeftKey, const TKey<T>& iRightKey ) const
{
    FVec2F leftKey = FVec2F( iLeftKey.mFrame, iLeftKey.mValue );
    FVec2F rightKey = FVec2F( iRightKey.mFrame, iRightKey.mValue );

    //This method of approximation is in fact faster than solving the maths around a cubic bezier. So for the interpolation in animation, we should keep this solution
    //If this is not enough, https://stackoverflow.com/questions/51879836/cubic-bezier-curves-get-y-for-given-x-special-case-where-x-of-control-points and https://pomax.github.io/bezierinfo/#whatis can lead to mathematical solution
    std::vector<FSplineParametricSample> bezier;
    CubicBezierGenerateLinearLUT( &bezier, leftKey, leftKey + mRightTangent, rightKey + mLeftTangent, rightKey, 1.f );    

    //Dichotomy to search for the closest point at iFrame
    int leftKeyIndex = 0;
    int rightKeyIndex = int(bezier.size() - 1);

    while(rightKeyIndex - leftKeyIndex > 1)
    {
        int searchIndex = (leftKeyIndex + rightKeyIndex) / 2;
        if(bezier[searchIndex].point.x > iFrame)
        {
            rightKeyIndex = searchIndex;
        }
        else
        {
            leftKeyIndex = searchIndex;
        }
    }
    
    return  bezier[leftKeyIndex].point.y;
}

template< typename T >
void TBezierInterpolation<T>::SetRightTangent(FVec2F iRightTangent)
{
    mRightTangent = iRightTangent;
}

template< typename T >
void TBezierInterpolation<T>::SetLeftTangent(FVec2F iLeftTangent)
{
    mLeftTangent = iLeftTangent;
}

ULIS_NAMESPACE_END


