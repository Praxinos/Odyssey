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

class FOdysseyVectorTagInbetweener;
class FInterpolatedPoint;
class FInterpolatedPath;
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
        void Make();

        /**
         * @brief Build the grid.
         * @param iSourcePositionBuffer source position of the points in the grid. Can be empty.
         * @param iTargetPositionBuffer target position of the points in the grid. Can be empty.
         */
        virtual void Make( const std::vector<::ULIS::FVec2D>& iSourcePositionBuffer
                         , const std::vector<::ULIS::FVec2D>& iTargetPositionBuffer );

        /**
         * @brief Deform the paths passed as parameter.
         * @param iInterpolatedPathBuffer deform the paths according to the shape of the grid
         * @param iInbetweenIndex Inbetween index
         */
        virtual void DeformPaths( std::vector<FInterpolatedPath>& iInterpolatedPathBuffer
                                , uint32 iInbetweenIndex
                                , eInbetweenerPointPositionType iPositionType );

        /**
         * @brief Get the quad the coords passed as parameter fit within.
         * @param iLocalCoords coords for testing hits.
         * @return the quad index that hits the coords passed as parameter. -1 if none.
         */
        int GetQuadIndex( const ::ULIS::FVec2D& iLocalCoords );

        /**
         * @brief Map paths to the grid according to the needs of the grid
         * @param iPathBuffer the paths to map
         */
        virtual void MapInterpolatedPaths( std::vector<FInterpolatedPath>& iPathBuffer );

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
                        , eInbetweenerPointPositionType iPositionType );

        FInbetweenerBreakdown* GetBreakdown();
        const ::ULIS::FRectD& GetTargetBBox();
        const ::ULIS::FRectD& GetSourceBBox();

        void UpdateBBox( uint32 iUpdateFlags
                       , uint64 iTagInvalidationFlags );
        void UpdateCenterOfMass( uint32 iUpdateFlags
                               , uint64 iTagInvalidationFlags );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        ::ULIS::FVec2D GetCenterOfMass( eInbetweenerPointPositionType iPositionType );
        ::ULIS::FRectD GetBBox( eInbetweenerPointPositionType iPositionType, bool iLinkedOnly );
        virtual ::ULIS::FVec2D DeformPoint( FInterpolatedPoint* iInterpolatedPoint
                                          , eInbetweenerPointPositionType iPositionType );


    // ARAP interpolation (do not confuse with ARAP deformation)
    // applies to all types of grid.
    protected:
        bool PrecomputeARAPInterpolation();
        bool ComputeARAPInterpolation( FInbetweenerDrawing* iInbetween
                                     , bool useRigidTransform );
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


    protected:
        FInbetweenerBreakdown* mBreakdown;
        std::vector<FInbetweenerPoint> mPointBuffer;
        std::vector<FInbetweenerQuad> mQuadBuffer;
        //std::vector<FInbetweenerQuad*> mQuadArray;
        //uint32 mUsedQuadCount;
        //uint32 mUsedPointCount;

    // ARAP interpolation (do not confuse with ARAP deformation)
    protected:
        uint32 mFlags;
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
};
