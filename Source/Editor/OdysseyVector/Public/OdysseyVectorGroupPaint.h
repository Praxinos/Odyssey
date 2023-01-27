#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorLoop.h"

#include "OdysseyVectorGroupPaint.generated.h"

typedef struct _FCycleNode
{
    int32 parentID;
    int32 ID;
    std::list<FOdysseyVectorSection*>* bypassEdgeList;
    UOdysseyVectorVertex* vertex;
    FOdysseyVectorSection* section;
    uint32 depth;
} FCycleNode;

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorGroupPaint : public UOdysseyVectorGroup
{
    public:
        GENERATED_BODY()

    public:
        void ApplyBucket( FOdysseyVectorBucket* iBucket );
        void Colorize();

        std::list<FOdysseyVectorBucket*> mBucketList;
        std::list<FOdysseyVectorLoop*> mLoopList;


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

        void BuildGraph( std::list<UOdysseyVectorVertexIntersection*>& intersectionVertexList
                       , std::list<FOdysseyVectorSection*>& iSectionList );

        uint32 MakeCycle( ::ULIS::FVec2D& minCoord
                      , std::vector<UOdysseyVectorVertex*>& iVertexArray
                      , std::vector<FOdysseyVectorSection*>& iSectionArray );
        void March( UOdysseyVectorVertexIntersection* iNode
                  , std::list<FOdysseyVectorSection*>& iSectionList );

        void FindCycles();
        void SimplifyGraph( std::list<FOdysseyVectorSection*>& sectionList );
        FOdysseyVectorBucket* Bucket( uint32 iColor, double iX, double iY );
        FOdysseyVectorBucket* GetBucket( double iX, double iY );

};
