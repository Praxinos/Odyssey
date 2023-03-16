#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"

#include "OdysseyVectorPath.generated.h"

class FOdysseyVectorCycle;

UENUM(BlueprintType)
enum class eJointType : uint8
{
    None   = 0,
    Radial = 1,
    Linear = 2,
    Miter  = 3
};

USTRUCT()
struct FPathParam
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Transform")
    eJointType JointType;

    UPROPERTY(EditAnywhere,Category="General")
    bool Filled;
};

class ODYSSEYVECTOR_API FOdysseyVectorPath : public FOdysseyVectorObject
{
    private:
        static const uint32 mStaticClass = 0x65f13c79; // value is crc32 FOdysseyVectorPath

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

    protected:
        void DrawJoint( FOdysseyVectorVertex* iVertex, ::ULIS::FRectD &iRoi, uint64 iFlags );

    protected:
        virtual void UpdateShape( uint32 iUpdateFlags );
        virtual void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        bool PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags ) { return nullptr; };
        FOdysseyVectorObject* CopyShape();

    protected :
        std::list<FOdysseyVectorVertex*> mVertexList;
        std::list<FOdysseyVectorSegment*> mSegmentList;
        std::list<FOdysseyVectorSegment*> mInvalidatedSegmentList;
        /*std::list<FOdysseyVectorCycle*> mInvalidatedLoopList;*/
        std::list<FOdysseyVectorPoint*> mSelectedPointList;
        BLPath mPath;

    public:
        FPathParam mPathParam;

    public:
        static const uint64 PICK_HANDLE_POINT   = 1;
        static const uint64 PICK_HANDLE_SEGMENT = 1 << 1;
        static const uint64 PICK_POINT          = 1 << 2;



        ~FOdysseyVectorPath();
        FOdysseyVectorPath();
        void Init( std::string iName );
        void AddSegment(FOdysseyVectorSegment* iSegment);
        void RemoveSegment(FOdysseyVectorSegment* iSegment);
        void AddVertex( FOdysseyVectorVertex* iVertex );
        void RemoveVertex( FOdysseyVectorVertex* iVertex );
        virtual FOdysseyVectorSegment* AppendVertex( FOdysseyVectorVertex* iVertex, FOdysseyVectorVertex* iPreviousVertex );
        /*FOdysseyVectorObject* PickLoops( double iX, double iY, double iRadius );
        void DrawLoops( ::ULIS::FRectD &iRoi, uint64 iFlags );*/
        virtual void Merge( FOdysseyVectorPath* iPath ){};
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
        bool IsFilled();
        void SetFilled(bool iIsFilled);
        void Clear();
        /*bool IsLoop();
        FOdysseyVectorCycle* GetLoopByID( uint64 iID );
        void AddLoop( FOdysseyVectorCycle* iLoop );
        void RemoveLoop( FOdysseyVectorCycle* iLoop );*/
        void InvalidateSegment( FOdysseyVectorSegment* iSegment );
        void InvalidateAllSegments();
        /*void InvalidateLoop( FOdysseyVectorCycle* iLoop );*/
        void UpdateBBox();
};
