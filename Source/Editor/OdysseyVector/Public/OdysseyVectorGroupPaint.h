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
#include "OdysseyVectorCycle.h"

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

USTRUCT()
struct FGroupPaintParam
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="General")
    bool Realtime; // relatime updates

    UPROPERTY(EditAnywhere, Category="General")
    double Tolerance;
};

class ODYSSEYVECTOR_API FOdysseyVectorGroupPaint : public FOdysseyVectorGroup
{
    private:
        static const uint32 mStaticClass =  0xa5a4b5bd; // value is crc32 FOdysseyVectorGroupPaint

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

        ~FOdysseyVectorGroupPaint();
        FOdysseyVectorGroupPaint();
        void Init( std::string iName );

        void UpdateShape( uint32 iUpdateFlags );
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        bool PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags );
        FOdysseyVectorObject* CopyShape();

        uint32 IntersectSegment( FOdysseyVectorSegmentCubic& iCubicSegment
                               , std::list<FOdysseyVectorSegment*>& cubicSegmenList
                               , std::vector<FOdysseyVectorVertexIntersection*>& iIntersectionVertexList );
        void CopyBuckets( FOdysseyVectorGroupPaint* iDestination );
        void BuildGraph( );

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

        void FindCycles();
        virtual uint32 GetType();
        void SimplifyGraph();
        FOdysseyVectorBucket* Bucket( double iX, double iY, uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        FOdysseyVectorBucket* PickBucket( double iX, double iY );
        FOdysseyVectorCycle* PickCycle( double iX, double iY );
        FOdysseyVectorHandleBucket* PickBucketHandle( double iX, double iY );
        void DrawBuckets( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void OrderCycles();
        void ClearCycles( std::list<FOdysseyVectorSegment*>& cubicSegmenList );
        void AddBucket( FOdysseyVectorBucket* iBucket );
        void RemoveBucket( FOdysseyVectorBucket* iBucket );
        void PropertyChanged(const FName& iPropertyName);
        void ApplyBucket( FOdysseyVectorBucket* iBucket );
        void Colorize();

    protected:
        uint32 MarchVertex( FOdysseyVectorVertexIntersection* iVertex );

    protected:
        static const uint32 NOCYCLE  = 0;
        static const uint32 BLOCKED  = 1;
        static const uint32 HASCYCLE = 2;
        std::list<FOdysseyVectorBucket*> mBucketList;
        std::vector<FOdysseyVectorCycle*> mLoopArray;
        std::vector<FOdysseyVectorVertexIntersection*> mIntersectionVertexArray;

    public:
        FGroupPaintParam mGroupPaintParam;
};
