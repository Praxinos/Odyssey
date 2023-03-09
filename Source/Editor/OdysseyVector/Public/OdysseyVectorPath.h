#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"

#include "OdysseyVectorPath.generated.h"

class UOdysseyVectorCycle;

UENUM(BlueprintType)
enum class eJointType : uint8
{
    None   = 0,
    Radial = 1,
    Linear = 2,
    Miter  = 3
};

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorPath : public UOdysseyVectorObject
{
    public:
        GENERATED_BODY()

    protected:
        void DrawJoint( FOdysseyVectorVertex* iVertex, ::ULIS::FRectD &iRoi, uint64 iFlags );

    protected:
        virtual void UpdateShape( uint32 iUpdateFlags );
        virtual void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        bool PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags ) { return nullptr; };
        UOdysseyVectorObject* CopyShape();

    protected :
        std::list<FOdysseyVectorVertex*> mVertexList;
        std::list<FOdysseyVectorSegment*> mSegmentList;
        std::list<FOdysseyVectorSegment*> mInvalidatedSegmentList;
        /*std::list<UOdysseyVectorCycle*> mInvalidatedLoopList;*/
        std::list<FOdysseyVectorPoint*> mSelectedPointList;
        BLPath mPath;

    public:
        static const uint64 PICK_HANDLE_POINT   = 1;
        static const uint64 PICK_HANDLE_SEGMENT = 1 << 1;
        static const uint64 PICK_POINT          = 1 << 2;

        UPROPERTY(EditAnywhere, Category="Transform")
        eJointType JointType;

        ~UOdysseyVectorPath();
        UOdysseyVectorPath();
        void Init( std::string iName );
        void AddSegment(FOdysseyVectorSegment* iSegment);
        void RemoveSegment(FOdysseyVectorSegment* iSegment);
        void AddVertex( FOdysseyVectorVertex* iVertex );
        void RemoveVertex( FOdysseyVectorVertex* iVertex );
        virtual FOdysseyVectorSegment* AppendVertex( FOdysseyVectorVertex* iVertex, FOdysseyVectorVertex* iPreviousVertex );
        /*UOdysseyVectorObject* PickLoops( double iX, double iY, double iRadius );
        void DrawLoops( ::ULIS::FRectD &iRoi, uint64 iFlags );*/
        virtual void Merge( UOdysseyVectorPath* iPath ){};
        virtual void DrawStructure( ::ULIS::FRectD &iRoi );
        void SetJointType( eJointType mJointType );
        eJointType GetJointType();
        /*virtual void InsertPoint( FOdysseyVectorSegment* iSegment, FOdysseyVectorVertex* iPoint );*/
        std::list<FOdysseyVectorSegment*>& GetSegmentList();
        std::list<FOdysseyVectorVertex*>& GetVertexList();
        FOdysseyVectorVertex* GetFirstVertex();
        FOdysseyVectorVertex* GetLastVertex();
        FOdysseyVectorSegment* GetFirstSegment();
        FOdysseyVectorSegment* GetLastSegment();
        std::list<FOdysseyVectorPoint*>& GetSelectedPointList();
        virtual bool PickPoint( double iX
                              , double iY
                              , double iRadius
                              , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                              , uint64 iSelectionFlags ){ return false; };
        virtual void Unselect( FOdysseyVectorVertex* iVertex ){};
        void Clear();
        /*bool IsLoop();
        UOdysseyVectorCycle* GetLoopByID( uint64 iID );
        void AddLoop( UOdysseyVectorCycle* iLoop );
        void RemoveLoop( UOdysseyVectorCycle* iLoop );*/
        void InvalidateSegment( FOdysseyVectorSegment* iSegment );
        void InvalidateAllSegments();
        /*void InvalidateLoop( UOdysseyVectorCycle* iLoop );*/
        void UpdateBBox();
};
