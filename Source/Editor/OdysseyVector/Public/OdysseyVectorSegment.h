#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorLink.h"

class UOdysseyVectorPath;
class FOdysseyVectorVertexIntersection;
class FOdysseyVectorSection;

class ODYSSEYVECTOR_API FOdysseyVectorSegment : public FOdysseyVectorLink
{
    public:
        static FOdysseyVectorSegment* New( UOdysseyVectorPath* iPath
                                         , FOdysseyVectorVertex* iVertex0
                                         , FOdysseyVectorVertex* iVertex1 );
        void Init( UOdysseyVectorPath* iPath
                 , FOdysseyVectorVertex* iVertex0
                 , FOdysseyVectorVertex* iVertex1 );

    protected:
        std::list<FOdysseyVectorVertexIntersection*> mIntersectionVertexList;
        std::list<FOdysseyVectorSection*> mSectionList;
        UOdysseyVectorPath* mPath;
        ::ULIS::FRectD mBBox;

    public:
        ~FOdysseyVectorSegment();
        FOdysseyVectorSegment();

        virtual void Draw( ::ULIS::FRectD &iRoi );
        virtual void DrawStructure( ::ULIS::FRectD &iRoi );

        virtual bool ProximityTest( double iLocalX, double iLocalY, double iDistanceTolerance, double &oSmallestDistance ){ return false; };
        FOdysseyVectorSegment* GetPreviousSegment();
        FOdysseyVectorSegment* GetNextSegment();
        std::list<FOdysseyVectorVertexIntersection*>& GetIntersectionVertexList();
        bool HasIntersectionVertex( FOdysseyVectorVertexIntersection& mIntersectionVertex );
        FOdysseyVectorVertex* GetNextVertex( double iT );
        FOdysseyVectorVertex* GetPreviousVertex( double iT );
        UOdysseyVectorPath* GetPath();
        void SetPath( UOdysseyVectorPath* iPath );
        virtual void Update() {};
        void Invalidate();
        virtual ::ULIS::FVec2D GetVectorAtEnd(bool iNormalize);
        virtual ::ULIS::FVec2D GetVectorAtStart(bool iNormalize);
        FOdysseyVectorVertex* GetVertex( uint32 iVertexID );
        void ClearIntersections();
        FOdysseyVectorSection* GetSection (double t);
        std::list<FOdysseyVectorSection*>& GetSectionList();

        void AddIntersection ( FOdysseyVectorVertexIntersection* iIntersectionVertex );
        void RemoveSection ( FOdysseyVectorSection* iSection );
        void AddSection ( FOdysseyVectorSection* iSection );

        virtual ::ULIS::FRectD& GetBoundingBox() { return mBBox; };
        virtual ::ULIS::FVec2D GetPointAt(double t);
        virtual ::ULIS::FVec2D GetTangentAt(double t);

        FOdysseyVectorSegment* GetOtherSegment( FOdysseyVectorSegment& iCurrentSegment );
};
