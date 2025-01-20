// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

// MACRO check exists in Unreal and conflicts with another one defined in Eigen. We temporarily undefine it.
#pragma push_macro("check")
#undef check

#include <Eigen/Geometry>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>

#pragma pop_macro("check")

class FInbetweenerQuad;
class FInbetweenerGrid;

enum eInbetweenerPointPositionType
{
    SourcePosition = 0,
    InterpPosition = 1,
    DeformPosition = 2,
    TargetPosition = 3
};

class ODYSSEYVECTOR_API FInbetweenerPoint
{
    public:
        typedef Eigen::Matrix<double, 2, 1> VectorType;
        typedef Eigen::Transform<double, 2, Eigen::Affine> Affine;

        virtual ~FInbetweenerPoint(){};
        FInbetweenerPoint( FInbetweenerGrid* iGrid
                         , double iSourcePositionX
                         , double iSourcePositionY
                         , double iU
                         , double iV );

        void SetTargetPosition( double iX, double iY, bool iInvalidate );
        void SetInterpPosition( double iX, double iY );
        void SetDeformPosition( double iX, double iY );
        ::ULIS::FVec2D GetSourcePosition();
        ::ULIS::FVec2D GetTargetPosition();
        void AddQuad( FInbetweenerQuad* iQuad );
        void RemoveQuad( FInbetweenerQuad* iQuad );
        std::list<FInbetweenerQuad*>& GetQuadList();
        uint32 GetQuadCount();
        void SetID( uint32 iID );
        uint32 GetID();
        ::ULIS::FVec2D GetPosition( eInbetweenerPointPositionType iPositionType );
        void SetPosition( eInbetweenerPointPositionType iPositionType, double iX, double iY, bool iInvalidate );
        void SetDeformable( bool iIsDeformable );
        bool IsDeformable();
        void SetU( double iU );
        void SetV( double iV );
        double GetU();
        double GetV();
        void Init( FInbetweenerGrid* iGrid );
        bool IsNeeded();
        void SetNeeded( bool iNeeded );
        FInbetweenerGrid* GetGrid();
        uint32 GetIndex();

        friend class FOdysseyVectorTagInbetweener;

    public:
        static const int DEFORMABLE = ( 1L << 0 );  // what is this ?
        static const int NEEDED     = ( 1L << 1 );

    protected:
        uint32 mFlags;
        std::list<FInbetweenerQuad*> mQuadList;
        FInbetweenerGrid* mGrid;
        ::ULIS::FVec2D mSourcePosition;
        ::ULIS::FVec2D mInterpPosition;
        ::ULIS::FVec2D mDeformPosition;
        ::ULIS::FVec2D mTargetPosition;
        uint32 mID;
        double mU, mV;
};
