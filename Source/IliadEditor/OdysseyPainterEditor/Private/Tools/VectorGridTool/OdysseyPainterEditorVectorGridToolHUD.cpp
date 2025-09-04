// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridToolHUD.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVector.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorGroupPaint.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorVectorGridToolHUD::~FOdysseyPainterEditorVectorGridToolHUD()
{
}

FOdysseyPainterEditorVectorGridToolHUD::FOdysseyPainterEditorVectorGridToolHUD( UOdysseyPainterEditorVectorGridTool* iGridTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iGridTool )
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
FOdysseyPainterEditorVectorGridToolHUD::EndSelectionRectangle( std::vector<FGridNode*>& oNodeArray )
{
    double xmin = ::ULIS::FMath::Min( mWorldSelDrag.x, mWorldSelStart.x );
    double ymin = ::ULIS::FMath::Min( mWorldSelDrag.y, mWorldSelStart.y );
    double xmax = ::ULIS::FMath::Max( mWorldSelDrag.x, mWorldSelStart.x );
    double ymax = ::ULIS::FMath::Max( mWorldSelDrag.y, mWorldSelStart.y );
    ::ULIS::FRectD worldRect = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );

    mWorldSelDrag.x = mWorldSelStart.x = 0.0f;
    mWorldSelDrag.y = mWorldSelStart.y = 0.0f;

    PickNodes( worldRect, oNodeArray );
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

void
FOdysseyPainterEditorVectorGridToolHUD::PickNodes( double iWorldX
                                                 , double iWorldY
                                                 , double iWorldRadius
                                                 , std::vector<FGridNode*>& oNodeArray )
{
    bool picked = false;

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
                oNodeArray.push_back( node );
            }
        }
    }
}

void
FOdysseyPainterEditorVectorGridToolHUD::PickNodes( ::ULIS::FRectD& iWorldRect
                                                  , std::vector<FGridNode*>& oNodeArray )
{
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
                oNodeArray.push_back( node );
            }
        }
    }
}

void
FOdysseyPainterEditorVectorGridToolHUD::DrawSelectionRectangle( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                              , FLinearColor& iHighlightColor )
{
    if( mWorldSelDrag != mWorldSelStart )
    {
        if( mSelectionBox.rect.Area() )
        {
            double xmin = ::ULIS::FMath::Min( mWorldSelDrag.x, mWorldSelStart.x );
            double ymin = ::ULIS::FMath::Min( mWorldSelDrag.y, mWorldSelStart.y );
            double xmax = ::ULIS::FMath::Max( mWorldSelDrag.x, mWorldSelStart.x );
            double ymax = ::ULIS::FMath::Max( mWorldSelDrag.y, mWorldSelStart.y );
            FVector2D hudCoords[4] = { iParams.mTextureToHUD.Execute( FVector2D( xmin, ymin ) )
                                     , iParams.mTextureToHUD.Execute( FVector2D( xmax, ymin ) )
                                     , iParams.mTextureToHUD.Execute( FVector2D( xmax, ymax ) )
                                     , iParams.mTextureToHUD.Execute( FVector2D( xmin, ymax ) ) };

            for( uint32 i = 0; i < 4; i++ )
            {
                uint32 n = ( i + 1 ) % 4;

                DrawPrimitiveLine( iParams
                                 , hudCoords[i]
                                 , hudCoords[n]
                                 , iHighlightColor
                                 , 1.0f );
            }
        }
    }
}

void
FOdysseyPainterEditorVectorGridToolHUD::Reset( )
{
    MakeGrid( mGridTool->GetEditor()->GetVectorHUDFlags() );
}

void
FOdysseyPainterEditorVectorGridToolHUD::Load()
{
    FText shiftInfoText = LOCTEXT("vector-grid-tool-hud-info-shift", "multi selection" );

    FormatModifierInfo( nullptr, &shiftInfoText, nullptr );
}

void
FOdysseyPainterEditorVectorGridToolHUD::Draw( BLContext* iBLContext )
{
}

void
FOdysseyPainterEditorVectorGridToolHUD::DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams )
{
    mCurrentHUDParams = iParams;

    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    uint64 hudFlags = mGridTool->GetEditor()->GetVectorHUDFlags();

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::DrawHUD( iParams );

    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D worldMatrix = mSelectionBox.worldMatrix;

        DrawSelectionRectangle( iParams, hcColor );

        for( int i = 0; i < mCellArray.size(); i++ )
        {
            ::ULIS::FVec2D texCoords[4] = { FOdysseyVector::MapPoint( worldMatrix, mCellArray[i].mNode[0]->GetCoords() ),
                                            FOdysseyVector::MapPoint( worldMatrix, mCellArray[i].mNode[1]->GetCoords() ),
                                            FOdysseyVector::MapPoint( worldMatrix, mCellArray[i].mNode[2]->GetCoords() ),
                                            FOdysseyVector::MapPoint( worldMatrix, mCellArray[i].mNode[3]->GetCoords() ) };
            FVector2D hudCoords[4] = { iParams.mTextureToHUD.Execute( FVector2D( texCoords[0].x, texCoords[0].y ) )
                                     , iParams.mTextureToHUD.Execute( FVector2D( texCoords[1].x, texCoords[1].y ) )
                                     , iParams.mTextureToHUD.Execute( FVector2D( texCoords[2].x, texCoords[2].y ) )
                                     , iParams.mTextureToHUD.Execute( FVector2D( texCoords[3].x, texCoords[3].y ) ) };

            for( uint32 j = 0; j < 4; j ++ )
            {
                uint32 n = ( j + 1 ) % 4;

                DrawPrimitiveLine( iParams, hudCoords[j], hudCoords[n], fgColor, 1.0f );
            }
        }

        for( int i = 0; i < mNodeArray.size(); i++ )
        {
            ::ULIS::FVec2D texCoords = FOdysseyVector::MapPoint( worldMatrix, mNodeArray[i].GetCoords() );
            FVector2D hudCoords = iParams.mTextureToHUD.Execute( FVector2D( texCoords.x, texCoords.y ) );

            DrawPrimitiveHandle( iParams
                               , hudCoords
                               , FOdysseyPainterEditorVectorGridToolHUD::HANDLE_RADIUS
                               , mNodeArray[i].IsSelected() ? hcColor : fgColor
                               , bgColor );
        }
    }

    // invisible plane will get mouse events
    DrawDummyPlane( iParams );

    //DrawModifierInfo( iParams );
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
FOdysseyPainterEditorVectorGridToolHUD::Map()
{
    mPointCount = 0;

    FOdysseyVectorObject::Traverse
    ( mGridTool->GetWorkingCell()->GetScene()
    , 0
    , [ this]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
      {
          if( mGridTool->GetWorkingCell()->ObjectHasFocus( object, travesalFlags ) )
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

              return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
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
FOdysseyPainterEditorVectorGridToolHUD::MakeGrid( uint64 iHUDFlags )
{
    // Updates the selection box
    UpdateSelectionBox( mGridTool->World, iHUDFlags );

    if( mSelectionBox.rect.Area() )
    {
        // Make the box a little bit bigger in order to have coordinates > 0.0f and < 1.0f.
        mSelectionBox.rect.x -= ( mSelectionBox.rect.w * 0.01f );
        mSelectionBox.rect.y -= ( mSelectionBox.rect.h * 0.01f );
        mSelectionBox.rect.w += ( mSelectionBox.rect.w * 0.02f );
        mSelectionBox.rect.h += ( mSelectionBox.rect.h * 0.02f );

        MakeNodes();
        MakeCells();

        Map();
    }
}

#undef LOCTEXT_NAMESPACE
