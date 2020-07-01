// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Interpolation/OdysseyInterpolationCatmullRom.h"
#include "OdysseyMathUtils.h"
#include <ULIS3>
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "OdysseyInterpolationCatmullRom"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyInterpolationCatmullRom::~FOdysseyInterpolationCatmullRom()
{
}

FOdysseyInterpolationCatmullRom::FOdysseyInterpolationCatmullRom()
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
bool
FOdysseyInterpolationCatmullRom::IsReady() const
{
    return ( mInputPoints.Num() >= MinimumRequiredPoints() );
}

int
FOdysseyInterpolationCatmullRom::MinimumRequiredPoints() const
{
    return 4;
}

const TArray< FOdysseyStrokePoint >& FOdysseyInterpolationCatmullRom::ComputePoints()
{
    mResultPoints.Empty();

    if( !IsReady() )
        return mResultPoints;

    ::ul3::FVec2F P0( mInputPoints[0].x, mInputPoints[0].y );
    ::ul3::FVec2F P1( mInputPoints[1].x, mInputPoints[1].y );
    ::ul3::FVec2F P2( mInputPoints[2].x, mInputPoints[2].y );
    ::ul3::FVec2F P3( mInputPoints[3].x, mInputPoints[3].y );
    ::ul3::FCatmullRomSpline spline( P0, P1, P2, P3 );
    std::vector< ::ul3::FCatmullRomLUTElement > LUT;
    spline.GenerateLinearLUT( &LUT, mStep );
    float length = LUT.back().length;
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
            ::ul3::FCatmullRomLUTElement prevElement;
            ::ul3::FCatmullRomLUTElement nextElement;
            for( int j = iLastSelectedLUTIndex; j < LUT.size() - 1; ++j )
            {
                prevElement = LUT[j];
                nextElement = LUT[j + 1];
                if( i >= prevElement.length && i <= nextElement.length ) {
                    iLastSelectedLUTIndex = j;
                    break;
                }
            }

            float prevPosParam = prevElement.length / length;
            float nextPosParam = nextElement.length / length;
            float currPosParam = i / length;
            float posParamDelta = nextPosParam - prevPosParam;
            float currPosParamDelta = ( posParamDelta == 0 ) ? 0 : ( currPosParam - prevPosParam ) / posParamDelta;
            ::ul3::FVec2F posU = prevElement.position + ( nextElement.position - prevElement.position ) * currPosParamDelta;
            FVector2D pos( posU.x, posU.y );
            FOdysseyStrokePoint point = mInputPoints[1] + ( mInputPoints[2] - mInputPoints[1] ) * currPosParamDelta;
            point.x = pos.X;
            point.y = pos.Y;
            mResultPoints.Add( point );
            ++drawn_steps;
        }

        mLastDrawnLength += (float)drawn_steps * mStep;
    }

    if( mTotalStrokeLength < 1.f )
    {
        mLastDrawnLength = 0.f;
        mResultPoints.Add( mInputPoints[1] );
    }

    mInputPoints.RemoveAt( 0, 1 );
    return  mResultPoints;
}

//---

#undef LOCTEXT_NAMESPACE
