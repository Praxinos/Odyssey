#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorTag.h"

class FOdysseyVectorPoint;
class FOdysseyVectorSegment;
class FOdysseyVectorSegmentCubic;
class FOdysseyVectorPath;


//////////////// interpolation data structures //////////////////

class FInterpolatedPoint
{
    public:
        virtual ~FInterpolatedPoint();
        FInterpolatedPoint( FOdysseyVectorPoint* iPoint
                          , double iU
                          , double iV
                          , uint32 iPositionCount );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        FOdysseyVectorPoint* mOriginalPoint;
        std::vector<::ULIS::FVec2D> positionBuffer;
        double mU;
        double mV;
};

class FInterpolatedSegment
{
    public:
        virtual ~FInterpolatedSegment();
        FInterpolatedSegment( FOdysseyVectorSegment* iSegment
                            , FInterpolatedPoint* iInterpolatedPoint0
                            , FInterpolatedPoint* iInterpolatedPoint1 );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        FOdysseyVectorSegment* mOriginalSegment;
        FInterpolatedPoint* mInterpolatedVertex[2];
};

class FInterpolatedSegmentCubic : public FInterpolatedSegment
{
    public:
        virtual ~FInterpolatedSegmentCubic();
        FInterpolatedSegmentCubic( FOdysseyVectorSegmentCubic* iCubicSegment
                                 , FInterpolatedPoint* iInterpolatedPoint0
                                 , FInterpolatedPoint* iInterpolatedHandle0
                                 , FInterpolatedPoint* iInterpolatedHandle1
                                 , FInterpolatedPoint* iInterpolatedPoint1 );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        FInterpolatedPoint* mInterpolatedHandle[2];
};

class FInterpolatedPath
{
    public:
        virtual ~FInterpolatedPath();
        FInterpolatedPath( FOdysseyVectorPath* iPath
                         , const ::ULIS::FRectD& iSpaceBBox
                         , const BLMatrix2D& iSpaceInverseMatrix
                         , uint32 iInbetweenCount );
        void Draw( BLContext* iBLContext
                 , const ::ULIS::FRectD& iInvalidationArea
                 , double iAncestorsOpacity
                 , uint64 iDrawingFlags );
        friend class FOdysseyVectorTagInbetweener;

    protected:
        FOdysseyVectorPath* mOriginalPath;
        std::vector<FInterpolatedPoint> mInterpolatedPointBuffer;
        std::vector<FInterpolatedSegmentCubic> mInterpolatedSegmentCubicBuffer;
};

//////////////// Grid data structures //////////////////

struct FInbetweenerInbetween
{
    float spacing;
};

struct FInbetweenerChart
{
    std::vector<FInbetweenerInbetween> inbetweenBuffer;
};

struct FInbetweenerPoint
{
    ::ULIS::FVec2D sourcePosition;
    ::ULIS::FVec2D motionPosition;
    ::ULIS::FVec2D targetPosition;
    double u, v;
};

struct FInbetweenerCell
{
    FInbetweenerPoint* point[4];
};

class ODYSSEYVECTOR_API FOdysseyVectorTagInbetweener : public FOdysseyVectorTag
{
    public:
        virtual ~FOdysseyVectorTagInbetweener();
        FOdysseyVectorTagInbetweener( FOdysseyVectorObject* iOwnerObject
                                    , uint32 iNumCellX
                                    , uint32 iNumCellY
                                    , uint32 iInbetweenCount );
        virtual void Reset() override;
        virtual void Draw( BLContext* iBLContext
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , uint64 iDrawingFlags ) override;
        // when drawn as a shared tag
        virtual void Draw( FOdysseyVectorGroupPaint* iCurrentScene
                         , BLContext* iBLContext
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , uint64 iDrawingFlags ) override;
        void Map();
        void Interpolate();
        void DrawPathsInbetween( uint32 iInbetweenIndex
                               , BLContext* iBLContext );
        std::vector<FInbetweenerCell>& GetGridCellBuffer();
        std::vector<FInbetweenerPoint>& GetGridPointBuffer();
        uint32 GetInbetweenCount();
        FInbetweenerChart& GetChart();
        void MoveInbetween( FInbetweenerInbetween* iInbetween
                          , float iNewSpacing
                          , bool iRelative );

    protected:
        void DrawGrid( BLContext* iBLContext
                     , const ::ULIS::FRectD& iInvalidationArea
                     , double iAncestorsOpacity
                     , uint64 iDrawingFlags );
        void DrawPaths( BLContext* iBLContext
                      , const ::ULIS::FRectD& iInvalidationArea
                      , double iAncestorsOpacity
                      , uint64 iDrawingFlags );


        void FFDComputeBinomialCoefficients();
        void FFDDeformPoint( FInterpolatedPoint* iInterpolatedPoint, uint32 iPositionIndex );
        void FFDDeformPaths( uint32 iPositionIndex );
        void ResetChart();

    protected:
        std::vector<FInterpolatedPath> mInterpolatedPathBuffer;
        std::vector<FInbetweenerPoint> mGridPointBuffer;
        std::vector<double> mUBinomialCoefficientBuffer;
        std::vector<double> mVBinomialCoefficientBuffer;
        std::vector<FInbetweenerCell> mGridCellBuffer;
        FInbetweenerChart mChart;
        uint32 mNumCellX;
        uint32 mNumCellY;
        uint32 mInbetweenCount;
};
