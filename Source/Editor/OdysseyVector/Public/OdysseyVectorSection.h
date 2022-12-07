#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"

class FOdysseyVectorSection
{
    protected:
        FOdysseyVectorSegment& mSegment;
        FOdysseyVectorPoint* mPoint[2];
        std::list<FOdysseyVectorLoop*> mLoopList;

    public:
        ~FOdysseyVectorSection();
        FOdysseyVectorSection( FOdysseyVectorSegment& iSegment, FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 );
        FOdysseyVectorSegment& GetSegment();
        FOdysseyVectorPoint* GetPoint( int iNum );
        void AddLoop( FOdysseyVectorLoop* iLoop );
        void RemoveLoop( FOdysseyVectorLoop* iLoop );
        std::list<FOdysseyVectorLoop*>& GetLoopList();
};
