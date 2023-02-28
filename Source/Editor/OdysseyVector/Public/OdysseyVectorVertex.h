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
class UOdysseyVectorPath;

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorVertex : public UOdysseyVectorPoint
{
    public:
        GENERATED_BODY()

    public:
        static const uint32 VISITED      = ( 1 << 2 );
        static const uint32 INCYCLE      = ( 1 << 3 );
        static const uint32 MARCHED      = ( 1 << 4 );

    public:
        static UOdysseyVectorVertex* New( double iX, double iY, double iRadius );

    private:

    protected:
        std::list<UOdysseyVectorSegment*> mSegmentList;
        std::list<FOdysseyVectorSection*> mSectionList;
        std::list<UOdysseyVectorLoop*> mLoopList;
        uint32 mFlags;
        uint32 mCycleID;
        UOdysseyVectorPath* mPath;

    public:
        ~UOdysseyVectorVertex();
        UOdysseyVectorVertex();
        virtual void AddSegment( UOdysseyVectorSegment* iSegment );
        void AddSection( FOdysseyVectorSection* iSection );
        void RemoveSection( FOdysseyVectorSection* iSection );
        ::ULIS::FRectD GetRectangle();
        FOdysseyVectorSection* GetMarchedSection( bool iStatus );
        FOdysseyVectorSection* GetFirstSection();
        FOdysseyVectorSection* GetLastSection();
        UOdysseyVectorSegment* GetLastSegment();
        UOdysseyVectorSegment* GetFirstSegment();
        virtual UOdysseyVectorSegment* GetSegment( UOdysseyVectorVertex& iOtherVertex );
        FOdysseyVectorSection* GetSection( UOdysseyVectorVertex& iOtherVertex );
        FOdysseyVectorSection* GetOtherSection( FOdysseyVectorSection* iSection );
        UOdysseyVectorSegment* GetOtherSegment( UOdysseyVectorSegment& iCurrentSegment );
        void RemoveSegment( UOdysseyVectorSegment* iSegment );
        virtual ::ULIS::FVec2D GetPosition( UOdysseyVectorSegment& iSegment );
        virtual void SetX( double iX );
        virtual void SetY( double iY );
        virtual void Set( double iX, double iY );
        uint32 GetSegmentCount();
        uint32 GetSectionCount();
        std::list<UOdysseyVectorSegment*>& GetSegmentList();
        virtual double GetT( UOdysseyVectorSegment& );
        virtual void SetRadius( double iRadius );
        void InvalidateSegments();
        void March();
        bool HasSegment( UOdysseyVectorSegment& iSegment );
        void AddLoop( UOdysseyVectorLoop* iLoop );
        void RemoveLoop( UOdysseyVectorLoop* iLoop );
        void InvalidateLoops();
        void SetVisited( bool iVisited );
        bool IsVisited();
        std::list<FOdysseyVectorSection*>& GetSectionList();
        bool IsClosestSection( FOdysseyVectorSection& iStartSection
                             , FOdysseyVectorSection& iEndSection );
        UOdysseyVectorPath* GetPath();
        virtual ::ULIS::FVec2D& GetCoordsOnSegment( UOdysseyVectorSegment* iSegment );
        void SetPath( UOdysseyVectorPath* iPath );
        void SetInCycle( bool iInCycle, uint32 iCycleID );
        bool IsInCycle();
        uint32 GetCycleID();
        void SetMarched( bool iMarched );
        bool IsMarched();
        FOdysseyVectorSection* GetCycleNextSection( FOdysseyVectorSection* iLastSection, double iOrientation );
};
