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

//#include "OdysseyVectorPathCubic.generated.h"

class ODYSSEYVECTOR_API FOdysseyVectorPathCubic: public FOdysseyVectorPath
{
    private:
        static const uint32 mStaticClass = 0x4cf439ca; // value is crc32 FOdysseyVectorPathCubic

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

    protected:
        FOdysseyVectorObject* CopyShape();
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        bool PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags );

    public:
        FOdysseyVectorPathCubic();
        void Init( std::string iName );
        FOdysseyVectorSegmentCubic* AppendVertex( FOdysseyVectorVertexCubic* iPoint, bool iConnect, bool iBuildSegments );
        bool PickPoint( double iX
                      , double iY
                      , double iSelectionRadius
                      , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                      , uint64 iSelectionFlags );
        void Unselect( FOdysseyVectorVertex* iPoint );
        void Fill( ::ULIS::FRectD& iRoi );
        void Merge( FOdysseyVectorPath* iPath );
        void DrawShapeVariable( ::ULIS::FRectD& iRoi, uint64 iFlags );

        void Mirror( bool iMirrorX, bool iMirrorY );
        void Cut( ::ULIS::FVec2D& linePoint0
                , ::ULIS::FVec2D& linePoint1
                , std::vector<FOdysseyVectorVertexCubic*>& oNewVertexArray
                , std::vector<FOdysseyVectorSegmentCubic*>& oNewSegmentArray
                , std::vector<FOdysseyVectorSegmentCubic*>& oOldSegmentArray );
        void SwitchSpace( FOdysseyVectorObject& iObject );
        bool Erase( ::ULIS::FRectD &iRoi );

        uint32 GetType();
};
