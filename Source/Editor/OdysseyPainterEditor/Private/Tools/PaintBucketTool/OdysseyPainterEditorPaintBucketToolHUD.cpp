#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorPaintBucketToolHUD::~FOdysseyPainterEditorPaintBucketToolHUD()
{
}

FOdysseyPainterEditorPaintBucketToolHUD::FOdysseyPainterEditorPaintBucketToolHUD( UOdysseyPainterEditorPaintBucketTool* iPaintBucketTool )
    : mPaintBucketTool( iPaintBucketTool )
    , mCycle ( nullptr )
{
}

void
FOdysseyPainterEditorPaintBucketToolHUD::Reset( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorPaintBucketToolHUD::Load(FOdysseyVectorScene* iScene)
{
}

void
FOdysseyPainterEditorPaintBucketToolHUD::SetCycle( FOdysseyVectorCycle* iCycle )
{
    mCycle = iCycle;
}

::ULIS::FVec2D
FOdysseyPainterEditorPaintBucketToolHUD::GetBucketPosition( FOdysseyVectorBucket* iBucket
                                                          , bool iWorld )
{
    ::ULIS::FVec2D& bucketCoords = iBucket->GetCoords();

    if( iWorld )
    {
        FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
        BLMatrix2D& worldMatrix = ownerObject->GetWorldMatrix();
        BLPoint worldPosition = worldMatrix.mapPoint( bucketCoords.x, bucketCoords.y );

        return ::ULIS::FVec2D( worldPosition.x, worldPosition.y );
    }

    return bucketCoords;
}

::ULIS::FVec2D
FOdysseyPainterEditorPaintBucketToolHUD::GetHandleVector( FOdysseyVectorBucket* iBucket
                                                        , bool iWorld )
{
    double a = iBucket->GetRotation();
    ::ULIS::FVec2D handleVector = ::ULIS::FVec2D( cos( a ), sin( a ) );

    if( iWorld )
    {
        FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
        BLMatrix2D& worldMatrix = ownerObject->GetWorldMatrix();
        BLPoint worldVector = worldMatrix.mapVector( handleVector.x, handleVector.y );
        ::ULIS::FVec2D normalizedVector = ::ULIS::FVec2D( worldVector.x, worldVector.y );

        normalizedVector.Normalize();

        return normalizedVector;
    }

    return handleVector;
}

uint32
FOdysseyPainterEditorPaintBucketToolHUD::PickBucketArea( FOdysseyVectorBucket* iBucket
                                                       , double iWorldX
                                                       , double iWorldY )
{
    ::ULIS::FVec2D bucketWorldCoords = GetBucketPosition( iBucket, true );
    ::ULIS::FVec2D handleWorldCoords = bucketWorldCoords + ( GetHandleVector( iBucket, true ) * HANDLE_DISTANCE );
    ::ULIS::FVec2D bucketDif = ::ULIS::FVec2D( iWorldX - bucketWorldCoords.x
                                             , iWorldY - bucketWorldCoords.y );
    ::ULIS::FVec2D handleDif = ::ULIS::FVec2D( iWorldX - handleWorldCoords.x
                                             , iWorldY - handleWorldCoords.y );

    if ( bucketDif.Distance() < mPaintBucketTool->PickingRadius )
    {
        return PICK_BUCKET;
    }

    if( mPaintBucketTool->ShowControls && ( iBucket->GetColorMode() == eBucketColorMode::LinearGradient )  )
    {
        if ( handleDif.Distance() < mPaintBucketTool->PickingRadius )
        {
            return PICK_HANDLE;
        }
    }

    return PICK_NONE;
}

//static
void
FOdysseyPainterEditorPaintBucketToolHUD::RecursivePickCycles( FOdysseyVectorObject* iObject
                                                            , double iWorldX
                                                            , double iWorldY
                                                            , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray )
{
    std::list<FOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();
    std::list<FOdysseyVectorObject*>::iterator it;

    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);
        FOdysseyVectorCycle* pickedCycle = paintGroup->PickCycle( iWorldX, iWorldY );

        if( pickedCycle )
        {
            oPickedCycleArray.push_back( pickedCycle );
        }
    }

    for( it = childrenList.begin(); it != childrenList.end(); ++it )
    {
        FOdysseyVectorObject* child = (*it);

        RecursivePickCycles( child, iWorldX, iWorldY, oPickedCycleArray );
    }
}

void
FOdysseyPainterEditorPaintBucketToolHUD::PickCycles( FOdysseyVectorScene* iScene
                                                   , double iWorldX
                                                   , double iWorldY
                                                   , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray )
{
    std::list<FOdysseyVectorObject*>& focusedObjectList = mPaintBucketTool->GetFocusedObjectList( iScene );
    std::list<FOdysseyVectorObject*>::iterator oit;

    oPickedCycleArray.clear();

    for( oit = focusedObjectList.begin(); oit != focusedObjectList.end(); ++oit )
    {
        FOdysseyVectorObject* focusedObject = *oit;

        RecursivePickCycles( focusedObject, iWorldX, iWorldY, oPickedCycleArray );
    }
}

FOdysseyVectorBucket*
FOdysseyPainterEditorPaintBucketToolHUD::RecursivePickBucket( FOdysseyVectorObject* iObject
                                                            , double iWorldX
                                                            , double iWorldY )
{
    std::list<FOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();
    std::list<FOdysseyVectorObject*>::iterator oit;
    FOdysseyVectorBucket *pickedBucket = nullptr;

    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);
        std::list<FOdysseyVectorBucket*>& bucketList = paintGroup->GetBucketList();
        std::list<FOdysseyVectorBucket*>::iterator bit;

        for( bit = bucketList.begin(); bit != bucketList.end(); ++bit )
        {
            FOdysseyVectorBucket *bucket = (*bit);

            if( PickBucketArea( bucket, iWorldX, iWorldY ) )
            {
                return bucket;
            }
        }
    }

    for( oit = childrenList.begin(); oit != childrenList.end(); ++oit )
    {
        FOdysseyVectorObject *child = (*oit);

        pickedBucket = RecursivePickBucket( child, iWorldX, iWorldY );

        if( pickedBucket )
            break;
    }

    return pickedBucket;
}

FOdysseyVectorBucket*
FOdysseyPainterEditorPaintBucketToolHUD::PickBucket( FOdysseyVectorScene* iScene
                                                   , double iWorldX
                                                   , double iWorldY )
{
    std::list<FOdysseyVectorObject*>& focusedObjectList = mPaintBucketTool->GetFocusedObjectList( iScene );
    std::list<FOdysseyVectorObject*>::iterator oit;
    FOdysseyVectorBucket* pickedBucket = nullptr;

    for( oit = focusedObjectList.begin(); oit != focusedObjectList.end(); ++oit )
    {
        FOdysseyVectorObject* focusedObject = *oit;

        pickedBucket = RecursivePickBucket( focusedObject, iWorldX, iWorldY );

        if( pickedBucket )
        {
            return pickedBucket;
        }
    }

    return nullptr;
}

/*
FOdysseyVectorHandleBucket*
FOdysseyVectorGroupPaint::PickBucketHandle( double iX, double iY )
{
    for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
    {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);

        if( bucket->PickHandle( iX, iY ) )
        {
            return bucket->GetHandle();
        }
    }

    return nullptr;
}


uint32
FOdysseyVectorBucket::Pick( double iWorldX, double iWorldY )
{
    BLPoint bucketWorldCoord = mParent.GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );
    ::ULIS::FVec2D pt = ::ULIS::FVec2D( iWorldX, iWorldY );
    ::ULIS::FRectD bucketRect;
    ::ULIS::FRectD crossRect;
    ::ULIS::FRectD propagateRect;

    MakeRects( bucketWorldCoord.x, bucketWorldCoord.y, bucketRect, crossRect, propagateRect );

    if( bucketRect.HitTest( pt ) )
    {
        return FOdysseyVectorBucket::PICKBUCKET;
    }

    if( crossRect.HitTest( pt ) )
    {
        return FOdysseyVectorBucket::PICKCROSS;
    }

    if( propagateRect.HitTest( pt ) )
    {
        return FOdysseyVectorBucket::PICKPROPAGATED;
    }

    if ( PickHandle( iWorldX, iWorldY ) )
    {
        return FOdysseyVectorBucket::PICKHANDLE;
    }

    return FOdysseyVectorBucket::PICKNONE;

}


FOdysseyVectorHandleBucket*
FOdysseyVectorBucket::PickHandle( double iWorldX, double iWorldY )
{
    BLPoint bucketWorldCoord = mParent.GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );
    BLPoint handleWorldCoord = mParent.GetWorldMatrix().mapPoint( mCoords.x + mCtrlPoint.GetX()
                                                                , mCoords.y + mCtrlPoint.GetY() );
    double difX = iWorldX - handleWorldCoord.x;
    double difY = iWorldY - handleWorldCoord.y;
    double distance = sqrt( ( difX * difX ) + ( difY * difY ) );

    if( distance < HANDLERADIUS )
    {
        return &mCtrlPoint;
    }

    return nullptr;
}
*/
void
FOdysseyPainterEditorPaintBucketToolHUD::DrawBucket( FOdysseyVectorBucket* iBucket
                                                   , BLContext* iBLContext
                                                   , BLRgba32 fgColor
                                                   , BLRgba32 bgColor
                                                   , BLRgba32 hcColor )
{
    ::ULIS::FVec2D bucketWorldCoords = GetBucketPosition( iBucket, true );
    ::ULIS::FVec2D handleWorldCoords = bucketWorldCoords + ( GetHandleVector( iBucket, true ) * HANDLE_DISTANCE );

    FColor bucketColor = iBucket->GetColor();
    BLRgba32 fillColor = BLRgba32( bucketColor.R, bucketColor.G, bucketColor.B, bucketColor.A );
    BLRgba32 propColor = iBucket->IsPropagated() ? BLRgba32( 0x00, 0xFF, 0x00, 0xFF )
                                                 : BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );

    iBLContext->save();
    iBLContext->resetMatrix();

    if( mPaintBucketTool->ShowControls && ( iBucket->GetColorMode() == eBucketColorMode::LinearGradient ) )
    {
        BLRgba32 blackColor = BLRgba32( 0x00, 0x00, 0x00, 0xFF );
        BLRgba32 whiteColor = BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );

        // Bucket-to-handle line
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->setStrokeStyle( blackColor );
        iBLContext->strokeLine( bucketWorldCoords.x, bucketWorldCoords.y
                              , handleWorldCoords.x, handleWorldCoords.y );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->setStrokeStyle( whiteColor );
        iBLContext->strokeLine( bucketWorldCoords.x, bucketWorldCoords.y
                              , handleWorldCoords.x, handleWorldCoords.y );

        // Handle
        iBLContext->setFillStyle( whiteColor );
        iBLContext->fillCircle( handleWorldCoords.x, handleWorldCoords.y, HANDLE_RADIUS );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->setStrokeStyle( blackColor );
        iBLContext->strokeCircle( handleWorldCoords.x, handleWorldCoords.y, HANDLE_RADIUS );
    }

    // Bucket
    iBLContext->setFillStyle( fillColor );
    iBLContext->fillCircle( bucketWorldCoords.x, bucketWorldCoords.y, PELLET_RADIUS );

    iBLContext->setStrokeWidth( 2.0f );
    iBLContext->setStrokeStyle( BLRgba32( 0x00, 0x00, 0x00, 0xFF ) );
    iBLContext->strokeCircle( bucketWorldCoords.x, bucketWorldCoords.y, PELLET_RADIUS );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->setStrokeStyle( propColor ); // green if propagated, white otherwise
    iBLContext->strokeCircle( bucketWorldCoords.x, bucketWorldCoords.y, PELLET_RADIUS );

    iBLContext->restore();
}

// static
void
FOdysseyPainterEditorPaintBucketToolHUD::RecursiveDrawObject( FOdysseyVectorObject* iObject
                                                            , BLContext* iBLContext
                                                            , BLRgba32 fgColor
                                                            , BLRgba32 bgColor
                                                            , BLRgba32 hcColor )
{
    std::list<FOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();

    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);
        std::list<FOdysseyVectorBucket*>& bucketList = paintGroup->GetBucketList();
        std::list<FOdysseyVectorBucket*>::iterator it;
        BLMatrix2D& worldMatrix = paintGroup->GetWorldMatrix();

        iBLContext->setMatrix( worldMatrix );

        for( it = bucketList.begin(); it != bucketList.end(); ++it )
        {
            FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*it);

            DrawBucket( bucket, iBLContext, fgColor, bgColor, hcColor );
        }
    }

    for( FOdysseyVectorObject* child : childrenList )
    {
        RecursiveDrawObject( child, iBLContext, fgColor, bgColor, hcColor );
    }
}

void
FOdysseyPainterEditorPaintBucketToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    std::list<FOdysseyVectorObject*>& focusedObjectList = mPaintBucketTool->GetFocusedObjectList( iScene );
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    std::vector<FOdysseyVectorCycle*>& pickedCycleArray = mPaintBucketTool->GetPickedCycleArray();

    blctx->save();
    blctx->resetMatrix();

    for( int i = 0; i < pickedCycleArray.size(); i++ )
    {
        FOdysseyVectorCycle* cycle = pickedCycleArray[i];
        FOdysseyVectorObject* owner = cycle->GetOwner();
        BLMatrix2D& worldMatrix = owner->GetWorldMatrix();

        blctx->setMatrix( worldMatrix );

        blctx->setCompOp( BL_COMP_OP_SRC_OVER );
        blctx->setStrokeStyle( bgColor );
        blctx->setStrokeWidth( 4.0f );

        cycle->StrokePath( true );

        blctx->setStrokeStyle( hcColor );
        blctx->setStrokeWidth( 3.0f );

        cycle->StrokePath( true );
    }

    blctx->restore();

    blctx->save();
    blctx->resetMatrix();

    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
        RecursiveDrawObject( focusedObject, blctx, fgColor, bgColor, hcColor );
    }

    blctx->restore();
}
