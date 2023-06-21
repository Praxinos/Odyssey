#include "HUD/OdysseyVectorHUDRotate.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDRotate::~FOdysseyVectorHUDRotate()
{
}

FOdysseyVectorHUDRotate::FOdysseyVectorHUDRotate()
    : FOdysseyVectorHUDSelection()
    , mShowBox( true )
{

}

bool
FOdysseyVectorHUDRotate::PickPivot( double iWorldX, double iWorldY )
{
    if( mSelectionBox.space )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.space->GetWorldMatrix();
        BLPoint pivot = worldMatrix.mapPoint( mPivot.x, mPivot.y );
        ::ULIS::FVec2D dist = ::ULIS::FVec2D( iWorldX - pivot.x, iWorldY - pivot.y );
        
        if( dist.Distance() <= FOdysseyVectorHUDRotate::PIVOT_RADIUS )
        {
            return true;
        }
    }

    return false;
}

void
FOdysseyVectorHUDRotate::SetShowBox( bool iShowBox )
{
    mShowBox = iShowBox;
}

::ULIS::FVec2D&
FOdysseyVectorHUDRotate::GetPivot()
{
    return mPivot;
}

void
FOdysseyVectorHUDRotate::SetPivot( double iLocalX, double iLocalY )
{
    mPivot.x = iLocalX;
    mPivot.y = iLocalY;
}

void
FOdysseyVectorHUDRotate::Reset(FOdysseyVectorScene* iScene)
{
    FOdysseyVectorHUDSelection::Reset( iScene );
}

void
FOdysseyVectorHUDRotate::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };

    if( mShowBox )
    {
        DrawSelectionBox( iScene, iFlags );
    }

    if( mSelectionBox.space )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.space->GetWorldMatrix();
        BLPoint pivot = worldMatrix.mapPoint( mPivot.x, mPivot.y );

        // matrix might get altered for displaying the selection rectangle of a single object. Save it.
        blctx->save();
        blctx->resetMatrix();

        blctx->setFillStyle( BLRgba32( 0xFFD0E040 ) );
        blctx->fillCircle( pivot.x, pivot.y, FOdysseyVectorHUDRotate::PIVOT_RADIUS );

        blctx->setStrokeWidth( 1.0f );
        blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
        blctx->strokeCircle( pivot.x, pivot.y, FOdysseyVectorHUDRotate::PIVOT_RADIUS );

        blctx->restore();
    }
}
