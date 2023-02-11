#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorLoop.h"

class FOdysseyVectorSection
{
    protected:
        UOdysseyVectorSegment* mSegment;
        UOdysseyVectorVertex* mVertex[2];
        std::list<UOdysseyVectorLoop*> mLoopList;
        uint32 mFlags;
        uint32 mCycleCount;

        static const uint32 BLOCKVERTEX0 = ( 1 << 0 );
        static const uint32 BLOCKVERTEX1 = ( 1 << 1 );
        static const uint32 VISITED = ( 1 << 2 );
        static const uint32 INCYCLE = ( 1 << 3 );

    public:
        ~FOdysseyVectorSection();
        FOdysseyVectorSection( UOdysseyVectorSegment* iSegment, UOdysseyVectorVertex* iVertex0, UOdysseyVectorVertex* iVertex1 );
        UOdysseyVectorSegment* GetSegment();
        UOdysseyVectorVertex* GetOtherVertex( UOdysseyVectorVertex* iVertex );
        UOdysseyVectorVertex* GetVertex( int iNum );
        void AddLoop( UOdysseyVectorLoop* iLoop );
        void RemoveLoop( UOdysseyVectorLoop* iLoop );
        std::list<UOdysseyVectorLoop*>& GetLoopList();
        ::ULIS::FVec2D GetVectorFromVertex( UOdysseyVectorVertex* iVertex );

        void IncrementCycleCount();

        void BlockAll();
        void Block( UOdysseyVectorVertex* iVertex );
        bool IsBlocked( UOdysseyVectorVertex* iVertex );
        /*bool IsBlocked();*/

        void SetVisited( bool iVisited );
        bool IsVisited();

        void SetInCycle( bool iInCycle );
        bool IsInCycle();
        void SetAverage( double iX, double iY );

};
