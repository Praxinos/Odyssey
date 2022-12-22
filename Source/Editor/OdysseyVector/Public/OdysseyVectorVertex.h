#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"

#include "OdysseyVectorVertex.generated.h"

class UOdysseyVectorSegment;
class UOdysseyVectorLoop;
class FOdysseyVectorSection;

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorVertex : public UOdysseyVectorPoint
{
    public:
        GENERATED_BODY()

    public:
        static UOdysseyVectorVertex* New( double iX, double iY, double iRadius );

    private:

    protected:
        std::list<UOdysseyVectorSegment*> mSegmentList;
        std::list<FOdysseyVectorSection*> mSectionList;
        std::list<UOdysseyVectorLoop*> mLoopList;

    public:
        ~UOdysseyVectorVertex();
        UOdysseyVectorVertex();
        virtual void AddSegment( UOdysseyVectorSegment* iSegment );
        void AddSection( FOdysseyVectorSection* iSection );
        void RemoveSection( FOdysseyVectorSection* iSection );
        ::ULIS::FRectD GetRectangle();
        UOdysseyVectorSegment* GetLastSegment();
        UOdysseyVectorSegment* GetFirstSegment();
        virtual UOdysseyVectorSegment* GetSegment( UOdysseyVectorVertex& iOtherVertex );
        UOdysseyVectorSegment* GetOtherSegment( UOdysseyVectorSegment& iCurrentSegment );
        void RemoveSegment( UOdysseyVectorSegment* iSegment );
        virtual ::ULIS::FVec2D GetPosition( UOdysseyVectorSegment& iSegment );
        virtual void SetX( double iX );
        virtual void SetY( double iY );
        virtual void Set( double iX, double iY );
        uint32 GetSegmentCount();
        std::list<UOdysseyVectorSegment*>& GetSegmentList();
        virtual double GetT( UOdysseyVectorSegment& );
        virtual void SetRadius( double iRadius );
        void InvalidateSegments();
        void March();
        bool HasSegment( UOdysseyVectorSegment& iSegment );
        void AddLoop( UOdysseyVectorLoop* iLoop );
        void RemoveLoop( UOdysseyVectorLoop* iLoop );
        void InvalidateLoops();
        std::list<FOdysseyVectorSection*>& GetSectionList();
        bool IsClosestSection( FOdysseyVectorSection& iStartSection
                             , FOdysseyVectorSection& iEndSection );
};
