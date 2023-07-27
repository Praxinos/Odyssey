#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorGridToolHUD::~FOdysseyPainterEditorVectorGridToolHUD()
{
}

FOdysseyPainterEditorVectorGridToolHUD::FOdysseyPainterEditorVectorGridToolHUD( UOdysseyPainterEditorVectorGridTool* iGridTool )
    : FOdysseyPainterEditorVectorPickToolHUD( iGridTool )
{
    mGridTool = iGridTool;
}

FGridNode::~FGridNode()
{
}

FGridNode::FGridNode()
    : FOdysseyVectorPoint( 0.0f, 0.0f, 0.0f )
    , mSelected( false )
{
}

void
FGridNode::SetSelected( bool iSelected )
{
    mSelected = iSelected;
}

bool
FGridNode::IsSelected()
{
   return mSelected;
}

void
FOdysseyPainterEditorVectorGridToolHUD::Deform()
{
    if( mSelectionBox.rect.Area() )
    {
        for( int i = 0; i < mCellArray.size(); i++ )
        {
            DeformCell( mCellArray[i] );
        }
    }
}

void
FOdysseyPainterEditorVectorGridToolHUD::DeformCell( FGridCell& iCell )
{
    ::ULIS::FVec2D& p0 = iCell.mNode[0]->GetCoords();
    ::ULIS::FVec2D& p1 = iCell.mNode[1]->GetCoords();
    ::ULIS::FVec2D& p2 = iCell.mNode[2]->GetCoords();
    ::ULIS::FVec2D& p3 = iCell.mNode[3]->GetCoords();
    ::ULIS::FVec2D p0p1 = p1 - p0;
    ::ULIS::FVec2D p1p2 = p2 - p1;
    ::ULIS::FVec2D p3p2 = p2 - p3; // same way as p0p1
    ::ULIS::FVec2D p0p3 = p3 - p0; // same way as p1p2

    for( int i = 0; i < iCell.mPointArray.size(); i++ )
    {
        ::ULIS::FVec2D p0p1s = p0 + ( p0p1 * iCell.mPointArray[i].s );
        ::ULIS::FVec2D p3p2s = p3 + ( p3p2 * iCell.mPointArray[i].s );
        ::ULIS::FVec2D sLine = ( p3p2s - p0p1s );
        ::ULIS::FVec2D p1p2t = p1 + ( p1p2 * iCell.mPointArray[i].t );
        ::ULIS::FVec2D p0p3t = p0 + ( p0p3 * iCell.mPointArray[i].t );
        ::ULIS::FVec2D isxCoords;
        BLPoint worldPt;
        BLPoint objectPt;
        double isx;

        FOdysseyVector::IntersectSegment( p0p1s, p3p2s, p1p2t, p0p3t, &isx, nullptr );

        isxCoords = p0p1s + ( sLine * isx );

        worldPt = mSelectionBox.worldMatrix.mapPoint( isxCoords.x, isxCoords.y );
        objectPt = iCell.mPointArray[i].mDeformedObject->GetInverseWorldMatrix().mapPoint( worldPt.x, worldPt.y );

        iCell.mPointArray[i].mPoint->Set( objectPt.x, objectPt.y );
    }
}

void
FOdysseyPainterEditorVectorGridToolHUD::StartSelectionRectangle( double iWorldX, double iWorldY )
{
    mWorldSelStart.x = iWorldX;
    mWorldSelStart.y = iWorldY;
    mWorldSelDrag.x  = iWorldX;
    mWorldSelDrag.y  = iWorldY;
}

void
FOdysseyPainterEditorVectorGridToolHUD::DragSelectionRectangle( double iWorldX, double iWorldY )
{
    mWorldSelDrag.x = iWorldX;
    mWorldSelDrag.y = iWorldY;
}

void
FOdysseyPainterEditorVectorGridToolHUD::EndSelectionRectangle( bool iClearSelection )
{
    double xmin = ::ULIS::FMath::Min( mWorldSelDrag.x, mWorldSelStart.x );
    double ymin = ::ULIS::FMath::Min( mWorldSelDrag.y, mWorldSelStart.y );
    double xmax = ::ULIS::FMath::Max( mWorldSelDrag.x, mWorldSelStart.x );
    double ymax = ::ULIS::FMath::Max( mWorldSelDrag.y, mWorldSelStart.y );
    ::ULIS::FRectD worldRect = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );

    mWorldSelDrag.x = mWorldSelStart.x = 0.0f;
    mWorldSelDrag.y = mWorldSelStart.y = 0.0f;

    PickNodes( worldRect, iClearSelection );
}

void
FOdysseyPainterEditorVectorGridToolHUD::GetSelection( std::vector<FGridNode*>& oNodeArray )
{
    oNodeArray.clear();

    for( int i = 0; i < mNodeArray.size(); i++ )
    {
        if( mNodeArray[i].IsSelected() )
        {
            oNodeArray.push_back( &mNodeArray[i] );
        }
    }
}

void
FOdysseyPainterEditorVectorGridToolHUD::ClearSelection()
{
    for( int i = 0; i < mNodeArray.size(); i++ )
    {
        mNodeArray[i].SetSelected( false );
    }
}

bool
FOdysseyPainterEditorVectorGridToolHUD::PickNodes( double iWorldX
                                                 , double iWorldY
                                                 , double iWorldRadius
                                                 , bool iClearSelection )
{
    bool picked = false;

    if( iClearSelection )
    {
        ClearSelection();
    }

    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D worldMatrix = mSelectionBox.worldMatrix;

        for( int i = 0; i < mNodeArray.size(); i++ )
        {
            FGridNode* node = &mNodeArray[i];
            BLPoint pt = worldMatrix.mapPoint( node->GetX(), node->GetY() );
            ::ULIS::FVec2D vec = ::ULIS::FVec2D( iWorldX - pt.x, iWorldY - pt.y );

            if ( vec.Distance() <= iWorldRadius )
            {
                node->SetSelected( true );

                picked = true;
            }
        }
    }

    return picked;
}

bool
FOdysseyPainterEditorVectorGridToolHUD::PickNodes( ::ULIS::FRectD& iWorldRect, bool iClearSelection )
{
    bool picked = false;

    if( iClearSelection )
    {
        ClearSelection();
    }

    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D worldMatrix = mSelectionBox.worldMatrix;

        for( int i = 0; i < mNodeArray.size(); i++ )
        {
            FGridNode* node = &mNodeArray[i];
            BLPoint pt = worldMatrix.mapPoint( node->GetX(), node->GetY() );
            ::ULIS::FVec2D coords = ::ULIS::FVec2D( pt.x, pt.y );

            if ( iWorldRect.HitTest( coords ) )
            {
                node->SetSelected( true );

                picked = true;
            }
        }
    }

    return picked;
}

void
FOdysseyPainterEditorVectorGridToolHUD::DrawSelectionRectangle( FOdysseyVectorScene* iScene, uint64 iFlags  )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    blctx->save();
    blctx->resetMatrix();

    blctx->setStrokeStyle( BLRgba32( hc.R, hc.G, hc.B, hc.A ) );
    blctx->setStrokeWidth( 1.0f );

    if( mWorldSelDrag != mWorldSelStart )
    {
        if( mSelectionBox.rect.Area() )
        {
            double xmin = ::ULIS::FMath::Min( mWorldSelDrag.x, mWorldSelStart.x );
            double ymin = ::ULIS::FMath::Min( mWorldSelDrag.y, mWorldSelStart.y );
            double xmax = ::ULIS::FMath::Max( mWorldSelDrag.x, mWorldSelStart.x );
            double ymax = ::ULIS::FMath::Max( mWorldSelDrag.y, mWorldSelStart.y );

            blctx->strokeLine( xmin, ymin, xmax, ymin );
            blctx->strokeLine( xmax, ymin, xmax, ymax );
            blctx->strokeLine( xmax, ymax, xmin, ymax );
            blctx->strokeLine( xmin, ymax, xmin, ymin );
        }
    }

    blctx->restore();
}

void
FOdysseyPainterEditorVectorGridToolHUD::Reset(FOdysseyVectorScene* iScene)
{
    MakeGrid( iScene, mCellCountX, mCellCountY );
}

void
FOdysseyPainterEditorVectorGridToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    blctx->save();
    blctx->resetMatrix();

    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D worldMatrix = mSelectionBox.worldMatrix;

        DrawSelectionRectangle( iScene, iFlags );

        for( int i = 0; i < mCellArray.size(); i++ )
        {
            BLPoint pt[4] = { worldMatrix.mapPoint( mCellArray[i].mNode[0]->GetX(), mCellArray[i].mNode[0]->GetY() ),
                              worldMatrix.mapPoint( mCellArray[i].mNode[1]->GetX(), mCellArray[i].mNode[1]->GetY() ),
                              worldMatrix.mapPoint( mCellArray[i].mNode[2]->GetX(), mCellArray[i].mNode[2]->GetY() ),
                              worldMatrix.mapPoint( mCellArray[i].mNode[3]->GetX(), mCellArray[i].mNode[3]->GetY() ) };

            blctx->setStrokeWidth( 2.0f );
            blctx->setStrokeStyle( bgColor );
            blctx->strokePolygon( pt, 4 );
            blctx->setStrokeStyle( fgColor );
            blctx->setStrokeWidth( 1.0f );
            blctx->strokePolygon( pt, 4 );
        }

        for( int i = 0; i < mNodeArray.size(); i++ )
        {
            BLPoint pt = worldMatrix.mapPoint( mNodeArray[i].GetX(), mNodeArray[i].GetY() );

            blctx->setFillStyle( mNodeArray[i].IsSelected() ? hcColor : fgColor );
            blctx->fillCircle( pt.x, pt.y, FOdysseyPainterEditorVectorGridToolHUD::HANDLE_RADIUS );
            blctx->setStrokeWidth( 1.0f );
            blctx->setStrokeStyle( bgColor );
            blctx->strokeCircle( pt.x, pt.y, FOdysseyPainterEditorVectorGridToolHUD::HANDLE_RADIUS );
        }
    }

    blctx->restore();
}

void
FOdysseyPainterEditorVectorGridToolHUD::Export( std::vector<FOdysseyVectorPoint*>& oPointArray )
{
    uint32 count = 0;

    if( mSelectionBox.rect.Area() ) // else, mPointCount is undefined 
    {
        oPointArray.resize( mPointCount );

        for( uint32 i = 0; i < mCellCountY * mCellCountX; i++ )
        {
            for( uint32 j = 0; j < mCellArray[i].mPointArray.size(); j++ )
            {
                oPointArray[count++] = mCellArray[i].mPointArray[j].mPoint;
            }
        }
    }
}

uint32
FOdysseyPainterEditorVectorGridToolHUD::MapPoint( FOdysseyVectorObject* iObject, FOdysseyVectorPoint* iPoint, double iSpaceX, double iSpaceY )
{
    double paramX = iSpaceX / mSelectionBox.rect.w;
    double paramY = iSpaceY / mSelectionBox.rect.h;

    if( ( paramX >= 0.0f ) && ( paramX < 1.0f ) && ( paramY >= 0.0f ) && ( paramY < 1.0f ) )
    {
        uint32 rowid = paramX * mCellCountX;
        uint32 colid = paramY * mCellCountY;
        uint32 offset = ( colid * mCellCountX ) + rowid;
        double s = ( iSpaceX - (double) rowid * mCellSizeX ) / mCellSizeX;
        double t = ( iSpaceY - (double) colid * mCellSizeY ) / mCellSizeY;
        FGridPoint gridPoint = { iObject, iPoint, s, t };

        mCellArray[offset].mPointArray.push_back( gridPoint );

        return 1;
    }

    return 0;
}

void
FOdysseyPainterEditorVectorGridToolHUD::Map( FOdysseyVectorScene* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    mPointCount = 0;

    for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *obj = (*it);

        mPointCount += MapObjectRecurse( obj );
    }
}

uint32
FOdysseyPainterEditorVectorGridToolHUD::MapPath( FOdysseyVectorPath* iPath
                                               , BLMatrix2D& iInverseGridMatrix )
{
    std::list<FOdysseyVectorVertex*>& vertexList = iPath->GetVertexList();
    std::list<FOdysseyVectorSegment*>& segmentList = iPath->GetSegmentList();
    BLMatrix2D conversionMatrix = iInverseGridMatrix;
    uint32 pointCount = 0;

    conversionMatrix.transform( iPath->GetWorldMatrix() );

    for( std::list<FOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
    {
        FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>(*it);
        BLPoint pt = conversionMatrix.mapPoint( cubicVertex->GetX(), cubicVertex->GetY() );
        double spaceX = pt.x - mSelectionBox.rect.x;
        double spaceY = pt.y - mSelectionBox.rect.y;

        pointCount += MapPoint( iPath, cubicVertex, spaceX, spaceY );
    }

    for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        FOdysseyVectorPoint* point[2] = { cubicSegment->GetHandle(0), cubicSegment->GetHandle(1) };

        for( int i = 0; i < 2; i++ )
        {
            BLPoint pt = conversionMatrix.mapPoint( point[i]->GetX(), point[i]->GetY() );
            double spaceX = pt.x - mSelectionBox.rect.x; // Hi again, Elon :) !
            double spaceY = pt.y - mSelectionBox.rect.y;

            pointCount += MapPoint( iPath, point[i], spaceX, spaceY );
        }
    }

    return pointCount;
}

uint32
FOdysseyPainterEditorVectorGridToolHUD::MapPaintGroupBuckets( FOdysseyVectorGroupPaint* iPaintGroup
                                                            , BLMatrix2D& iInverseGridMatrix )
{
    std::list<FOdysseyVectorBucket*>& bucketList = iPaintGroup->GetBucketList();
    std::list<FOdysseyVectorBucket*>::iterator bit;
    BLMatrix2D conversionMatrix = iInverseGridMatrix;
    uint32 pointCount = 0;

    conversionMatrix.transform( iPaintGroup->GetWorldMatrix() );

    // map buckets
    for( bit = bucketList.begin(); bit != bucketList.end(); ++bit )
    {
        FOdysseyVectorBucket* bucket = *bit;
        BLPoint pt = conversionMatrix.mapPoint( bucket->GetX(), bucket->GetY() );
        double spaceX = pt.x - mSelectionBox.rect.x;
        double spaceY = pt.y - mSelectionBox.rect.y;

        pointCount += MapPoint( iPaintGroup, bucket, spaceX, spaceY );
    }

    return pointCount;
}

uint32
FOdysseyPainterEditorVectorGridToolHUD::MapPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup
                                                     , BLMatrix2D& iInverseGridMatrix )
{
    std::list<FOdysseyVectorObject*>& childrenList = iPaintGroup->GetChildrenList();
    std::list<FOdysseyVectorObject*>::iterator oit;
    BLMatrix2D conversionMatrix = iInverseGridMatrix;
    uint32 pointCount = 0;

    conversionMatrix.transform( iPaintGroup->GetWorldMatrix() );

    // map paths
    for( oit = childrenList.begin(); oit != childrenList.end(); ++oit )
    {
        FOdysseyVectorObject* child = *oit;

        if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);

            pointCount += MapPath( path, iInverseGridMatrix );
        }
    }
 
    pointCount += MapPaintGroupBuckets( iPaintGroup, iInverseGridMatrix );

    return pointCount;
}

uint32
FOdysseyPainterEditorVectorGridToolHUD::MapObjectRecurse( FOdysseyVectorObject* iObject )
{
    BLMatrix2D& inverseSpaceMatrix = mSelectionBox.inverseWorldMatrix;
    std::list<FOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();
    uint32 pointCount = 0;

    if( iObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);

        pointCount += MapPaintGroupBuckets( paintGroup, inverseSpaceMatrix );
    }

    if( iObject->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);

        pointCount += MapPath( path, inverseSpaceMatrix );
    }

    // Recurse
    for( std::list<FOdysseyVectorObject*>::iterator it = childrenList.begin(); it != childrenList.end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);

        pointCount += MapObjectRecurse( child );
    }

    return pointCount;
}

uint32
FOdysseyPainterEditorVectorGridToolHUD::MapObjectNoRecurse( FOdysseyVectorObject* iObject )
{
    BLMatrix2D& inverseSpaceMatrix = mSelectionBox.inverseWorldMatrix;
    std::list<FOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();
    uint32 pointCount = 0;

    if( iObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);

        pointCount += MapPaintGroup( paintGroup, inverseSpaceMatrix );
    }

    if( iObject->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);

        pointCount += MapPath( path, inverseSpaceMatrix );
    }

    return pointCount;
}

void
FOdysseyPainterEditorVectorGridToolHUD::MakeNodes( uint32 iCellCountX, uint32 iCellCountY  )
{
    double y = mSelectionBox.rect.y;

    mNodeArray.clear();

    mCellSizeX = mSelectionBox.rect.w / iCellCountX;
    mCellSizeY = mSelectionBox.rect.h / iCellCountY;

    mNodeCountX = iCellCountX + 1;
    mNodeCountY = iCellCountY + 1;

    mNodeArray.resize( mNodeCountX * mNodeCountY );

    for( uint32 i = 0; i < mNodeCountY; i++ )
    {
        double x = mSelectionBox.rect.x;

        for( uint32 j = 0; j < mNodeCountX; j++ )
        {
            uint32 offset = ( i * mNodeCountX ) + j;

            mNodeArray[offset].Set( x, y );

            x += mCellSizeX;
        }
        y += mCellSizeY;
    }
}

void
FOdysseyPainterEditorVectorGridToolHUD::MakeCells( uint32 iCellCountX, uint32 iCellCountY )
{
    mCellArray.clear();

    mCellCountX = iCellCountX;
    mCellCountY = iCellCountY;

    mCellArray.resize( mCellCountX * mCellCountY );

    for( uint32 i = 0; i < mCellCountY; i++ )
    {
        uint32 n = i + 1;

        for( uint32 j = 0; j < mCellCountX; j++ )
        {
            uint32 k = j + 1;
            uint32 node0idx = ( i * mNodeCountX ) + j;
            uint32 node1idx = ( i * mNodeCountX ) + k;
            uint32 node2idx = ( n * mNodeCountX ) + k;
            uint32 node3idx = ( n * mNodeCountX ) + j;
            uint32 offset = ( i * mCellCountX ) + j;

            mCellArray[offset].mNode[0] = &mNodeArray[node0idx];
            mCellArray[offset].mNode[1] = &mNodeArray[node1idx];
            mCellArray[offset].mNode[2] = &mNodeArray[node2idx];
            mCellArray[offset].mNode[3] = &mNodeArray[node3idx];
        }
    }
}

void
FOdysseyPainterEditorVectorGridToolHUD::MakeGrid( FOdysseyVectorScene* iScene, uint32 iCellCountX, uint32 iCellCountY )
{
    UpdateSelectionBox( iScene, false );

    if( mSelectionBox.rect.Area() )
    {
        // Make the box a little bit bigger in order to have coordinates > 0.0f and < 1.0f.
        mSelectionBox.rect.x -= ( mSelectionBox.rect.w * 0.01f );
        mSelectionBox.rect.y -= ( mSelectionBox.rect.h * 0.01f );
        mSelectionBox.rect.w += ( mSelectionBox.rect.w * 0.02f );
        mSelectionBox.rect.h += ( mSelectionBox.rect.h * 0.02f );

        MakeNodes( iCellCountX, iCellCountY );
        MakeCells( iCellCountX, iCellCountY );

        Map( iScene );
    }
}
