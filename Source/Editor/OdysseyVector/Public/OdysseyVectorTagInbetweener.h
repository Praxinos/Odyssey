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
#include "InbetweenerTag/InbetweenerChart.h"
#include "InbetweenerTag/InbetweenerDrawing.h"
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerQuad.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "InbetweenerTag/InterpolatedPoint.h"
#include "InbetweenerTag/InterpolatedSegment.h"
#include "InbetweenerTag/InterpolatedPath.h"

#include "OdysseyVectorTagInbetweener.generated.h"

class FOdysseyVectorObject;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorPoint;
class FOdysseyVectorSegment;
class FOdysseyVectorSegmentCubic;
class FOdysseyVectorPath;
class FOdysseyVectorSharedEnv;
class IOdysseyVectorCell;

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

UENUM()
enum class eInbetweenerInterpolationDirection : int8
{
    Forward  = 1,
    Backward = -1
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
                                    , uint32 iNumCellY );

        /**
         * @brief Draw the tag to the Blend2D context passed as parameter
         * @param iBLContext The Blend2D context to draw to
         * @param iInvalidationArea
         * @param iAncestorsOpacity The cumulated opacity from parent objects
         * @param iDrawingFlags drawing flags
         */
        virtual void Draw( BLContext* iBLContext
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , uint64 iDrawingFlags ) override;

        /**
         * @brief Draw the object to the Blend2D context passed as parameter
         *        Note: This is called when the tag is dran as a shared tag. The current scene
         *        may therefore NOT be the top level object
         * FOdysseyVectorGroupPaint* iCurrentScene current scene in which to draw the tag
         * @param iBLContext The Blend2D context to draw to
         * @param iInvalidationArea
         * @param iAncestorsOpacity The cumulated opacity from parent objects
         * @param iDrawingFlags drawing flags
         */
        virtual void Draw( FOdysseyVectorGroupPaint* iCurrentScene
                         , BLContext* iBLContext
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , uint64 iDrawingFlags ) override;

        /**
         * @brief Add a trajectory
         * @param iLocalCoords X and Y coords at which to create the trajectory
         * @return the newly created trajectory or nullptr if coords are outside the grid.
         */
        //FInbetweenerTrajectory* AddTrajectory( const ::ULIS::FVec2D& iLocalCoords );

        /**
         * @brief Add a trajectory
         * @param iTrajectory a pointer to the added trajectory.
         */
        //void AddTrajectory( FInbetweenerTrajectory* iTrajectory );

        /**
         * @brief Remove all trajectories
         */
        //void RemoveAllTrajectories();

        /**
         * @brief Remove a trajectory
         * @param iTrajectory a pointer to the trajectory that must be removed.
         */
        //void RemoveTrajectory( FInbetweenerTrajectory* iTrajectory );

        /**
         * @brief Reset the target grid
         */
        void ResetGrid();

        /**
         * @brief Get the number of inbetween
         * @return the number of inbetween
         */
        uint32 GetLength();

        /**
         * @brief Get the spacing chart
         * @return a reference to the spacing chart
         */
        //FInbetweenerChart& GetChart();

        /**
         * @brief Get the spacing chart
         * @param iInbetween
         * @param iNewSpacing
         * @param iRelative move all inbetweens relative to the one passed as parameter
         */
        void MoveInbetween( FChartDivision* iInbetween
                          , float iNewSpacing
                          , bool iRelative );

        void RedrawCells();
        void RedrawCells( uint32 iInbetweenCount );
        //void ResetChart();
        //void ResizeChart();
        //void SetDrawingCount( uint32 iInbetweenCount );
        void SetGrid( eInbetweenerGridType iGridType
                    , uint32 iGridNumQuadX
                    , uint32 iGridNumQuadY
                    , bool iSquare );
        void SetGridNumQuad( uint32 iNumQuadX, uint32 iNumQuadY, bool iSquare );
        void SetGridNumQuad( uint32 iNumQuadX
                           , uint32 iNumQuadY
                           , bool iSquare
                           , const std::vector<::ULIS::FVec2D>& iSourcePositionBuffer
                           , const std::vector<::ULIS::FVec2D>& iTargetPositionBuffer );

        eInbetweenerInterpolationType GetInterpolationType();
        void SetInterpolationType( eInbetweenerInterpolationType iInterpolationType );

        eInbetweenerGridType GetGridType();
        uint32 GetGridNumQuadX();
        uint32 GetGridNumQuadY();
        void AddRoute( FInbetweenerRoute* iRoute );
        FInbetweenerRoute* AddRoute( const ::ULIS::FVec2D& iLocalCoords );
        std::list<FInbetweenerRoute*>& GetRouteList();
        void RemoveRoute( FInbetweenerRoute* iRoute );
        void RemoveAllRoutes();

        virtual void Update( uint32 iUpdateFlags
                           , uint64 iOwnerInvalidationFlags ) override;
        void Commit( std::list<FOdysseyVectorTag*>& oRemovedTagList
                   , std::list<FOdysseyVectorObject*>& oAddedObjectList
                   , std::list<FOdysseyVectorGroupPaint*>& oCommittedSceneList );
        void Invalidate( uint64 iInvalidationFlags );
        std::vector<FInterpolatedPath>& GetInterpolatedPathBuffer();

        const FColor& GetColor();
        void SetColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        void SetColor( const FColor& iColor );
        void SetMapAsPolyline( bool iMapAsPolyline );
        bool GetMapAsPolyline();
        void DrawPathsInbetween( FChartDivision* inbetween
                               , BLContext* iBLContext
                               , bool iLock );
        void DrawPathsTarget( BLContext* iBLContext );

        void DeformPathsAtTarget();
        void DeformPathsAtSource();

        std::list<FInbetweenerBreakdown*>& GetBreakdownList();
        FInbetweenerBreakdown* AddBreakdown( uint32 iDrawingIndex, bool iCopyGeometry );
        FInbetweenerBreakdown* AddBreakdown( FInbetweenerBreakdown* iNewBreakdown, uint32 iDrawingIndex, bool iCopyGeometry );

        void SetUsedQuadCount( uint32 );
        void SetUsedPointCount( uint32 );
        uint32 GetUsedQuadCount();
        uint32 GetUsedPointCount();
        void DrawPathAt( FInterpolatedPath* iInterpolatedPath
                       , ::ULIS::FVec2D* iPointPositionBuffer
                       , const BLMatrix2D& iWorldMatrix
                       , BLContext* iBLContext
                       , bool iLock );
        uint32 GetBreakdownCount();
        FInbetweenerBreakdown* GetMasterBreakdown();
        void RemoveBreakdown( FInbetweenerBreakdown* iBreakdown, bool iFreeMemNow );
        FInbetweenerBreakdown* GetBreakdown( uint32 iDrawingIndex, bool iStrict );
        std::list<FInbetweenerBreakdown*>::iterator GetBreakdownItem( uint32 iDrawingIndex, bool iStrict  );
        void ResizeRoutes();
        //void SetChart( const FInbetweenerChart& iChart );
        void ResetLayout( bool iFreeMemNow );
        FInbetweenerDrawing* GetDrawing( uint32 iIndex );
        virtual void UpdateMatrix() override;
        virtual void ObjectAdded() override;
        virtual void ObjectRemoved() override;
        virtual void Added() override;
        virtual void Removed() override;
        IOdysseyVectorCell* GetCell();
        void SetInterpolationDirection( eInbetweenerInterpolationDirection iDirection );
        eInbetweenerInterpolationDirection GetInterpolationDirection();
        int32 GetDrawingIndexFromCellIndex( uint32 iCellIndex );
        void InvertInterpolationDirection();
        std::vector<FInbetweenerDrawing>& GetDrawingBuffer();
        void ResizeDrawings();
        FInbetweenerBreakdown* GetBreakdownByTargetIndex( uint32 iDrawingIndex );
        bool IsTopSelectedTag();
        bool IsSquare();
        FInbetweenerBreakdown* GetBreakdownByCellIndex( uint32 iCellIndex );
        const FColor& GetChartColor();
        void SetChartColor( const FColor& iChartColor );
        const FColor& GetGridColor();
        void SetGridColor( const FColor& iGridColor );
        int32 GetSourceCellIndex();
        int32 GetTargetCellIndex();
        std::vector<uint32>& GetUsedQuadIndexBuffer();
        std::vector<uint32>& GetUsedPointIndexBuffer();

    protected:
        /**
         * @brief Map paths to the grid
         */
        void Map();

        /**
         * @brief Compute interpolations for all the mapped paths
         */
        void Interpolate();

        void DrawMotionGrid( uint32 iDrawingIndex
                           , BLContext* iBLContext
                           , const ::ULIS::FRectD& iInvalidationArea
                           , double iAncestorsOpacity
                           , uint64 iDrawingFlags );

        void UpdateBBox( ::ULIS::FRectD& iBBox
                       , eInbetweenerPointPositionType iPositionType );
        void InterpolateGeometry( uint32 iDrawingIndex );
        void InterpolateTransform( uint32 iDrawingIndex );
        void Reset( bool iResetGridShape );
        void AllocBuffers();

        void MoveWaypointsWithInbetween( uint32 iDrawingIndex
                                       , double iInbetweenOldSpacing
                                       , double iInbetweenNewSpacing );
        void ChainBreakdowns();
        void DeformGridAtInbetween( FChartDivision *iInbetween );
        void DeformPathsAtInbetween( FChartDivision *iInbetween );
        void DispatchDrawings();


    public:
        static const uint8 DEFAULT_RED_UINT8   = 255;
        static const uint8 DEFAULT_GREEN_UINT8 = 0;
        static const uint8 DEFAULT_BLUE_UINT8  = 255;
        static const uint8 DEFAULT_ALPHA_UINT8 = 255;
        static const uint8 CHART_DEFAULT_RED_UINT8   = 64;
        static const uint8 CHART_DEFAULT_GREEN_UINT8 = 64;
        static const uint8 CHART_DEFAULT_BLUE_UINT8  = 64;
        static const uint8 CHART_DEFAULT_ALPHA_UINT8 = 255;
        static const uint8 GRID_DEFAULT_RED_UINT8   = 255;
        static const uint8 GRID_DEFAULT_GREEN_UINT8 = 0;
        static const uint8 GRID_DEFAULT_BLUE_UINT8  = 0;
        static const uint8 GRID_DEFAULT_ALPHA_UINT8 = 255;

        static const uint64 INVALIDATE_MAP               = ( 1LL <<  0 );
        static const uint64 INVALIDATE_RANGE             = ( 1LL <<  1 );
        static const uint64 INVALIDATE_BUFFERS           = ( 1LL <<  2 );
        static const uint64 INVALIDATE_SPACING           = ( 1LL <<  3 );
        static const uint64 INVALIDATE_GRIDTYPE          = ( 1LL <<  4 );
        static const uint64 INVALIDATE_INTERPOLATIONTYPE = ( 1LL <<  5 );
        static const uint64 INVALIDATE_CELLS             = ( 1LL <<  6 );
        static const uint64 INVALIDATE_SOURCEGRID        = ( 1LL <<  7 );
        static const uint64 INVALIDATE_TARGETGRID        = ( 1LL <<  8 );
        static const uint64 INVALIDATE_ROUTES            = ( 1LL <<  9 );
        static const uint64 INVALIDATE_ROUTE_LIST        = ( 1LL << 10 );
        static const uint64 INVALIDATE_BREAKDOWN_LIST    = ( 1LL << 11 );
        static const uint64 INVALIDATE_ALL               = ( INVALIDATE_MAP
                                                           | INVALIDATE_RANGE
                                                           | INVALIDATE_BUFFERS
                                                           | INVALIDATE_SPACING
                                                           | INVALIDATE_GRIDTYPE
                                                           | INVALIDATE_INTERPOLATIONTYPE
                                                           | INVALIDATE_CELLS
                                                           | INVALIDATE_SOURCEGRID
                                                           | INVALIDATE_TARGETGRID
                                                           | INVALIDATE_ROUTES
                                                           | INVALIDATE_ROUTE_LIST
                                                           | INVALIDATE_BREAKDOWN_LIST );

    protected:
        FOdysseyVectorGroupPaint* mScene;
        FOdysseyVectorSharedEnv* mSharedEnv;
        //FOdysseyVectorSharedEnv* mSharedEnv;
        std::vector<FInterpolatedPath> mInterpolatedPathBuffer;
        std::list<FInbetweenerRoute*> mRouteList;
        std::list<FInbetweenerBreakdown*> mBreakdownList;
        eInbetweenerGridType mGridType;
        uint32 mGridNumQuadX;
        uint32 mGridNumQuadY;
        eInbetweenerInterpolationType mInterpolationType;
        std::vector<FInbetweenerDrawing> mDrawingBuffer;
        uint64 mInvalidationFlags;
        FColor mColor;
        bool bMapAsPolyline;
        FInbetweenerBreakdown mMasterBreakdown;
        uint32 mUsedQuadCount;
        uint32 mUsedPointCount;
        bool bARAPPrecomputeSucceded;
        eInbetweenerInterpolationDirection mInterpolationDirection;
        bool bSquare;
        FColor mChartColor;
        FColor mGridColor;
        // arrays for accessing only useful grid quads (for faster processing or ARAP interpolation)
        std::vector<uint32> mUsedQuadIndexBuffer;
        // arrays for accessing only useful grid points (for faster processing or ARAP interpolation)
        std::vector<uint32> mUsedPointIndexBuffer;
};
