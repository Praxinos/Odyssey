#include "HUD/OdysseyVectorHUDEraser.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDEraser::~FOdysseyVectorHUDEraser()
{
}

FOdysseyVectorHUDEraser::FOdysseyVectorHUDEraser( std::vector<::ULIS::FVec2D>& iPointArray )
{
}

void
FOdysseyVectorHUDEraser::Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = iScene.GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };
    BLPath path;

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();

    blctx->restore();
}
