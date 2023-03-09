#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorLoop.h"

#include "OdysseyVectorGroupPaint.generated.h"

typedef struct _FCycleNode
{
    struct _FCycleNode* parent;
    std::list<FOdysseyVectorSection*>* bypassEdgeList;
    FOdysseyVectorVertex* vertex;
    FOdysseyVectorSection* section;
    uint32 depth;
} FCycleNode;

typedef struct _FCycleSearch
{
    FOdysseyVectorSection* startSection;
    FOdysseyVectorSection* endSection;
} FCycleSearch;

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorGroupPaint : public UOdysseyVectorGroup
{
    public:
        GENERATED_BODY()

    protected:
        uint32 MarchVertex( FOdysseyVectorVertexIntersection* iVertex );

        static const uint32 NOCYCLE  = 0;
        static const uint32 BLOCKED  = 1;
        static const uint32 HASCYCLE = 2;


        std::list<FOdysseyVectorBucket*> mBucketList;
        std::vector<FOdysseyVectorCycle*> mLoopArray;

    public:
        UPROPERTY(EditAnywhere, Category="General")
        bool Realtime; // relatime updates

        UPROPERTY(EditAnywhere, Category="General")
        double Tolerance;

    public:
        void ApplyBucket( FOdysseyVectorBucket* iBucket );
        void Colorize();

    public:
        ~UOdysseyVectorGroupPaint();
        UOdysseyVectorGroupPaint();
        void Init( std::string iName );

        void UpdateShape( uint32 iUpdateFlags );
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        bool PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags );
        UOdysseyVectorObject* CopyShape();

        uint32 IntersectSegment( FOdysseyVectorSegmentCubic& iCubicSegment
                               , std::list<FOdysseyVectorSegment*>& cubicSegmenList
                               , std::vector<FOdysseyVectorVertexIntersection*>& iIntersectionVertexList );
        void CopyBuckets( UOdysseyVectorGroupPaint* iDestination );
        uint32 BuildGraph( std::vector<FOdysseyVectorVertexIntersection*>& iIntersectionVertexArray );

        FOdysseyVectorCycle* March( FOdysseyVectorVertexIntersection* iNode
                                 , FOdysseyVectorSection* iStartSection
                                 , FOdysseyVectorSection* iEndSection );

        uint32 FindPath( FOdysseyVectorVertex* iVertex
                                  , FOdysseyVectorSection* iSection
                                  , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                  , std::vector<FOdysseyVectorSection*>& iSectionArray
                                  , double iOrientation
                                  , uint32 );

        std::list<FOdysseyVectorBucket*>& GetBucketList();
        void MarchCycle( FOdysseyVectorCycle& iCycle );

        void FindCycles();
        virtual uint32 GetType();
        void SimplifyGraph();
        FOdysseyVectorBucket* Bucket( double iX, double iY, uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        FOdysseyVectorBucket* PickBucket( double iX, double iY );
        FOdysseyVectorCycle* PickCycle( double iX, double iY );
        FOdysseyVectorHandleBucket* PickBucketHandle( double iX, double iY );
        void DrawBuckets( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void OrderCycles();
        void ClearCycles();
        void AddBucket( FOdysseyVectorBucket* iBucket );
        void RemoveBucket( FOdysseyVectorBucket* iBucket );
        void PropertyChanged(const FName& iPropertyName);
};
