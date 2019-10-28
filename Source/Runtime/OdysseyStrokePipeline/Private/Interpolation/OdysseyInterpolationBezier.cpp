// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "Interpolation/OdysseyInterpolationBezier.h"
#include "OdysseyMathUtils.h"


#define LOCTEXT_NAMESPACE "OdysseyInterpolationBezier"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyInterpolationBezier::FOdysseyInterpolationBezier()
{
}


FOdysseyInterpolationBezier::~FOdysseyInterpolationBezier()
{
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
bool
FOdysseyInterpolationBezier::IsReady()  const
{
    return ( inputPoints.Num() >= MinimumRequiredPoints() );
}


int
FOdysseyInterpolationBezier::MinimumRequiredPoints()  const
{
    return  3;
}


const TArray< FOdysseyStrokePoint >& FOdysseyInterpolationBezier::ComputePoints()
{
    resultPoints.Empty();

    if( !IsReady() )
        return  resultPoints;

    inputPoints[2] = ( inputPoints[1] + inputPoints[2] ) / 2;
    TArray< FOdysseyMathUtils::FOdysseyBezierLutElement > LUT;
    FVector2D A( inputPoints[0].x, inputPoints[0].y );
    FVector2D B( inputPoints[1].x, inputPoints[1].y );
    FVector2D C( inputPoints[2].x, inputPoints[2].y );
    float length = FOdysseyMathUtils::QuadraticBezierGenerateLinearLUT( &LUT, A, B, C, step );
    float  previousStrokeLength = fTotalStrokeLength;
    fTotalStrokeLength += length;

    float remaining = fTotalStrokeLength - fLastDrawnLength;
    float delta = remaining - step;
    float next = length - delta;

    int     iLastSelectedLUTIndex = 0;
    int     drawn_steps = 0;

    bool point_in_substroke = next > 0;

    if( point_in_substroke )
    {
        for( float i = next; i <= length; i+= step )
        {
            FOdysseyMathUtils::FOdysseyBezierLutElement  prevElement;
            FOdysseyMathUtils::FOdysseyBezierLutElement  nextElement;
            for( int j = iLastSelectedLUTIndex; j < LUT.Num()-1; ++j )
            {
                prevElement     = LUT[j];
                nextElement     = LUT[j+1];
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
            float currPosParamDelta = posParamDelta == 0 ? 0 : ( currPosParam - prevPosParam ) / posParamDelta;
            FVector2D pos = prevElement.point + ( nextElement.point - prevElement.point ) * currPosParamDelta;
            float propertyParam = currPosParam < 0.5 ? currPosParam * 2 : ( currPosParam - 0.5 ) * 2;
            FOdysseyStrokePoint point = currPosParam < 0.5 ?
                                        inputPoints[0] + ( inputPoints[1] - inputPoints[0] ) * propertyParam :
                                        inputPoints[1] + ( inputPoints[2] - inputPoints[1] ) * propertyParam ;
            point.x = pos.X;
            point.y = pos.Y;
            resultPoints.Add( point );
            ++drawn_steps;
        }

        fLastDrawnLength += (float)drawn_steps * step;
    }

    if( fTotalStrokeLength == 0.f )
    {
        fLastDrawnLength = 0.f;
        resultPoints.Add( inputPoints[2] );
    }

    inputPoints.RemoveAt( 0, 2 );
    return resultPoints;
}


#undef LOCTEXT_NAMESPACE
