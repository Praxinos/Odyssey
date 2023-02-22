#include "HUD/OdysseyVectorHUDSelection.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDSelection::~FOdysseyVectorHUDSelection()
{
}

FOdysseyVectorHUDSelection::FOdysseyVectorHUDSelection( std::vector<::ULIS::FVec2D>& iPointArray )
    : mPointArray(iPointArray)
    , mSelecting(true)
{
}

void
FOdysseyVectorHUDSelection::SetSelecting( bool iSelecting )
{
    mSelecting = iSelecting;
}

void
FOdysseyVectorHUDSelection::Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = iScene.GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };


    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->setStrokeStyle( BLRgba32( 0xFF0000FF ) );
    blctx->setStrokeWidth( 1.0f );

    if( mSelecting )
    {
        BLPath path;

        for( int i = 0; i < mPointArray.size(); i++ )
        {
            int n = ( i + 1 ) % mPointArray.size();

            path.moveTo( mPointArray[i].x, mPointArray[i].y );
            path.lineTo( mPointArray[n].x, mPointArray[n].y );
        }

        blctx->strokePath( path );
    }
    else
    {
        std::list<UOdysseyVectorObject*>& selectedObjectList = iScene.GetSelectedObjectList();

        if( selectedObjectList.size() )
        {
            BLPath path;

            if( selectedObjectList.size() == 1 )
            {
                UOdysseyVectorObject* selectedObject = iScene.GetLastSelected();
                BLMatrix2D& worldMatrix = selectedObject->GetWorldMatrix();

                bbox = selectedObject->GetBBox( false );

                blctx->setMatrix( worldMatrix );
                path.moveTo( bbox.x         , bbox.y          );
                path.lineTo( bbox.x + bbox.w, bbox.y          );
                path.lineTo( bbox.x + bbox.w, bbox.y + bbox.h );
                path.lineTo( bbox.x         , bbox.y + bbox.h );
                path.lineTo( bbox.x         , bbox.y          );
            }

            if( selectedObjectList.size() > 1 )
            {
                bbox = UOdysseyVectorObject::GetBoundingBoxFromList( selectedObjectList );

                path.moveTo( bbox.x         , bbox.y          );
                path.lineTo( bbox.x + bbox.w, bbox.y          );
                path.lineTo( bbox.x + bbox.w, bbox.y + bbox.h );
                path.lineTo( bbox.x         , bbox.y + bbox.h );
                path.lineTo( bbox.x         , bbox.y          );
            }

            blctx->strokePath( path );
        }
    }

    if( iScene.GetEngine()->GetSelectionSpace() )
    {
        UOdysseyVectorGroup* selectionSpace = iScene.GetEngine()->GetSelectionSpace();
        ::ULIS::FRectD selectionSpaceBBox = selectionSpace->GetBBox( false );
        BLRgba32 strokeColor = { 0x80, 0x80, 0x80, 0xFF };
        BLMatrix2D& worldMatrix = selectionSpace->GetWorldMatrix();

        blctx->setMatrix( worldMatrix );
        blctx->setStrokeStyle( BLRgba32( strokeColor ) );
        blctx->setStrokeWidth( 1.0f );
        blctx->strokeRect( selectionSpaceBBox.x, selectionSpaceBBox.y, selectionSpaceBBox.w, selectionSpaceBBox.h );
    }

    blctx->restore();
}
