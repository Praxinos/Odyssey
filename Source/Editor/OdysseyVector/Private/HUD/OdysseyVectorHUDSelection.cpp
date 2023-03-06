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
    mSelectionBox.space = nullptr;
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
FOdysseyVectorHUDSelection::DrawSelectionSpace( UOdysseyVectorRoot& iScene,::ULIS::FRectD& iRoi,uint64 iFlags )
{
    BLContext* blctx = iScene.GetEngine()->GetBLContext();
    BLPoint topLeft = { 0, 0 };

    blctx->save();
    blctx->resetMatrix();

    if( iScene.GetEngine()->GetSelectionSpace() )
    {
        UOdysseyVectorGroup* selectionSpace = iScene.GetEngine()->GetSelectionSpace();
        ::ULIS::FRectD selectionSpaceBBox = selectionSpace->GetBBox( false );
        BLRgba32 strokeColor = { 0x80, 0x80, 0x80, 0xFF };
        BLMatrix2D& worldMatrix = selectionSpace->GetWorldMatrix();

        iScene.GetEngine()->UseImage( mSelectionMask );

        blctx->clearAll();
        blctx->setFillStyle( BLRgba32( 0x80808080 ) );
        blctx->fillRect( 0, 0, mSelectionMask->width(), mSelectionMask->height() );

        blctx->setMatrix( worldMatrix );
        blctx->setCompOp( BL_COMP_OP_SRC_COPY );
        blctx->setFillStyle( BLRgba32( 0x800000FF ) );
        blctx->fillRect( selectionSpaceBBox.x, selectionSpaceBBox.y, selectionSpaceBBox.w, selectionSpaceBBox.h );

        iScene.GetEngine()->UseColorImage();

        blctx->blitImage( topLeft, *mSelectionMask );
    }

    blctx->restore();
}

void
FOdysseyVectorHUDSelection::UpdateSelectionBox( UOdysseyVectorRoot& iScene )
{
    std::list<UOdysseyVectorObject*>& selectedObjectList = iScene.GetSelectedObjectList();

    mSelectionBox.space = nullptr;

    if( selectedObjectList.size() )
    {
        if( selectedObjectList.size() == 1 )
        {
            UOdysseyVectorObject* selectedObject = iScene.GetLastSelected();
            BLMatrix2D& worldMatrix = selectedObject->GetWorldMatrix();

            mSelectionBox.space = selectedObject;
            mSelectionBox.rect = selectedObject->GetBBox( false );
        }

        if( selectedObjectList.size() > 1 )
        {
            BLPoint p0;
            BLPoint p1;

            mSelectionBox.space = &iScene;
            mSelectionBox.rect = UOdysseyVectorObject::GetBoundingBoxFromList( selectedObjectList );
            p0 = mSelectionBox.space->GetInverseWorldMatrix().mapPoint( mSelectionBox.rect.x, mSelectionBox.rect.y );
            p1 = mSelectionBox.space->GetInverseWorldMatrix().mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w
                                                                      , mSelectionBox.rect.y + mSelectionBox.rect.h );

            mSelectionBox.rect = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min( p0.x, p1.x )
                                                           , ::ULIS::FMath::Min( p0.y, p1.y )
                                                           , ::ULIS::FMath::Max( p0.x, p1.x )
                                                           , ::ULIS::FMath::Max( p0.y, p1.y ) );
        }
    }
}

void
FOdysseyVectorHUDSelection::DrawSelectionBox( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    std::list<UOdysseyVectorObject*>& selectedObjectList = iScene.GetSelectedObjectList();
    BLContext* blctx = iScene.GetEngine()->GetBLContext();

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();

    blctx->setStrokeStyle( BLRgba32( 0xFF0000FF ) );
    blctx->setStrokeWidth( 1.0f );

    if( mSelectionBox.space )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.space->GetWorldMatrix();
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

        blctx->strokePath( path );
    }

    blctx->restore();
}

void
FOdysseyVectorHUDSelection::DrawSelecting( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = iScene.GetEngine()->GetBLContext();
    BLPath path;

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();

    blctx->setStrokeStyle( BLRgba32( 0xFF0000FF ) );
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
FOdysseyVectorHUDSelection::Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = iScene.GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };

    DrawSelectionSpace( iScene, iRoi, iFlags );

    if( mSelecting )
    {
        DrawSelecting( iScene, iRoi, iFlags );
    }
    else
    {
        DrawSelectionBox( iScene, iRoi, iFlags );
    }
}
