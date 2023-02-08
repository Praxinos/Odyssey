#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorVertexCubic.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorLoop.h"

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

        void DrawJoint( UOdysseyVectorSegmentCubic* iPrevSegment
                      , UOdysseyVectorSegmentCubic& iSegment
                      , double iRadius );

        uint32 mJointType;

    protected:
        UOdysseyVectorObject* CopyShape();
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags );

    public:
        UOdysseyVectorPathCubic();
        void Init( std::string iName );
        UOdysseyVectorSegmentCubic* AppendVertex( UOdysseyVectorVertexCubic* iPoint, bool iConnect, bool iBuildSegments );

        bool PickPoint ( double iX, double iY, double iRadius, uint64 iSelectionFlags );
        void Unselect( UOdysseyVectorVertex* iPoint );

        void DrawStructure( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void setJointRadial();
        void setJointLinear();
        void setJointMiter();
        void setJointNone();
        void Fill( ::ULIS::FRectD& iRoi );
        void Merge( UOdysseyVectorPathCubic& iCubicPath );
        void DrawShapeVariable( ::ULIS::FRectD& iRoi, uint64 iFlags );

        void Mirror( bool iMirrorX, bool iMirrorY );
        void Cut( ::ULIS::FVec2D& linePoint0, ::ULIS::FVec2D& linePoint1 );
        void SwitchSpace( UOdysseyVectorObject& iObject );
        void Erase( ::ULIS::FRectD &iRoi );

        uint32 GetType();
};
