#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorVertexCubic.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorCycle.h"

#include "OdysseyVectorPathCubic.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorPathCubic: public UOdysseyVectorPath
{
    public:
        GENERATED_BODY()

    private:

    protected:
        UOdysseyVectorObject* CopyShape();
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        bool PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags );

    public:
        UPROPERTY(EditAnywhere,Category="General")
        bool Filled;

    public:
        UOdysseyVectorPathCubic();
        void Init( std::string iName );
        FOdysseyVectorSegmentCubic* AppendVertex( FOdysseyVectorVertexCubic* iPoint, bool iConnect, bool iBuildSegments );
        bool PickPoint( double iX
                      , double iY
                      , double iSelectionRadius
                      , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                      , uint64 iSelectionFlags );
        void Unselect( FOdysseyVectorVertex* iPoint );
        bool IsFilled();
        void SetFilled( bool iIsFilled );
        void DrawStructure( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void Fill( ::ULIS::FRectD& iRoi );
        void Merge( UOdysseyVectorPath* iPath );
        void DrawShapeVariable( ::ULIS::FRectD& iRoi, uint64 iFlags );

        void Mirror( bool iMirrorX, bool iMirrorY );
        void Cut( ::ULIS::FVec2D& linePoint0, ::ULIS::FVec2D& linePoint1 );
        void SwitchSpace( UOdysseyVectorObject& iObject );
        bool Erase( ::ULIS::FRectD &iRoi );

        uint32 GetType();
};
