#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorTag.h"

#include "OdysseyVectorTagInbetweener.generated.h"

class FOdysseyVectorPoint;
class FOdysseyVectorSegment;
class FOdysseyVectorSegmentCubic;
class FOdysseyVectorPath;
class FOdysseyVectorSharedEnv;
class FOdysseyVectorTagInbetweener;

UENUM()
enum class eInbetweenerGridType : uint8
{
    FFD = 0,
    ARAP = 1
};

//////////////// interpolation data structures //////////////////

class FInterpolatedPoint
{
    public:
        virtual ~FInterpolatedPoint();
        FInterpolatedPoint( FOdysseyVectorPoint* iPoint
                          , uint32 iIndex
                          , double iU
                          , double iV );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        FOdysseyVectorPoint* mOriginalPoint;
        uint32 mIndex;
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
        // we alloc point position in one single big array.
        std::vector<::ULIS::FVec2D> mInterpolatedPointPositionBuffer;
};

//////////////// Grid data structures //////////////////

struct FInbetweenerInbetween
{
    float spacing;
    BLMatrix2D matrix;
};

struct FInbetweenerChart
{
    std::vector<FInbetweenerInbetween> inbetweenBuffer;
};

class ODYSSEYVECTOR_API FInbetweenerGridPoint
{
    public:
        virtual ~FInbetweenerGridPoint(){};
        FInbetweenerGridPoint( );

        void SetSourcePosition( double iX, double iY );
        void SetTargetPosition( double iX, double iY );
        const ::ULIS::FVec2D& GetSourcePosition();
        const ::ULIS::FVec2D& GetTargetPosition();

        friend class FOdysseyVectorTagInbetweener;

    private: 
        void Init( FOdysseyVectorTagInbetweener* iInbetweenerTag );

    private:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        ::ULIS::FVec2D mSourcePosition;
        ::ULIS::FVec2D mMotionPosition;
        ::ULIS::FVec2D mTargetPosition;
        double u, v;
};

class ODYSSEYVECTOR_API FInbetweenerGridCell
{
    public:
        FInbetweenerGridPoint** GetGridPoints();

        friend class FOdysseyVectorTagInbetweener;

    private:
        FInbetweenerGridPoint* mPoint[4];
};

class ODYSSEYVECTOR_API FOdysseyVectorTagInbetweener : public FOdysseyVectorTag
{
    private:
        static const uint32 mStaticClass =  0x7cf60edf; // value is crc32 FOdysseyVectorTagInbetweener

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

        virtual ~FOdysseyVectorTagInbetweener();
        FOdysseyVectorTagInbetweener( FOdysseyVectorSharedEnv* iSharedEnv
                                    , FOdysseyVectorObject* iOwnerObject
                                    , uint32 iNumCellX
                                    , uint32 iNumCellY
                                    , uint32 iInbetweenCount );
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
        virtual void Added() override;
        virtual void Removed() override;

        void MakeGrid();
        void Map();
        void Interpolate();
        void DrawPathsInbetween( uint32 iInbetweenIndex
                               , BLContext* iBLContext );
        void DrawPathsTarget( BLContext* iBLContext );
        std::vector<FInbetweenerGridCell>& GetGridCellBuffer();
        std::vector<FInbetweenerGridPoint>& GetGridPointBuffer();
        uint32 GetInbetweenCount();
        FInbetweenerChart& GetChart();
        void MoveInbetween( FInbetweenerInbetween* iInbetween
                          , float iNewSpacing
                          , bool iRelative );
        void UpdateAnimationCells();
        void ResetChart();
        void SetInbetweenCount( uint32 iInbetweenCount );
        void SetGridType( eInbetweenerGridType iGridType );
        void SetFFDNumCellX( uint32 iNumCellX );
        void SetFFDNumCellY( uint32 iNumCellY );
        void SetFFDNumCell( uint32 iNumCellX, uint32 iNumCellY );
        eInbetweenerGridType GetGridType();
        uint32 GetFFDNumCellX();
        uint32 GetFFDNumCellY();

        virtual void Update( uint32 iUpdateFlags ) override;
        void Commit();
        virtual void UpdateMatrix() override;
        void Translate( double iX, double iY );
        void Rotate( double iAngle );
        void Scale( double iX, double iY );
        ::ULIS::FRectD GetTargetGridBBox( bool iWorld );
        BLMatrix2D& GetTargetWorldMatrix();
        BLMatrix2D& GetTargetInverseWorldMatrix();
        void Invalidate( uint64 iInvalidationFlags );

    protected:
        void UpdateAnimationCells( uint32 iInbetweenCount );
        void UpdateGridBBox();
        void FFDComputeBinomialCoefficients();
        ::ULIS::FVec2D FFDDeformPoint( FInterpolatedPoint* iInterpolatedPoint );
        void FFDDeformPaths( uint32 iPositionIndex );
        void InterpolateGeometry( uint32 iInbetweenIndex );
        void InterpolateTransform( uint32 iInbetweenIndex );
        void Reset( bool iResetGridShape );
        void AllocBuffers();
        void DrawPathAt( FInterpolatedPath* iInterpolatedPath
                       , ::ULIS::FVec2D* iPointPositionBuffer
                       , const BLMatrix2D& iWorldMatrix
                       , BLContext* iBLContext );


    public:
        static const uint64 INVALIDATE_MAP        = ( 1LL << 0 );
        static const uint64 INVALIDATE_BUFFERS    = ( 1LL << 1 );
        static const uint64 INVALIDATE_SPACING    = ( 1LL << 2 );
        static const uint64 INVALIDATE_CELLS      = ( 1LL << 3 );
        static const uint64 INVALIDATE_BBOX       = ( 1LL << 4 );
        static const uint64 INVALIDATE_ALL        = ( INVALIDATE_MAP
                                                    | INVALIDATE_BUFFERS
                                                    | INVALIDATE_SPACING
                                                    | INVALIDATE_CELLS
                                                    | INVALIDATE_BBOX );

    protected:
        double mTargetTranslationX;
        double mTargetTranslationY;
        double mTargetRotation;
        double mTargetScalingX;
        double mTargetScalingY;
        BLMatrix2D mTargetLocalMatrix;
        BLMatrix2D mTargetWorldMatrix;
        BLMatrix2D mTargetInverseWorldMatrix;
        FOdysseyVectorSharedEnv* mSharedEnv;
        std::vector<FInterpolatedPath> mInterpolatedPathBuffer;
        std::vector<FInbetweenerGridPoint> mGridPointBuffer;
        std::vector<double> mUBinomialCoefficientBuffer;
        std::vector<double> mVBinomialCoefficientBuffer;
        std::vector<FInbetweenerGridCell> mGridCellBuffer;
        eInbetweenerGridType mGridType;
        ::ULIS::FRectD mTargetGridBBox;
        FInbetweenerChart mChart;
        uint32 mNumCellX;
        uint32 mNumCellY;
        uint32 mInbetweenCount;
        uint64 mInvalidationFlags;
};
