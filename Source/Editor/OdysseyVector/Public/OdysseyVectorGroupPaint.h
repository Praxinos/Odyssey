#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroup.h"

#include "OdysseyVectorGroupPaint.generated.h"

class FOdysseyVectorBucket
{
    public:
        ::ULIS::FVec2D mPosition;
        uint32_t mColor;
};

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorGroupPaint : public UOdysseyVectorGroup
{
    public:
        GENERATED_BODY()

        std::list<FOdysseyVectorBucket*> mBucketList;

    public:
        ~UOdysseyVectorGroupPaint(){};
        UOdysseyVectorGroupPaint(){};
        void Init( std::string iName );

        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags );
        UOdysseyVectorObject* CopyShape();

        uint32 IntersectSegment( UOdysseyVectorSegmentCubic& iCubicSegment
                               , std::list<UOdysseyVectorSegment*>& cubicSegmenList
                              , std::list<UOdysseyVectorVertexIntersection*>& intersectionVertexList );
        void BuildGraph( std::list<UOdysseyVectorVertexIntersection*>& intersectionVertexList );
        void FindCycles();
        void SimplifyGraph( std::list<FOdysseyVectorSection*>& sectionList );
};
