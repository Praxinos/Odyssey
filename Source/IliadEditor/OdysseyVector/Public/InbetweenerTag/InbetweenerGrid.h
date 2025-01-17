// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include <set>
#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

// MACRO check exists in Unreal and conflicts with another one defined in Eigen. We temporarily undefine it.
#pragma push_macro("check")
#undef check

#include <Eigen/Geometry>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>

#pragma pop_macro("check")

#include "InbetweenerTrajectory.h"
#include "InbetweenerPoint.h"
#include "InbetweenerQuad.h"
#include "InbetweenerChart.h"

class FOdysseyVectorTagInbetweener;
class FInterpolatedPoint;
class FInterpolatedPath;
class FInterpolatedObject;
struct FInbetweenerDrawing;
class FInbetweenerBreakdown;

typedef Eigen::Triplet<double> TripletD;

class ODYSSEYVECTOR_API FInbetweenerGrid
{
    public:
        virtual ~FInbetweenerGrid();
        FInbetweenerGrid( FInbetweenerBreakdown* iBreakdown );

        /**
         * @brief Build the grid.
         */
        void Make( bool iInvalidate );

        /**
         * @brief Build the grid.
         * @param iSourcePositionBuffer source position of the points in the grid. Can be empty.
         * @param iTargetPositionBuffer target position of the points in the grid. Can be empty.
         */
        virtual void Make( const std::vector<::ULIS::FVec2D>& iTargetPositionBuffer
                         , bool iInvalidate );

        /**
         * @brief Deform the paths passed as parameter.
         * @param iInbetweenIndex Inbetween index
         */
        void DeformObjects( FInbetweenerChart::Inbetween *iInbetween
                          , eInbetweenerPointPositionType iPositionType );

        void DeformPoints( FInterpolatedObject* iInterpolatedObject
                         , FInbetweenerChart::Inbetween *iInbetween
                         , eInbetweenerPointPositionType iPositionType );

        /**
         * @brief Get the quad the coords passed as parameter fit within.
         * @param iLocalCoords coords for testing hits.
         * @return the quad index that hits the coords passed as parameter. -1 if none.
         */
        int GetQuadIndex( const ::ULIS::FVec2D& iLocalCoords );

        /**
         * @brief Map paths to the grid according to the needs of the grid
         */
        virtual void MapInterpolatedObjects();

        /**
         * @brief Get the quad buffer
         * @return a reference to the quad buffer
         */
        std::vector<FInbetweenerQuad>& GetQuadBuffer();

        /**
         * @brief Get the point buffer
         * @return a reference to the point buffer
         */
        std::vector<FInbetweenerPoint>& GetPointBuffer();

        /**
         * @brief Get the grid's geometry
         * @param oGeometry returned positions
         * @param iPositionType position type (source, target, interp, deform)
         */
        void GetGeometry( std::vector<::ULIS::FVec2D>& oGeometry
                        , eInbetweenerPointPositionType iPositionType );

        /**
         * @brief Set the grid's geometry
         * @param oGeometry positions to set
         * @param iPositionType position type (source, target, interp, deform)
         */
        void SetGeometry( const std::vector<::ULIS::FVec2D>& iGeometry
                        , eInbetweenerPointPositionType iPositionType
                        , bool iInvalidate );

        FInbetweenerBreakdown* GetBreakdown();
        const ::ULIS::FRectD& GetTargetBBox();
        const ::ULIS::FRectD& GetSourceBBox();

        void UpdateBBox( uint32 iUpdateFlags
                       , uint64 iTagInvalidationFlags );
        void UpdateCenterOfMass( uint32 iUpdateFlags
                               , uint64 iTagInvalidationFlags );
        void ResetDeformation( bool iInvalidate );

        bool ComputeARAPInterpolation( FInbetweenerChart::Inbetween* iInbetween
                                     , bool useRigidTransform );
        bool PrecomputeARAPInterpolation();
        void Invalidate( uint32 iInvalidationFlags );

    protected:
        ::ULIS::FVec2D GetCenterOfMass( eInbetweenerPointPositionType iPositionType );
        ::ULIS::FRectD GetBBox( eInbetweenerPointPositionType iPositionType );
        virtual ::ULIS::FVec2D DeformPoint( FInterpolatedPoint* iInterpolatedPoint
                                          , eInbetweenerPointPositionType iPositionType );
        static void SquareBBox( ::ULIS::FRectD& iBBox );


    // ARAP interpolation (do not confuse with ARAP deformation)
    // applies to all types of grid.
    protected:
        void ComputePStar( FInbetweenerPoint* iTriangle[3]
                         , int triRow
                         , eInbetweenerPointPositionType iPositionType
                         , std::vector<TripletD>& PTriplets );
        void ComputeJAM( FInbetweenerPoint* iTriangle[3]
                       , bool iInverseOrientation
                       , Eigen::Matrix2d& iA );
        void ComputeQuadA( FInbetweenerQuad* iQuad
                         , Eigen::MatrixXd& iAt
                         , int &i
                         , float iT
                         , bool iInverseOrientation );
        double PolarDecomp( Eigen::Matrix2d &A, Eigen::Matrix2d &S );
        void GetValidRouteArray( std::vector<FInbetweenerRoute*>& oValidRouteArray );

    public:
        static const uint32 INVALIDATE_SOURCECENTEROFMASS = ( 1LL <<  0 );
        static const uint32 INVALIDATE_TARGETCENTEROFMASS = ( 1LL <<  1 );
        static const uint32 INVALIDATE_SOURCEBBOX         = ( 1LL <<  2 );
        static const uint32 INVALIDATE_TARGETBBOX         = ( 1LL <<  3 );
        static const uint32 INVALIDATE_SOURCE             = ( INVALIDATE_SOURCECENTEROFMASS | INVALIDATE_SOURCEBBOX );
        static const uint32 INVALIDATE_TARGET             = ( INVALIDATE_TARGETCENTEROFMASS | INVALIDATE_TARGETBBOX );

    protected:
        FInbetweenerBreakdown* mBreakdown;
        std::vector<FInbetweenerPoint> mPointBuffer;
        std::vector<FInbetweenerQuad> mQuadBuffer;

    // ARAP interpolation (do not confuse with ARAP deformation)
    protected:
        uint32 mFlags;
        uint32 mInvalidationFlags;
        // center of mass of the lattice in its reference and target positions
        ::ULIS::FVec2D mSourceCenterOfMass;
        ::ULIS::FVec2D mTargetCenterOfMass;
        // Matrices for ARAP interpolation
        Eigen::SparseMatrix<double, Eigen::ColMajor> mPt;
        Eigen::SparseLU<Eigen::SparseMatrix<double, Eigen::ColMajor>, Eigen::COLAMDOrdering<int>> mLU;
        Eigen::VectorXd mW;
        double mQuadArea;
        ::ULIS::FRectD mSourceBBox;
        ::ULIS::FRectD mTargetBBox;
        ::ULIS::FRectD mGridBBox;
};
