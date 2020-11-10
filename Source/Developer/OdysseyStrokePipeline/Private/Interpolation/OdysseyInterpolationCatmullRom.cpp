// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    //We need 4 points to draw a spline with Catmul-Rom
    return ( mInputPoints.Num() >= 4 );
}

int
FOdysseyInterpolationCatmullRom::MinimumRequiredPoints() const
{
    //The line starts at the second point in Catmul-Rom
    return 2;
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
            //Get prev and next element of the LUT
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

            //Pos of the prev element from 0% to 100% of the whole LUT
            float prevPosParam = prevElement.length / length;

            //Pos of the prev element from 0% to 100% of the whole LUT
            float nextPosParam = nextElement.length / length;

            //Pos of the current element from 0% to 100% of the whole LUT
            float currPosParam = i / length;

            //the length of the section between prev and next element, in percent of the global LUT
            float posParamDelta = nextPosParam - prevPosParam;

            //the amount to lerp between prev and next elements
            float currPosParamDelta = ( posParamDelta == 0 ) ? 0 : ( currPosParam - prevPosParam ) / posParamDelta;

            //The lerp of the position
            ::ul3::FVec2F posU = prevElement.position + ( nextElement.position - prevElement.position ) * currPosParamDelta;
            FVector2D pos( posU.x, posU.y );

            //Lerp the point parameters between input1 and input2 points
            FOdysseyStrokePoint point = FOdysseyStrokePoint::Lerp( mInputPoints[1], mInputPoints[2], currPosParam);
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
