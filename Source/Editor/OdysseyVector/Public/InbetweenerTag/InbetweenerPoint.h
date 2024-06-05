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
    MotionPosition = 1,
    TargetPosition = 2
};

class ODYSSEYVECTOR_API FInbetweenerPoint
{
    public:
        typedef Eigen::Matrix<double, 2, 1> VectorType;
        typedef Eigen::Transform<double, 2, Eigen::Affine> Affine;

        virtual ~FInbetweenerPoint(){};
        FInbetweenerPoint( );

        void SetSourcePosition( double iX, double iY );
        void SetTargetPosition( double iX, double iY );
        void SetMotionPosition( double iX, double iY );
        const ::ULIS::FVec2D& GetSourcePosition();
        const ::ULIS::FVec2D& GetTargetPosition();
        void AddQuad( FInbetweenerQuad* iQuad );
        void RemoveQuad( FInbetweenerQuad* iQuad );
        std::list<FInbetweenerQuad*>& GetQuadList();
        void SetID( uint32 iID );
        uint32 GetID();
        ::ULIS::FVec2D GetPosition( eInbetweenerPointPositionType iPositionType );

        friend class FOdysseyVectorTagInbetweener;
        friend class FInbetweenerGridFFD;
        friend class FInbetweenerGrid;

    protected: 
        void Init( FInbetweenerGrid* iGrid );

    protected:
        std::list<FInbetweenerQuad*> mQuadList;
        FInbetweenerGrid* mGrid;
        ::ULIS::FVec2D mSourcePosition;
        ::ULIS::FVec2D mMotionPosition;
        ::ULIS::FVec2D mTargetPosition;
        uint32 mID;
        double u, v;
};
