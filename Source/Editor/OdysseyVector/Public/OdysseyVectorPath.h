#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorLoop.h"

class FOdysseyVectorPath : public FOdysseyVectorObject
{
    private:
        void UpdateShape();
        virtual void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        FOdysseyVectorObject* PickShape( double iX, double iY, double iRadius ) { return nullptr; };
        FOdysseyVectorObject* CopyShape();

    protected :
        std::list<FOdysseyVectorLoop*> mLoopList; // list of loops
        std::list<FOdysseyVectorPoint*> mPointList;
        std::list<FOdysseyVectorSegment*> mSegmentList;
        std::list<FOdysseyVectorSegment*> mInvalidatedSegmentList;
        std::list<FOdysseyVectorLoop*> mInvalidatedLoopList;
        std::list<FOdysseyVectorPoint*> mSelectedPointList;
        BLPath mPath;

    public:
        static const uint64 PICK_HANDLE_POINT   = 1;
        static const uint64 PICK_HANDLE_SEGMENT = 1 << 1;
        static const uint64 PICK_POINT          = 1 << 2;

        ~FOdysseyVectorPath();
        FOdysseyVectorPath();
        FOdysseyVectorPath( std::string iName );
        void AddSegment(FOdysseyVectorSegment* iSegment);
        void RemoveSegment(FOdysseyVectorSegment* iSegment);
        void AddPoint( FOdysseyVectorPoint* iPoint );
        virtual FOdysseyVectorSegment* AppendPoint( FOdysseyVectorPoint* iPoint, FOdysseyVectorPoint* iPreviousPoint );
        FOdysseyVectorObject* PickLoops( double iX, double iY, double iRadius );
        void DrawLoops( ::ULIS::FRectD &iRoi, uint64 iFlags );

        virtual void DrawStructure( ::ULIS::FRectD &iRoi );

        /*virtual void InsertPoint( FOdysseyVectorSegment* iSegment, FOdysseyVectorPoint* iPoint );*/
        std::list<FOdysseyVectorSegment*>& GetSegmentList();
        FOdysseyVectorPoint* GetFirstPoint();
        FOdysseyVectorPoint* GetLastPoint();
        FOdysseyVectorSegment* GetFirstSegment();
        FOdysseyVectorSegment* GetLastSegment();
        std::list<FOdysseyVectorPoint*>& GetSelectedPointList();
        virtual bool PickPoint( double iX, double iY, double iRadius, uint64 iSelectionFlags ) = 0;
        virtual void Unselect( FOdysseyVectorPoint* iPoint ) = 0;
        void Clear();
        bool IsLoop();
        FOdysseyVectorLoop* GetLoopByID( uint64 iID );
        void AddLoop( FOdysseyVectorLoop* iLoop );
        void RemoveLoop( FOdysseyVectorLoop* iLoop );

        void InvalidateSegment(FOdysseyVectorSegment* iSegment);
        void InvalidateLoop( FOdysseyVectorLoop* iLoop );
        void UpdateBBox();

};
