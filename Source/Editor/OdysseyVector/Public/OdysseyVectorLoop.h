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
class ODYSSEYVECTOR_API UOdysseyVectorLoop : public UOdysseyVectorObject
{
    public:
        GENERATED_BODY()

    public:
        static UOdysseyVectorLoop* New( UOdysseyVectorPath* iParent
                                      , uint64 iID
                                      , UOdysseyVectorVertex* iLoopVertex
                                      , std::list<UOdysseyVectorVertex*>& iVertexList
                                      , std::list<FOdysseyVectorSection*>& iSectionList );
        void Init( UOdysseyVectorPath* iParent
                 , uint64 iID
                 , UOdysseyVectorVertex* iLoopVertex
                 , std::list<UOdysseyVectorVertex*>& iVertexList
                 , std::list<FOdysseyVectorSection*>& iSectionList );

    private:
        void UpdateShape();
        UOdysseyVectorObject* CopyShape();

        void BuildSegmentCubic( std::vector<BLPoint>& iPointArray
                              , UOdysseyVectorSegmentCubic& iSegment
                              , double iFromT
                              , double iToT );

    protected :
        uint64 mID;
        UOdysseyVectorVertex* mLoopVertex;
        std::list<UOdysseyVectorVertex*> mVertexList;
        std::list<FOdysseyVectorSection*> mSectionList;

        std::vector<BLPoint> mPointArray;
        BLPath mPath;

    public:
        ~UOdysseyVectorLoop();
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( double iX, double iY, double iRadius );
        void DrawPoints( ::ULIS::FRectD& iRoi );

        void Unselect( UOdysseyVectorVertex* iPoint ) { };
        bool PickPoint( double iX, double iY, double iRadius ) { return false; };
        uint64 GetID();
        static uint64 GenerateID( std::list<FOdysseyVectorSection*> iSectionList );
        void Build();
        void Invalidate();

        void Attach();
        void Detach();
};
