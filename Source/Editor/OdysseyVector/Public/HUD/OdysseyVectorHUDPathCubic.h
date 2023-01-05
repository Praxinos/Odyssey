#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDPathCubic : public FOdysseyVectorHUD
{
    private:
        uint32 mDisplayMode;
        void DrawSegment( UOdysseyVectorSegmentCubic* iCubicSegment
                        , ::ULIS::FVec2D& iFactor
                        , ::ULIS::FRectD& iRoi
                        , uint64 iFlags );
        void DrawVertex( UOdysseyVectorVertexCubic* iCubicVertex
                       , ::ULIS::FVec2D& iFactor
                       , ::ULIS::FRectD& iRoi
                       , uint64 iFlags );
    public:
        static const uint32 VIEW_PATH           = ( 1 << 0 );
        static const uint32 VIEW_POINT          = ( 1 << 1 );
        static const uint32 VIEW_HANDLE_POINT   = ( 1 << 2 );
        static const uint32 VIEW_HANDLE_SEGMENT = ( 1 << 3 );

        ~FOdysseyVectorHUDPathCubic();
        FOdysseyVectorHUDPathCubic();
        FOdysseyVectorHUDPathCubic( uint32 iDisplayMode );

        void Draw( UOdysseyVectorObject* iObject, ::ULIS::FRectD& iRoi, uint64 iFlags );
};
