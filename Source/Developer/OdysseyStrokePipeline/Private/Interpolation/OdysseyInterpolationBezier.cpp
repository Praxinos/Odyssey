// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Interpolation/OdysseyInterpolationBezier.h"
#include "OdysseyMathUtils.h"


#define LOCTEXT_NAMESPACE "OdysseyInterpolationBezier"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyInterpolationBezier::~FOdysseyInterpolationBezier()
{
}

FOdysseyInterpolationBezier::FOdysseyInterpolationBezier()
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
bool
FOdysseyInterpolationBezier::IsReady() const
{
    return ( mInputPoints.Num() >= MinimumRequiredPoints() );
}

int
FOdysseyInterpolationBezier::MinimumRequiredPoints() const
{
    return 3;
}

const TArray< FOdysseyStrokePoint >& FOdysseyInterpolationBezier::ComputePoints()
{
    mResultPoints.Empty();

    if( !IsReady() )
        return mResultPoints;

    mInputPoints[2] = ( mInputPoints[1] + mInputPoints[2] ) / 2;
    TArray< FOdysseyMathUtils::FOdysseyBezierLutElement > LUT;
    FVector2D A( mInputPoints[0].x, mInputPoints[0].y );
    FVector2D B( mInputPoints[1].x, mInputPoints[1].y );
    FVector2D C( mInputPoints[2].x, mInputPoints[2].y );
    float length = FOdysseyMathUtils::QuadraticBezierGenerateLinearLUT( &LUT, A, B, C, mStep );
    float previousStrokeLength = mTotalStrokeLength;
    mTotalStrokeLength += length;

    float remaining = mTotalStrokeLength - mLastDrawnLength;
    float delta = remaining - mStep;
    float next = length - delta;

    int iLastSelectedLUTIndex = 0;
    int drawn_steps = 0;

    bool point_in_substroke = next > 0;

    if( point_in_substroke )
    {
        for( float i = next; i <= length; i += mStep )
        {
            FOdysseyMathUtils::FOdysseyBezierLutElement prevElement;
            FOdysseyMathUtils::FOdysseyBezierLutElement nextElement;
            for( int j = iLastSelectedLUTIndex; j < LUT.Num() - 1; ++j )
            {
                prevElement = LUT[j];
                nextElement = LUT[j + 1];
                if( i >= prevElement.length && i <= nextElement.length )
                {
                    iLastSelectedLUTIndex = j;
                    break;
                }
            }

            float prevPosParam = prevElement.length / length;
            float nextPosParam = nextElement.length / length;
            float currPosParam = i / length;
            float posParamDelta = nextPosParam - prevPosParam;
            float currPosParamDelta = ( posParamDelta == 0 ) ? 0 : ( currPosParam - prevPosParam ) / posParamDelta;
            FVector2D pos = prevElement.point + ( nextElement.point - prevElement.point ) * currPosParamDelta;
            float propertyParam = ( currPosParam < 0.5 ) ? currPosParam * 2 : ( currPosParam - 0.5 ) * 2;
            FOdysseyStrokePoint point = ( currPosParam < 0.5 ) ?
                mInputPoints[0] + ( mInputPoints[1] - mInputPoints[0] ) * propertyParam :
                mInputPoints[1] + ( mInputPoints[2] - mInputPoints[1] ) * propertyParam;
            point.x = pos.X;
            point.y = pos.Y;
            mResultPoints.Add( point );
            ++drawn_steps;
        }

        mLastDrawnLength += (float)drawn_steps * mStep;
    }

    if( mTotalStrokeLength == 0.f )
    {
        mLastDrawnLength = 0.f;
        mResultPoints.Add( mInputPoints[2] );
    }

    mInputPoints.RemoveAt( 0, 2 );

    return mResultPoints;
}

//---

#undef LOCTEXT_NAMESPACE
