#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

class FOdysseyVectorSegment;
class UOdysseyVectorLoop;
class FOdysseyVectorSection;

class ODYSSEYVECTOR_API FOdysseyVectorPoint
{
    private:

    protected:
        ::ULIS::FVec2D mCoords;
        std::list<FOdysseyVectorSegment*> mSegmentList;
        std::list<FOdysseyVectorSection*> mSectionList;
        std::list<UOdysseyVectorLoop*> mLoopList;
        double mRadius;

    public:
        static  const uint32 POINT_TYPE_REGULAR        = 0;
        static  const uint32 POINT_TYPE_INTERSECTION   = 1;
        static  const uint32 POINT_TYPE_HANDLE_POINT   = 2;
        static  const uint32 POINT_TYPE_HANDLE_SEGMENT = 3;

        ~FOdysseyVectorPoint();
        FOdysseyVectorPoint();
        FOdysseyVectorPoint( double iX, double iY );
        FOdysseyVectorPoint( double iX, double iY, double iRadius );
        virtual void AddSegment( FOdysseyVectorSegment* iSegment );
        void AddSection( FOdysseyVectorSection* iSection );
        void RemoveSection( FOdysseyVectorSection* iSection );
        ::ULIS::FRectD GetRectangle();
        FOdysseyVectorSegment* GetLastSegment();
        FOdysseyVectorSegment* GetFirstSegment();
        virtual FOdysseyVectorSegment* GetSegment( FOdysseyVectorPoint& iOtherPoint );
        FOdysseyVectorSegment* GetOtherSegment( FOdysseyVectorSegment& iCurrentSegment );
        void RemoveSegment( FOdysseyVectorSegment* iSegment );
        virtual ::ULIS::FVec2D& GetCoords();
        virtual ::ULIS::FVec2D GetPosition( FOdysseyVectorSegment& iSegment );
        double GetX();
        double GetY();
        virtual void SetX( double iX );
        virtual void SetY( double iY );
        virtual void Set( double iX, double iY );
        uint32 GetSegmentCount();
        std::list<FOdysseyVectorSegment*>& GetSegmentList();
        virtual uint32 GetType();
        virtual double GetT( FOdysseyVectorSegment& );
        double GetRadius();
        virtual void SetRadius( double iRadius );
        void InvalidateSegments();
        void March();
        bool HasSegment( FOdysseyVectorSegment& iSegment );
        void AddLoop( UOdysseyVectorLoop* iLoop );
        void RemoveLoop( UOdysseyVectorLoop* iLoop );
        void InvalidateLoops();
        std::list<FOdysseyVectorSection*>& GetSectionList();
        bool IsClosestSection( FOdysseyVectorSection& iStartSection
                             , FOdysseyVectorSection& iEndSection );
};
