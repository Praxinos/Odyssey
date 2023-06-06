#include "HUD/OdysseyVectorHUDScale.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDScale::~FOdysseyVectorHUDScale()
{
}

FOdysseyVectorHUDScale::FOdysseyVectorHUDScale()
    : FOdysseyVectorHUDSelection()
{

}

int32
FOdysseyVectorHUDScale::Pick( double iWorldX, double iWorldY )
{
    if( mSelectionBox.space )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.space->GetWorldMatrix();
        BLPoint point[4] = { worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y + mSelectionBox.rect.h )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y + mSelectionBox.rect.h ) };

        // pick the handles
        for ( int i = 0; i < 4; i++ )
        {
            ::ULIS::FVec2D vec = { iWorldX - point[i].x, iWorldY - point[i].y };
 
            if ( vec.Distance() <= FOdysseyVectorHUDScale::HANDLE_RADIUS )
            {
                return i;
            }
        }
    }

    return -1;
}

void
FOdysseyVectorHUDScale::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };

    DrawSelectionBox( iScene, iFlags );

    if( mSelectionBox.space )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.space->GetWorldMatrix();
        BLPoint point[4] = { worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y + mSelectionBox.rect.h )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y + mSelectionBox.rect.h ) };

        // matrix might get altered for displaying the selection rectangle of a single object. Save it.
        blctx->save();
        blctx->resetMatrix();

        blctx->setFillStyle( BLRgba32( 0xFF0000FF ) );

        blctx->fillCircle( point[0].x, point[0].y, FOdysseyVectorHUDScale::HANDLE_RADIUS );
        blctx->fillCircle( point[1].x, point[1].y, FOdysseyVectorHUDScale::HANDLE_RADIUS );
        blctx->fillCircle( point[2].x, point[2].y, FOdysseyVectorHUDScale::HANDLE_RADIUS );
        blctx->fillCircle( point[3].x, point[3].y, FOdysseyVectorHUDScale::HANDLE_RADIUS );

        blctx->restore();
    }
}
