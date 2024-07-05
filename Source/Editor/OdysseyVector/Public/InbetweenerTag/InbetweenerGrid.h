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
struct FInbetweenerInbetween;

typedef Eigen::Triplet<double> TripletD;

class ODYSSEYVECTOR_API FInbetweenerGrid
{
    public:
        virtual ~FInbetweenerGrid();
        FInbetweenerGrid( FOdysseyVectorTagInbetweener* iInbetweenerTag
                        , uint32 iNumQuadX
                        , uint32 iNumQuadY );

        void Make( uint32 iNumQuadX
                 , uint32 iNumQuadY
                 , const ::ULIS::FRectD& iBBox );

        virtual void Make( uint32 iNumQuadX
                         , uint32 iNumQuadY
                         , const ::ULIS::FRectD& iBBox
                         , const std::vector<::ULIS::FVec2D>& iSourcePositionBuffer
                         , const std::vector<::ULIS::FVec2D>& iTargetPositionBuffer );

        virtual void DeformPaths( std::vector<FInterpolatedPath>& iInterpolatedPathBuffer
                                , uint32 iInbetweenIndex );

        FInbetweenerTrajectory* AddTrajectory( const ::ULIS::FVec2D& iLocalCoords );
        void AddTrajectory( FInbetweenerTrajectory* iTrajectory );
        void RemoveTrajectory( FInbetweenerTrajectory* iTrajectory );
        void RemoveAllTrajectories();
        int GetQuadIndex( const ::ULIS::FVec2D& iLocalCoords );

        FOdysseyVectorTagInbetweener* GetInbetweenerTag();
        virtual void Update( uint32 iUpdateFlags
                           , uint64 iTagInvalidationFlags );
        virtual void MapInterpolatedPaths( std::vector<FInterpolatedPath>& iPathBuffer
                                         , const BLMatrix2D& iSpaceInverseMatrix  );
        std::list<FInbetweenerTrajectory*>& GetTrajectoryList();
        std::vector<FInbetweenerQuad>& GetQuadBuffer();
        std::vector<FInbetweenerPoint>& GetPointBuffer();
        uint32 GetNumQuadX();
        uint32 GetNumQuadY();
        void GetGeometry( std::vector<::ULIS::FVec2D>& oGeometry
                        , eInbetweenerPointPositionType iPositionType );
        void SetGeometry( const std::vector<::ULIS::FVec2D>& iGeometry
                        , eInbetweenerPointPositionType iPositionType );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        ::ULIS::FVec2D GetCenterOfMass( eInbetweenerPointPositionType iPositionType );
        virtual ::ULIS::FVec2D DeformPoint( FInterpolatedPoint* iInterpolatedPoint
                                          , const ::ULIS::FRectD& isourceBBox );


    // ARAP interpolation (do not confuse with ARAP deformation)
    // applies to all types of grid.
    protected:
        bool PrecomputeARAPInterpolation();
        bool ComputeARAPInterpolation( //float alphaLinear
                                     //, float alpha
                                     // , const FInbetweenerPoint::Affine &globalRigidTransform
                                       const FInbetweenerInbetween* iInbetween
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
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        std::list<FInbetweenerTrajectory*> mTrajectoryList;
        std::vector<FInbetweenerPoint> mPointBuffer;
        std::vector<FInbetweenerQuad> mQuadBuffer;
        std::vector<FInbetweenerQuad*> mQuadArray;
        uint32 mNumQuadX;
        uint32 mNumQuadY;
        uint32 mUsedQuadCount;
        uint32 mUsedPointCount;

    // ARAP interpolation (do not confuse with ARAP deformation)
    protected:
        // center of mass of the lattice in its reference and target positions
        ::ULIS::FVec2D mSourceCenterOfMass;
        ::ULIS::FVec2D mTargetCenterOfMass;
        // Matrices for ARAP interpolation
        Eigen::SparseMatrix<double, Eigen::ColMajor> mPt;
        Eigen::SparseLU<Eigen::SparseMatrix<double, Eigen::ColMajor>, Eigen::COLAMDOrdering<int>> mLU;
        Eigen::VectorXd mW;
        double mQuadArea;
};
