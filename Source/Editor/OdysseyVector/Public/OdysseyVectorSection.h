#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorLoop.h"

class FOdysseyVectorSection
{
    protected:
        FOdysseyVectorSegment& mSegment;
        FOdysseyVectorPoint* mPoint[2];
        std::list<UOdysseyVectorLoop*> mLoopList;

    public:
        ~FOdysseyVectorSection();
        FOdysseyVectorSection( FOdysseyVectorSegment& iSegment, FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 );
        FOdysseyVectorSegment& GetSegment();
        FOdysseyVectorPoint* GetPoint( int iNum );
        void AddLoop( UOdysseyVectorLoop* iLoop );
        void RemoveLoop( UOdysseyVectorLoop* iLoop );
        std::list<UOdysseyVectorLoop*>& GetLoopList();
};
