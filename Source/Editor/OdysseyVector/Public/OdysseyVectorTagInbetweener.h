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
#include "InbetweenerTag/InbetweenerRoute.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
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

struct FInbetweenerDrawing
{
    float spacing;
    float breakdownSpacing; // spacing relative to the current breakdown
    FInbetweenerBreakdown* breakdown;
    BLMatrix2D matrix;
    BLMatrix2D inverseMatrix;
    uint32 index;
};

struct FInbetweenerChart
{
    std::vector<FInbetweenerDrawing> drawingBuffer;
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
                                    , uint32 iDrawingCount );

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
         * @brief Callback called when the tag is added to the object's list of tags
         */
        virtual void Added() override;

        /**
         * @brief Callback called when the tag is removed from the object's list of tags
         */
        virtual void Removed() override;

        /**
         * @brief Get the number of inbetween
         * @return the number of inbetween
         */
        uint32 GetDrawingCount();

        /**
         * @brief Get the spacing chart
         * @return a reference to the spacing chart
         */
        FInbetweenerChart& GetChart();

        /**
         * @brief Get the spacing chart
         * @param iInbetween
         * @param iNewSpacing
         * @param iRelative move all inbetweens relative to the one passed as parameter
         */
        void MoveInbetween( FInbetweenerDrawing* iInbetween
                          , float iNewSpacing
                          , bool iRelative );

        /**
         * @brief Get the rigidity for ARAP deformation
         * @return the rigidity
         */
        uint32 GetARAPRigidity();

        /**
         * @brief Set the rigidity for ARAP deformation
         * @param iRigidity
         */
        void SetARAPRigidity( uint32 iRigidity );

        void RedrawAnimationCells();
        void RedrawAnimationCells( uint32 iInbetweenCount );
        void ResetChart();
        void SetDrawingCount( uint32 iInbetweenCount );
        void SetGrid( eInbetweenerGridType iGridType
                    , uint32 iGridNumQuadX
                    , uint32 iGridNumQuadY );
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
        void DrawPathsInbetween( uint32 iDrawingIndex
                               , BLContext* iBLContext );
        void DrawPathsTarget( BLContext* iBLContext );
        void DeformPathsAtInbetween( uint32 iDrawingIndex );
        void DeformPathsAtTarget();
        std::list<FInbetweenerBreakdown*>& GetBreakdownList();
        FInbetweenerBreakdown* AddBreakdown( uint32 iDrawingIndex, bool iCopyGeometry );
        FInbetweenerBreakdown* AddBreakdown( FInbetweenerBreakdown* iNewBreakdown, uint32 iDrawingIndex, bool iCopyGeometry );
        void DispatchInbetweensToBreakdowns();
        void SetUsedQuadCount( uint32 );
        void SetUsedPointCount( uint32 );
        uint32 GetUsedQuadCount();
        uint32 GetUsedPointCount();
        void DrawPathAt( FInterpolatedPath* iInterpolatedPath
                       , ::ULIS::FVec2D* iPointPositionBuffer
                       , const BLMatrix2D& iWorldMatrix
                       , BLContext* iBLContext );
        uint32 GetBreakdownCount();
        FInbetweenerBreakdown* GetMasterBreakdown();
        void RemoveBreakdown( FInbetweenerBreakdown* iBreakdown, bool iFreeMemNow );
        FInbetweenerBreakdown* GetBreakdown( uint32 iDrawingIndex );
        std::list<FInbetweenerBreakdown*>::iterator GetBreakdownItem( uint32 iDrawingIndex );

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
        void Share();
        void Unshare();
        void ChainBreakdowns();

    public:
        static const uint64 INVALIDATE_MAP            = ( 1LL << 0 );
        static const uint64 INVALIDATE_BUFFERS        = ( 1LL << 1 );
        static const uint64 INVALIDATE_SPACING        = ( 1LL << 2 );
        static const uint64 INVALIDATE_GRIDTYPE       = ( 1LL << 3 );
        static const uint64 INVALIDATE_CELLS          = ( 1LL << 4 );
        static const uint64 INVALIDATE_SOURCEBBOX     = ( 1LL << 5 );
        static const uint64 INVALIDATE_TARGET         = ( 1LL << 6 );
        static const uint64 INVALIDATE_ROUTES         = ( 1LL << 7 );
        static const uint64 INVALIDATE_ROUTE_LIST     = ( 1LL << 8 );
        static const uint64 INVALIDATE_BREAKDOWN_LIST = ( 1LL << 9 );
        static const uint64 INVALIDATE_ALL            = ( INVALIDATE_MAP
                                                        | INVALIDATE_BUFFERS
                                                        | INVALIDATE_SPACING
                                                        | INVALIDATE_GRIDTYPE
                                                        | INVALIDATE_CELLS
                                                        | INVALIDATE_SOURCEBBOX
                                                        | INVALIDATE_TARGET
                                                        | INVALIDATE_ROUTES
                                                        | INVALIDATE_ROUTE_LIST
                                                        | INVALIDATE_BREAKDOWN_LIST );

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
        std::list<FInbetweenerRoute*> mRouteList;
        std::list<FInbetweenerBreakdown*> mBreakdownList;
        eInbetweenerGridType mGridType;
        uint32 mGridNumQuadX;
        uint32 mGridNumQuadY;
        eInbetweenerInterpolationType mInterpolationType;
        ::ULIS::FRectD mSourceBBox;
        ::ULIS::FRectD mTargetBBox;
        FInbetweenerChart mChart;
        uint64 mInvalidationFlags;
        FColor mColor;
        bool bMapAsPolyline;
        bool bShared;
        FInbetweenerBreakdown mMasterBreakdown;
        uint32 mUsedQuadCount;
        uint32 mUsedPointCount;
        uint32 mARAPRigidity;
};
