#include "HUD/OdysseyVectorHUDSelection.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDSelection::~FOdysseyVectorHUDSelection()
{
}

FOdysseyVectorHUDSelection::FOdysseyVectorHUDSelection( std::vector<::ULIS::FVec2D>& iPointArray )
    : mPointArray(iPointArray)
{
}

void
FOdysseyVectorHUDSelection::Draw( UOdysseyVectorObject* iObject, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = iObject->GetRoot()->GetEngine()->GetBLContext();
    BLPath path;

    blctx->setStrokeStyle( BLRgba32( 0xFF0000FF ) );
    blctx->setStrokeWidth( 1.0f );

    for( int i = 0; i < mPointArray.size(); i++ )
    {
        int n = ( i + 1 ) % mPointArray.size();

        path.moveTo( mPointArray[i].x, mPointArray[i].y );
        path.lineTo( mPointArray[n].x, mPointArray[n].y );
    }

    blctx->strokePath( path );
}
