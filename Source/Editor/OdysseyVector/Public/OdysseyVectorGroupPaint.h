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
    struct _FCycleNode* parent;
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

    private:
        FCycleNode* mNodeMemoryPool;
        void ClearCycles();

    public:
        void ApplyBucket( FOdysseyVectorBucket* iBucket );
        void Colorize();

        std::list<FOdysseyVectorBucket*> mBucketList;
        std::list<FOdysseyVectorLoop*> mLoopList;


    public:
        ~UOdysseyVectorGroupPaint();
        UOdysseyVectorGroupPaint(){};
        void Init( std::string iName );

        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags );
        UOdysseyVectorObject* CopyShape();

        uint32 IntersectSegment( UOdysseyVectorSegmentCubic& iCubicSegment
                               , std::list<UOdysseyVectorSegment*>& cubicSegmenList
                              , std::list<UOdysseyVectorVertexIntersection*>& intersectionVertexList );

        uint32 BuildGraph( std::list<UOdysseyVectorVertexIntersection*>& intersectionVertexList
                         , std::list<FOdysseyVectorSection*>& iSectionList );

        FOdysseyVectorLoop* MakeCycle( ::ULIS::FVec2D& minCoord
                                     , uint64 iCycleID
                                     , std::vector<UOdysseyVectorVertex*>& iVertexArray
                                     , std::vector<FOdysseyVectorSection*>& iSectionArray );
        FOdysseyVectorLoop* March( UOdysseyVectorVertexIntersection* iNode
                                 , FOdysseyVectorSection* iStartSection
                                 , FOdysseyVectorSection* iEndSection
                                 , std::list<FOdysseyVectorSection*>& iSectionList );
        void MarchCycle( FOdysseyVectorLoop& iCycle, std::list<FOdysseyVectorSection*>& iSectionList );

        void FindCycles();
        void SimplifyGraph( std::list<FOdysseyVectorSection*>& sectionList );
        FOdysseyVectorBucket* Bucket( uint32 iColor, double iX, double iY );
        FOdysseyVectorBucket* GetBucket( double iX, double iY );

};
