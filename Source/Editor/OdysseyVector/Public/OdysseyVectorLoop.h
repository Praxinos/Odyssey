#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorObject.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorBucket.h"

class ODYSSEYVECTOR_API FOdysseyVectorLoop
{
    private:
        void BuildSegmentCubic( FOdysseyVectorSegmentCubic& iSegment
                              , double iFromT
                              , double iToT );

    public:
        static const uint32 MARCHED      = ( 1 << 4 );

    protected :
        BLPath mPath;
        UOdysseyVectorObject& mParent;
        uint64 mID;
        FColor mColor;
        FOdysseyVectorBucket* mBucket;
        std::vector<FOdysseyVectorVertex*> mVertexArray;
        std::vector<FOdysseyVectorSection*> mSectionArray;
        uint32 mFlags;
        uint32 mValence;
        ::ULIS::FVec2D mMin;
        ::ULIS::FVec2D mMax;
        std::list<FOdysseyVectorLoop*> mChildrenList;
        FOdysseyVectorLoop* mParentCycle;

    public:
        static FOdysseyVectorLoop* Exists( uint64 iID
                                         , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                         , std::vector<FOdysseyVectorSection*>& iSectionArray );
        static uint64 GenerateID( std::vector<FOdysseyVectorSection*>& iSectionArray );

        ~FOdysseyVectorLoop();
         FOdysseyVectorLoop( UOdysseyVectorObject& iParent
                           , uint64 iID
                           , std::vector<FOdysseyVectorVertex*>& iVertexArray
                           , std::vector<FOdysseyVectorSection*>& iSectionArray );
        void Draw( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void Build( std::vector<FOdysseyVectorVertex*>& iVertexArray
                  , std::vector<FOdysseyVectorSection*>& iSectionArray );
        bool HitTest( double iX, double iY );
        void SetColor( uint32 iColor );
        FColor GetColor();

        void AppendChild( FOdysseyVectorLoop *iChild );
        void RemoveChild( FOdysseyVectorLoop *iChild );
        FOdysseyVectorLoop* GetParentCycle();

        FOdysseyVectorBucket* GetBucket();
        void SetBucket( FOdysseyVectorBucket* iBucket );
        std::vector<FOdysseyVectorVertex*>& GetVertexArray();
        std::vector<FOdysseyVectorSection*>& GetSectionArray();
        void SetMarched( bool iMarched );
        bool IsMarched();
        bool FitsIn( FOdysseyVectorLoop* iParentCandidate );

        void Block();
        void UnBlock();

        uint32 GetValence();
        uint64 GetID();
};
