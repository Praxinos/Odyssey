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
class FInbetweenerGridQuad;

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
        friend class FInbetweenerGridFFD;
        friend class FInbetweenerGrid;

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
        std::vector<FInterpolatedPoint>& GetInterpolatedPointBuffer();
        std::vector<::ULIS::FVec2D>& GetInterpolatedPointPositionBuffer();

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
        void AddQuad( FInbetweenerGridQuad* iQuad );
        void RemoveQuad( FInbetweenerGridQuad* iQuad );

        friend class FOdysseyVectorTagInbetweener;
        friend class FInbetweenerGridFFD;
        friend class FInbetweenerGrid;

    protected: 
        void Init( FInbetweenerGrid* iGrid );

    protected:
        std::list<FInbetweenerGridQuad*> mQuadList;
        FInbetweenerGrid* mGrid;
        ::ULIS::FVec2D mSourcePosition;
        ::ULIS::FVec2D mMotionPosition;
        ::ULIS::FVec2D mTargetPosition;
        double u, v;
};

class ODYSSEYVECTOR_API FInbetweenerGridQuad
{
    public:
        virtual ~FInbetweenerGridQuad(){};
        FInbetweenerGridQuad( );

        FInbetweenerGridPoint** GetPoints();
        void Link();
        void Unlink();
        bool IsLinked();

        friend class FOdysseyVectorTagInbetweener;

    public:
        static const uint32 LINKED = 1L << 0;

    protected:
        uint32 mFlags;
        FInbetweenerGridPoint* mPoint[4];
};

class FInbetweenerGrid
{
    public:
        virtual ~FInbetweenerGrid(){};
        FInbetweenerGrid( FOdysseyVectorTagInbetweener* iInbetweenerTag );

        virtual void Make( uint32 iNumQuadX
                         , uint32 iNumQuadY );

        virtual void DeformPaths( std::vector<FInterpolatedPath>& iInterpolatedPathBuffer
                                , uint32 iInbetweenIndex );
        FOdysseyVectorTagInbetweener* GetInbetweenerTag();

        friend class FOdysseyVectorTagInbetweener;

    protected:
        std::vector<FInbetweenerGridQuad>& GetQuadBuffer();
        std::vector<FInbetweenerGridPoint>& GetPointBuffer();
        uint32 GetNumQuadX();
        uint32 GetNumQuadY();

    protected:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        std::vector<FInbetweenerGridPoint> mPointBuffer;
        std::vector<FInbetweenerGridQuad> mQuadBuffer;
        uint32 mNumQuadX;
        uint32 mNumQuadY;
};

class FInbetweenerGridFFD : public FInbetweenerGrid
{
    public:
        virtual ~FInbetweenerGridFFD(){};
        FInbetweenerGridFFD( FOdysseyVectorTagInbetweener* iInbetweenerTag );

        virtual void Make( uint32 iNumQuadX
                         , uint32 iNumQuadY  ) override;
        virtual void DeformPaths( std::vector<FInterpolatedPath>& iInterpolatedPathBuffer
                                , uint32 iInbetweenIndex ) override;
        void ComputeBinomialCoefficients();
        ::ULIS::FVec2D DeformPoint( FInterpolatedPoint* iInterpolatedPoint );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        std::vector<double> mUBinomialCoefficientBuffer;
        std::vector<double> mVBinomialCoefficientBuffer;
};

class FInbetweenerGridARAP : public FInbetweenerGrid
{
    public:
        virtual ~FInbetweenerGridARAP(){};
        FInbetweenerGridARAP( FOdysseyVectorTagInbetweener* iInbetweenerTag );

        virtual void Make(  uint32 iNumQuadX
                          , uint32 iNumQuadY ) override;

        friend class FOdysseyVectorTagInbetweener;

    protected:
        int dummy;
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
        uint32 GetInbetweenCount();
        FInbetweenerChart& GetChart();
        void MoveInbetween( FInbetweenerInbetween* iInbetween
                          , float iNewSpacing
                          , bool iRelative );
        void UpdateAnimationCells();
        void ResetChart();
        void SetInbetweenCount( uint32 iInbetweenCount );
        void SetGridType( eInbetweenerGridType iGridType );
        void SetGridNumQuadX( uint32 iNumQuadX );
        void SetGridNumQuadY( uint32 iNumQuadY );
        void SetGridNumQuad( uint32 iNumQuadX, uint32 iNumQuadY );

        eInbetweenerGridType GetGridType();
        uint32 GetGridNumQuadX();
        uint32 GetGridNumQuadY();
        std::vector<FInbetweenerGridPoint>& GetGridPointBuffer();
        std::vector<FInbetweenerGridQuad>& GetGridQuadBuffer();

        virtual void Update( uint32 iUpdateFlags ) override;
        void Commit();
        virtual void UpdateMatrix() override;
        void Translate( double iX, double iY );
        void Rotate( double iAngle );
        void Scale( double iX, double iY );
        double GetTargetTranslationX();
        double GetTargetTranslationY();
        double GetTargetRotation();
        double GetTargetScalingX();
        double GetTargetScalingY();
        ::ULIS::FRectD GetTargetGridBBox( bool iWorld );
        BLMatrix2D& GetTargetWorldMatrix();
        BLMatrix2D& GetTargetInverseWorldMatrix();
        void Invalidate( uint64 iInvalidationFlags );
        FInbetweenerGrid* GetGrid();

    protected:
        void UpdateAnimationCells( uint32 iInbetweenCount );
        void UpdateGridBBox();
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
        FInbetweenerGrid* mGrid;
        eInbetweenerGridType mGridType;
        ::ULIS::FRectD mTargetGridBBox;
        FInbetweenerChart mChart;
        uint32 mInbetweenCount;
        uint64 mInvalidationFlags;
};
