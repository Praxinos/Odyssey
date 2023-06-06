#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDPathCubic : public FOdysseyVectorHUD
{
    private:
        uint32 mDisplayMode;
        void DrawSegment( FOdysseyVectorPathCubic* iPath
                        , FOdysseyVectorSegmentCubic* iCubicSegment
                        , uint64 iFlags );
        void DrawVertex( FOdysseyVectorPathCubic* iPath
                       , FOdysseyVectorVertex* iCubicVertex
                       , uint64 iFlags );
    public:
        static const uint32 VIEW_PATH           = ( 1 << 0 );
        static const uint32 VIEW_POINT          = ( 1 << 1 );
        static const uint32 VIEW_HANDLE_POINT   = ( 1 << 2 );
        static const uint32 VIEW_HANDLE_SEGMENT = ( 1 << 3 );

        ~FOdysseyVectorHUDPathCubic();
        FOdysseyVectorHUDPathCubic();
        FOdysseyVectorHUDPathCubic( uint32 iDisplayMode );
        void SetDisplayMode( uint32 iDisplayMode );
        uint32 GetDisplayMode();
        void Draw( FOdysseyVectorScene* iScene, uint64 iFlags );
};
