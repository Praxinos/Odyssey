#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "CoreMinimal.h"

#include "OdysseyVectorObject.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"

#include "OdysseyVectorLoop.generated.h"

UCLASS()
class UOdysseyVectorLoop : public UOdysseyVectorObject
{
    public:
        GENERATED_BODY()

    private:
        void UpdateShape();
        UOdysseyVectorObject* CopyShape();

        void BuildSegmentCubic( std::vector<BLPoint>& iPointArray
                              , FOdysseyVectorSegmentCubic& iSegment
                              , double iFromT
                              , double iToT );

    protected :
        uint64 mID;
        FOdysseyVectorPoint* mLoopPoint;
        std::list<FOdysseyVectorPoint*> mPointList;
        std::list<FOdysseyVectorSection*> mSectionList;

        std::vector<BLPoint> mPointArray;
        BLPath mPath;

    public:
        ~UOdysseyVectorLoop();
        void Init( UOdysseyVectorPath* iParent
                 , uint64 iID
                 , FOdysseyVectorPoint* iLoopPoint
                 , std::list<FOdysseyVectorPoint*>& iPointList
                 , std::list<FOdysseyVectorSection*>& iSectionList );

        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( double iX, double iY, double iRadius );
        void DrawPoints( ::ULIS::FRectD& iRoi );

        void Unselect( FOdysseyVectorPoint* iPoint ) { };
        bool PickPoint( double iX, double iY, double iRadius ) { return false; };
        uint64 GetID();
        static uint64 GenerateID( std::list<FOdysseyVectorSection*> iSectionList );
        void Build();
        void Invalidate();

        void Attach();
        void Detach();
};
