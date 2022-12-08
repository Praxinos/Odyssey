#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"

#include "OdysseyVectorPath.generated.h"

class UOdysseyVectorLoop;

UCLASS()
class UOdysseyVectorPath : public UOdysseyVectorObject
{
    public:
        GENERATED_BODY()

    private:
        void UpdateShape();
        virtual void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( double iX, double iY, double iRadius ) { return nullptr; };
        UOdysseyVectorObject* CopyShape();

    protected :
        std::list<UOdysseyVectorLoop*> mLoopList; // list of loops
        std::list<FOdysseyVectorPoint*> mPointList;
        std::list<FOdysseyVectorSegment*> mSegmentList;
        std::list<FOdysseyVectorSegment*> mInvalidatedSegmentList;
        std::list<UOdysseyVectorLoop*> mInvalidatedLoopList;
        std::list<FOdysseyVectorPoint*> mSelectedPointList;
        BLPath mPath;

    public:
        static const uint64 PICK_HANDLE_POINT   = 1;
        static const uint64 PICK_HANDLE_SEGMENT = 1 << 1;
        static const uint64 PICK_POINT          = 1 << 2;

        ~UOdysseyVectorPath(){};
        UOdysseyVectorPath(){};
        void Init( std::string iName );
        void AddSegment(FOdysseyVectorSegment* iSegment);
        void RemoveSegment(FOdysseyVectorSegment* iSegment);
        void AddPoint( FOdysseyVectorPoint* iPoint );
        virtual FOdysseyVectorSegment* AppendPoint( FOdysseyVectorPoint* iPoint, FOdysseyVectorPoint* iPreviousPoint );
        UOdysseyVectorObject* PickLoops( double iX, double iY, double iRadius );
        void DrawLoops( ::ULIS::FRectD &iRoi, uint64 iFlags );

        virtual void DrawStructure( ::ULIS::FRectD &iRoi );

        /*virtual void InsertPoint( FOdysseyVectorSegment* iSegment, FOdysseyVectorPoint* iPoint );*/
        std::list<FOdysseyVectorSegment*>& GetSegmentList();
        FOdysseyVectorPoint* GetFirstPoint();
        FOdysseyVectorPoint* GetLastPoint();
        FOdysseyVectorSegment* GetFirstSegment();
        FOdysseyVectorSegment* GetLastSegment();
        std::list<FOdysseyVectorPoint*>& GetSelectedPointList();
        virtual bool PickPoint( double iX, double iY, double iRadius, uint64 iSelectionFlags ) PURE_VIRTUAL(__func__,return false;);
        virtual void Unselect( FOdysseyVectorPoint* iPoint ) PURE_VIRTUAL(__func__,);
        void Clear();
        bool IsLoop();
        UOdysseyVectorLoop* GetLoopByID( uint64 iID );
        void AddLoop( UOdysseyVectorLoop* iLoop );
        void RemoveLoop( UOdysseyVectorLoop* iLoop );

        void InvalidateSegment(FOdysseyVectorSegment* iSegment);
        void InvalidateLoop( UOdysseyVectorLoop* iLoop );
        void UpdateBBox();

};
