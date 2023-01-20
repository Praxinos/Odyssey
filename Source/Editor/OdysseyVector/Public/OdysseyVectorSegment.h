#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorLink.h"

#include "OdysseyVectorSegment.generated.h"

class UOdysseyVectorPath;
class UOdysseyVectorVertexIntersection;
class FOdysseyVectorSection;

UCLASS()
class UOdysseyVectorSegment : public UOdysseyVectorLink
{
    public:
        GENERATED_BODY()

    public:
        static UOdysseyVectorSegment* New( UOdysseyVectorPath* iPath
                                         , UOdysseyVectorVertex* iVertex0
                                         , UOdysseyVectorVertex* iVertex1 );
        void Init( UOdysseyVectorPath* iPath
                 , UOdysseyVectorVertex* iVertex0
                 , UOdysseyVectorVertex* iVertex1 );

    protected:
        std::list<UOdysseyVectorVertexIntersection*> mIntersectionVertexList;
        std::list<FOdysseyVectorSection*> mSectionList;
        UOdysseyVectorPath* mPath;
        ::ULIS::FRectD mBBox;

    public:
        ~UOdysseyVectorSegment();
        UOdysseyVectorSegment();

        virtual void Draw( ::ULIS::FRectD &iRoi );
        virtual void DrawStructure( ::ULIS::FRectD &iRoi );

        UOdysseyVectorSegment* GetPreviousSegment();
        UOdysseyVectorSegment* GetNextSegment();
        std::list<UOdysseyVectorVertexIntersection*>& GetIntersectionVertexList();
        bool HasIntersectionVertex( UOdysseyVectorVertexIntersection& mIntersectionVertex );
        UOdysseyVectorVertex* GetNextVertex( double iT );
        UOdysseyVectorVertex* GetPreviousVertex( double iT ); 
        UOdysseyVectorPath* GetPath();
        virtual void Update() {};
        void Invalidate();

        void ClearIntersections();
        FOdysseyVectorSection* GetSection (double t);
        std::list<FOdysseyVectorSection*>& GetSectionList();

        void AddIntersection ( UOdysseyVectorVertexIntersection* iIntersectionVertex );
        void RemoveSection ( FOdysseyVectorSection* iSection );
        void AddSection ( FOdysseyVectorSection* iSection );

        virtual ::ULIS::FRectD& GetBoundingBox() { return mBBox; };

        UOdysseyVectorSegment* GetOtherSegment( UOdysseyVectorSegment& iCurrentSegment );
};
