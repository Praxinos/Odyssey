#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorObject.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"

class ODYSSEYVECTOR_API FOdysseyVectorLoop
{
    private:
        void BuildSegmentCubic( UOdysseyVectorSegmentCubic& iSegment
                              , double iFromT
                              , double iToT );

    protected :
        BLPath mPath;
        UOdysseyVectorObject& mParent;

    public:
        ~FOdysseyVectorLoop();
         FOdysseyVectorLoop( UOdysseyVectorObject& iParent
                           , std::vector<UOdysseyVectorVertex*>& iVertexArray
                           , std::vector<FOdysseyVectorSection*>& iSectionArray );
        void Draw( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void Build( std::vector<UOdysseyVectorVertex*>& iVertexArray
                  , std::vector<FOdysseyVectorSection*>& iSectionArray );
};
