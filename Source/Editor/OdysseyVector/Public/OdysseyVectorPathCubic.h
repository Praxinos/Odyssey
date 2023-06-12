#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorVertex.h"
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

        bool HasBaseClass( uint32 iBaseClassID );

    protected:
        FOdysseyVectorObject* CopyShape();
        void DrawShape( uint64 iFlags );
        bool PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags );

    public:
        static void SmoothSegments( FOdysseyVectorVertex* iVertex, bool iBuildSegments, bool iPreserveHandleLength );
        static void SmoothSegments( FOdysseyVectorVertex* iVertex
                                  , ::ULIS::FVec2D iPerpendicularVector
                                  , bool iBuildSegments
                                  , bool iPreserveHandleLength );
        static ::ULIS::FVec2D GetPerpendicularVector( FOdysseyVectorVertex* iVertex, bool iNormalize );

        FOdysseyVectorPathCubic( const FString& iName );
        void Init( FString& iName );
        bool PickPoint( double iWorldX
                      , double iWorldY
                      , double iSelectionRadius
                      , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                      , uint64 iSelectionFlags );
        void Fill();
        void Merge( FOdysseyVectorPath* iPath
                  , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray );
        void Merge( FOdysseyVectorPath* iMergedPath
                  , std::vector<FOdysseyVectorVertex*>& iVertexLookup
                  , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray );
        void DrawShapeVariable( uint64 iFlags );

        void Mirror( bool iMirrorX, bool iMirrorY );
        void Cut( ::ULIS::FVec2D& linePoint0
                , ::ULIS::FVec2D& linePoint1
                , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray
                , std::vector<FOdysseyVectorSegment*>& oOldSegmentArray );
        void SwitchSpace( FOdysseyVectorObject& iObject );
        bool Erase( ::ULIS::FRectD &iRoi
                  , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                  , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray );

        uint32 GetType();
};
