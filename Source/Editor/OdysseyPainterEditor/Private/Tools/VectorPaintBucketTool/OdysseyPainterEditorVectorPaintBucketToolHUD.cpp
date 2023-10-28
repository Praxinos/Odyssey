#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPaintBucketToolHUD::~FOdysseyPainterEditorVectorPaintBucketToolHUD()
{
}

FOdysseyPainterEditorVectorPaintBucketToolHUD::FOdysseyPainterEditorVectorPaintBucketToolHUD( UOdysseyPainterEditorVectorPaintBucketTool* iPaintBucketTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPaintBucketTool )
    , mPaintBucketTool( iPaintBucketTool )
{
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    mPickedCycleArray.clear();

    // Updates the selection box
    FOdysseyPainterEditorVectorBaseToolHUD::Reset( iScene );
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Load(FOdysseyVectorScene* iScene)
{
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Unload(FOdysseyVectorScene* iScene)
{
}

::ULIS::FVec2D
FOdysseyPainterEditorVectorPaintBucketToolHUD::GetBucketPosition( FOdysseyVectorBucket* iBucket
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
FOdysseyPainterEditorVectorPaintBucketToolHUD::GetRadialHandlePosition( FOdysseyVectorBucket* iBucket
                                                                , bool iWorld )
{
    ::ULIS::FVec2D radialHandleCoords = iBucket->GetCoords() + iBucket->GetRadialOffset();

    radialHandleCoords.x += iBucket->GetRadialRadius();

    if( iWorld )
    {
        FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
        BLMatrix2D& worldMatrix = ownerObject->GetWorldMatrix();
        BLPoint worldPosition = worldMatrix.mapPoint( radialHandleCoords.x, radialHandleCoords.y );

        return ::ULIS::FVec2D( worldPosition.x, worldPosition.y );
    }

    return radialHandleCoords;
}

::ULIS::FVec2D
FOdysseyPainterEditorVectorPaintBucketToolHUD::GetRadialPosition( FOdysseyVectorBucket* iBucket
                                                          , bool iWorld )
{
    ::ULIS::FVec2D radialCoords = iBucket->GetCoords() + iBucket->GetRadialOffset();

    if( iWorld )
    {
        FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
        BLMatrix2D& worldMatrix = ownerObject->GetWorldMatrix();
        BLPoint worldPosition = worldMatrix.mapPoint( radialCoords.x, radialCoords.y );

        return ::ULIS::FVec2D( worldPosition.x, worldPosition.y );
    }

    return radialCoords;
}

::ULIS::FVec2D
FOdysseyPainterEditorVectorPaintBucketToolHUD::GetHandleVector( FOdysseyVectorBucket* iBucket
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
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickBucketArea( FOdysseyVectorBucket* iBucket
                                                       , double iWorldX
                                                       , double iWorldY )
{
    ::ULIS::FVec2D bucketWorldCoords = GetBucketPosition( iBucket, true );

    if( mPaintBucketTool->GetShowControls() )
    {
        if( iBucket->GetColorMode() == eBucketColorMode::LinearGradient )
        {
            ::ULIS::FVec2D handleWorldCoords = bucketWorldCoords + ( GetHandleVector( iBucket, true ) * HANDLE_DISTANCE );
            ::ULIS::FVec2D pickDif = ::ULIS::FVec2D( iWorldX - handleWorldCoords.x
                                                   , iWorldY - handleWorldCoords.y );

            if ( pickDif.Distance() < mPaintBucketTool->PickingRadius )
            {
                return PICK_HANDLE;
            }
        }

        if( iBucket->GetColorMode() == eBucketColorMode::RadialGradient )
        {
            ::ULIS::FVec2D radialHandleWorldCoords = GetRadialHandlePosition( iBucket, true );
            ::ULIS::FVec2D radialHandleDif = ::ULIS::FVec2D( iWorldX - radialHandleWorldCoords.x
                                                           , iWorldY - radialHandleWorldCoords.y );
            ::ULIS::FVec2D radialWorldCoords = GetRadialPosition( iBucket, true );
            ::ULIS::FVec2D radialDif = ::ULIS::FVec2D( iWorldX - radialWorldCoords.x
                                                     , iWorldY - radialWorldCoords.y );

            if ( radialHandleDif.Distance() < mPaintBucketTool->PickingRadius )
            {
                return PICK_RADIAL_HANDLE;
            }

            if ( radialDif.Distance() < RADIAL_AREA_RADIUS )
            {
                return PICK_RADIAL_AREA;
            }
        }
    }
    else
    {
        ::ULIS::FVec2D pickDif = ::ULIS::FVec2D( iWorldX - bucketWorldCoords.x
                                               , iWorldY - bucketWorldCoords.y );

        if ( pickDif.Distance() < mPaintBucketTool->PickingRadius )
        {
            return PICK_BUCKET;
        }
    }

    return PICK_NONE;
}

//static
void
FOdysseyPainterEditorVectorPaintBucketToolHUD::RecursivePickCycles( FOdysseyVectorObject* iObject
                                                            , double iWorldX
                                                            , double iWorldY
                                                            , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray )
{
    std::list<FOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();

    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);
        FOdysseyVectorCycle* pickedCycle = paintGroup->PickCycle( iWorldX, iWorldY );

        if( pickedCycle )
        {
            oPickedCycleArray.push_back( pickedCycle );
        }
    }

    for( FOdysseyVectorObject* child : childrenList )
    {
        RecursivePickCycles( child, iWorldX, iWorldY, oPickedCycleArray );
    }
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickCycles( FOdysseyVectorScene* iScene
                                                   , double iWorldX
                                                   , double iWorldY
                                                   , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray )
{
    std::list<FOdysseyVectorObject*>& focusedObjectList = mPaintBucketTool->GetFocusedObjectList( iScene );

    oPickedCycleArray.clear();

    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
        RecursivePickCycles( focusedObject, iWorldX, iWorldY, oPickedCycleArray );
    }
}

FOdysseyVectorBucket*
FOdysseyPainterEditorVectorPaintBucketToolHUD::RecursivePickBucket( FOdysseyVectorObject* iObject
                                                            , double iWorldX
                                                            , double iWorldY )
{
    std::list<FOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();
    FOdysseyVectorBucket *pickedBucket = nullptr;

    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);
        std::list<FOdysseyVectorBucket*>& bucketList = paintGroup->GetBucketList();

        for( FOdysseyVectorBucket *bucket : bucketList )
        {
            if( PickBucketArea( bucket, iWorldX, iWorldY ) )
            {
                return bucket;
            }
        }
    }

    for( FOdysseyVectorObject *child : childrenList )
    {
        pickedBucket = RecursivePickBucket( child, iWorldX, iWorldY );

        if( pickedBucket )
            break;
    }

    return pickedBucket;
}

FOdysseyVectorBucket*
FOdysseyPainterEditorVectorPaintBucketToolHUD::PickBucket( FOdysseyVectorScene* iScene
                                                   , double iWorldX
                                                   , double iWorldY )
{
    std::list<FOdysseyVectorObject*>& focusedObjectList = mPaintBucketTool->GetFocusedObjectList( iScene );
    FOdysseyVectorBucket* pickedBucket = nullptr;

    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
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
FOdysseyPainterEditorVectorPaintBucketToolHUD::DrawBucket( BLContext* iBLContext
                                                   , FOdysseyVectorBucket* iBucket
                                                   , BLRgba32 fgColor
                                                   , BLRgba32 bgColor
                                                   , BLRgba32 hcColor )
{
    ::ULIS::FVec2D bucketWorldCoords = GetBucketPosition( iBucket, true );
    FColor bucketColor = iBucket->GetColor();
    BLRgba32 fillColor = BLRgba32( bucketColor.R, bucketColor.G, bucketColor.B, bucketColor.A );
    BLRgba32 propColor = iBucket->IsPropagated() ? BLRgba32( 0x00, 0xFF, 0x00, 0xFF )
                                                 : BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );

    iBLContext->save();
    iBLContext->resetMatrix();

    if( mPaintBucketTool->GetShowControls() )
    {
        if( iBucket->GetColorMode() == eBucketColorMode::LinearGradient )
        {
            ::ULIS::FVec2D handleWorldCoords = bucketWorldCoords + ( GetHandleVector( iBucket, true ) * HANDLE_DISTANCE );
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

        if( iBucket->GetColorMode() == eBucketColorMode::RadialGradient )
        {
            ::ULIS::FVec2D radialWorldCoords = GetRadialPosition( iBucket, true );
            ::ULIS::FVec2D radialHandleWorldCoords = GetRadialHandlePosition( iBucket, true );
            double radialRadius = ( radialHandleWorldCoords - radialWorldCoords ).Distance();

            BLRgba32 blackColor = BLRgba32( 0x00, 0x00, 0x00, 0xFF );
            BLRgba32 whiteColor = BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );

            // Bucket-to-radial line
            iBLContext->setStrokeWidth( 2.0f );
            iBLContext->setStrokeStyle( blackColor );
            iBLContext->strokeLine( bucketWorldCoords.x, bucketWorldCoords.y
                                  , radialWorldCoords.x, radialWorldCoords.y );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( whiteColor );
            iBLContext->strokeLine( bucketWorldCoords.x, bucketWorldCoords.y
                                  , radialWorldCoords.x, radialWorldCoords.y );

            // Radial Circle
            iBLContext->setStrokeWidth( 2.0f );
            iBLContext->setStrokeStyle( blackColor );
            iBLContext->strokeCircle( radialWorldCoords.x, radialWorldCoords.y, RADIAL_AREA_RADIUS );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( whiteColor );
            iBLContext->strokeCircle( radialWorldCoords.x, radialWorldCoords.y, RADIAL_AREA_RADIUS );

            // Radial-to-RadialHandle line
            iBLContext->setStrokeWidth( 2.0f );
            iBLContext->setStrokeStyle( blackColor );
            iBLContext->strokeLine( radialWorldCoords.x, radialWorldCoords.y
                                  , radialHandleWorldCoords.x, radialHandleWorldCoords.y );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( whiteColor );
            iBLContext->strokeLine( radialWorldCoords.x, radialWorldCoords.y
                                  , radialHandleWorldCoords.x, radialHandleWorldCoords.y );

            // RadialHandle Circle
            iBLContext->setStrokeWidth( 2.0f );
            iBLContext->setStrokeStyle( blackColor );
            iBLContext->strokeCircle( radialWorldCoords.x, radialWorldCoords.y, radialRadius );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( whiteColor );
            iBLContext->strokeCircle( radialWorldCoords.x, radialWorldCoords.y, radialRadius );

            // RadialHandle
            iBLContext->setFillStyle( whiteColor );
            iBLContext->fillCircle( radialHandleWorldCoords.x, radialHandleWorldCoords.y, HANDLE_RADIUS );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( blackColor );
            iBLContext->strokeCircle( radialHandleWorldCoords.x, radialHandleWorldCoords.y, HANDLE_RADIUS );
        }
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
FOdysseyPainterEditorVectorPaintBucketToolHUD::RecursiveDrawObject( BLContext* iBLContext
                                                            , FOdysseyVectorObject* iObject
                                                            , BLRgba32 fgColor
                                                            , BLRgba32 bgColor
                                                            , BLRgba32 hcColor )
{
    std::list<FOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();

    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);
        std::list<FOdysseyVectorBucket*>& bucketList = paintGroup->GetBucketList();
        BLMatrix2D& worldMatrix = paintGroup->GetWorldMatrix();

        iBLContext->setMatrix( worldMatrix );

        for( FOdysseyVectorBucket *bucket : bucketList )
        {
            DrawBucket( iBLContext, bucket, fgColor, bgColor, hcColor );
        }
    }

    for( FOdysseyVectorObject* child : childrenList )
    {
        RecursiveDrawObject( iBLContext, child, fgColor, bgColor, hcColor );
    }
}

// must be set at eache mouse hover event for memory safety issues.
void
FOdysseyPainterEditorVectorPaintBucketToolHUD::SetPickedCycles( std::vector<FOdysseyVectorCycle*>& pickedCycleArray )
{
    mPickedCycleArray = pickedCycleArray;
}

void
FOdysseyPainterEditorVectorPaintBucketToolHUD::Draw( BLContext* iBLContext
                                                   , FOdysseyVectorScene* iScene
                                                   , uint64 iDrawingFlags )
{
    std::list<FOdysseyVectorObject*>& focusedObjectList = mPaintBucketTool->GetFocusedObjectList( iScene );
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    // Draw scene in object or vertex mode
    FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene, iDrawingFlags );

    iBLContext->save();
    iBLContext->resetMatrix();

    for( int i = 0; i < mPickedCycleArray.size(); i++ )
    {
        FOdysseyVectorCycle* cycle = mPickedCycleArray[i];
        FOdysseyVectorObject* owner = cycle->GetOwner();
        BLMatrix2D& worldMatrix = owner->GetWorldMatrix();

        iBLContext->setMatrix( worldMatrix );

        iBLContext->setCompOp( BL_COMP_OP_SRC_OVER );
        iBLContext->setStrokeStyle( bgColor );
        iBLContext->setStrokeWidth( 4.0f );

        cycle->StrokePath( iBLContext, true );

        iBLContext->setStrokeStyle( hcColor );
        iBLContext->setStrokeWidth( 3.0f );

        cycle->StrokePath( iBLContext, true );
    }

    iBLContext->restore();

    iBLContext->save();
    iBLContext->resetMatrix();

    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
        RecursiveDrawObject( iBLContext, focusedObject, fgColor, bgColor, hcColor );
    }

    iBLContext->restore();
}
