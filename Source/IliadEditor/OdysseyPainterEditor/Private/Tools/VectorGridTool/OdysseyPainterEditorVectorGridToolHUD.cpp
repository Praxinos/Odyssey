// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridToolHUD.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorGroupPaint.h"

FOdysseyPainterEditorVectorGridToolHUD::~FOdysseyPainterEditorVectorGridToolHUD()
{
}

FOdysseyPainterEditorVectorGridToolHUD::FOdysseyPainterEditorVectorGridToolHUD( UOdysseyPainterEditorVectorGridTool* iGridTool )
    : FOdysseyPainterEditorVectorSelectionToolHUD( iGridTool )
{
    mGridTool = iGridTool;
}

FGridNode::~FGridNode()
{
}

FGridNode::FGridNode()
    : FOdysseyVectorPoint( 0.0f, 0.0f )
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
        for( FGridCell& cell : mCellArray )
        {
            DeformCell( cell );
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
        //::ULIS::FVec2D p1p2t = p1 + ( p1p2 * iCell.mPointArray[i].t );
        //::ULIS::FVec2D p0p3t = p0 + ( p0p3 * iCell.mPointArray[i].t );
        ::ULIS::FVec2D newCoords;
        BLPoint worldPt;
        BLPoint objectPt;
        //double isx;

        //FOdysseyVector::IntersectSegment( p0p1s, p3p2s, p1p2t, p0p3t, &isx, nullptr );
        //isxCoords = p0p1s + ( sLine * isx );

        newCoords = p0p1s + ( ( p3p2s - p0p1s ) * iCell.mPointArray[i].t );

        worldPt = mSelectionBox.worldMatrix.mapPoint( newCoords.x, newCoords.y );
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
FOdysseyPainterEditorVectorGridToolHUD::DrawSelectionRectangle( BLContext* iBLContext
                                                              , BLRgba32& iHighlightColor )
{
    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( iHighlightColor );
    iBLContext->setStrokeWidth( 1.0f );

    if( mWorldSelDrag != mWorldSelStart )
    {
        if( mSelectionBox.rect.Area() )
        {
            double xmin = ::ULIS::FMath::Min( mWorldSelDrag.x, mWorldSelStart.x );
            double ymin = ::ULIS::FMath::Min( mWorldSelDrag.y, mWorldSelStart.y );
            double xmax = ::ULIS::FMath::Max( mWorldSelDrag.x, mWorldSelStart.x );
            double ymax = ::ULIS::FMath::Max( mWorldSelDrag.y, mWorldSelStart.y );

            iBLContext->strokeLine( xmin, ymin, xmax, ymin );
            iBLContext->strokeLine( xmax, ymin, xmax, ymax );
            iBLContext->strokeLine( xmax, ymax, xmin, ymax );
            iBLContext->strokeLine( xmin, ymax, xmin, ymin );
        }
    }

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorGridToolHUD::Reset( FOdysseyVectorGroupPaint* iScene )
{
    MakeGrid( iScene, mGridTool->GetEditor()->GetVectorHUDFlags() );
}

void
FOdysseyPainterEditorVectorGridToolHUD::Draw( BLContext* iBLContext
                                            , FOdysseyVectorGroupPaint* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 hudFlags = mGridTool->GetEditor()->GetVectorHUDFlags();

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    iBLContext->save();
    iBLContext->resetMatrix();

    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D worldMatrix = mSelectionBox.worldMatrix;

        DrawSelectionRectangle( iBLContext, hcColor );

        for( int i = 0; i < mCellArray.size(); i++ )
        {
            BLPoint pt[4] = { worldMatrix.mapPoint( mCellArray[i].mNode[0]->GetX(), mCellArray[i].mNode[0]->GetY() ),
                              worldMatrix.mapPoint( mCellArray[i].mNode[1]->GetX(), mCellArray[i].mNode[1]->GetY() ),
                              worldMatrix.mapPoint( mCellArray[i].mNode[2]->GetX(), mCellArray[i].mNode[2]->GetY() ),
                              worldMatrix.mapPoint( mCellArray[i].mNode[3]->GetX(), mCellArray[i].mNode[3]->GetY() ) };

            iBLContext->setStrokeWidth( 2.0f );
            iBLContext->setStrokeStyle( bgColor );
            iBLContext->strokePolygon( pt, 4 );
            iBLContext->setStrokeStyle( fgColor );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->strokePolygon( pt, 4 );
        }

        for( int i = 0; i < mNodeArray.size(); i++ )
        {
            BLPoint pt = worldMatrix.mapPoint( mNodeArray[i].GetX(), mNodeArray[i].GetY() );

            iBLContext->setFillStyle( mNodeArray[i].IsSelected() ? hcColor : fgColor );
            iBLContext->fillCircle( pt.x, pt.y, FOdysseyPainterEditorVectorGridToolHUD::HANDLE_RADIUS );
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( bgColor );
            iBLContext->strokeCircle( pt.x, pt.y, FOdysseyPainterEditorVectorGridToolHUD::HANDLE_RADIUS );
        }
    }

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorGridToolHUD::Export( std::vector<FOdysseyVectorPoint*>& oPointArray )
{
    uint32 count = 0;

    if( mSelectionBox.rect.Area() ) // else, mPointCount is undefined
    {
        oPointArray.resize( mPointCount );

        for( uint32 i = 0; i < mGridTool->DivisionsY * mGridTool->DivisionsX; i++ )
        {
            for( uint32 j = 0; j < mCellArray[i].mPointArray.size(); j++ )
            {
                oPointArray[count++] = mCellArray[i].mPointArray[j].mPoint;
            }
        }
    }
}

uint32
FOdysseyPainterEditorVectorGridToolHUD::MapPoint( FOdysseyVectorObject* iObject
                                                , FOdysseyVectorPoint* iPoint
                                                , double iSpaceX
                                                , double iSpaceY )
{
    double paramX = iSpaceX / mSelectionBox.rect.w;
    double paramY = iSpaceY / mSelectionBox.rect.h;

    if( ( paramX >= 0.0f ) && ( paramX < 1.0f ) && ( paramY >= 0.0f ) && ( paramY < 1.0f ) )
    {
        uint32 rowid = paramX * mGridTool->DivisionsX;
        uint32 colid = paramY * mGridTool->DivisionsY;
        uint32 offset = ( colid * mGridTool->DivisionsX ) + rowid;
        double s = ( iSpaceX - (double) rowid * mCellSizeX ) / mCellSizeX;
        double t = ( iSpaceY - (double) colid * mCellSizeY ) / mCellSizeY;
        FGridPoint gridPoint = { iObject, iPoint, s, t };

        mCellArray[offset].mPointArray.push_back( gridPoint );

        return 1;
    }

    return 0;
}

void
FOdysseyPainterEditorVectorGridToolHUD::Map( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    mPointCount = 0;

    vectorEngine->Traverse
    ( iScene
    , 0
    , [ this
      , iScene
      , vectorEngine ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, travesalFlags ) )
          {
              BLMatrix2D& inverseSpaceMatrix = mSelectionBox.inverseWorldMatrix;

              if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
              {
                  FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

                  mPointCount += MapPaintGroupBuckets( paintGroup, inverseSpaceMatrix );
              }

              if( object->GetClass() == FOdysseyVectorPath::StaticClass() )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                  mPointCount += MapPath( path, inverseSpaceMatrix );
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );
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

    for( FOdysseyVectorVertex* vertex : vertexList )
    {
        BLPoint pt = conversionMatrix.mapPoint( vertex->GetX(), vertex->GetY() );
        double spaceX = pt.x - mSelectionBox.rect.x;
        double spaceY = pt.y - mSelectionBox.rect.y;

        pointCount += MapPoint( iPath, vertex, spaceX, spaceY );
    }

    for( FOdysseyVectorSegment* segment : segmentList )
    {
        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
            FOdysseyVectorPoint* point[2] = { cubicSegment->GetHandle(0), cubicSegment->GetHandle(1) };

            for( int i = 0; i < 2; i++ )
            {
                BLPoint pt = conversionMatrix.mapPoint( point[i]->GetX(), point[i]->GetY() );
                double spaceX = pt.x - mSelectionBox.rect.x; // Hi again, Elon :) !
                double spaceY = pt.y - mSelectionBox.rect.y;

                pointCount += MapPoint( iPath, point[i], spaceX, spaceY );
            }
        }
    }

    return pointCount;
}

uint32
FOdysseyPainterEditorVectorGridToolHUD::MapPaintGroupBuckets( FOdysseyVectorGroupPaint* iPaintGroup
                                                            , BLMatrix2D& iInverseGridMatrix )
{
    std::list<FOdysseyVectorBucket*>& bucketList = iPaintGroup->GetBucketList();
    BLMatrix2D conversionMatrix = iInverseGridMatrix;
    uint32 pointCount = 0;

    conversionMatrix.transform( iPaintGroup->GetWorldMatrix() );

    // map buckets
    for( FOdysseyVectorBucket* bucket : bucketList )
    {
        BLPoint pt = conversionMatrix.mapPoint( bucket->GetX(), bucket->GetY() );
        double spaceX = pt.x - mSelectionBox.rect.x;
        double spaceY = pt.y - mSelectionBox.rect.y;

        pointCount += MapPoint( iPaintGroup, bucket, spaceX, spaceY );
    }

    return pointCount;
}

void
FOdysseyPainterEditorVectorGridToolHUD::MakeNodes()
{
    double y = mSelectionBox.rect.y;

    mNodeArray.clear();

    mCellSizeX = mSelectionBox.rect.w / mGridTool->DivisionsX;
    mCellSizeY = mSelectionBox.rect.h / mGridTool->DivisionsY;

    mNodeCountX = mGridTool->DivisionsX + 1;
    mNodeCountY = mGridTool->DivisionsY + 1;

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
FOdysseyPainterEditorVectorGridToolHUD::MakeCells()
{
    mCellArray.clear();

    mCellArray.resize( mGridTool->DivisionsX * mGridTool->DivisionsY );

    for( uint32 i = 0; i < mGridTool->DivisionsY; i++ )
    {
        uint32 n = i + 1;

        for( uint32 j = 0; j < mGridTool->DivisionsX; j++ )
        {
            uint32 k = j + 1;
            uint32 node0idx = ( i * mNodeCountX ) + j;
            uint32 node1idx = ( i * mNodeCountX ) + k;
            uint32 node2idx = ( n * mNodeCountX ) + k;
            uint32 node3idx = ( n * mNodeCountX ) + j;
            uint32 offset = ( i * mGridTool->DivisionsX ) + j;

            mCellArray[offset].mNode[0] = &mNodeArray[node0idx];
            mCellArray[offset].mNode[1] = &mNodeArray[node1idx];
            mCellArray[offset].mNode[2] = &mNodeArray[node2idx];
            mCellArray[offset].mNode[3] = &mNodeArray[node3idx];
        }
    }
}

void
FOdysseyPainterEditorVectorGridToolHUD::MakeGrid( FOdysseyVectorGroupPaint* iScene, uint64 iHUDFlags )
{
    // Updates the selection box
    UpdateSelectionBox( iScene, mGridTool->World, iHUDFlags );

    if( mSelectionBox.rect.Area() )
    {
        // Make the box a little bit bigger in order to have coordinates > 0.0f and < 1.0f.
        mSelectionBox.rect.x -= ( mSelectionBox.rect.w * 0.01f );
        mSelectionBox.rect.y -= ( mSelectionBox.rect.h * 0.01f );
        mSelectionBox.rect.w += ( mSelectionBox.rect.w * 0.02f );
        mSelectionBox.rect.h += ( mSelectionBox.rect.h * 0.02f );

        MakeNodes();
        MakeCells();

        Map( iScene );
    }
}
