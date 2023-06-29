#include "HUD/OdysseyVectorHUDSelection.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDSelection::~FOdysseyVectorHUDSelection()
{
}

FOdysseyVectorHUDSelection::FOdysseyVectorHUDSelection()
    : mPointArray(nullptr)
    , mSelecting(true)
    , mSelectionMask ( nullptr )
{
}

FSelectionBox&
FOdysseyVectorHUDSelection::GetSelectionBox()
{
    return mSelectionBox;
}

void
FOdysseyVectorHUDSelection::Init( uint32 iWidth, uint32 iHeight )
{
    if( mSelectionMask )
    {
        delete mSelectionMask;
    }

    mSelectionMask = new BLImage( iWidth, iHeight, BL_FORMAT_A8 );
}

void
FOdysseyVectorHUDSelection::SetSelecting( bool iSelecting, std::vector<::ULIS::FVec2D>* iPointArray )
{
    mSelecting = iSelecting;
    mPointArray = iPointArray;
}

void
FOdysseyVectorHUDSelection::DrawSelectionSpace( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    BLPoint topLeft = { 0, 0 };

    blctx->save();
    blctx->resetMatrix();

    if( iScene->GetEngine()->GetSelectionSpace() )
    {
        FOdysseyVectorGroup* selectionSpace = iScene->GetEngine()->GetSelectionSpace();
        ::ULIS::FRectD selectionSpaceBBox = selectionSpace->GetBBox( false );
        BLRgba32 strokeColor = { 0x80, 0x80, 0x80, 0xFF };
        BLMatrix2D& worldMatrix = selectionSpace->GetWorldMatrix();

        iScene->GetEngine()->UseImage( mSelectionMask );

        blctx->setCompOp( BL_COMP_OP_SRC_COPY );

        blctx->clearAll();
        blctx->setFillStyle( BLRgba32( 0x80FFFFFF ) );
        blctx->fillRect( 0, 0, mSelectionMask->width(), mSelectionMask->height() );

        blctx->setMatrix( worldMatrix );
        blctx->setFillStyle( BLRgba32( 0x00000000/*0x800000FF*/ ) );
        blctx->fillRect( selectionSpaceBBox.x, selectionSpaceBBox.y, selectionSpaceBBox.w, selectionSpaceBBox.h );

        iScene->GetEngine()->UseColorImage();

        blctx->blitImage( topLeft, *mSelectionMask );
    }

    blctx->restore();
}

void
FOdysseyVectorHUDSelection::Reset( FOdysseyVectorScene* iScene )
{
    UpdateSelectionBox( iScene, false );
}

void
FOdysseyVectorHUDSelection::UpdateSelectionBox( FOdysseyVectorScene* iScene, bool iForceWorld )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );

    if( selectedObjectList.size() )
    {
        if( ( selectedObjectList.size() == 1 ) && ( iForceWorld == false ) )
        {
            FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

            mSelectionBox.rect = selectedObject->GetBBox( false );

            mSelectionBox.worldMatrix = selectedObject->GetWorldMatrix();
            mSelectionBox.inverseWorldMatrix = selectedObject->GetInverseWorldMatrix();
        }
        else
        {
            BLPoint p0, p1, p2, p3;
            ::ULIS::FRectD rect = FOdysseyVectorObject::GetBoundingBoxFromList( selectedObjectList );
            ::ULIS::FVec2D origin = ::ULIS::FVec2D( rect.x + (rect.w * 0.5f)
                                                  , rect.y + (rect.h * 0.5f) );
            mSelectionBox.worldMatrix.reset();
            mSelectionBox.worldMatrix.translate( origin.x, origin.y );

            BLMatrix2D::invert( mSelectionBox.inverseWorldMatrix, mSelectionBox.worldMatrix );

            p0 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x         , rect.y          );
            p1 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x + rect.w, rect.y          );
            p2 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x + rect.w, rect.y + rect.h );
            p3 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x         , rect.y + rect.h );

            mSelectionBox.rect = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                           , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                           , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                           , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );
        }
    }
}

void
FOdysseyVectorHUDSelection::DrawSelectionBox( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();

    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.worldMatrix;
        BLPoint point[4] = { worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y + mSelectionBox.rect.h )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y + mSelectionBox.rect.h ) };
        BLPath path;

        path.moveTo( point[0] );
        path.lineTo( point[1] );
        path.lineTo( point[2] );
        path.lineTo( point[3] );
        path.close();

        blctx->setStrokeStyle( bgColor );
        blctx->setStrokeWidth( 2.0f );
        blctx->strokePath( path );

        blctx->setStrokeStyle( fgColor );
        blctx->setStrokeWidth( 1.0f );
        blctx->strokePath( path );
    }

    blctx->restore();
}

void
FOdysseyVectorHUDSelection::DrawSelecting( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    BLPath path;

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();

    blctx->setStrokeStyle( hcColor );
    blctx->setStrokeWidth( 1.0f );

    if( mPointArray )
    {
        for( int i = 0; i < mPointArray->size(); i++ )
        {
            int n = ( i + 1 ) % mPointArray->size();

            path.moveTo( (*mPointArray)[i].x, (*mPointArray)[i].y );
            path.lineTo( (*mPointArray)[n].x, (*mPointArray)[n].y );
        }
    }

    blctx->strokePath( path );

    blctx->restore();
}

void
FOdysseyVectorHUDSelection::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };

    DrawSelectionSpace( iScene, iFlags );

    if( mSelecting )
    {
        DrawSelecting( iScene, iFlags );
    }
    else
    {
        DrawSelectionBox( iScene, iFlags );
    }
}
