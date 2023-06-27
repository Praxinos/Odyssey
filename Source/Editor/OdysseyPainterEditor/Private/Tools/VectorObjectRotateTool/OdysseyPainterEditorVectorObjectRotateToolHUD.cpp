#include "Tools/VectorObjectRotateTool/OdysseyPainterEditorVectorObjectRotateToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorObjectRotateToolHUD::~FOdysseyPainterEditorVectorObjectRotateToolHUD()
{
}

FOdysseyPainterEditorVectorObjectRotateToolHUD::FOdysseyPainterEditorVectorObjectRotateToolHUD( UOdysseyPainterEditorVectorObjectRotateTool* iObjectRotateTool )
    : mObjectRotateTool( iObjectRotateTool )
{
/*
    BLFontFace face;
   // TODO: do something depending on to the O.S
    BLResult err = face.createFromFile("C:/Windows/Fonts/lucon.ttf"); // Lucida console
 
    mFont.createFromFace( face, 16.0f );
*/
}

bool
FOdysseyPainterEditorVectorObjectRotateToolHUD::PickPivot( double iWorldX, double iWorldY )
{
    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.worldMatrix;
        BLPoint pivot = worldMatrix.mapPoint( mPivot.x, mPivot.y );
        ::ULIS::FVec2D dist = ::ULIS::FVec2D( iWorldX - pivot.x, iWorldY - pivot.y );
        
        if( dist.Distance() <= PIVOT_RADIUS )
        {
            return true;
        }
    }

    return false;
}

::ULIS::FVec2D&
FOdysseyPainterEditorVectorObjectRotateToolHUD::GetPivot()
{
    return mPivot;
}

void
FOdysseyPainterEditorVectorObjectRotateToolHUD::SetPivot( double iLocalX, double iLocalY )
{
    mPivot.x = iLocalX;
    mPivot.y = iLocalY;
}

void
FOdysseyPainterEditorVectorObjectRotateToolHUD::Reset(FOdysseyVectorScene* iScene)
{
    FOdysseyVectorHUDSelection::Reset( iScene ); // Updates the selection box

    SetPivot( mSelectionBox.rect.x + ( mSelectionBox.rect.w * 0.5f )
            , mSelectionBox.rect.y + ( mSelectionBox.rect.h * 0.5f ) );
}

void
FOdysseyPainterEditorVectorObjectRotateToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };

    if( mObjectRotateTool->IsRotating() == false )
    {
        DrawSelectionBox( iScene, iFlags );
    }


    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.worldMatrix;
        BLPoint pivot = worldMatrix.mapPoint( mPivot.x, mPivot.y );
        //char str[255];

        // matrix might get altered for displaying the selection rectangle of a single object. Save it.
        blctx->save();
        blctx->resetMatrix();

        blctx->setFillStyle( BLRgba32( 0x80D0E040 ) ); // teal
        blctx->fillCircle( pivot.x, pivot.y, PIVOT_RADIUS );

        blctx->setStrokeWidth( 1.0f );
        blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
        blctx->strokeCircle( pivot.x, pivot.y, PIVOT_RADIUS );

        // Angle
/*
        snprintf( str, 255, "%.2f", mObjectRotateTool->GetCumulAngle() );

        blctx->fillUtf8Text( BLPoint( pivot.x + PIVOT_RADIUS, pivot.y ), mFont, str );
*/
        blctx->restore();
    }
}

