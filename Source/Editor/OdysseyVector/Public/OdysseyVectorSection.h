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
        bool mBlocked;

    public:
        ~FOdysseyVectorSection();
        FOdysseyVectorSection( UOdysseyVectorSegment* iSegment, UOdysseyVectorVertex* iVertex0, UOdysseyVectorVertex* iVertex1 );
        UOdysseyVectorSegment* GetSegment();
        UOdysseyVectorVertex* GetVertex( int iNum );
        void AddLoop( UOdysseyVectorLoop* iLoop );
        void RemoveLoop( UOdysseyVectorLoop* iLoop );
        std::list<UOdysseyVectorLoop*>& GetLoopList();
        void SetBlocked( bool iBlocked );
        bool IsBlocked();
};
