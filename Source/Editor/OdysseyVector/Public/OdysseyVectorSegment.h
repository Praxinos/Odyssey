#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorLink.h"

class FOdysseyVectorPath;
class FOdysseyVectorPointIntersection;
class FOdysseyVectorSection;

class FOdysseyVectorSegment : public FOdysseyVectorLink
{
    protected:
        std::list<FOdysseyVectorPointIntersection*> mIntersectionPointList;
        std::list<FOdysseyVectorSection*> mSectionList;
        FOdysseyVectorPath& mPath;
        ::ULIS::FRectD mBBox;

    public:
        ~FOdysseyVectorSegment();
        FOdysseyVectorSegment( FOdysseyVectorPath& iPath, FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 );
        virtual void Draw( ::ULIS::FRectD &iRoi );
        virtual void DrawStructure( ::ULIS::FRectD &iRoi );

        FOdysseyVectorSegment* GetPreviousSegment();
        FOdysseyVectorSegment* GetNextSegment();
        std::list<FOdysseyVectorPointIntersection*>& GetIntersectionPointList();
        bool HasIntersectionPoint( FOdysseyVectorPointIntersection& mIntersectionPoint );
        FOdysseyVectorPoint* GetNextPoint( double iT );
        FOdysseyVectorPoint* GetPreviousPoint( double iT ); 
        FOdysseyVectorPath& GetPath();
        virtual void Update() {};
        void Invalidate();

        void ClearIntersections();
        FOdysseyVectorSection* GetSection (double t);

        void AddIntersection ( FOdysseyVectorPointIntersection* iIntersectionPoint );
        void RemoveSection ( FOdysseyVectorSection* iSection );
        void AddSection ( FOdysseyVectorSection* iSection );

        virtual ::ULIS::FRectD& GetBoundingBox() { return mBBox; };

        FOdysseyVectorSegment* GetOtherSegment( FOdysseyVectorSegment& iCurrentSegment );
};
