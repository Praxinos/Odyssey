#pragma once

#include "CoreMinimal.h"

#include <set>
#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorTag.h"

#include "InbetweenerTag/InbetweenerGrid.h"
#include "InbetweenerTag/InbetweenerGridFFD.h"
#include "InbetweenerTag/InbetweenerGridARAP.h"
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerQuad.h"
#include "InbetweenerTag/InterpolatedPoint.h"
#include "InbetweenerTag/InterpolatedSegment.h"
#include "InbetweenerTag/InterpolatedPath.h"

#include "OdysseyVectorTagInbetweener.generated.h"

class FOdysseyVectorObject;
class FOdysseyVectorPoint;
class FOdysseyVectorSegment;
class FOdysseyVectorSegmentCubic;
class FOdysseyVectorPath;
class FOdysseyVectorSharedEnv;

UENUM()
enum class eInbetweenerGridType : uint8
{
    FFD = 0,
    ARAP = 1
};

UENUM()
enum class eInbetweenerInterpolationType : uint8
{
    Linear = 0,
    ARAP = 1
};

//////////////// Grid data structures //////////////////

struct FInbetweenerInbetween
{
    float spacing;
    BLMatrix2D matrix;
    BLMatrix2D inverseMatrix;
};

struct FInbetweenerChart
{
    std::vector<FInbetweenerInbetween> inbetweenBuffer;
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
        void DrawMotionGrid( uint32 iInbetweenIndex
                           , BLContext* iBLContext
                           , const ::ULIS::FRectD& iInvalidationArea
                           , double iAncestorsOpacity
                           , uint64 iDrawingFlags );
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
        void SetGridNumQuad( uint32 iNumQuadX
                           , uint32 iNumQuadY
                           , const std::vector<::ULIS::FVec2D>& iSourcePositionBuffer
                           , const std::vector<::ULIS::FVec2D>& iTargetPositionBuffer );

        eInbetweenerInterpolationType GetInterpolationType();
        void SetInterpolationType( eInbetweenerInterpolationType iInterpolationType );

        eInbetweenerGridType GetGridType();
        uint32 GetGridNumQuadX();
        uint32 GetGridNumQuadY();
        std::vector<FInbetweenerPoint>& GetGridPointBuffer();
        std::vector<FInbetweenerQuad>& GetGridQuadBuffer();

        virtual void Update( uint32 iUpdateFlags
                           , uint64 iOwnerInvalidationFlags ) override;
        void Commit( std::list<FOdysseyVectorTag*>& oRemovedTagList
                   , std::list<FOdysseyVectorObject*>& oAddedObjectList );
        virtual void UpdateMatrix() override;
        void Translate( double iX, double iY );
        void Rotate( double iAngle );
        void Scale( double iX, double iY );
        double GetTargetTranslationX();
        double GetTargetTranslationY();
        double GetTargetRotation();
        double GetTargetScalingX();
        double GetTargetScalingY();
        ::ULIS::FRectD GetSourceBBox( bool iWorld );
        ::ULIS::FRectD GetTargetBBox( bool iWorld );
        BLMatrix2D& GetTargetLocalMatrix();
        BLMatrix2D& GetTargetWorldMatrix();
        BLMatrix2D& GetTargetInverseWorldMatrix();
        void Invalidate( uint64 iInvalidationFlags );
        FInbetweenerGrid* GetGrid();
        std::vector<FInterpolatedPath>& GetInterpolatedPathBuffer();
        void GetTargetTransform( double& oTranslationX
                               , double& oTranslationY
                               , double& oRotation
                               , double& oScalingX
                               , double& oScalingY );
        void SetTargetTransform( double iTranslationX
                               , double iTranslationY
                               , double iRotation
                               , double iScalingX
                               , double iScalingY );
        const FColor& GetColor();
        void SetColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        void SetColor( const FColor& iColor );
        void SetMapAsPolyline( bool iMapAsPolyline );
        bool GetMapAsPolyline();

    protected:
        void UpdateAnimationCells( uint32 iInbetweenCount );
        void UpdateBBox( ::ULIS::FRectD& iBBox
                       , eInbetweenerPointPositionType iPositionType );
        void InterpolateGeometry( uint32 iInbetweenIndex );
        void InterpolateTransform( uint32 iInbetweenIndex );
        void Reset( bool iResetGridShape );
        void AllocBuffers();
        void DrawPathAt( FInterpolatedPath* iInterpolatedPath
                       , ::ULIS::FVec2D* iPointPositionBuffer
                       , const BLMatrix2D& iWorldMatrix
                       , BLContext* iBLContext );


    public:
        static const uint64 INVALIDATE_MAP          = ( 1LL << 0 );
        static const uint64 INVALIDATE_BUFFERS      = ( 1LL << 1 );
        static const uint64 INVALIDATE_SPACING      = ( 1LL << 2 );
        static const uint64 INVALIDATE_CELLS        = ( 1LL << 3 );
        static const uint64 INVALIDATE_SOURCEBBOX   = ( 1LL << 4 );
        static const uint64 INVALIDATE_TARGETBBOX   = ( 1LL << 5 );
        static const uint64 INVALIDATE_TRAJECTORIES = ( 1LL << 6 );
        static const uint64 INVALIDATE_ALL          = ( INVALIDATE_MAP
                                                      | INVALIDATE_BUFFERS
                                                      | INVALIDATE_SPACING
                                                      | INVALIDATE_CELLS
                                                      | INVALIDATE_SOURCEBBOX
                                                      | INVALIDATE_TARGETBBOX
                                                      | INVALIDATE_TRAJECTORIES );

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
        eInbetweenerInterpolationType mInterpolationType;
        ::ULIS::FRectD mSourceBBox;
        ::ULIS::FRectD mTargetBBox;
        FInbetweenerChart mChart;
        uint32 mInbetweenCount;
        uint64 mInvalidationFlags;
        FColor mColor;
        bool bMapAsPolyline;
};
