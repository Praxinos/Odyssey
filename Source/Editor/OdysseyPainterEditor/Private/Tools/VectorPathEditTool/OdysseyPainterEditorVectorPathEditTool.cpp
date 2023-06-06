// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathEditTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathEditTool::~UOdysseyPainterEditorVectorPathEditTool()
{
}

UOdysseyPainterEditorVectorPathEditTool::UOdysseyPainterEditorVectorPathEditTool()
    : mCubicPathHUD( FOdysseyVectorHUDPathCubic::VIEW_PATH
                   | FOdysseyVectorHUDPathCubic::VIEW_POINT )
    , Radius(10.0f)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoEdit64");

    mPickingHUD.SetRadius( Radius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathEditTool::ActivateVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD(&mCubicPathHUD);
    iEngine->AddHUD(&mPickingHUD);

    iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FKey& iKey )
{
    if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl ) )
    {
        mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                    | FOdysseyVectorHUDPathCubic::VIEW_HANDLE_SEGMENT );
    }

    if ( ( iKey == EKeys::LeftShift ) || ( iKey == EKeys::RightShift ) )
    {
        mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                    | FOdysseyVectorHUDPathCubic::VIEW_HANDLE_POINT );
    }

    if ( ( iKey == EKeys::LeftAlt ) || ( iKey == EKeys::RightAlt ) )
    {
        mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                    | FOdysseyVectorHUDPathCubic::VIEW_POINT );
    }

    iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW );

    return false;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyUpVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FKey& iKey )
{
    mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                | FOdysseyVectorHUDPathCubic::VIEW_POINT );

    iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW );

    return false;
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownDeletePoint( FOdysseyVectorEngine* iEngine
                                                               , FOdysseyVectorScene* iScene
                                                               , FOdysseyVectorPath* iPath
                                                               , const FOdysseyPoint& iPointInTexture
                                                               , const FKey& iKey )
{
    BLPoint localCoords = iPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray;
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    uint64 selectionFlags = 0;

    mPickedPointArray.clear();

    if( mCubicPathHUD.GetDisplayMode() & FOdysseyVectorHUDPathCubic::VIEW_POINT )
    {
        selectionFlags = FOdysseyVectorPath::PICK_POINT;
    }

    iPath->PickPoint( iPointInTexture.x, iPointInTexture.y, Radius, mPickedPointArray, selectionFlags );

    // deletion is made for only 1 vertex. It would be to complicated to keep consistency when undoing/redoing
    // if we delete several vertices at once because of the stitched segments.
    if( mPickedPointArray.size() > 0 )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(mPickedPointArray[0]);
        uint32 segmentCount = vertex->GetSegmentCount();

        // remove segment
        if( segmentCount == 1 )
        {
            FOdysseyVectorSegment* segment = vertex->GetSegmentList().front();
            FOdysseyVectorVertex* otherVertex = segment->GetOtherVertex( vertex );

            iPath->RemoveSegment( segment );
            // for undoing
            removedSegmentArray.push_back( segment );

            if( otherVertex->GetSegmentCount() == 0 )
            {
                iPath->RemoveVertex( otherVertex );
                // for undoing
                removedVertexArray.push_back( otherVertex );
            }
        }

        // stitch
        if( segmentCount == 2 )
        {
            FOdysseyVectorSegmentCubic* segment0 = static_cast<FOdysseyVectorSegmentCubic*>(vertex->GetSegmentList().front());
            FOdysseyVectorSegmentCubic* segment1 = static_cast<FOdysseyVectorSegmentCubic*>(vertex->GetSegmentList().back());
            FOdysseyVectorVertex* stitchedVertex0 = segment0->GetOtherVertex( vertex );
            FOdysseyVectorVertex* stitchedVertex1 = segment1->GetOtherVertex( vertex );
            ::ULIS::FVec2D handle0 = stitchedVertex0->GetCoords() + ( vertex->GetCoords() - stitchedVertex0->GetCoords() ) * 0.9f;
            ::ULIS::FVec2D handle1 = stitchedVertex1->GetCoords() + ( vertex->GetCoords() - stitchedVertex1->GetCoords() ) * 0.9f;
            FOdysseyVectorSegmentCubic* stitchedSegment = new FOdysseyVectorSegmentCubic( iPath
                                                                                        , stitchedVertex0
                                                                                        , handle0.x
                                                                                        , handle0.y
                                                                                        , handle1.x
                                                                                        , handle1.y
                                                                                        , stitchedVertex1 );

            iPath->RemoveSegment( segment0 );
            iPath->RemoveSegment( segment1 );
            iPath->AddSegment( stitchedSegment );
            // for undoing
            removedSegmentArray.push_back( segment0 );
            removedSegmentArray.push_back( segment1 );
            addedSegmentArray.push_back( stitchedSegment );
        }

        iPath->RemoveVertex( vertex );
        // for undoing
        removedVertexArray.push_back( vertex );
    }
    //TODO: should be called in RemoveVertex() / AddVertex()
    iPath->Invalidate();

    mPickedPointArray.clear();

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathEditTool","Vector Path Edit Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                  , removedVertexArray
                                                                  , removedSegmentArray
                                                                  , addedVertexArray
                                                                  , addedSegmentArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownMovePoint( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , FOdysseyVectorPath* iPath
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    BLPoint localCoords = iPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    uint64 selectionFlags = 0;

    mPickedPointArray.clear();

    mOldLocalMouseX = localCoords.x;
    mOldLocalMouseY = localCoords.y;

    iPath->Unselect( nullptr );

    if( mCubicPathHUD.GetDisplayMode() & FOdysseyVectorHUDPathCubic::VIEW_HANDLE_SEGMENT )
    {
        selectionFlags = FOdysseyVectorPath::PICK_HANDLE_SEGMENT;
    }

    if( mCubicPathHUD.GetDisplayMode() & FOdysseyVectorHUDPathCubic::VIEW_HANDLE_POINT )
    {
        selectionFlags = FOdysseyVectorPath::PICK_HANDLE_POINT;
    }

    if( mCubicPathHUD.GetDisplayMode() & FOdysseyVectorHUDPathCubic::VIEW_POINT )
    {
        selectionFlags = FOdysseyVectorPath::PICK_POINT;
    }

    iPath->PickPoint( iPointInTexture.x, iPointInTexture.y, Radius, mPickedPointArray, selectionFlags );

    // TODO: put this in a function or something
    // Control point must move with the point. Store it in the mPickedPointArray as well
    for( int i = 0; i < mPickedPointArray.size(); i++ )
    {
        if( mPickedPointArray[i]->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>( mPickedPointArray[i] );
            std::list<FOdysseyVectorSegment*> segmentList = cubicVertex->GetSegmentList();

            for( std::list<FOdysseyVectorSegment*>::iterator segit = segmentList.begin(); segit != segmentList.end(); ++segit )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*segit);
                FOdysseyVectorHandleSegment* handle = ( cubicVertex == cubicSegment->GetVertex( 0 ) ) ? cubicSegment->GetHandle( 0 ) :
                                                                                                        cubicSegment->GetHandle( 1 );

                mPickedPointArray.push_back( handle );
            }
        }
    }
    // End-of TODO
    //TODO: should be called in RemoveVertex() / AddVertex()
    iPath->Invalidate();

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathEditTool","Vector Path Edit Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPointPosition( iScene, mPickedPointArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

    if ( selectedObject )
    {
        bool picked = false;

        if ( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);

            if( FSlateApplication::Get().GetModifierKeys().IsAltDown() )
            {
                OnMouseDownDeletePoint( iEngine, iScene, path, iPointInTexture, iKey );
            }
            else
            {
                OnMouseDownMovePoint( iEngine, iScene, path, iPointInTexture, iKey );
            }
        }
    }

    iScene->Update( 0 ); // updated invalidated objects
    iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW | FOdysseyVectorScene::OBJECT_MODIFIED );

    return true;
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
/*
    ::ULIS::FRectI formerRegion = ::ULIS::FRectI( iPointInTexture.x - iPointInTexture.deltaPosition.X - Radius
                                                , iPointInTexture.y - iPointInTexture.deltaPosition.Y - Radius
                                                , Radius * 2
                                                , Radius * 2 );
    ::ULIS::FRectI currentRegion = ::ULIS::FRectI( iPointInTexture.x - Radius
                                                 , iPointInTexture.y - Radius
                                                 , Radius * 2
                                                 , Radius * 2 );
    ::ULIS::FRectI finalRegion = currentRegion | formerRegion;
*/
    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW );
}

static ::ULIS::FRectD
DragPoint( double iLocalX
         , double iLocalY
         , double iOldLocalX
         , double iOldLocalY
         , FOdysseyVectorPoint *iPoint )
{
    double difx = iLocalX - iOldLocalX;
    double dify = iLocalY - iOldLocalY;

    if ( iPoint->GetClass() == FOdysseyVectorHandlePoint::StaticClass() )
    {
        FOdysseyVectorHandlePoint* pointHandle = static_cast<FOdysseyVectorHandlePoint*>( iPoint );
        FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>(pointHandle->GetParent());
        ::ULIS::FVec2D dif = { cubicVertex->GetX() - iLocalX, cubicVertex->GetY() - iLocalY };

        cubicVertex->SetRadius( dif.Distance() );

        return cubicVertex->GetBoundingBox( false );
    }

    if( iPoint->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
    {
        FOdysseyVectorHandleSegment* segmentHandle = static_cast<FOdysseyVectorHandleSegment*>( iPoint );
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segmentHandle->GetParent());

        iPoint->Set( iPoint->GetX() + difx
                   , iPoint->GetY() + dify );

        return cubicSegment->GetBoundingBox( false );
    }

    if( iPoint->GetClass() == FOdysseyVectorVertex::StaticClass() )
    {
        FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>( iPoint );
        std::list<FOdysseyVectorSegment*> segmentList = cubicVertex->GetSegmentList();

        cubicVertex->Set( iPoint->GetX() + difx
                        , iPoint->GetY() + dify );

        return cubicVertex->GetBoundingBox( false );
    }

    return { 0, 0, 0, 0 };
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();
    static ::ULIS::FRectI oldInvalidatedArea = { 0, 0, 0, 0 };

    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    if ( selectedObject )
    {
        if( selectedObject->GetClass() == FOdysseyVectorPathCubic::StaticClass() )
        {
            FOdysseyVectorPathCubic *cubicPath = static_cast<FOdysseyVectorPathCubic*>( selectedObject );
            BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            ::ULIS::FRectD localInvalidatedArea = { 0, 0, 0, 0 };
            ::ULIS::FRectI invalidatedArea;
            ::ULIS::FRectI totalInvalidatedArea;
            BLPoint worldAreaP1;
            BLPoint worldAreaP2;
            bool inited = false;

            for( int i = 0; i < mPickedPointArray.size(); i++ )
            {
                FOdysseyVectorPoint *selectedPoint = mPickedPointArray[i];
                ::ULIS::FRectD rect;

                rect = DragPoint( localCoords.x, localCoords.y, mOldLocalMouseX, mOldLocalMouseY, selectedPoint );

                localInvalidatedArea = ( inited == false ) ? rect : localInvalidatedArea | rect;

                inited = true;
            }

            worldAreaP1 = cubicPath->GetWorldMatrix().mapPoint( localInvalidatedArea.x, localInvalidatedArea.y );
            worldAreaP2 = cubicPath->GetWorldMatrix().mapPoint( localInvalidatedArea.x + localInvalidatedArea.w
                                                              , localInvalidatedArea.y + localInvalidatedArea.h );

            invalidatedArea = ::ULIS::FRectI::FromMinMax( ::ULIS::FMath::Min(worldAreaP1.x,worldAreaP2.x)
                                                        , ::ULIS::FMath::Min(worldAreaP1.y,worldAreaP2.y)
                                                        , ::ULIS::FMath::Max(worldAreaP1.x,worldAreaP2.x)
                                                        , ::ULIS::FMath::Max(worldAreaP1.y,worldAreaP2.y) );

            totalInvalidatedArea = invalidatedArea | oldInvalidatedArea;

//UE_LOG(LogTemp, Warning, TEXT("%d %d %d %d"), invalidatedArea.x, invalidatedArea.y, invalidatedArea.w, invalidatedArea.h );
            // update invalidated objects
            iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;

            oldInvalidatedArea = invalidatedArea;
        }
    }

    iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    iScene->Update( 0 );
    iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW | FOdysseyVectorScene::OBJECT_MODIFIED );

    return true;
}

void
UOdysseyPainterEditorVectorPathEditTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPathEditTool::PropertyChanged( const FName& iPropertyName )
{
    mPickingHUD.SetRadius( Radius );
}

#undef LOCTEXT_NAMESPACE
