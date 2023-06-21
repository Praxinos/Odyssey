#include "Tools/VectorObjectMoveTool/OdysseyPainterEditorVectorObjectMoveToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorObjectMoveToolHUD::~FOdysseyPainterEditorVectorObjectMoveToolHUD()
{
}

FOdysseyPainterEditorVectorObjectMoveToolHUD::FOdysseyPainterEditorVectorObjectMoveToolHUD( UOdysseyPainterEditorVectorObjectMoveTool* iObjectMoveTool )
    : mObjectMoveTool( iObjectMoveTool )
    , mGizmoFlags( 0 )
{
}

void
FOdysseyPainterEditorVectorObjectMoveToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    UpdateSelectionBox( iScene, mObjectMoveTool->World );

    UpdateGizmo();
}

uint32
FOdysseyPainterEditorVectorObjectMoveToolHUD::GetGizmoFlags()
{
    return mGizmoFlags;
}

void
FOdysseyPainterEditorVectorObjectMoveToolHUD::PickGizmo( double iWorldX
                                                       , double iWorldY )
{
    FSelectionBox& selectionBox = GetSelectionBox();
    uint32 newFlags = 0;

    mGizmoFlags &= (~PICK_CHANGED);

    if( selectionBox.space )
    {
        ::ULIS::FVec2D toPivot = ::ULIS::FVec2D( mPivot.x - iWorldX
                                               , mPivot.y - iWorldY );
        double distToXAxis = DBL_MAX;
        double distToYAxis = DBL_MAX;

        if( toPivot.Distance() < mObjectMoveTool->Radius )
        {
            newFlags |= PICK_XAXIS;
            newFlags |= PICK_YAXIS;
        }

        FOdysseyVector::DistanceToSegment( ::ULIS::FVec2D( iWorldX, iWorldY )
                                         , mPivot
                                         , ::ULIS::FVec2D( mPivot.x + mXAxis.x
                                                         , mPivot.y + mXAxis.y )
                                         , distToXAxis );

        if( distToXAxis < mObjectMoveTool->Radius )
        {
            newFlags |= PICK_XAXIS;
        }


        FOdysseyVector::DistanceToSegment( ::ULIS::FVec2D( iWorldX, iWorldY )
                                         , mPivot
                                         , ::ULIS::FVec2D( mPivot.x + mYAxis.x
                                                         , mPivot.y + mYAxis.y )
                                         , distToYAxis );

        if( distToYAxis < mObjectMoveTool->Radius )
        {
            newFlags |= PICK_YAXIS;
        }
    }

    mGizmoFlags = newFlags | ( ( mGizmoFlags != newFlags ) ? PICK_CHANGED : 0 );
}

void
FOdysseyPainterEditorVectorObjectMoveToolHUD::UpdateGizmo()
{
    FSelectionBox& selectionBox = GetSelectionBox();

    if( selectionBox.space )
    {
        BLMatrix2D& worldMatrix = selectionBox.space->GetWorldMatrix();
        BLPoint origin = worldMatrix.mapPoint( selectionBox.rect.x + selectionBox.rect.w * 0.5f
                                             , selectionBox.rect.y + selectionBox.rect.h * 0.5f );
        BLPoint xVector = worldMatrix.mapVector( 1.0f, 0.0f );
        BLPoint yVector = worldMatrix.mapVector( 0.0f, 1.0f );

        mXAxis = ::ULIS::FVec2D( xVector.x, xVector.y );
        mYAxis = ::ULIS::FVec2D( yVector.x, yVector.y );
        mPivot = ::ULIS::FVec2D( origin.x , origin.y  );

        if( mXAxis.DistanceSquared() )
        {
            mXAxis.Normalize();

            mXAxis = mXAxis * 80.0f;
        }

        if( mYAxis.DistanceSquared() )
        {
            mYAxis.Normalize();

            mYAxis = mYAxis * 80.0f;
        }
    }
}

void
FOdysseyPainterEditorVectorObjectMoveToolHUD::DrawGizmo( FOdysseyVectorScene* iScene )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    BLRgba32 xAxisColor = ( mGizmoFlags & PICK_XAXIS ) ? BLRgba32( 0xFF0000FF )  // teal (ABGR)
                                                       : BLRgba32( 0xFFD0E040 );
    BLRgba32 yAxisColor = ( mGizmoFlags & PICK_YAXIS ) ? BLRgba32( 0xFF0000FF )  // teal (ABGR)
                                                       : BLRgba32( 0xFFD0E040 );

    blctx->save();
    blctx->resetMatrix();

    blctx->setStrokeWidth( 2.0f );
    blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
    blctx->strokeLine( mPivot.x, mPivot.y, mPivot.x + mXAxis.x, mPivot.y + mXAxis.y );
    blctx->strokeLine( mPivot.x, mPivot.y, mPivot.x + mYAxis.x, mPivot.y + mYAxis.y );

    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle( xAxisColor );
    blctx->strokeLine( mPivot.x, mPivot.y, mPivot.x + mXAxis.x, mPivot.y + mXAxis.y );
    blctx->setStrokeStyle( yAxisColor );
    blctx->strokeLine( mPivot.x, mPivot.y, mPivot.x + mYAxis.x, mPivot.y + mYAxis.y );

    blctx->restore();
}

void
FOdysseyPainterEditorVectorObjectMoveToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();

    DrawGizmo( iScene );
}

uint32
FOdysseyPainterEditorVectorObjectMoveToolHUD::SetCursorPosition( double iX, double iY )
{
    mX = iX;
    mY = iY;

    PickGizmo( mX, mY );

    return mGizmoFlags;
}
