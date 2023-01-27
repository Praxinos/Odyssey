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

        static const uint32 BLOCKVERTEX0 = ( 1 << 0 );
        static const uint32 BLOCKVERTEX1 = ( 1 << 1 );
        static const uint32 VISITED      = ( 1 << 2 );
        static const uint32 INCYCLE      = ( 1 << 3 );

    public:
        ~FOdysseyVectorSection();
        FOdysseyVectorSection( UOdysseyVectorSegment* iSegment, UOdysseyVectorVertex* iVertex0, UOdysseyVectorVertex* iVertex1 );
        UOdysseyVectorSegment* GetSegment();
        UOdysseyVectorVertex* GetVertex( int iNum );
        void AddLoop( UOdysseyVectorLoop* iLoop );
        void RemoveLoop( UOdysseyVectorLoop* iLoop );
        std::list<UOdysseyVectorLoop*>& GetLoopList();
        void Block( UOdysseyVectorVertex* iVertex );
        bool IsBlocked( UOdysseyVectorVertex* iVertex );
        void SetVisited( bool iVisited );
        bool IsVisited();
        void SetInCycle( bool iInCycle );
        bool IsInCycle();
        void SetAverage( double iX, double iY );
};
