#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPointCubic.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"

#include "OdysseyVectorPathCubic.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorPathCubic: public UOdysseyVectorPath
{
    public:
        GENERATED_BODY()

    private:
        static const uint32 JOINT_TYPE_NONE   = 0;
        static const uint32 JOINT_TYPE_RADIAL = 1;
        static const uint32 JOINT_TYPE_LINEAR = 2;
        static const uint32 JOINT_TYPE_MITER  = 3;

        void DrawJoint( FOdysseyVectorSegmentCubic* iPrevSegment
                      , FOdysseyVectorSegmentCubic& iSegment
                      , double iRadius );

        uint32 mJointType;

    protected:
        UOdysseyVectorObject* CopyShape();
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( double iX, double iY, double iRadius );

    public:
        UOdysseyVectorPathCubic();
        void Init( std::string iName );
        FOdysseyVectorSegmentCubic* AppendPoint( FOdysseyVectorPointCubic* iPoint, bool iConnect, bool iBuildSegments );

        bool PickPoint ( double iX, double iY, double iRadius, uint64 iSelectionFlags );
        void Unselect( FOdysseyVectorPoint* iPoint );

        void DrawStructure( ::ULIS::FRectD& iRoi );
        void setJointRadial();
        void setJointLinear();
        void setJointMiter();
        void setJointNone();
        void Fill( ::ULIS::FRectD& iRoi );
        void Merge( UOdysseyVectorPathCubic& iCubicPath );
        void DrawShapeVariable( ::ULIS::FRectD& iRoi, uint64 iFlags );

        void Mirror( bool iMirrorX, bool iMirrorY );
        void Cut( ::ULIS::FVec2D& linePoint0, ::ULIS::FVec2D& linePoint1 );
};
