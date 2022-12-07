#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

class FOdysseyVectorSegment;
class FOdysseyVectorLoop;
class FOdysseyVectorSection;

class FOdysseyVectorPoint
{
    private:

    protected:
        ::ULIS::FVec2D mCoords;
        std::list<FOdysseyVectorSegment*> mSegmentList;
        std::list<FOdysseyVectorSection*> mSectionList;
        std::list<FOdysseyVectorLoop*> mLoopList;
        double mRadius;

    public:
        static  const uint32 POINT_TYPE_REGULAR        = 0;
        static  const uint32 POINT_TYPE_INTERSECTION   = 1;
        static  const uint32 POINT_TYPE_HANDLE_POINT   = 2;
        static  const uint32 POINT_TYPE_HANDLE_SEGMENT = 3;

        ~FOdysseyVectorPoint();
        FOdysseyVectorPoint();
        FOdysseyVectorPoint( double iX, double iY );
        virtual void AddSegment( FOdysseyVectorSegment* iSegment );
        void AddSection( FOdysseyVectorSection* iSection );
        void RemoveSection( FOdysseyVectorSection* iSection );
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
        virtual void SetRadius( double iRadius, bool iBuildSegments );
        void InvalidateSegments();
        void March();
        bool HasSegment( FOdysseyVectorSegment& iSegment );
        void AddLoop( FOdysseyVectorLoop* iLoop );
        void RemoveLoop( FOdysseyVectorLoop* iLoop );
        void InvalidateLoops();
        std::list<FOdysseyVectorSection*>& GetSectionList();
        bool IsClosestSection( FOdysseyVectorSection& iStartSection
                             , FOdysseyVectorSection& iEndSection );
};
