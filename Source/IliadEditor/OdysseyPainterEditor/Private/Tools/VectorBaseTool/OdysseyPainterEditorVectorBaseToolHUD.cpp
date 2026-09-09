// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"

#include "BatchedElements.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"
#include "GlobalRenderResources.h"
#include "HitProxies.h"
#include "Misc/Optional.h"
#include "TextureResource.h"
#include "UnrealClient.h"

#include "OdysseyKeyState.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyVector.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorCycle.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyViewportCommands.h"

FPointQuadTree::~FPointQuadTree()
{
    for( int i = 0 ; i < 4; i++ )
    {
        if( mChildren[i] )
        {
            delete mChildren[i];
        }
    }
}

FPointQuadTree::FPointQuadTree( const ::ULIS::FRectD& iRect
                              , uint32 iMaxPointsPerQuad
                              , std::vector<FPointQuadTreeEntry>& iPointQuadTreeEntryArray
                              , uint32 iDepth
                              , uint32 iMaxDepth )
    : mChildren { nullptr, nullptr, nullptr, nullptr }
    , mRect( iRect )
{
    Build( iMaxPointsPerQuad, iPointQuadTreeEntryArray, iDepth, iMaxDepth );
}

void
FPointQuadTree::Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene, uint64 iFlags )
{
    iBLContext->save();
    iBLContext->reset_transform();
    iBLContext->set_stroke_style( BLRgba32( 0xFF0000FF )  );
    iBLContext->set_stroke_width( 1.0f );
    iBLContext->stroke_rect( mRect.x, mRect.y, mRect.w, mRect.h );
    iBLContext->restore();

    for( int i = 0 ; i < 4; i++ )
    {
        if( mChildren[i] )
        {
            mChildren[i]->Draw( iBLContext, iScene, iFlags );
        }
    }
}

void
FPointQuadTree::Build( uint32 iMaxPointsPerQuad
                     , std::vector<FPointQuadTreeEntry>& iParentPointQuadTreeEntryArray
                     , uint32 iDepth
                     , uint32 iMaxDepth )
{
    mPointQuadTreeEntryArray.reserve( iParentPointQuadTreeEntryArray.size() );

    for( int i = 0; i < iParentPointQuadTreeEntryArray.size(); i++ )
    {
        if( mRect.HitTest( iParentPointQuadTreeEntryArray[i].worldCoords ) )
        {
            mPointQuadTreeEntryArray.push_back( iParentPointQuadTreeEntryArray[i] );
        }
    }

    if( ( mPointQuadTreeEntryArray.size() > iMaxPointsPerQuad ) && ( iDepth < iMaxDepth ) )
    {
        double minX =   mRect.x;
        double minY =   mRect.y;
        double maxX = ( mRect.x +   mRect.w );
        double maxY = ( mRect.y +   mRect.h );
        double avgX =   mRect.x + ( mRect.w * 0.5f );
        double avgY =   mRect.y + ( mRect.h * 0.5f );

        mChildren[0] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( minX, minY, avgX, avgY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray, iDepth + 1, iMaxDepth );
        mChildren[1] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( avgX, minY, maxX, avgY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray, iDepth + 1, iMaxDepth );
        mChildren[2] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( avgX, avgY, maxX, maxY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray, iDepth + 1, iMaxDepth );
        mChildren[3] = new FPointQuadTree( ::ULIS::FRectD::FromMinMax( minX, avgY, avgX, maxY ), iMaxPointsPerQuad, mPointQuadTreeEntryArray, iDepth + 1, iMaxDepth );

        mPointQuadTreeEntryArray.clear();
    }
}

void
FPointQuadTree::PickPoints( double iWorldX
                          , double iWorldY
                          , double iSelectionRadius
                          , std::vector<FOdysseyVectorPoint*>& oPickedPointArray )
{
   ::ULIS::FRectD rect = ::ULIS::FRectD( mRect.x -   iSelectionRadius,
                                         mRect.y -   iSelectionRadius,
                                         mRect.w + ( iSelectionRadius * 2 ),
                                         mRect.h + ( iSelectionRadius * 2 ) );

    if( rect.HitTest( ::ULIS::FVec2D( iWorldX, iWorldY ) ) )
    {
        if( mChildren[0] == nullptr )
        {
            for( int i = 0; i < mPointQuadTreeEntryArray.size(); i++ )
            {
                ::ULIS::FVec2D dif = ::ULIS::FVec2D( mPointQuadTreeEntryArray[i].worldCoords.x - iWorldX
                                                   , mPointQuadTreeEntryArray[i].worldCoords.y - iWorldY );

                if( dif.Distance() <= iSelectionRadius )
                {
                    oPickedPointArray.push_back( mPointQuadTreeEntryArray[i].point );
                }
            }
        }
        else
        {
            for( int i = 0; i < 4; i++ )
            {
                mChildren[i]->PickPoints( iWorldX, iWorldY, iSelectionRadius, oPickedPointArray );
            }
        }
    }
}

static void
MapPath( FOdysseyVectorPath* iPath
       , const ::ULIS::FRectD& iRect
       , std::vector<FPointQuadTreeEntry>& oPointQuadTreeEntryArray )
{
    for( FOdysseyVectorVertex* vertex : iPath->GetVertexList() )
    {
        ::ULIS::FVec2D& coords = vertex->GetCoords();
        BLPoint worldCoords = iPath->GetWorldMatrix().map_point( coords.x, coords.y );
        ::ULIS::FVec2D screenCoords = ::ULIS::FVec2D( worldCoords.x, worldCoords.y );

        if( iRect.HitTest( screenCoords ) )
        {
            oPointQuadTreeEntryArray.push_back( FPointQuadTreeEntry( vertex, screenCoords ) );
        }
    }
}

static void
MapPoints( FOdysseyVectorObject* iObject
         , const ::ULIS::FRectD& iRect
         , std::vector<FPointQuadTreeEntry>& oPointQuadTreeEntryArray )
{
    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( iObject );

        MapPath( path, iRect, oPointQuadTreeEntryArray );
    }
}

struct HOdysseyHUDDummyCursorHitProxy : public HOdysseyHUDElementHitProxy
{
    DECLARE_HIT_PROXY();

    HOdysseyHUDDummyCursorHitProxy( TSharedPtr<FOdysseyPainterEditorVectorBaseToolHUD> iToolHUD
                                  , TOptional<EMouseCursor::Type> iMouseCursor = TOptional<EMouseCursor::Type>() )
        : HOdysseyHUDElementHitProxy( iToolHUD, iMouseCursor )
    {

    }
};

IMPLEMENT_HIT_PROXY(HOdysseyHUDDummyCursorHitProxy, HOdysseyHUDElementHitProxy)

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawInbetweens( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                      , FInbetweenerBreakdown* iBreakdown
                                                      , eShowInbetweens ShowInbetweens
                                                      , uint64 iSourceExtraHUDFlags
                                                      , uint64 iTargetExtraHUDFlags )
{
    iBreakdown->GetInbetweenerTag()->GetOwner()->LockDrawing();

    FInbetweenerBreakdown* firstBreakdown = iBreakdown->GetInbetweenerTag()->GetBreakdownList().front();
    FInbetweenerBreakdown* lastBreakdown = iBreakdown->GetInbetweenerTag()->GetBreakdownList().back();

    if( ( ShowInbetweens == eShowInbetweens::All )
     || ( ShowInbetweens == eShowInbetweens::SourceOnly )
     || ( ShowInbetweens == eShowInbetweens::SourceAndBreakdownsOnly ) )
    {
        DrawBreakdown( iParams
                     , firstBreakdown
                     , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                     , FLinearColor( 1.0f, 0.5f, 0.5f, 1.0f )
                     , FOdysseyVectorHUD::HUD_BREAKDOWN_SOURCE
                     | iSourceExtraHUDFlags );
    }

    if( ShowInbetweens == eShowInbetweens::All )
    {
        for( FInbetweenerBreakdown* otherBreakdown : iBreakdown->GetInbetweenerTag()->GetBreakdownList() )
        {
            DrawBreakdown( iParams
                         , otherBreakdown
                         , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                         , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                         , ( ( otherBreakdown == firstBreakdown ) ? 0
                                                                  : FOdysseyVectorHUD::HUD_BREAKDOWN_SOURCE )
                         | FOdysseyVectorHUD::HUD_BREAKDOWN_INBETWEEN
                         | ( ( otherBreakdown == iBreakdown     ) ? 0
                                                                  : FOdysseyVectorHUD::HUD_BREAKDOWN_TARGET ) );
        }
    }

    if( ShowInbetweens == eShowInbetweens::Surrounding )
    {
        FInbetweenerBreakdown* nextBreakdown = iBreakdown->GetNextBreakdown();

        if( nextBreakdown )
        {
            DrawBreakdown( iParams
                         , nextBreakdown
                         , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                         , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                         , FOdysseyVectorHUD::HUD_BREAKDOWN_INBETWEEN );
        }

        DrawBreakdown( iParams
                     , iBreakdown
                     , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                     , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                     , FOdysseyVectorHUD::HUD_BREAKDOWN_INBETWEEN );
    }

    if( ShowInbetweens == eShowInbetweens::SourceAndBreakdownsOnly )
    {
        for( FInbetweenerBreakdown* otherBreakdown : iBreakdown->GetInbetweenerTag()->GetBreakdownList() )
        {
            DrawBreakdown( iParams
                         , otherBreakdown
                         , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                         , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                         , 0
                         | ( ( otherBreakdown == iBreakdown     ) ? 0
                                                                  : FOdysseyVectorHUD::HUD_BREAKDOWN_TARGET ) );
        }
    }

    DrawBreakdown( iParams
                 , iBreakdown
                 , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                 , FLinearColor( 1.0f, 0.5f, 0.5f, 1.0f )
                 , FOdysseyVectorHUD::HUD_BREAKDOWN_TARGET
                 | iTargetExtraHUDFlags );

    iBreakdown->GetInbetweenerTag()->GetOwner()->UnlockDrawing();
}

void
FOdysseyPainterEditorVectorBaseToolHUD::MakePointQuadTree( bool iFocusedObjectsOnly
                                                         , uint64 iHUDFlags )
{
    FOdysseyVectorGroupPaint* scene = mBaseTool->GetWorkingCell()->GetScene();
    std::vector<FPointQuadTreeEntry> pointQuadTreeEntryArray;
    uint32 width = scene->GetLayer()->GetWidth();
    uint32 height = scene->GetLayer()->GetHeight();
    ::ULIS::FRectD screenRect = scene->GetBBox( true, true );

    //screenRect = ::ULIS::FRectD::FromXYWH( 0, 0, width, height );

    pointQuadTreeEntryArray.reserve( 200 );

    FOdysseyVectorObject::Traverse( scene
                          , iHUDFlags
                          , [ this
                            , scene
                            , iFocusedObjectsOnly
                            , &screenRect
                            , &pointQuadTreeEntryArray ]( FOdysseyVectorObject* object, uint64 traverseFlags ) -> uint64
    {
        if( ( iFocusedObjectsOnly == false ) || scene->GetCell()->ObjectHasFocus( object, traverseFlags ) )
        {
            MapPoints( object, screenRect, pointQuadTreeEntryArray );

            return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
        }

        return 0;
    } );

    if( mPointQuadTree )
    {
        delete mPointQuadTree;
    }

    mPointQuadTree = new FPointQuadTree( screenRect, 20, pointQuadTreeEntryArray, 0, 8 );
}

void
FOdysseyPainterEditorVectorBaseToolHUD::PickPoints( double iWorldX
                                                  , double iWorldY
                                                  , double iSelectionRadius
                                                  , std::vector<FOdysseyVectorPoint*>& oPickedPointArray )
{
    if( mPointQuadTree )
    {
        mPointQuadTree->PickPoints( iWorldX, iWorldY, iSelectionRadius, oPickedPointArray );
    }
}

FOdysseyPainterEditorVectorBaseToolHUD::~FOdysseyPainterEditorVectorBaseToolHUD()
{
    if( mPointQuadTree )
    {
        delete mPointQuadTree;
    }
}

FOdysseyPainterEditorVectorBaseToolHUD::FOdysseyPainterEditorVectorBaseToolHUD( UOdysseyPainterEditorVectorBaseTool* iBaseTool )
    : mBaseTool ( iBaseTool )
    , mVertexTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Vertex_Full") ) )
    , mVertexContourTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Vertex_Contour") ) )
    , mHandleTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Handle_Full") ) )
    , mBucketInnerTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Bucket_Inner") ) )
    , mBucketOuterTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Bucket_Outer") ) )
    , mBucketPropagateTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Bucket_Propagate") ) )
    , mInfoBorderLeftTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Info_Border_Left") ) )
    , mInfoBorderTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Info_Border") ) )
    , mInfoBorderRightTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Info_Border_Right") ) )
    , mLineOutlinedTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Line_Outlined") ) )
    , mX( 0.0f )
    , mY( 0.0f )
    , mPointQuadTree( nullptr )
{
//    mFontInfo = FSlateFontInfo( LoadObject<UFont>( nullptr, TEXT("/Odyssey/Fonts/Inconsolata-Regular_Font") ), 9 );

    if (GEngine)
        mFontInfo = FSlateFontInfo( GEngine->GetSmallFont(), 9 );
}

void
FOdysseyPainterEditorVectorBaseToolHUD::Load()
{
}

void
FOdysseyPainterEditorVectorBaseToolHUD::Unload()
{
}

void
FOdysseyPainterEditorVectorBaseToolHUD::Reset()
{
    uint64 hudFlags = mBaseTool->GetEditor()->GetVectorHUDFlags();

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        UpdateSelectionInbetweenMode( true );
    }
}

FSelectionBox&
FOdysseyPainterEditorVectorBaseToolHUD::GetSelectionBox()
{
    return mSelectionBox;
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionInbetweenMode( bool iOnTargetCellOnly )
{
    FOdysseyVectorGroupPaint* scene = mBaseTool->GetWorkingCell()->GetScene();
    FOdysseyVectorLayer* sharedEnv = scene->GetLayer();
    int32 cellIndex = scene->GetCell()->GetIndex();

    mSelectedInbetweenerTagList.clear();
    mSelectedBreakdownList.clear();

    for( FOdysseyVectorTag* tag : sharedEnv->GetSharedTagList() )
    {
        if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
            uint32 tagCellIndex = inbetweenerTag->GetSourceCellIndex();

            if( tag->GetOwner()->IsSelected() )
            {
                FInbetweenerBreakdown* selectedBreakdown = nullptr;

                mSelectedInbetweenerTagList.push_back( inbetweenerTag );

                for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
                {
                    int32 drawingIndex = ( cellIndex - breakdown->GetInbetweenerTag()->GetSourceCellIndex() ) * static_cast<int8>(breakdown->GetInbetweenerTag()->GetInterpolationDirection());

                    if( ( ( drawingIndex >  static_cast<int32>(breakdown->GetSourceDrawingIndex()) ) && ( drawingIndex <= static_cast<int32>(breakdown->GetTargetDrawingIndex()) ) )
                     || ( ( drawingIndex == static_cast<int32>(breakdown->GetSourceDrawingIndex()) ) && ( breakdown->GetPrevBreakdown() == nullptr     ) ) )
                    {
                        if( iOnTargetCellOnly == false )
                        {
                            mSelectedBreakdownList.push_back( breakdown );
                        }
                        else
                        {
                            if ( drawingIndex == breakdown->GetTargetDrawingIndex() )
                            {
                                mSelectedBreakdownList.push_back( breakdown );
                            }
                        }
                    }
                }
            }
        }
    }
}

std::list<FInbetweenerBreakdown*>&
FOdysseyPainterEditorVectorBaseToolHUD::GetSelectedBreakdownList()
{
    return mSelectedBreakdownList;
}

std::list<FOdysseyVectorTagInbetweener*>&
FOdysseyPainterEditorVectorBaseToolHUD::GetSelectedInbetweenerTagList()
{
    return mSelectedInbetweenerTagList;
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawText( BLContext* iBLContext
                                                , const BLFont& iBLFont
                                                , const BLRgba32& iForegroundColor
                                                , const BLRgba32& iBackgroundColor
                                                , const BLRgba32& iHighlightColor
                                                , char* iText
                                                , uint32 iX
                                                , uint32 iY )
{
    iBLContext->set_comp_op( BL_COMP_OP_SRC_OVER  );
    iBLContext->set_fill_style( iForegroundColor );
    iBLContext->set_stroke_width( 10.0f );

    iBLContext->stroke_utf8_text( BLPoint( iX, iY ), iBLFont, iText );
    iBLContext->fill_utf8_text( BLPoint( iX, iY ), iBLFont, iText );

//    blctx->set_stroke_style( BLRgba32( 0xFF000000 ) );
//    blctx->set_stroke_width( 1.0f );
//    blctx->strokeUtf8Text( BLPoint( iFrame.x + 10, iFrame.y + iFrame.h - 10 ), mFont, str );
}

FOdysseyVectorObject*
FOdysseyPainterEditorVectorBaseToolHUD::PickPath( FOdysseyVectorGroup* iSelectionSpace
                                                , FOdysseyVectorPath* iPath
                                                , const BLImage& iHUDMaskImage
                                                , const ::ULIS::FRectD& iHUDRoi )
{
    const BLMatrix2D& worldMatrix = iPath->GetWorldMatrix();
    BLImageData imageData;

    iHUDMaskImage.get_data( &imageData );

    for( FOdysseyVectorSegment* segment : iPath->GetSegmentList() )
    {
        std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();

        for( uint32 i = 0; i < fractionCache.size(); i++ )
        {
            ::ULIS::FVec2D& p0Coords = fractionCache[i].point[0]->GetCoords();
            ::ULIS::FVec2D& p1Coords = fractionCache[i].point[1]->GetCoords();
            ::ULIS::FVec2D texP0Coords = FOdysseyVector::MapPoint( worldMatrix, ::ULIS::FVec2D( p0Coords.x
                                                                                              , p0Coords.y ) );
            ::ULIS::FVec2D texP1Coords = FOdysseyVector::MapPoint( worldMatrix, ::ULIS::FVec2D( p1Coords.x
                                                                                              , p1Coords.y ) );
            FVector2D hudP0Coords = TextureToHUD( FVector2D( texP0Coords.x, texP0Coords.y ) );
            FVector2D hudP1Coords = TextureToHUD( FVector2D( texP1Coords.x, texP1Coords.y ) );

            bool pointHitMask = FOdysseyVectorEngine::TraceGenericLine( hudP0Coords.X, hudP0Coords.Y, 0.0f
                                                                      , hudP1Coords.X, hudP1Coords.Y, 0.0f
                                                                      , [&imageData]( int32 iX, int32 iY, double iT)
            {
                if( ( iX >= 0 && iX < imageData.size.w )
                 && ( iY >= 0 && iY < imageData.size.h ) )
                {
                    uint8 *pixel = static_cast<uint8*>(imageData.pixel_data);
                    uint32 offset = ( iY * imageData.stride ) + iX;

                    return ( pixel[offset] ) ? true : false;
                }

                return false;
            } );

            if( pointHitMask )
            {
                return iPath;
            }
        }
    }

    return nullptr;
}

FOdysseyVectorObject*
FOdysseyPainterEditorVectorBaseToolHUD::PickObject( FOdysseyVectorGroup* iSelectionSpace
                                                  , FOdysseyVectorObject* iObj
                                                  , const BLImage& iHUDMaskImage
                                                  , const ::ULIS::FRectD& iHUDRoi )
{
    if( iObj->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObj);

        return PickPath( iSelectionSpace, path, iHUDMaskImage, iHUDRoi );
    }

    return nullptr;
}

void
FOdysseyPainterEditorVectorBaseToolHUD::RecursivePick( FOdysseyVectorGroup* iSelectionSpace
                                                     , FOdysseyVectorObject* iObj
                                                     , const BLImage& iHUDMaskImage
                                                     , const ::ULIS::FRectD& iHUDRoi
                                                     , std::vector<FOdysseyVectorObject*>& oSelectedObjectArray )
{
    FOdysseyVectorObject* pickedObject = ( iObj->HasAncestor( iSelectionSpace ) ) ? PickObject( iSelectionSpace
                                                                                              , iObj
                                                                                              , iHUDMaskImage
                                                                                              , iHUDRoi ) : nullptr;

    for( FOdysseyVectorObject* child : iObj->GetChildrenList() )
    {
        RecursivePick( iSelectionSpace
                     , child
                     , iHUDMaskImage
                     , iHUDRoi
                     , oSelectedObjectArray );
    }

    if( pickedObject )
    {
        oSelectedObjectArray.push_back( pickedObject );
    }
}

bool
FOdysseyPainterEditorVectorBaseToolHUD::PickPathPoints( FOdysseyVectorPath* iPath
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , double iSelectionRadius
                                                      , std::vector<FOdysseyVectorVertex*>& oPickedVertexArray
                                                      , std::vector<FOdysseyVectorHandleSegment*>& oPickedHandleArray
                                                      , uint64 iSelectionFlags )
{
    FVector2D hudCoords = TextureToHUD( iWorldX, iWorldY );
    BLMatrix2D& worldMatrix = iPath->GetWorldMatrix();
    bool anythingPicked = false;

    for( FOdysseyVectorVertex* vertex : iPath->GetVertexList() )
    {
        ::ULIS::FVec2D perpendicularVector = FOdysseyVectorPath::GetPerpendicularVector( vertex, true );
        BLPoint worldPerpendicularVector = worldMatrix.map_vector( perpendicularVector.x * vertex->GetRadius()
                                                                , perpendicularVector.y * vertex->GetRadius() );

        // Pick vertex
        if ( iSelectionFlags & PICK_VERTEX )
        {
            ::ULIS::FVec2D& vertexLocalCoords = vertex->GetCoords();
            // convert vertex coordinates to world coordinates. Easier to detect collision inside the picking circle.
            ::ULIS::FVec2D vertexWorldCoords = FOdysseyVector::MapPoint( worldMatrix
                                                                       , vertexLocalCoords.x
                                                                       , vertexLocalCoords.y );
            FVector2D vertexHUDCoords = TextureToHUD( vertexWorldCoords.x, vertexWorldCoords.y );
            FVector2D dif = vertexHUDCoords - hudCoords;

            if( dif.Length() <= iSelectionRadius )
            {
                oPickedVertexArray.push_back( vertex );

                anythingPicked = true;
            }
        }

        // Pick vertex handle
        if( iSelectionFlags & PICK_HANDLE_VERTEX )
        {
            ::ULIS::FVec2D handlelocalCoords[2];

            vertex->GetHandlePosition( handlelocalCoords );

            for( int i = 0; i < 2; i++ )
            {
                ::ULIS::FVec2D handleWorldCoords = FOdysseyVector::MapPoint( worldMatrix
                                                                           , handlelocalCoords[i].x
                                                                           , handlelocalCoords[i].y );
                FVector2D handleHUDCoords = TextureToHUD( handleWorldCoords.x, handleWorldCoords.y );
                FVector2D dif = handleHUDCoords - hudCoords;

                if( dif.Length() <= iSelectionRadius )
                {
                    oPickedVertexArray.push_back( vertex );

                    anythingPicked = true;

                    break;
                }
            }
        }
    }

    // Pick segment handles
    if( iSelectionFlags & PICK_HANDLE_SEGMENT )
    {
        for( FOdysseyVectorSegment* segment : iPath->GetSegmentList() )
        {
            // TODO: hit-test with segment's bounding box.
            FOdysseyVectorHandleSegment* handle0 = segment->GetHandle(0);
            FOdysseyVectorHandleSegment* handle1 = segment->GetHandle(1);
            ::ULIS::FVec2D& handle0LocalCoords = handle0->GetCoords();
            ::ULIS::FVec2D& handle1LocalCoords = handle1->GetCoords();
            // convert handles coordinates to world coordinates. Easier to detect collision inside
            // the picking circle.
            ::ULIS::FVec2D handle0WorldCoords = FOdysseyVector::MapPoint( worldMatrix
                                                                        , handle0LocalCoords.x
                                                                        , handle0LocalCoords.y );
            ::ULIS::FVec2D handle1WorldCoords = FOdysseyVector::MapPoint( worldMatrix
                                                                        , handle1LocalCoords.x
                                                                        , handle1LocalCoords.y );
            FVector2D handle0HUDCoords = TextureToHUD( handle0WorldCoords.x, handle0WorldCoords.y );
            FVector2D handle1HUDCoords = TextureToHUD( handle1WorldCoords.x, handle1WorldCoords.y );
            FVector2D dif0 = handle0HUDCoords - hudCoords;
            FVector2D dif1 = handle1HUDCoords - hudCoords;

            if( dif0.Length() <= iSelectionRadius )
            {
                oPickedHandleArray.push_back( handle0 );

                anythingPicked = true;
            }

            if( dif1.Length() <= iSelectionRadius )
            {
                oPickedHandleArray.push_back( handle1 );

                anythingPicked = true;
            }
        }
    }

    return anythingPicked;
}

// mask-based picking
bool
FOdysseyPainterEditorVectorBaseToolHUD::Pick( FOdysseyVectorGroupPaint* iScene
                                            , const BLImage& iHUDMaskImage
                                            , const ::ULIS::FRectD& iHUDRoi
                                            , std::vector<FOdysseyVectorObject*>& oPickedObjectArray )
{
    uint32 pickedObjectCount = oPickedObjectArray.size();

    FOdysseyVectorGroup* selectionSpace = iScene->GetCell()->GetSelectionSpace();

    RecursivePick( selectionSpace ? selectionSpace : iScene
                 , iScene
                 , iHUDMaskImage
                 , iHUDRoi
                 , oPickedObjectArray );

    return ( oPickedObjectArray.size() == pickedObjectCount ) ? false : true;
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBoxVertexMode()
{
    FOdysseyVectorGroupPaint* scene = mBaseTool->GetWorkingCell()->GetScene();
    double xmin, ymin, xmax, ymax;

    mSelectionBox.inited = false;
    mSelectionBox.aabb = true;
    mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );
    mSelectionBox.worldMatrix.reset(); /* = mScene->GetWorldMatrix()*/;
    mSelectionBox.inverseWorldMatrix.reset(); /* = mScene->GetInverseWorldMatrix()*/;

    // call lambda on each object of the tree
    FOdysseyVectorObject::Traverse
    ( scene
    , 0
    , [ this
      , scene
      , &xmin
      , &ymin
      , &xmax
      , &ymax ]( FOdysseyVectorObject* object, uint64 iTraversalFlags ) -> uint64
      {
          if( scene->GetCell()->ObjectHasFocus( object, iTraversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);
                  double vertexXMin, vertexYMin, vertexXMax, vertexYMax;
                  ::ULIS::FRectD pathBBox;

                  if( FOdysseyVectorVertex::GetMinMaxFromList( path->GetSelectedVertexList()
                                                             , false
                                                             , true
                                                             , vertexXMin
                                                             , vertexYMin
                                                             , vertexXMax
                                                             , vertexYMax ) )
                  {
                      if( mSelectionBox.inited == false )
                      {
                          xmin = vertexXMin;
                          xmax = vertexXMin;
                          ymin = vertexYMin;
                          ymax = vertexYMin;
                      }

                      if( vertexXMin < xmin ) xmin = vertexXMin;
                      if( vertexYMin < ymin ) ymin = vertexYMin;
                      if (vertexXMax > xmax ) xmax = vertexXMax;
                      if( vertexYMax > ymax ) ymax = vertexYMax;

                      mSelectionBox.inited = true;
                  }
              }

              return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );

    if( mSelectionBox.inited )
    {
        BLPoint p0, p1, p2, p3;

        p0 = mSelectionBox.inverseWorldMatrix.map_point( xmin, ymin );
        p1 = mSelectionBox.inverseWorldMatrix.map_point( xmax, ymin );
        p2 = mSelectionBox.inverseWorldMatrix.map_point( xmax, ymax );
        p3 = mSelectionBox.inverseWorldMatrix.map_point( xmin, ymax );

        mSelectionBox.rect = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                       , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                       , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                       , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBoxObjectMode( bool iForceWorld )
{
    FOdysseyVectorGroupPaint* scene = mBaseTool->GetWorkingCell()->GetScene();
    std::list<FOdysseyVectorObject*>& selectedObjectList = scene->GetCell()->GetSelectedObjectList();

    if( ( selectedObjectList.size() <= 1 ) && ( iForceWorld == false ) )
    {
        FOdysseyVectorObject* selectedObject = selectedObjectList.size() ? selectedObjectList.front() : scene;
        mSelectionBox.inited = true;
        mSelectionBox.aabb = false;
        mSelectionBox.rect = selectedObject->GetBBox( true, false );
        mSelectionBox.worldMatrix = selectedObject->GetWorldMatrix();
        mSelectionBox.inverseWorldMatrix = selectedObject->GetInverseWorldMatrix();
    }
    else
    {
        mSelectionBox.inited = false;
        mSelectionBox.aabb = true;
        mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );
        mSelectionBox.worldMatrix.reset(); /* = mScene->GetWorldMatrix()*/;
        mSelectionBox.inverseWorldMatrix.reset(); /* = mScene->GetInverseWorldMatrix()*/;

        // call lambda on each object of the tree
        FOdysseyVectorObject::Traverse
        ( scene
        , 0
        , [ this
          , scene
          , &selectedObjectList ]( FOdysseyVectorObject* object, uint64 iTraversalFlags ) -> uint64
          {
              if( scene->GetCell()->ObjectHasFocus( object, iTraversalFlags ) )
              {
                  ::ULIS::FRectD selectedObjectBBox = object->GetBBox( true, true );

                  mSelectionBox.rect = mSelectionBox.inited ? mSelectionBox.rect | selectedObjectBBox
                                                            : selectedObjectBBox;

                  mSelectionBox.inited = true;

                  return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
              }

              return 0;
          } );

        if( mSelectionBox.inited )
        {
            ::ULIS::FRectD rect = mSelectionBox.rect;
            BLPoint p0, p1, p2, p3;

            p0 = mSelectionBox.inverseWorldMatrix.map_point( rect.x         , rect.y          );
            p1 = mSelectionBox.inverseWorldMatrix.map_point( rect.x + rect.w, rect.y          );
            p2 = mSelectionBox.inverseWorldMatrix.map_point( rect.x + rect.w, rect.y + rect.h );
            p3 = mSelectionBox.inverseWorldMatrix.map_point( rect.x         , rect.y + rect.h );

            mSelectionBox.rect = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                           , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                           , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                           , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBoxInbetweenMode( bool iForceWorld )
{
    FOdysseyVectorGroupPaint* scene = mBaseTool->GetWorkingCell()->GetScene();

    if( mSelectedBreakdownList.size() == 1 )
    {
        FInbetweenerBreakdown* breakdown = mSelectedBreakdownList.front();

        mSelectionBox.inited = true;
        mSelectionBox.aabb = false;
        mSelectionBox.rect = breakdown->GetTargetBBox( false );
        mSelectionBox.worldMatrix = breakdown->GetTargetWorldMatrix();
        mSelectionBox.inverseWorldMatrix = breakdown->GetTargetInverseWorldMatrix();
    }
    else
    {
        mSelectionBox.inited = false;
        mSelectionBox.aabb = true;
        mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );
        mSelectionBox.worldMatrix.reset(); /* = mScene->GetWorldMatrix()*/;
        mSelectionBox.inverseWorldMatrix.reset(); /* = mScene->GetInverseWorldMatrix()*/;

        for( FInbetweenerBreakdown* breakdown : mSelectedBreakdownList )
        {
            ::ULIS::FRectD breakdownBBox = breakdown->GetTargetBBox( true );

             mSelectionBox.rect = mSelectionBox.inited ? mSelectionBox.rect | breakdownBBox
                                                       : breakdownBBox;

            mSelectionBox.inited = true;
        }

        if( mSelectionBox.inited )
        {
            ::ULIS::FRectD rect = mSelectionBox.rect;
            BLPoint p0, p1, p2, p3;

            p0 = mSelectionBox.inverseWorldMatrix.map_point( rect.x         , rect.y          );
            p1 = mSelectionBox.inverseWorldMatrix.map_point( rect.x + rect.w, rect.y          );
            p2 = mSelectionBox.inverseWorldMatrix.map_point( rect.x + rect.w, rect.y + rect.h );
            p3 = mSelectionBox.inverseWorldMatrix.map_point( rect.x         , rect.y + rect.h );

            mSelectionBox.rect = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                           , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                           , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                           , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBox( bool iForceWorld
                                                          , uint64 iHUDFlags )
{
    if( iHUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        //case eVectorEditionMode::Object :
        UpdateSelectionBoxObjectMode( iForceWorld );
    }

    if( iHUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        //case eVectorEditionMode::Vertex:
        UpdateSelectionBoxVertexMode( );
    }

    if( iHUDFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        //case eVectorEditionMode::Vertex:
        UpdateSelectionBoxInbetweenMode( iForceWorld );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawSelectionBox( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                        , const FLinearColor& iForegroundColor
                                                        , const FLinearColor& iBackgroundColor
                                                        , const FLinearColor& iHighlightColor
                                                        , uint64 iHUDFlags )
{
    FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Line);
    FLinearColor white = FLinearColor( 1.0f, 1.0f, 1.0f, 1.0f );

    if( mSelectionBox.inited )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.worldMatrix;
        // texture space
        BLPoint pt[4] = { worldMatrix.map_point( mSelectionBox.rect.x                       , mSelectionBox.rect.y                        )
                        , worldMatrix.map_point( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y                        )
                        , worldMatrix.map_point( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y + mSelectionBox.rect.h )
                        , worldMatrix.map_point( mSelectionBox.rect.x                       , mSelectionBox.rect.y + mSelectionBox.rect.h ) };
        BLPath path;
        // HUD space
        FVector2D scr[4] = { TextureToHUD( FVector2D( pt[0].x, pt[0].y ) )
                           , TextureToHUD( FVector2D( pt[1].x, pt[1].y ) )
                           , TextureToHUD( FVector2D( pt[2].x, pt[2].y ) )
                           , TextureToHUD( FVector2D( pt[3].x, pt[3].y ) ) };

        FCanvasLineItem line[4] = { FCanvasLineItem ( FVector2D( scr[0].X, scr[0].Y ), FVector2D( scr[1].X, scr[1].Y ) )
                                  , FCanvasLineItem ( FVector2D( scr[1].X, scr[1].Y ), FVector2D( scr[2].X, scr[2].Y ) )
                                  , FCanvasLineItem ( FVector2D( scr[2].X, scr[2].Y ), FVector2D( scr[3].X, scr[3].Y ) )
                                  , FCanvasLineItem ( FVector2D( scr[3].X, scr[3].Y ), FVector2D( scr[0].X, scr[0].Y ) ) };
/*
        // First pass. We draw in 2 passes with different colors / thickness
        // so that the box is always visible whatever the background
        for( uint32 i = 0; i < 4; i++ )
        {
            line[i].SetColor( iBackgroundColor );
            line[i].LineThickness = 2;

            iParams.mCanvas->DrawItem( line[i] );
        }
*/
        // Second pass, thiner
        for( uint32 i = 0; i < 4; i++ )
        {
            // draw box as white if nothing is selected, colored if something is selected
            //line[i].SetColor( ( mScene->GetCell()->GetSelectedObjectList().size() == 0 ) ? white : iForegroundColor );
            line[i].SetColor( iForegroundColor );
            line[i].LineThickness = 2.0;

            iParams.mCanvas->DrawItem( line[i] );
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawBucket( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                  , FOdysseyVectorBucket* iBucket
                                                  , const FLinearColor& fgColor
                                                  , const FLinearColor& bgColor
                                                  , const FLinearColor& hcColor
                                                  , uint64 iHUDFlags )
{
    ::ULIS::FVec2D bucketWorldCoords = FOdysseyVectorHUD::GetBucketPosition( iBucket, true );
    FVector2D bucketHUDCoords = TextureToHUD( FVector2D( bucketWorldCoords.x, bucketWorldCoords.y ) );
    FColor bucketColor = iBucket->GetColor();
    FLinearColor fillColor = FLinearColor( bucketColor );
    FLinearColor propColor = iBucket->IsPropagated() ? FLinearColor( 0.0f, 1.0f, 0.0f, 1.0f ) : fgColor;
    static FLinearColor blackColor = FLinearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    static FLinearColor whiteColor = FLinearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    static FLinearColor greyColor = FLinearColor( 0.75f, 0.75f, 0.75f, 0.5 );

    if( iHUDFlags & FOdysseyVectorHUD::HUD_GROUPPAINT_BUCKET_HANDLE )
    {
        if( iBucket->GetColorMode() == eBucketColorMode::LinearGradient )
        {
            BLMatrix2D& worldMatrix = iBucket->GetOwner()->GetWorldMatrix();
            ::ULIS::FVec2D handleVector = FOdysseyVectorHUD::GetBucketHandleVector( iBucket, true );
            FVector2D handleHUDCoords = bucketHUDCoords + ( FVector2D( handleVector.x, handleVector.y ) * FOdysseyVectorHUD::HANDLE_DISTANCE );
            ::ULIS::FVec2D localP0 = iBucket->GetLinearP0();
            ::ULIS::FVec2D localP1 = iBucket->GetLinearP1();
            ::ULIS::FVec2D worldHandle[2] = { FOdysseyVector::MapPoint( worldMatrix, localP0 )
                                            , FOdysseyVector::MapPoint( worldMatrix, localP1 ) };
            FVector2D hudHandle[2] = { WorldPointToHUD ( FVector2D( worldHandle[0].x,  worldHandle[0].y ) )
                                     , WorldPointToHUD ( FVector2D( worldHandle[1].x,  worldHandle[1].y ) ) };
            FLinearColor linearColor0 = FLinearColor( iBucket->GetGradientColor0() );
            FLinearColor linearColor1 = FLinearColor( iBucket->GetGradientColor1() );

            DrawPrimitiveLineOutlined( iParams
                                     , bucketHUDCoords
                                     , ( hudHandle[1] + hudHandle[0] ) * 0.5f
                                     , greyColor
                                     , 1.0f );

            DrawPrimitiveLineOutlined( iParams
                                     , hudHandle[0]
                                     , hudHandle[1]
                                     , whiteColor
                                     , 1.0f );

            DrawPrimitiveHandle( iParams, hudHandle[0], FOdysseyVectorHUD::HANDLE_RADIUS, linearColor0, blackColor );
            DrawPrimitiveHandle( iParams, hudHandle[1], FOdysseyVectorHUD::HANDLE_RADIUS, linearColor1, blackColor );

/*
            // Bucket-to-handle line
            DrawPrimitiveLine( iParams, bucketHUDCoords, handleHUDCoords, blackColor, 2.0f );
            DrawPrimitiveLine( iParams, bucketHUDCoords, handleHUDCoords, whiteColor, 1.0f );

            // Handle
            DrawPrimitiveHandle( iParams, handleHUDCoords, FOdysseyVectorHUD::HANDLE_RADIUS, whiteColor, blackColor );
*/
        }

        if( iBucket->GetColorMode() == eBucketColorMode::RadialGradient )
        {
            ::ULIS::FVec2D radialWorldCoords = FOdysseyVectorHUD::GetBucketRadialPosition( iBucket, true );
            FVector2D radialHUDCoords = TextureToHUD( FVector2D( radialWorldCoords.x, radialWorldCoords.y ) );
            ::ULIS::FVec2D radialHandleWorldCoords = FOdysseyVectorHUD::GetBucketRadialHandlePosition( iBucket, true );
            FVector2D radialHandleHUDCoords = TextureToHUD( FVector2D( radialHandleWorldCoords.x, radialHandleWorldCoords.y ) );
            double radialRadius = ( radialHandleHUDCoords - radialHUDCoords ).Length();

            // Bucket-to-radial line
            DrawPrimitiveLineOutlined( iParams, bucketHUDCoords, radialHUDCoords, greyColor, 1.0f );

            // Radial Circle
            DrawPrimitiveCircle( iParams, radialHUDCoords, FOdysseyVectorHUD::RADIAL_AREA_RADIUS, blackColor, 2.0f );
            DrawPrimitiveCircle( iParams, radialHUDCoords, FOdysseyVectorHUD::RADIAL_AREA_RADIUS, whiteColor, 1.0f );

            // Radial-to-RadialHandle line
            DrawPrimitiveLineOutlined( iParams, radialHUDCoords, radialHandleHUDCoords, whiteColor, 1.0f );

            // RadialHandle Circle
            DrawPrimitiveCircle( iParams, radialHUDCoords, radialRadius, blackColor, 2.0f );
            DrawPrimitiveCircle( iParams, radialHUDCoords, radialRadius, whiteColor, 1.0f );

            // RadialHandle
            DrawPrimitiveHandle( iParams, radialHandleHUDCoords, FOdysseyVectorHUD::HANDLE_RADIUS, whiteColor, blackColor );
        }
    }

    if (iBucket->GetColorMode() == eBucketColorMode::Transparent)
    {
        //As FillColor is transparent, we need something to
        //indicate this bucket is transparent
        //So we draw a cross in the circle

        static float cos45 = FMath::Cos(FMath::DegreesToRadians(45.f));
        static float sin45 = FMath::Sin(FMath::DegreesToRadians(45.f));

        static FVector2D topLeft(-cos45, -sin45);
        static FVector2D topRight(cos45, -sin45);
        static FVector2D bottomLeft(-cos45, sin45);
        static FVector2D bottomRight(cos45, sin45);

        DrawPrimitiveLineOutlined(
            iParams,
            bucketHUDCoords + topLeft * FOdysseyVectorHUD::PELLET_RADIUS,
            bucketHUDCoords + bottomRight * FOdysseyVectorHUD::PELLET_RADIUS,
            blackColor,
            2.0f
        );

        DrawPrimitiveLineOutlined(
            iParams,
            bucketHUDCoords + topRight * FOdysseyVectorHUD::PELLET_RADIUS,
            bucketHUDCoords + bottomLeft * FOdysseyVectorHUD::PELLET_RADIUS,
            blackColor,
            2.0f
        );
    }
    else
    {
        iParams.mCanvas->DrawTile( bucketHUDCoords.X - FOdysseyVectorHUD::PELLET_RADIUS
                                , bucketHUDCoords.Y - FOdysseyVectorHUD::PELLET_RADIUS
                                , FOdysseyVectorHUD::PELLET_RADIUS * 2.0f
                                , FOdysseyVectorHUD::PELLET_RADIUS * 2.0f
                                , 0.0f
                                , 0.0f
                                , 1.0f
                                , 1.0f
                                , fillColor
                                , mBucketInnerTexture->GetResource()
                                , ESimpleElementBlendMode::SE_BLEND_Masked );
    }

    iParams.mCanvas->DrawTile( bucketHUDCoords.X - FOdysseyVectorHUD::PELLET_RADIUS
                             , bucketHUDCoords.Y - FOdysseyVectorHUD::PELLET_RADIUS
                             , FOdysseyVectorHUD::PELLET_RADIUS * 2.0f
                             , FOdysseyVectorHUD::PELLET_RADIUS * 2.0f
                             , 0.0f
                             , 0.0f
                             , 1.0f
                             , 1.0f
                             , ( fillColor.GetLuminance() > 0.5f ) ? blackColor : whiteColor
                             , mBucketOuterTexture->GetResource()
                             , ESimpleElementBlendMode::SE_BLEND_AlphaBlend );

    if( iBucket->IsPropagated() )
    {
        iParams.mCanvas->DrawTile( bucketHUDCoords.X - FOdysseyVectorHUD::PELLET_RADIUS
                                 , bucketHUDCoords.Y - FOdysseyVectorHUD::PELLET_RADIUS
                                 , FOdysseyVectorHUD::PELLET_RADIUS * 2.0f
                                 , FOdysseyVectorHUD::PELLET_RADIUS * 2.0f
                                 , 0.0f
                                 , 0.0f
                                 , 1.0f
                                 , 1.0f
                                 , propColor
                                 , mBucketPropagateTexture->GetResource()
                                 , ESimpleElementBlendMode::SE_BLEND_Masked );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawGroupPaint( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                      , FOdysseyVectorGroupPaint* iPaintGroup
                                                      , const FLinearColor& fgColor
                                                      , const FLinearColor& bgColor
                                                      , const FLinearColor& hcColor
                                                      , uint64 iHUDFlags )
{
    if( iHUDFlags & FOdysseyVectorHUD::HUD_GROUPPAINT_BUCKET )
    {
        std::list<FOdysseyVectorBucket*>& bucketList = iPaintGroup->GetBucketList();

        for( FOdysseyVectorBucket *bucket : bucketList )
        {
            DrawBucket( iParams, bucket, fgColor, bgColor, hcColor, iHUDFlags );
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawHierarchy( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                     , FOdysseyVectorObject* iTopObject
                                                     , const FLinearColor& iForegroundColor
                                                     , const FLinearColor& iBackgroundColor
                                                     , const FLinearColor& iHighlightColor
                                                     , uint64 iHUDFlags )
{
    // Run lambda
    FOdysseyVectorObject::Traverse
    ( iTopObject
    , 0
    , [ this
      , iParams
      , &iHUDFlags
      , &iForegroundColor
      , &iBackgroundColor
      , &iHighlightColor ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( object->GetCell()->ObjectHasFocus( object, traversalFlags ) || ( iHUDFlags & FOdysseyVectorHUD::HUD_DRAW_ALL ) )
          {
              if( iHUDFlags & FOdysseyVectorHUD::HUD_TAGINBETWEENER_ALL )
              {
                  FOdysseyVectorTag* tag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                  if( tag )
                  {
                      FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
/*
                      FOdysseyVectorHUD::DrawInbetweens( iScene
                                                       , iBLContext
                                                       , inbetweenerTag
                                                       , iForegroundColor
                                                       , iBackgroundColor
                                                       , iHighlightColor
                                                       , iHUDFlags );
*/
                  }
              }

              if( iHUDFlags & FOdysseyVectorHUD::HUD_PATH_ALL )
              {
                  if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                  {
                      FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                      DrawPath( iParams
                              , path
                              , /*iForegroundColor*/path->GetHUDColor()
                              , iBackgroundColor
                              , iHighlightColor
                              , iHUDFlags );
                  }
              }

              if( iHUDFlags & FOdysseyVectorHUD::HUD_GROUPPAINT_ALL )
              {
                  if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
                  {
                      FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

                      DrawGroupPaint( iParams
                                    , paintGroup
                                    , iForegroundColor
                                    , iBackgroundColor
                                    , iHighlightColor
                                    , iHUDFlags );
                  }
              }

              return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );
}

//3D HUD
void
FOdysseyPainterEditorVectorBaseToolHUD::DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams )
{
    if (!mBaseTool->GetWorkingCell() || !mBaseTool->GetWorkingCell()->GetScene())
        return;

    FOdysseyVectorGroupPaint* scene = mBaseTool->GetWorkingCell()->GetScene();
    FLinearColor fg = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bg = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hc = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    uint64 hudFlags = mBaseTool->GetEditor()->GetVectorHUDFlags();

    // Draw object details only in vertex mode
    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        DrawHierarchy( iParams
                     , scene
                     , fg
                     , bg
                     , hc
                     , hudFlags
                     | FOdysseyVectorHUD::HUD_PATH_VERTEX
                     | FOdysseyVectorHUD::HUD_PATH_SEGMENT );
    }

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        DrawHierarchy( iParams
                     , scene
                     , fg
                     , bg
                     , hc
                     , hudFlags
                     | FOdysseyVectorHUD::HUD_TAGINBETWEENER_ALL );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::Draw( BLContext* iBLContext )
{
}

// static
void
FOdysseyPainterEditorVectorBaseToolHUD::DrawVertex( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                  , FOdysseyVectorVertex* iVertex
                                                  , const FLinearColor& fgColor
                                                  , const FLinearColor& bgColor
                                                  , const FLinearColor& hcColor
                                                  , uint64 iHUDFlags )
{
    ::ULIS::FRectD screen = ::ULIS::FRectD::FromXYWH( 0, 0, iParams.mTextureWidth, iParams.mTextureHeight );
    FOdysseyVectorPath* path = iVertex->GetOwnerAsPath();
    BLMatrix2D& worldMatrix = path->GetWorldMatrix();
    // TODO: compute that once and pass it as parameter for all vertices
    BLPoint worldPoint = worldMatrix.map_point( iVertex->GetX(), iVertex->GetY() );
    FVector2D hudPoint = TextureToHUD( FVector2D( worldPoint.x, worldPoint.y ) );
    static FLinearColor greenColor  = FLinearColor(  0.0f,  1.0f,  0.0f, 1.0f );
    static FLinearColor ltgrayColor = FLinearColor(  0.5f,  0.5f,  0.5f, 1.0f );
    static FLinearColor dkgrayColor = FLinearColor( 0.25f, 0.25f, 0.25f, 1.0f );
    static FLinearColor redColor    = FLinearColor(  1.0f,  0.0f,  0.0f, 1.0f );
    double vertexRadius = FOdysseyVectorHUD::VERTEXRADIUS;
    double handleRadius = FOdysseyVectorHUD::HANDLERADIUS;
    ::ULIS::FRectD vertexBBox = ::ULIS::FRectD::FromMinMax( hudPoint.X - FOdysseyVectorHUD::VERTEXRADIUS
                                                          , hudPoint.Y - FOdysseyVectorHUD::VERTEXRADIUS
                                                          , hudPoint.X + FOdysseyVectorHUD::VERTEXRADIUS
                                                          , hudPoint.Y + FOdysseyVectorHUD::VERTEXRADIUS );

    if ( iHUDFlags & FOdysseyVectorHUD::HUD_PATH_VERTEX_HANDLE )
    {
        static FLinearColor whiteColor = FLinearColor( 1.0f, 1.0f, 1.0f, 1.0f );
        static FLinearColor blackColor = FLinearColor( 0x00, 0x00, 0x00, 1.0f );
        ::ULIS::FVec2D localHandleCoords[2];
        BLPoint texHandleCoords[2];
        FVector2D hudHandleCoords[2];
        FCanvasLineItem hudHandleLine[2];

        iVertex->GetHandlePosition( localHandleCoords );

        texHandleCoords[0] = worldMatrix.map_point( localHandleCoords[0].x, localHandleCoords[0].y );
        texHandleCoords[1] = worldMatrix.map_point( localHandleCoords[1].x, localHandleCoords[1].y );

        hudHandleCoords[0] = TextureToHUD( FVector2D( texHandleCoords[0].x, texHandleCoords[0].y ) );
        hudHandleCoords[1] = TextureToHUD( FVector2D( texHandleCoords[1].x, texHandleCoords[1].y ) );

        hudHandleLine[0] = FCanvasLineItem( hudPoint, hudHandleCoords[0] );
        hudHandleLine[1] = FCanvasLineItem( hudPoint, hudHandleCoords[1] );

        ::ULIS::FRectD bbox0 = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min( hudPoint.X, hudHandleCoords[0].X ) - handleRadius
                                                         , ::ULIS::FMath::Min( hudPoint.Y, hudHandleCoords[0].Y ) - handleRadius
                                                         , ::ULIS::FMath::Max( hudPoint.X, hudHandleCoords[0].X ) + handleRadius
                                                         , ::ULIS::FMath::Max( hudPoint.Y, hudHandleCoords[0].Y ) + handleRadius );

        if( ( bbox0 & screen ).Area() )
        {
            // Line to handle
            DrawPrimitiveLineOutlined( iParams
                                     , hudPoint
                                     , hudHandleCoords[0]
                                     , whiteColor
                                     , 1.0f );

            // handle
            DrawPrimitiveHandle( iParams
                               , hudHandleCoords[0]
                               , handleRadius
                               , whiteColor
                               , blackColor );
        }

        ::ULIS::FRectD bbox1 = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min( hudPoint.X, hudHandleCoords[1].X ) - handleRadius
                                                         , ::ULIS::FMath::Min( hudPoint.Y, hudHandleCoords[1].Y ) - handleRadius
                                                         , ::ULIS::FMath::Max( hudPoint.X, hudHandleCoords[1].X ) + handleRadius
                                                         , ::ULIS::FMath::Max( hudPoint.Y, hudHandleCoords[1].Y ) + handleRadius );

        if( ( bbox1 & screen ).Area() )
        {
            // Line to handle
            DrawPrimitiveLineOutlined( iParams
                                     , hudPoint
                                     , hudHandleCoords[1]
                                     , whiteColor
                                     , 1.0f );

            // handle
            DrawPrimitiveHandle( iParams
                               , hudHandleCoords[1]
                               , handleRadius
                               , whiteColor
                               , blackColor );
        }
    }

    if( ( vertexBBox & screen ).Area() )
    {
        if( iVertex->IsLocked() == false )
        {
            DrawPrimitiveVertex( iParams
                               , hudPoint
                               , vertexRadius
                               , iVertex->IsSelected() && ( iHUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX ) ? hcColor  : fgColor
                               , bgColor );
        }
        else
        {
            DrawPrimitiveVertex( iParams
                               , hudPoint
                               , vertexRadius
                               , iVertex->IsSelected() && ( iHUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX ) ? ( hcColor * 0.5f )  : ltgrayColor
                               , dkgrayColor );
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::FormatModifierInfo( const FText* iCtrlText
                                                          , const FText* iShiftText
                                                          , const FText* iAltText )
{
    FString modifierInfoString = FString("");
    FString tab = FString("");

    if( iCtrlText )
    {
#if PLATFORM_WINDOWS
        modifierInfoString = modifierInfoString + tab + "CTRL: " + iCtrlText->ToString();
#endif
#if PLATFORM_MAC
        modifierInfoString = modifierInfoString + tab + "CMD: " + iCtrlText->ToString();
#endif

        tab = FString("        ");
    }

    if( iShiftText )
    {
        modifierInfoString = modifierInfoString + tab + "SHIFT: " + iShiftText->ToString();

        tab = FString("        ");
    }

    if( iAltText )
    {
        modifierInfoString = modifierInfoString + tab + "ALT: " + iAltText->ToString();

        tab = FString("        ");
    }

    mModifierInfoText = FText::FromString( modifierInfoString );
}

FVector2D
FOdysseyPainterEditorVectorBaseToolHUD::TextureToHUD( const ::ULIS::FVec2D& iPosition )
{
    return TextureToHUD( FVector2D( iPosition.x, iPosition.y ) );
}

FVector2D
FOdysseyPainterEditorVectorBaseToolHUD::TextureToHUD( double iX
                                                    , double iY )
{
    return TextureToHUD( FVector2D( iX, iY ) );
}

FVector2D
FOdysseyPainterEditorVectorBaseToolHUD::TextureToHUD( const FVector2D& iPosition )
{
    return mCurrentHUDParams.mTextureToHUD.Execute( iPosition );
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawModifierInfo( const FOdysseyHUDElement::FDrawHUDParams& iParams )
{
    DrawInfo( iParams, mModifierInfoText, FLinearColor( 1.0f, 0.5f, 0.0f, 1.0f ) );
}

FVector2D
FOdysseyPainterEditorVectorBaseToolHUD::WorldPointToHUD( const FVector2D& iWorldCoords )
{
    FVector2D point = TextureToHUD( iWorldCoords );

    return point;
}

FVector2D
FOdysseyPainterEditorVectorBaseToolHUD::WorldVectorToHUD( const FVector2D& iWorldOriginCoords
                                                        , const FVector2D& iWorldVectorCoords )
{
    FVector2D origin = TextureToHUD( iWorldOriginCoords );
    FVector2D point = TextureToHUD( iWorldOriginCoords + iWorldVectorCoords );

    return FVector2D ( point.X - origin.X, point.Y - origin.Y );
}

::ULIS::FRectD
FOdysseyPainterEditorVectorBaseToolHUD::WorldRectToHUD( const ::ULIS::FRectD& iWorldRect )
{
    FVector2D p0 = TextureToHUD( FVector2D( iWorldRect.x, iWorldRect.y ) );
    FVector2D p1 = TextureToHUD( FVector2D( iWorldRect.x + iWorldRect.w
                                          , iWorldRect.y + iWorldRect.h ) );

    return ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min( p0.X, p1.X )
                                     , ::ULIS::FMath::Min( p0.Y, p1.Y )
                                     , ::ULIS::FMath::Max( p0.X, p1.X )
                                     , ::ULIS::FMath::Max( p0.Y, p1.Y ) );
}

::ULIS::FVec2D
FOdysseyPainterEditorVectorBaseToolHUD::WorldPointToHUD( const ::ULIS::FVec2D& iWorldCoords )
{
    FVector2D point = WorldPointToHUD( FVector2D( iWorldCoords.x, iWorldCoords.y ) );

    return ::ULIS::FVec2D( point.X, point.Y );
}

::ULIS::FVec2D
FOdysseyPainterEditorVectorBaseToolHUD::WorldVectorToHUD( const ::ULIS::FVec2D& iWorldOriginCoords
                                                        , const ::ULIS::FVec2D& iWorldVectorCoords )
{
    FVector2D vector = WorldVectorToHUD( FVector2D( iWorldOriginCoords.x, iWorldOriginCoords.y )
                                       , FVector2D( iWorldVectorCoords.x, iWorldVectorCoords.y ) );

    return ::ULIS::FVec2D ( vector.X, vector.Y );
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawInfo( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                , const FText& iText
                                                , const FLinearColor& iColor )
{
    const UFont* font = Cast<UFont>( mFontInfo.FontObject );
    int32 textWidth, textHeight;
    uint32 borderThickness = 2;
    FVector2D infoAt;

    font->GetStringHeightAndWidth( iText.ToString(), textHeight, textWidth );

    infoAt = FVector2D( iParams.mCanvas->GetViewRect().Width() * 0.5f
                      , iParams.mCanvas->GetViewRect().Height() - textHeight - 2 - borderThickness );

    infoAt.X -= ( textWidth * 0.5f );

    iParams.mCanvas->DrawTile( infoAt.X - 8
                             , infoAt.Y - borderThickness
                             , 8
                             , borderThickness + textHeight + borderThickness
                             , 0.0f
                             , 0.0f
                             , 1.0f
                             , 1.0f
                             , FLinearColor( 0.075f, 0.075f, 0.075f, 0.50f )
                             , mInfoBorderLeftTexture->GetResource()
                             , ESimpleElementBlendMode::SE_BLEND_AlphaBlend );

    iParams.mCanvas->DrawTile( infoAt.X
                             , infoAt.Y - borderThickness
                             , textWidth
                             , borderThickness + textHeight + borderThickness
                             , 0.0f
                             , 0.0f
                             , 1.0f
                             , 1.0f
                             , FLinearColor( 0.075f, 0.075f, 0.075f, 0.50f )
                             , mInfoBorderTexture->GetResource()
                             , ESimpleElementBlendMode::SE_BLEND_AlphaBlend );

    iParams.mCanvas->DrawTile( infoAt.X + textWidth
                             , infoAt.Y - borderThickness
                             , 8
                             , borderThickness + textHeight + borderThickness
                             , 0.0f
                             , 0.0f
                             , 1.0f
                             , 1.0f
                             , FLinearColor( 0.075f, 0.075f, 0.075f, 0.50f )
                             , mInfoBorderRightTexture->GetResource()
                             , ESimpleElementBlendMode::SE_BLEND_AlphaBlend );

    FCanvasTextItem infoItem = FCanvasTextItem( infoAt
                                              , iText
                                              , mFontInfo
                                              , iColor );

    //infoItem.EnableShadow( FLinearColor( 0, 0, 0, 1 ) );

    iParams.mCanvas->DrawItem( infoItem );


}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawPrimitiveVertex( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                           , const FVector2D& iHUDCoords
                                                           , double iRadius
                                                           , const FLinearColor& iFillColor
                                                           , const FLinearColor& iContourColor )
{
    iParams.mCanvas->DrawTile( iHUDCoords.X - iRadius
                             , iHUDCoords.Y - iRadius
                             , iRadius * 2.0f
                             , iRadius * 2.0f
                             , 0.0f
                             , 0.0f
                             , 1.0f
                             , 1.0f
                             , iFillColor
                             , mVertexTexture->GetResource()
                             , ESimpleElementBlendMode::SE_BLEND_Masked );
/*
    iParams.mCanvas->DrawTile( iHUDCoords.X - iRadius
                             , iHUDCoords.Y - iRadius
                             , iRadius * 2.0f
                             , iRadius * 2.0f
                             , 0.0f
                             , 0.0f
                             , 1.0f
                             , 1.0f
                             , iContourColor
                             , mVertexContourTexture->GetResource()
                             , ESimpleElementBlendMode::SE_BLEND_Masked );
*/

    // inner
//    iBLContext->set_fill_style( fgColor );
//    iBLContext->fill_circle( iWorldx, iWorldY, iRadius );
//    iBLContext->set_stroke_width( 1.0f );
//    iBLContext->set_stroke_style( bgColor );
//    iBLContext->stroke_circle( iWorldx, iWorldY, iRadius );
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawPrimitiveHandle( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                           , const FVector2D& iHUDCoords
                                                           , double iRadius
                                                           , const FLinearColor& iFillColor
                                                           , const FLinearColor& iContourColor )
{
    iParams.mCanvas->DrawTile( iHUDCoords.X - iRadius
                             , iHUDCoords.Y - iRadius
                             , iRadius * 2.0f
                             , iRadius * 2.0f
                             , 0.0f
                             , 0.0f
                             , 1.0f
                             , 1.0f
                             , iFillColor
                             , mHandleTexture->GetResource()
                             , ESimpleElementBlendMode::SE_BLEND_Masked );

    // inner
//    iBLContext->set_fill_style( fgColor );
//    iBLContext->fill_circle( iWorldx, iWorldY, iRadius );
//    iBLContext->set_stroke_width( 1.0f );
//    iBLContext->set_stroke_style( bgColor );
//    iBLContext->stroke_circle( iWorldx, iWorldY, iRadius );
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawBatchedLines( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                        , const std::vector<BatchedLine>& iLineBuffer
                                                        , const std::vector<FVector2D>& iPointBuffer
                                                        , const FLinearColor& iColor
                                                        , float iThickness )
{
    FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Triangle);
    FVector2D averagePerpendicular0 = iLineBuffer[0].perpendicular;
    double radius = iThickness * 0.5f;

    for( int32 i = 0; i < iLineBuffer.size(); i++ )
    {
        int32 n = i + 1;
        const BatchedLine& line = iLineBuffer[i];
        const BatchedLine* nextLine = ( n < iLineBuffer.size() ) ? &iLineBuffer[n] : nullptr;
        FVector2D averagePerpendicular1 = nextLine ? ( nextLine->perpendicular + line.perpendicular ) * 0.5f
                                                    : line.perpendicular;
        int32 idx0, idx1, idx2, idx3;

        if( averagePerpendicular1.SizeSquared() )
        {
            averagePerpendicular1.Normalize();
        }

        FVector4 vertex[4] = { FVector4( line.p0->X + ( averagePerpendicular0.X * radius )
                                       , line.p0->Y + ( averagePerpendicular0.Y * radius )
                                       , 0.0f
                                       , 1.0f )
                             , FVector4( line.p1->X + ( averagePerpendicular1.X * radius )
                                       , line.p1->Y + ( averagePerpendicular1.Y * radius )
                                       , 0.0f
                                       , 1.0f )
                             , FVector4( line.p1->X - ( averagePerpendicular1.X * radius )
                                       , line.p1->Y - ( averagePerpendicular1.Y * radius )
                                       , 0.0f
                                       , 1.0f )
                             , FVector4( line.p0->X - ( averagePerpendicular0.X * radius )
                                       , line.p0->Y - ( averagePerpendicular0.Y * radius )
                                       , 0.0f
                                       , 1.0f ) };

        idx0 = batchedElements->AddVertex( vertex[0], FVector2D(0, 0), iColor, iParams.mCanvas->GetHitProxyId() );
        idx1 = batchedElements->AddVertex( vertex[1], FVector2D(0, 0), iColor, iParams.mCanvas->GetHitProxyId() );
        idx2 = batchedElements->AddVertex( vertex[2], FVector2D(0, 0), iColor, iParams.mCanvas->GetHitProxyId() );
        idx3 = batchedElements->AddVertex( vertex[3], FVector2D(0, 0), iColor, iParams.mCanvas->GetHitProxyId() );

        batchedElements->AddTriangle( idx0, idx1, idx2, GWhiteTexture, BLEND_Translucent );
        batchedElements->AddTriangle( idx2, idx3, idx0, GWhiteTexture, BLEND_Translucent );

        averagePerpendicular0 = averagePerpendicular1;
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawPrimitiveBezierCubic( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                                , const FVector2D& iHUDCoordsP0
                                                                , const FVector2D& iHUDCoordsP1
                                                                , const FVector2D& iHUDCoordsP2
                                                                , const FVector2D& iHUDCoordsP3
                                                                , uint32 iFractionCount
                                                                , const FLinearColor& iColor
                                                                , float iThickness )
{
    ::ULIS::FVec2D p0 = ::ULIS::FVec2D( iHUDCoordsP0.X, iHUDCoordsP0.Y );
    double stepT = 1.0f / iFractionCount;
    double t = stepT;

    if( ( iThickness <= 1.0f ) || ( iColor.A == 1.0f ) )
    {
        for( uint32 i = 0; i < iFractionCount; i++ )
        {
            ::ULIS::FVec2D p1 = ::ULIS::CubicBezierPointAtParameter( ::ULIS::FVec2D( iHUDCoordsP0.X, iHUDCoordsP0.Y )
                                                                   , ::ULIS::FVec2D( iHUDCoordsP1.X, iHUDCoordsP1.Y )
                                                                   , ::ULIS::FVec2D( iHUDCoordsP2.X, iHUDCoordsP2.Y )
                                                                   , ::ULIS::FVec2D( iHUDCoordsP3.X, iHUDCoordsP3.Y )
                                                                   , t );

            DrawPrimitiveLine( iParams
                             , FVector2D( p0.x, p0.y )
                             , FVector2D( p1.x, p1.y )
                             , iColor
                             , iThickness );

            t += stepT;

            p0 = p1;
        }
    }
    else // using DrawPrimitiveLine gives visually bad results when we need some transparency with thickness > 1.0f
         // So we draw it a a set of triangles.
    {
        std::vector<FVector2D> pointBuffer;
        std::vector<BatchedLine> lineBuffer;

        pointBuffer.reserve( iFractionCount + 1 );
        lineBuffer.reserve( iFractionCount );

        pointBuffer.push_back( FVector2D( p0.x, p0.y ) );

        for( uint32 i = 0; i < iFractionCount; i++ )
        {
            ::ULIS::FVec2D p1 = ::ULIS::CubicBezierPointAtParameter( ::ULIS::FVec2D( iHUDCoordsP0.X, iHUDCoordsP0.Y )
                                                                   , ::ULIS::FVec2D( iHUDCoordsP1.X, iHUDCoordsP1.Y )
                                                                   , ::ULIS::FVec2D( iHUDCoordsP2.X, iHUDCoordsP2.Y )
                                                                   , ::ULIS::FVec2D( iHUDCoordsP3.X, iHUDCoordsP3.Y )
                                                                   , t );
            uint32 n = i + 1;

            pointBuffer.push_back( FVector2D( p1.x, p1.y ) );

            lineBuffer.emplace_back( &pointBuffer[i], &pointBuffer[n] );

            t += stepT;

            p0 = p1;
        }

        if( lineBuffer.size() )
        {
            DrawBatchedLines( iParams, lineBuffer, pointBuffer, iColor, iThickness );
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawPrimitiveBezierQuadratic( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                                    , const FVector2D& iHUDCoordsP0
                                                                    , const FVector2D& iHUDCoordsP1
                                                                    , const FVector2D& iHUDCoordsP2
                                                                    , uint32 iFractionCount
                                                                    , const FLinearColor& iColor
                                                                    , float iThickness )
{
    ::ULIS::FVec2D p0 = ::ULIS::FVec2D( iHUDCoordsP0.X, iHUDCoordsP0.Y );
    double stepT = 1.0f / iFractionCount;
    double t = stepT;

    if( ( iThickness <= 1.0f ) || ( iColor.A == 1.0f ) )
    {
        for( uint32 i = 0; i < iFractionCount; i++ )
        {
            ::ULIS::FVec2D p1 = ::ULIS::QuadraticBezierPointAtParameter( ::ULIS::FVec2D( iHUDCoordsP0.X, iHUDCoordsP0.Y )
                                                                       , ::ULIS::FVec2D( iHUDCoordsP1.X, iHUDCoordsP1.Y )
                                                                       , ::ULIS::FVec2D( iHUDCoordsP2.X, iHUDCoordsP2.Y )
                                                                       , t );

            DrawPrimitiveLine( iParams
                             , FVector2D( p0.x, p0.y )
                             , FVector2D( p1.x, p1.y )
                             , iColor
                             , iThickness );

            t += stepT;

            p0 = p1;
        }
    }
    else // using DrawPrimitiveLine gives visually bad results when we need some transparency with thickness > 1.0f
         // So we draw it a a set of triangles.
    {
        std::vector<FVector2D> pointBuffer;
        std::vector<BatchedLine> lineBuffer;

        pointBuffer.reserve( iFractionCount + 1 );
        lineBuffer.reserve( iFractionCount );

        pointBuffer.push_back( FVector2D( p0.x, p0.y ) );

        for( uint32 i = 0; i < iFractionCount; i++ )
        {
            ::ULIS::FVec2D p1 = ::ULIS::QuadraticBezierPointAtParameter( ::ULIS::FVec2D( iHUDCoordsP0.X, iHUDCoordsP0.Y )
                                                                       , ::ULIS::FVec2D( iHUDCoordsP1.X, iHUDCoordsP1.Y )
                                                                       , ::ULIS::FVec2D( iHUDCoordsP2.X, iHUDCoordsP2.Y )
                                                                       , t );
            uint32 n = i + 1;

            pointBuffer.push_back( FVector2D( p1.x, p1.y ) );

            lineBuffer.emplace_back( &pointBuffer[i], &pointBuffer[n] );

            t += stepT;

            p0 = p1;
        }

        if( lineBuffer.size() )
        {
            DrawBatchedLines( iParams, lineBuffer, pointBuffer, iColor, iThickness );
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawPrimitivePlus( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                         , const FVector2D& iHUDCoords
                                                         , uint32 iSize
                                                         , const FLinearColor& iColor
                                                         , float iThickness )
{
    FCanvasLineItem horzLine = FCanvasLineItem( FVector2D( iHUDCoords.X - iSize, iHUDCoords.Y )
                                              , FVector2D( iHUDCoords.X + iSize, iHUDCoords.Y ) );
    FCanvasLineItem vertLine = FCanvasLineItem( FVector2D( iHUDCoords.X, iHUDCoords.Y - iSize )
                                              , FVector2D( iHUDCoords.X, iHUDCoords.Y + iSize ) );

    horzLine.LineThickness = iThickness;
    horzLine.SetColor( iColor );
    iParams.mCanvas->DrawItem( horzLine );

    vertLine.LineThickness = iThickness;
    vertLine.SetColor( iColor );
    iParams.mCanvas->DrawItem( vertLine );
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawPrimitiveLineOutlined( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                                 , const FVector2D& iHUDCoordsP0
                                                                 , const FVector2D& iHUDCoordsP1
                                                                 , const FLinearColor& iColor
                                                                 , float iThickness )
{
    FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Triangle);
    FVector2D vec = ( iHUDCoordsP1 - iHUDCoordsP0 );
    FVector2D perpendicular = FVector2D( -vec.Y, vec.X );
    double radius = ( iThickness + 1.0f ) * 0.5f;

    if( perpendicular.SizeSquared() )
    {
        perpendicular.Normalize();

        FVector4 vertex[4] = { FVector4( iHUDCoordsP0.X + ( perpendicular.X * radius )
                                       , iHUDCoordsP0.Y + ( perpendicular.Y * radius )
                                       , 0.0f
                                       , 1.0f )
                             , FVector4( iHUDCoordsP1.X + ( perpendicular.X * radius )
                                       , iHUDCoordsP1.Y + ( perpendicular.Y * radius )
                                       , 0.0f
                                       , 1.0f )
                             , FVector4( iHUDCoordsP1.X - ( perpendicular.X * radius )
                                       , iHUDCoordsP1.Y - ( perpendicular.Y * radius )
                                       , 0.0f
                                       , 1.0f )
                             , FVector4( iHUDCoordsP0.X - ( perpendicular.X * radius )
                                       , iHUDCoordsP0.Y - ( perpendicular.Y * radius )
                                       , 0.0f
                                       , 1.0f ) };
        int32 idx0, idx1, idx2, idx3;

        idx0 = batchedElements->AddVertex( vertex[0], FVector2D(0, 0), iColor, iParams.mCanvas->GetHitProxyId() );
        idx1 = batchedElements->AddVertex( vertex[1], FVector2D(1, 0), iColor, iParams.mCanvas->GetHitProxyId() );
        idx2 = batchedElements->AddVertex( vertex[2], FVector2D(1, 1), iColor, iParams.mCanvas->GetHitProxyId() );
        idx3 = batchedElements->AddVertex( vertex[3], FVector2D(0, 1), iColor, iParams.mCanvas->GetHitProxyId() );

        batchedElements->AddTriangle( idx0, idx1, idx2, mLineOutlinedTexture->GetResource(), BLEND_Translucent );
        batchedElements->AddTriangle( idx2, idx3, idx0, mLineOutlinedTexture->GetResource(), BLEND_Translucent );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawPrimitiveLine( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                         , const FVector2D& iHUDCoordsP0
                                                         , const FVector2D& iHUDCoordsP1
                                                         , const FLinearColor& iColor
                                                         , float iThickness )
{
    // use FCanvasLineItem if opaque, as it is faster
    if( iColor.A == 1.0f )
    {
        FCanvasLineItem line = FCanvasLineItem( iHUDCoordsP0, iHUDCoordsP1 );

        line.LineThickness = iThickness;
        line.SetColor( iColor );
        iParams.mCanvas->DrawItem( line );
    }
    else
    {
        FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Line);

        batchedElements->AddTranslucentLine( FVector( iHUDCoordsP0, 0.f)
                                           , FVector( iHUDCoordsP1, 0.f)
                                           , iColor
                                           , iParams.mCanvas->GetHitProxyId()
                                           , iThickness
                                           , 0.f
                                           , true );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawPrimitiveCircle( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                           , const FVector2D& iHUDCoords
                                                           , double iRadius
                                                           , const FLinearColor& iColor
                                                           , float iThickness )
{
    ::ULIS::TArray<::ULIS::FVec2I> points;
    ::ULIS::GenerateEllipsePoints( ::ULIS::FVec2I ( iHUDCoords.X, iHUDCoords.Y ), iRadius, iRadius, points );

    if( points.Size() )
    {
        FVector2D hudFirstPoint = FVector2D( points[0].x, points[0].y );
        FVector2D hudLineP0 = hudFirstPoint;

        for( uint32 i = 1; i < points.Size(); i++)
        {
            uint32 n = ( i + 1 ) % points.Size();
            FVector2D hudLineP1 = FVector2D( points[i].x, points[i].y );
            FCanvasLineItem hudLine = FCanvasLineItem( hudLineP0, hudLineP1 );

            DrawPrimitiveLine( iParams, hudLineP0, hudLineP1, iColor, iThickness );

            hudLineP0 = hudLineP1;
        }

        DrawPrimitiveLine( iParams, hudLineP0, hudFirstPoint, iColor, iThickness );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawCubicSegment( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                        , FOdysseyVectorSegmentCubic* iCubicSegment
                                                        , const FLinearColor& fgColor
                                                        , const FLinearColor& bgColor
                                                        , const FLinearColor& hcColor
                                                        , uint64 iHUDFlags )
{
    ::ULIS::FRectD screen = ::ULIS::FRectD::FromXYWH( 0, 0, iParams.mTextureWidth, iParams.mTextureHeight );
    ::ULIS::FRectD segmentBBox = WorldRectToHUD( iCubicSegment->GetBoundingBox( true ) );

    if( ( segmentBBox & screen ).Area() )
    {
        FOdysseyVectorObject* owner = iCubicSegment->GetOwner();
        BLMatrix2D& worldMatrix = owner->GetWorldMatrix();
        FOdysseyVectorVertex* vertex[2] = { iCubicSegment->GetVertex(0)
                                          , iCubicSegment->GetVertex(1) };
        FOdysseyVectorHandleSegment* handle[2] = { iCubicSegment->GetHandle(0)
                                                 , iCubicSegment->GetHandle(1) };
        ::ULIS::FVec2D texVertex[2] = { FOdysseyVector::MapPoint( worldMatrix, vertex[0]->GetCoords() )
                                      , FOdysseyVector::MapPoint( worldMatrix, vertex[1]->GetCoords() ) };
        FVector2D hudVertex[2] = { TextureToHUD( FVector2D( texVertex[0].x, texVertex[0].y ) )
                                 , TextureToHUD( FVector2D( texVertex[1].x, texVertex[1].y ) ) };
        ::ULIS::FVec2D texHandle[2] = { FOdysseyVector::MapPoint( worldMatrix, handle[0]->GetCoords() )
                                      , FOdysseyVector::MapPoint( worldMatrix, handle[1]->GetCoords() ) };
        FVector2D hudHandle[2] = { TextureToHUD( FVector2D( texHandle[0].x, texHandle[0].y ) )
                                 , TextureToHUD( FVector2D( texHandle[1].x, texHandle[1].y ) ) };
        FVector2D hudFractionP0 = hudVertex[0];

        DrawPrimitiveBezierCubic( iParams
                                 , hudVertex[0]
                                 , hudHandle[0]
                                 , hudHandle[1]
                                 , hudVertex[1]
                                 , iCubicSegment->GetFractionCache().size()
                                 , fgColor
                                 , 1.0f );

        if ( iHUDFlags & FOdysseyVectorHUD::HUD_PATH_SEGMENT_HANDLE )
        {
            static FLinearColor whiteColor = FLinearColor( 1.0f, 1.0f, 1.0f, 1.0f );
            static FLinearColor blackColor = FLinearColor( 0.0f, 0.0f, 0.0f, 1.0f );
            static FLinearColor greenColor = FLinearColor( 0.0f, 1.0f, 0.0f, 1.0f );

            for( int i = 0; i < 2; i++ )
            {
                // line to handle
                DrawPrimitiveLineOutlined( iParams
                                         , hudVertex[i]
                                         , hudHandle[i]
                                         , vertex[i]->IsHandleAligned() ? greenColor : whiteColor
                                         , 1.0f );

                // control handle
                DrawPrimitiveHandle( iParams
                                   , hudHandle[i]
                                   , FOdysseyVectorHUD::HANDLERADIUS
                                   , whiteColor
                                   , blackColor );
            }
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawSectionArray( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                        , std::vector<FOdysseyVectorSection*>& iSectionArray
                                                        , const FLinearColor& fgColor
                                                        , const FLinearColor& bgColor
                                                        , uint64 iHUDFlags )
{
    struct HUDBezierCubic
    {
        HUDBezierCubic( const FVector2D *iPoint, uint32 iResolution )
        {
            point[0] = iPoint[0];
            point[1] = iPoint[1];
            point[2] = iPoint[2];
            point[3] = iPoint[3];

            resolution = iResolution;
        }

        FVector2D point[4];
        uint32 resolution;
    };

    std::vector<HUDBezierCubic> HUDBezierCubicBuffer;

    HUDBezierCubicBuffer.reserve( iSectionArray.size() );

    for( FOdysseyVectorSection* section : iSectionArray )
    {
         BLMatrix2D& worldMatrix = section->GetOwner()->GetWorldMatrix();
        ::ULIS::FVec2D* bezier = section->GetBezier();
        ::ULIS::FVec2D texBezier[4] = { FOdysseyVector::MapPoint( worldMatrix, bezier[0] )
                                      , FOdysseyVector::MapPoint( worldMatrix, bezier[1] )
                                      , FOdysseyVector::MapPoint( worldMatrix, bezier[2] )
                                      , FOdysseyVector::MapPoint( worldMatrix, bezier[3] ) };
        FVector2D hudbezier[4] = { TextureToHUD( FVector2D( texBezier[0].x, texBezier[0].y ) )
                                 , TextureToHUD( FVector2D( texBezier[1].x, texBezier[1].y ) )
                                 , TextureToHUD( FVector2D( texBezier[2].x, texBezier[2].y ) )
                                 , TextureToHUD( FVector2D( texBezier[3].x, texBezier[3].y ) ) };

        HUDBezierCubicBuffer.emplace_back( hudbezier, section->GetSegment()->GetFractionCache().size() );
    }

    // we do it in to passes or else it creates some artefact. We first draw the outlines, then the inner lines.
    // first pass
    for( HUDBezierCubic& cubicBezier : HUDBezierCubicBuffer )
    {
        DrawPrimitiveBezierCubic( iParams
                                , cubicBezier.point[0]
                                , cubicBezier.point[1]
                                , cubicBezier.point[2]
                                , cubicBezier.point[3]
                                , cubicBezier.resolution
                                , bgColor
                                , 3.0f );
    }

    // second pass
    for( HUDBezierCubic& cubicBezier : HUDBezierCubicBuffer )
    {
        DrawPrimitiveBezierCubic( iParams
                                , cubicBezier.point[0]
                                , cubicBezier.point[1]
                                , cubicBezier.point[2]
                                , cubicBezier.point[3]
                                , cubicBezier.resolution
                                , fgColor
                                , 2.0f );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawCycle( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                 , FOdysseyVectorCycle* iCycle
                                                 , const FLinearColor& fgColor
                                                 , const FLinearColor& bgColor
                                                 , uint64 iHUDFlags )
{
    DrawSectionArray( iParams, iCycle->GetInnerSectionArray(), fgColor, bgColor, iHUDFlags );
    DrawSectionArray( iParams, iCycle->GetContourSectionArray(), fgColor, bgColor, iHUDFlags );
}

::ULIS::FRectD
FOdysseyPainterEditorVectorBaseToolHUD::BBoxToHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                 , const ::ULIS::FRectD& iBBox )
{
    FVector2D origin = WorldPointToHUD( FVector2D( iBBox.x, iBBox.y ) );
    FVector2D size = WorldVectorToHUD( origin, FVector2D( iBBox.w, iBBox.h ) );

    return ::ULIS::FRectD::FromXYWH( origin.X, origin.Y, size.X, size.Y );
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawPath( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                , FOdysseyVectorPath* iPath
                                                , const FLinearColor& fgColor
                                                , const FLinearColor& bgColor
                                                , const FLinearColor& hcColor
                                                , uint64 iHUDFlags )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iPath->GetSegmentList();
    std::list<FOdysseyVectorVertex*>& vertexList = iPath->GetVertexList();

    if( iHUDFlags & FOdysseyVectorHUD::HUD_PATH_SEGMENT )
    {
        for( FOdysseyVectorSegment* segment : segmentList )
        {
            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

                DrawCubicSegment( iParams, cubicSegment, fgColor, bgColor, hcColor, iHUDFlags );
            }
        }
    }

    if( iHUDFlags & FOdysseyVectorHUD::HUD_PATH_VERTEX )
    {
        // Points and Point size handles
        for( FOdysseyVectorVertex* vertex : vertexList )
        {
            uint32 valence = vertex->GetSegmentCount();

            if( ( valence == 0 ) && ( iHUDFlags & FOdysseyVectorHUD::HUD_PATH_VERTEX_VALENCE0 ) )
            {
                DrawVertex( iParams, vertex, fgColor, bgColor, hcColor, iHUDFlags );
            }
            else
            if( ( valence == 1 ) && ( iHUDFlags & FOdysseyVectorHUD::HUD_PATH_VERTEX_VALENCE1 ) )
            {
                DrawVertex( iParams, vertex, fgColor, bgColor, hcColor, iHUDFlags );
            }
            else
            if( ( valence == 2 ) && ( iHUDFlags & FOdysseyVectorHUD::HUD_PATH_VERTEX_VALENCE2 ) )
            {
                DrawVertex( iParams, vertex, fgColor, bgColor, hcColor, iHUDFlags );
            }
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawGrid( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                , FInbetweenerGrid* iGrid
                                                , eInbetweenerPointPositionType iPositionType
                                                , const FLinearColor& iColor
                                                , uint64 iHUDFlags )
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = iGrid->GetBreakdown()->GetInbetweenerTag();
    BLMatrix2D worldMatrix = inbetweenerTag->GetOwner()->GetWorldMatrix();
    std::vector<FInbetweenerPoint>& pointBuffer = iGrid->GetPointBuffer();
    std::vector<FInbetweenerQuad>& quadBuffer = iGrid->GetQuadBuffer();

    if( iPositionType == eInbetweenerPointPositionType::TargetPosition )
    {
        worldMatrix.transform( iGrid->GetBreakdown()->GetTargetLocalMatrix() );
    }

    if( iHUDFlags & FOdysseyVectorHUD::HUD_BREAKDOWN_GRID_DOTTED )
    {
        for( uint32 pointIndex : inbetweenerTag->GetUsedPointIndexBuffer() )
        {
            FInbetweenerPoint& point = pointBuffer[pointIndex];
            ::ULIS::FVec2D position = point.GetPosition( iPositionType );
            ::ULIS::FVec2D texCoords = FOdysseyVector::MapPoint( worldMatrix, position );
            FVector2D hudCoords = TextureToHUD( FVector2D( texCoords.x, texCoords.y ) );

            DrawPrimitiveVertex( iParams, hudCoords, 2.0f, iColor, iColor );
        }
    }
    else
    {
        for( uint32 quadIndex : inbetweenerTag->GetUsedQuadIndexBuffer() )
        {
            FInbetweenerQuad& quad = quadBuffer[quadIndex];
            ::ULIS::FVec2D position[4] = { quad.GetPoints()[0]->GetPosition( iPositionType )
                                         , quad.GetPoints()[1]->GetPosition( iPositionType )
                                         , quad.GetPoints()[2]->GetPosition( iPositionType )
                                         , quad.GetPoints()[3]->GetPosition( iPositionType ) };
            ::ULIS::FVec2D pointTexCoords[4] = { FOdysseyVector::MapPoint( worldMatrix, position[0] )
                                               , FOdysseyVector::MapPoint( worldMatrix, position[1] )
                                               , FOdysseyVector::MapPoint( worldMatrix, position[2] )
                                               , FOdysseyVector::MapPoint( worldMatrix, position[3] ) };
            FVector2D pointHUDCoords[4] = { TextureToHUD( FVector2D( pointTexCoords[0].x, pointTexCoords[0].y ) )
                                          , TextureToHUD( FVector2D( pointTexCoords[1].x, pointTexCoords[1].y ) )
                                          , TextureToHUD( FVector2D( pointTexCoords[2].x, pointTexCoords[2].y ) )
                                          , TextureToHUD( FVector2D( pointTexCoords[3].x, pointTexCoords[3].y ) ) };

            DrawPrimitiveLine( iParams, pointHUDCoords[0], pointHUDCoords[1], iColor, 1.0f );
            DrawPrimitiveLine( iParams, pointHUDCoords[1], pointHUDCoords[2], iColor, 1.0f );
            DrawPrimitiveLine( iParams, pointHUDCoords[2], pointHUDCoords[3], iColor, 1.0f );
            DrawPrimitiveLine( iParams, pointHUDCoords[3], pointHUDCoords[0], iColor, 1.0f );
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawInbetweenerInterpolatedPathAt( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                                         , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                         , FInterpolatedPath* iInterpolatedPath
                                                                         , FInbetweenerChart::Inbetween* iInbetween
                                                                         , const FLinearColor& iColor )
{
    uint32 pathPointCount = iInterpolatedPath->GetInterpolatedPointBuffer().size();
    uint32 inbetweenAbsoluteIndex = iInbetween->GetIndexInInbetweener();
    FInterpolatedPath::PointGeometry* interpolatedPointGeometryBuffer = &iInterpolatedPath->GetInterpolatedPointGeometryBuffer()[pathPointCount * inbetweenAbsoluteIndex];
    BLMatrix2D worldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();
    bool mapAsPolyline = iInbetweenerTag->GetMapAsPolyline();
    FOdysseyVectorPath* originalPath = iInterpolatedPath->GetOriginalPath();

    // passed to DrawPathAt()
    worldMatrix.transform( iInbetween->GetDrawing()->localMatrix );
    worldMatrix.transform( iInterpolatedPath->GetRelativeMatrix() );

    for( FInterpolatedSegment& interpolatedSegment : iInterpolatedPath->GetInterpolatedSegmentBuffer() )
    {
        std::vector<FInterpolatedPoint*>& interpolatedPointArray = interpolatedSegment.GetInterpolatedPointArray();
        FOdysseyVectorSegment* segment = interpolatedSegment.GetOriginalSegment();

        if( mapAsPolyline )
        {
            uint32 fractionCount = interpolatedSegment.GetInterpolatedPointArray().size() - 1;
            FInterpolatedPoint* point0 = interpolatedPointArray[0];
            ::ULIS::FVec2D* localPointPosition0 = &interpolatedPointGeometryBuffer[point0->GetIndex()].position;
            ::ULIS::FVec2D texCoords0 = FOdysseyVector::MapPoint( worldMatrix, *localPointPosition0 );
            FVector2D hudCoords0 = TextureToHUD( FVector2D( texCoords0.x, texCoords0.y ) );
            std::vector<FVector2D> pointBuffer;
            std::vector<BatchedLine> lineBuffer;

            pointBuffer.reserve( fractionCount + 1 );
            lineBuffer.reserve( fractionCount );

            pointBuffer.push_back( hudCoords0 );

            for( uint32 i = 0; i < fractionCount; i++ )
            {
                uint32 n = i + 1;
                FInterpolatedPoint* point1 = interpolatedPointArray[n];
                ::ULIS::FVec2D* localPointPosition1 = &interpolatedPointGeometryBuffer[point1->GetIndex()].position;
                ::ULIS::FVec2D texCoords1 = FOdysseyVector::MapPoint( worldMatrix, *localPointPosition1 );
                FVector2D hudCoords1 = TextureToHUD( FVector2D( texCoords1.x, texCoords1.y ) );

                pointBuffer.push_back( hudCoords1 );
                lineBuffer.emplace_back( &pointBuffer[i], &pointBuffer[n] );
            }

            DrawBatchedLines( iParams, lineBuffer, pointBuffer, iColor, 4.0f );
        }
        else
        {
            if( interpolatedSegment.GetOriginalSegment()->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FInterpolatedPoint* interpolatedPoint[4] = { interpolatedPointArray[0]
                                                           , interpolatedPointArray[1]
                                                           , interpolatedPointArray[2]
                                                           , interpolatedPointArray[3] };
                ::ULIS::FVec2D texCoords[4] = { FOdysseyVector::MapPoint( worldMatrix, interpolatedPointGeometryBuffer[interpolatedPoint[0]->GetIndex()].position )
                                              , FOdysseyVector::MapPoint( worldMatrix, interpolatedPointGeometryBuffer[interpolatedPoint[1]->GetIndex()].position )
                                              , FOdysseyVector::MapPoint( worldMatrix, interpolatedPointGeometryBuffer[interpolatedPoint[2]->GetIndex()].position )
                                              , FOdysseyVector::MapPoint( worldMatrix, interpolatedPointGeometryBuffer[interpolatedPoint[3]->GetIndex()].position ) };
                FVector2D hudCoords[4] = { TextureToHUD( FVector2D( texCoords[0].x, texCoords[0].y ) )
                                         , TextureToHUD( FVector2D( texCoords[1].x, texCoords[1].y ) )
                                         , TextureToHUD( FVector2D( texCoords[2].x, texCoords[2].y ) )
                                         , TextureToHUD( FVector2D( texCoords[3].x, texCoords[3].y ) ) };

                DrawPrimitiveBezierCubic( iParams
                                        , hudCoords[0]
                                        , hudCoords[1]
                                        , hudCoords[2]
                                        , hudCoords[3]
                                        , 24
                                        , iColor
                                        , 4.0f );
            }
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawBreakdown( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                                     , FInbetweenerBreakdown* iBreakdown
                                                     , const FLinearColor& iSourceDrawingColor
                                                     , const FLinearColor& iTargetDrawingColor
                                                     , uint64 iHUDFlags )
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = iBreakdown->GetInbetweenerTag();
    BLMatrix2D worldMatrix = inbetweenerTag->GetOwner()->GetWorldMatrix();
    FLinearColor inbetweenColor = FLinearColor( inbetweenerTag->GetInbetweenColor() );

    inbetweenColor.A = 1.0f;

    if( iHUDFlags & FOdysseyVectorHUD::HUD_BREAKDOWN_INBETWEEN )
    {
        int32 sourceDrawingIndex = iBreakdown->GetSourceDrawingIndex();
        int32 targetDrawingIndex = iBreakdown->GetTargetDrawingIndex();
        double sourceDrawingSpacing = (double) sourceDrawingIndex / inbetweenerTag->GetDrawingBuffer().size();
        double targetDrawingSpacing = (double) targetDrawingIndex / inbetweenerTag->GetDrawingBuffer().size();

        for( uint32 i = 1; i < iBreakdown->GetDrawingCount() - 1; i++ )
        {
            FInbetweenerChart::Inbetween* inbetween = &iBreakdown->GetChart()->GetInbetweenBuffer()[i];
            uint32 drawingIndex = inbetween->GetDrawing()->GetIndex();
            double spacing = ( iHUDFlags & FOdysseyVectorHUD::HUD_INBETWEEN_FADERELATIVE ) ? inbetween->GetSpacing()
                                                                                           : sourceDrawingSpacing + ( ( targetDrawingSpacing - sourceDrawingSpacing ) * inbetween->GetSpacing() );
            inbetweenColor.A = 0.25f + ( 0.75f * spacing ); // minimum alpha is 0.25f

            for( FInterpolatedPath& interpolatedPath : inbetweenerTag->GetInterpolatedPathBuffer() )
            {
                DrawInbetweenerInterpolatedPathAt( iParams
                                                 , inbetweenerTag
                                                 , &interpolatedPath
                                                 , inbetween
                                                 , inbetweenColor );
            }
        }
    }

    if( iHUDFlags & FOdysseyVectorHUD::HUD_BREAKDOWN_SOURCE )
    {
        for( FInterpolatedPath& interpolatedPath : inbetweenerTag->GetInterpolatedPathBuffer() )
        {
            DrawInbetweenerInterpolatedPathAt( iParams
                                             , inbetweenerTag
                                             , &interpolatedPath
                                             , &iBreakdown->GetChart()->GetInbetweenBuffer().front()
                                             , iSourceDrawingColor );
        }
    }

    if( iHUDFlags & FOdysseyVectorHUD::HUD_BREAKDOWN_TARGET )
    {
        for( FInterpolatedPath& interpolatedPath : inbetweenerTag->GetInterpolatedPathBuffer() )
        {
            DrawInbetweenerInterpolatedPathAt( iParams
                                             , inbetweenerTag
                                             , &interpolatedPath
                                             , &iBreakdown->GetChart()->GetInbetweenBuffer().back()
                                             , iTargetDrawingColor );
        }
    }

    if( iHUDFlags & FOdysseyVectorHUD::HUD_BREAKDOWN_SOURCE_GRID )
    {
        DrawGrid( iParams
                , iBreakdown->GetGrid()
                , eInbetweenerPointPositionType::SourcePosition
                , iSourceDrawingColor
                , iHUDFlags );
    }

    if( iHUDFlags & FOdysseyVectorHUD::HUD_BREAKDOWN_TARGET_GRID )
    {
        DrawGrid( iParams
                , iBreakdown->GetGrid()
                , eInbetweenerPointPositionType::TargetPosition
                , FLinearColor( inbetweenerTag->GetGridColor() )
                , iHUDFlags );
    }
}

bool
FOdysseyPainterEditorVectorBaseToolHUD::OnMouseClick( const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey)
{
    return mBaseTool->OnMouseClickViaHUD( iPointInTexture, iKey );
}

void
FOdysseyPainterEditorVectorBaseToolHUD::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    return mBaseTool->OnMouseHoverViaHUD( iPointInTexture );
}

bool
FOdysseyPainterEditorVectorBaseToolHUD::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                   , const FKey& iKey)
{
    /**
     * Save the MaintTool to Recent Tools when clicking on a HUD
     * because it does not go in the Tool OnMouseDown in this case.
     */
    if (mBaseTool->GetEditor())
        mBaseTool->GetEditor()->SaveMainToolToRecentTools();

    return mBaseTool->OnMouseDownViaHUD( iPointInTexture, iKey );
}

bool
FOdysseyPainterEditorVectorBaseToolHUD::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                                 , const FKey& iKey)
{
    return mBaseTool->OnMouseUpViaHUD( iPointInTexture, iKey );
}

void
FOdysseyPainterEditorVectorBaseToolHUD::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    //SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    return mBaseTool->OnMouseDragViaHUD( iPointInTexture );
}

bool
FOdysseyPainterEditorVectorBaseToolHUD::CanReceiveEvents()
{
    FKey lastKey = FOdysseyKeyState::GetLastKey();

    if ( lastKey != FKey() )
    {
        FModifierKeysState ModifierKeysState = FSlateApplication::Get().GetModifierKeys();
        const FInputChord activeChord( lastKey,
            EModifierKey::FromBools(
                ModifierKeysState.IsControlDown(),
                ModifierKeysState.IsAltDown(),
                ModifierKeysState.IsShiftDown(),
                ModifierKeysState.IsCommandDown()
            )
        );

    if ( ( FOdysseyViewportCommands::Get().PanZoomViewport->HasActiveChord( activeChord ) )
      || ( FOdysseyViewportCommands::Get().RotateViewport->HasActiveChord( activeChord ) ) )
        {
            return false;
        }
    }

    return true;
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawDummyPlane( const FOdysseyHUDElement::FDrawHUDParams& iParams )
{
    if( iParams.mCanvas->IsHitTesting() && CanReceiveEvents() )
    {
        FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements( FCanvas::ET_Triangle );
        // Note: when HITProxy is testing, mCanvas->ViewRect is 0. so we use GetRenderTarget() instead of GetViewRect().
        int32 width= iParams.mCanvas->GetRenderTarget()->GetSizeXY().X;
        int32 height = iParams.mCanvas->GetRenderTarget()->GetSizeXY().Y;

        FVector4 vertex[4] = { FVector4( 0
                                       , 0
                                       , 0.0f
                                       , 1.0f )
                             , FVector4( width
                                       , 0
                                       , 0.0f
                                       , 1.0f )
                             , FVector4( width
                                       , height
                                       , 0.0f
                                       , 1.0f )
                             , FVector4( 0
                                       , height
                                       , 0.0f
                                       , 1.0f ) };

        const FLinearColor color( 0.f, 1.f, 0.f, 0.15f );
        int32 idx0, idx1, idx2, idx3;

        iParams.mCanvas->SetHitProxy( new HOdysseyHUDDummyCursorHitProxy( SharedThis ( this ) ) );

        idx0 = batchedElements->AddVertex( vertex[0], FVector2D(0, 0), color, iParams.mCanvas->GetHitProxyId() );
        idx1 = batchedElements->AddVertex( vertex[1], FVector2D(1, 0), color, iParams.mCanvas->GetHitProxyId() );
        idx2 = batchedElements->AddVertex( vertex[2], FVector2D(1, 1), color, iParams.mCanvas->GetHitProxyId() );
        idx3 = batchedElements->AddVertex( vertex[3], FVector2D(0, 1), color, iParams.mCanvas->GetHitProxyId() );

        batchedElements->AddTriangle( idx0, idx1, idx2, GWhiteTexture, BLEND_Translucent );
        batchedElements->AddTriangle( idx2, idx3, idx0, GWhiteTexture, BLEND_Translucent );

        iParams.mCanvas->SetHitProxy( nullptr );
    }

    FOdysseyHUDElement::DrawHUD(iParams);
}

void
FOdysseyPainterEditorVectorBaseToolHUD::SetCursorPosition( double iWorldX, double iWorldY )
{
    mX = iWorldX;
    mY = iWorldY;
}
