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
class ODYSSEYVECTOR_API UOdysseyVectorPath : public UOdysseyVectorObject
{
    public:
        GENERATED_BODY()

    private:
        void UpdateShape();
        virtual void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags ) { return nullptr; };
        UOdysseyVectorObject* CopyShape();

    protected :
        std::list<UOdysseyVectorLoop*> mLoopList; // list of loops
        std::list<UOdysseyVectorVertex*> mVertexList;
        std::list<UOdysseyVectorSegment*> mSegmentList;
        std::list<UOdysseyVectorSegment*> mInvalidatedSegmentList;
        std::list<UOdysseyVectorLoop*> mInvalidatedLoopList;
        std::list<UOdysseyVectorPoint*> mSelectedPointList;
        BLPath mPath;

    public:
        static const uint64 PICK_HANDLE_POINT   = 1;
        static const uint64 PICK_HANDLE_SEGMENT = 1 << 1;
        static const uint64 PICK_POINT          = 1 << 2;

        ~UOdysseyVectorPath(){};
        UOdysseyVectorPath(){};
        void Init( std::string iName );
        void AddSegment(UOdysseyVectorSegment* iSegment);
        void RemoveSegment(UOdysseyVectorSegment* iSegment);
        void AddVertex( UOdysseyVectorVertex* iVertex );
        virtual UOdysseyVectorSegment* AppendVertex( UOdysseyVectorVertex* iVertex, UOdysseyVectorVertex* iPreviousVertex );
        UOdysseyVectorObject* PickLoops( double iX, double iY, double iRadius );
        void DrawLoops( ::ULIS::FRectD &iRoi, uint64 iFlags );

        virtual void DrawStructure( ::ULIS::FRectD &iRoi );

        /*virtual void InsertPoint( UOdysseyVectorSegment* iSegment, UOdysseyVectorVertex* iPoint );*/
        std::list<UOdysseyVectorSegment*>& GetSegmentList();
        std::list<UOdysseyVectorVertex*>& GetVertexList();
        UOdysseyVectorVertex* GetFirstVertex();
        UOdysseyVectorVertex* GetLastVertex();
        UOdysseyVectorSegment* GetFirstSegment();
        UOdysseyVectorSegment* GetLastSegment();
        std::list<UOdysseyVectorPoint*>& GetSelectedPointList();
        virtual bool PickPoint( double iX, double iY, double iRadius, uint64 iSelectionFlags ){ return false; };
        virtual void Unselect( UOdysseyVectorVertex* iVertex ){};
        void Clear();
        bool IsLoop();
        UOdysseyVectorLoop* GetLoopByID( uint64 iID );
        void AddLoop( UOdysseyVectorLoop* iLoop );
        void RemoveLoop( UOdysseyVectorLoop* iLoop );

        void InvalidateSegment( UOdysseyVectorSegment* iSegment );
        void InvalidateLoop( UOdysseyVectorLoop* iLoop );
        void UpdateBBox();
};
