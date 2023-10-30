
// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditToolHUD.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "Undo/OdysseyVectorUndoVertexRadius.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathEditTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathEditTool::~UOdysseyPainterEditorVectorPathEditTool()
{
}

UOdysseyPainterEditorVectorPathEditTool::UOdysseyPainterEditorVectorPathEditTool()
    : mPickingFlags ( FOdysseyVectorPath::PICK_POINT )
    , mPickingMode  ( ePathPickingMode::Vertex )
    , PickingRadius(10.0f)
    , WidenAllAlong( true )
    ,RestrictToSelection( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoEdit64");

    mPathEditHUD = new FOdysseyPainterEditorVectorPathEditToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPathEditTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorPathEditTool::UnloadVector( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    iEngine->RemoveHUD( mPathEditHUD );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::LoadVector( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    TSharedPtr< SViewport > viewportWidget; // to force keyboard focus on mouse hover.
                                            // Prevents the user from having to click at least once in the viewport.
    // we need the focus on the viewport for keyboard 
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
    viewportWidget = viewportTab->GetViewport()->GetViewportWidget();

    // we need the focus on the viewport for keyboard 
    FSlateApplication::Get().SetKeyboardFocus( viewportWidget );

    iEngine->ClearHUD();
    iEngine->AddHUD( mPathEditHUD );

    iEngine->ResetHUD();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnKeyDownVector( FOdysseyVectorScene* iScene
                                                        , const FKey& iKey )
{
    // then detect which keys are pressed and set display mode
    if ( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        mPickingMode   = ePathPickingMode::SegmentHandle;
        mPickingFlags  = FOdysseyVectorPath::PICK_HANDLE_SEGMENT
                       | FOdysseyVectorPath::PICK_POINT;
    }

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        mPickingMode  = ePathPickingMode::VertexHandle;
        mPickingFlags = FOdysseyVectorPath::PICK_HANDLE_POINT;
    }

    if ( FSlateApplication::Get().GetModifierKeys().IsAltDown() )
    {
        mPickingMode  = ePathPickingMode::Vertex;
        mPickingFlags = FOdysseyVectorPath::PICK_POINT;
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( iScene, iKey )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnKeyUpVector( FOdysseyVectorScene* iScene
                                                      , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // first reset display mode
    mPickingMode = ePathPickingMode::Vertex;
    mPickingFlags = FOdysseyVectorPath::PICK_POINT;

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( iScene, iKey )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

void
UOdysseyPainterEditorVectorPathEditTool::GroupPaintDeletePoint( FOdysseyVectorGroupPaint* iGroupPaint
                                                              , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                                              , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                                              , std::vector<FOdysseyVectorPath*>& iRemovedPathArray
                                                              , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                                              , double iSelectionRadius
                                                              , const FOdysseyPoint& iPointInTexture )
{
    std::list<FOdysseyVectorObject*>& childrenList = iGroupPaint->GetChildrenList();

    mPickedPointArray.clear();

    for( FOdysseyVectorObject* child : childrenList )
    {
        if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);

            mPickedPointArray.reserve( 10 );

            path->PickPoint( iPointInTexture.x
                           , iPointInTexture.y
                           , iSelectionRadius
                           , mPickedPointArray
                           , FOdysseyVectorPath::PICK_POINT );

            FOdysseyVectorPath::DeletePoint( path
                                           , mPickedPointArray
                                           , iRemovedVertexArray
                                           , iRemovedSegmentArray
                                           , iRemovedPathArray
                                           , iAddedSegmentArray );
        }
    }
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownDeletePoint( FOdysseyVectorScene* iScene
                                                               , const FOdysseyPoint& iPointInTexture
                                                               , const FKey& iKey )
{
    std::vector<FOdysseyVectorPath*> removedPathArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorPath*> addedPathArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray; // not filled, here just for the undo record
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;

    removedPathArray.reserve( 10 );
    removedVertexArray.reserve( 10 );
    removedSegmentArray.reserve( 10 );
    addedSegmentArray.reserve( 10 );

    mPickedPointArray.clear();
    mPickedPointArray.reserve( 10 );

    mPathEditHUD->Traverse( iScene
                          , iScene
                          , GetEditor()->GetVectorEditionFlags()
                          , [ this
                            , &iPointInTexture
                            , &removedPathArray
                            , &removedVertexArray
                            , &removedSegmentArray
                            , &addedSegmentArray ]( FOdysseyVectorObject* object ) -> bool
                            {
                                if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                                {
                                    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                                    path->PickPoint( iPointInTexture.x
                                                   , iPointInTexture.y
                                                   , PickingRadius
                                                   , mPickedPointArray
                                                   , FOdysseyVectorPath::PICK_POINT );

                                    FOdysseyVectorPath::DeletePoint( path
                                                                   , mPickedPointArray
                                                                   , removedVertexArray
                                                                   , removedSegmentArray
                                                                   , removedPathArray
                                                                   , addedSegmentArray );
                                }

                                return false; // keep traversing
                            } );

    if( mPickedPointArray.size() )
    {
        for( int i = 0; i < removedPathArray.size(); i++ )
        {
            FOdysseyVectorPath* path = removedPathArray[i];

            path->GetParent()->RemoveChild( path );
        }

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // updated invalidated objects

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("VectorPathEditTool","Vector Path Edit Tool"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                      , removedPathArray
                                                                      , removedVertexArray
                                                                      , removedSegmentArray
                                                                      , addedPathArray
                                                                      , addedVertexArray
                                                                      , addedSegmentArray );

            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();
    }
}

static void
PathPickPoint( FOdysseyVectorPath* iPath
             , std::vector<FOdysseyVectorPoint*>& iPickedPointArray
             , double iSelectionRadius
             , uint64 iSelectionFlags
             , const FOdysseyPoint& iPointInTexture )
{
    BLPoint localCoords = iPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    uint32 pointCount = iPickedPointArray.size();

    iPath->PickPoint( iPointInTexture.x, iPointInTexture.y, iSelectionRadius, iPickedPointArray, iSelectionFlags );

    // Control point must move with the point. Store it in the mPickedPointArray as well
    if( iSelectionFlags == FOdysseyVectorPath::PICK_POINT )
    {
        for( int i = pointCount; i < iPickedPointArray.size(); i++ )
        {
            if( iPickedPointArray[i]->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>( iPickedPointArray[i] );
                std::list<FOdysseyVectorSegment*> segmentList = cubicVertex->GetSegmentList();

                for( std::list<FOdysseyVectorSegment*>::iterator segit = segmentList.begin(); segit != segmentList.end(); ++segit )
                {
                    FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*segit);

                    if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
                    {
                        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
                        FOdysseyVectorHandleSegment* handle = ( cubicVertex == cubicSegment->GetVertex( 0 ) ) ? cubicSegment->GetHandle( 0 ) :
                                                                                                                cubicSegment->GetHandle( 1 );

                        iPickedPointArray.push_back( handle );
                    }
                }
            }
        }
    }

    //TODO: should be called in RemoveVertex() / AddVertex()
    iPath->Invalidate();
}

void
UOdysseyPainterEditorVectorPathEditTool::GetPathsFromSelection( FOdysseyVectorScene* iScene
                                                              , std::vector<FOdysseyVectorPath*>& oPathArray )
{
    mPathEditHUD->Traverse( iScene
                          , iScene
                          , GetEditor()->GetVectorEditionFlags()
                          , [ &oPathArray ]( FOdysseyVectorObject* object ) -> bool
                            {
                                if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                                {
                                    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);
 
                                    oPathArray.push_back( path );
                                }

                                return false; // keep traversing
                            } );
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownPickPoint( FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    mSelectedPathArray.clear();
    mPickedPointArray.clear();

    mPathEditHUD->Traverse( iScene
                          , iScene
                          , GetEditor()->GetVectorEditionFlags()
                          , [ this
                            , &iPointInTexture ]( FOdysseyVectorObject* object ) -> bool
                            {
                                if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                                {
                                    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);
 
                                    PathPickPoint( path
                                                 , mPickedPointArray
                                                 , PickingRadius
                                                 , mPickingFlags
                                                 , iPointInTexture );
                                }

                                return false; // keep traversing
                            } );

    if( mPickedPointArray.size() )
    {
        // Link or Unlink segment handles
        if( ( mPickingMode == ePathPickingMode::SegmentHandle )
        &&  ( mPickedPointArray.size() == 1 )
        &&  ( mPickedPointArray[0]->GetClass() == FOdysseyVectorVertex::StaticClass() ) )
        {
            FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>( mPickedPointArray[0] );

            vertex->SetHandleAligned( vertex->IsHandleAligned() ? false : true );

            if( vertex->GetSegmentCount() )
            {
                vertex->AlignHandles( vertex->GetFirstSegment()->GetHandle( vertex ) );
            }
        }
        // Else, save point coordinates before changing them
        else
        {
            switch( mPickingMode )
            {
                case ePathPickingMode::VertexHandle :
                    // needed for valid GUndo pointer
                    GEditor->BeginTransaction(LOCTEXT("VectorPathEditTool","Vector Path Edit Tool"));
                    if( GUndo )
                    {
                        FOdysseyVectorUndo* undo;

                        if( WidenAllAlong && ( mPickedPointArray.size() == 1 ) )
                        {
                            GetPathsFromSelection( iScene, mSelectedPathArray );

                            undo = new FOdysseyVectorUndoVertexRadius( iScene, mSelectedPathArray );
                        }
                        else
                        {
                            undo = new FOdysseyVectorUndoVertexRadius( iScene, mPickedPointArray );
                        }

                        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
                    }
                    GEditor->EndTransaction();
                break;

                default :
                    // needed for valid GUndo pointer
                    GEditor->BeginTransaction(LOCTEXT("VectorPathEditTool","Vector Path Edit Tool"));
                    if( GUndo )
                    {
                        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPointPosition( iScene, mPickedPointArray );

                        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
                    }
                    GEditor->EndTransaction();
                break;
            }
        }
    }
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownVector( FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mOldPointInTexture = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

        if( FSlateApplication::Get().GetModifierKeys().IsAltDown() )
        {
            if( mPickingFlags & FOdysseyVectorPath::PICK_POINT )
            {
                OnMouseDownDeletePoint( iScene, iPointInTexture, iKey );
            }
        }
        else
        {
            OnMouseDownPickPoint( iScene, iPointInTexture, iKey );
        }
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW 
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseHoverVector( FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    // we need the focus on the viewport for keyboard 
    //FSlateApplication::Get().SetKeyboardFocus( mViewportWidget.ToSharedRef() );

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

    mPathEditHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

static FOdysseyVectorObject*
GetPointParentObject( FOdysseyVectorPoint *iPoint )
{
    FOdysseyVectorObject* object = nullptr;

    if( iPoint->GetClass() == FOdysseyVectorVertex::StaticClass() )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(iPoint);

        object = vertex->GetPath();
    }

    if( iPoint->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
    {
        FOdysseyVectorHandleSegment* handle = static_cast<FOdysseyVectorHandleSegment*>(iPoint);

        object = handle->GetOwner()->GetPath();
    }

    return object;
}

::ULIS::FRectD
UOdysseyPainterEditorVectorPathEditTool::DragPoint( FOdysseyVectorPoint *iPoint
                                                  , double iWorldX
                                                  , double iWorldY
                                                  , double iDeltaX
                                                  , double iDeltaY
                                                  , bool iWidenAllAlong )
{
    FOdysseyVectorObject* object = GetPointParentObject( iPoint );
    BLPoint localCoords = object->GetInverseWorldMatrix().mapPoint( iWorldX, iWorldY );
    BLPoint localVector = object->GetInverseWorldMatrix().mapVector( iDeltaX, iDeltaY );

    if( mPickingMode == ePathPickingMode::VertexHandle  )
    {
        if( iPoint->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>( iPoint );
            ::ULIS::FVec2D dif = { cubicVertex->GetX() - localCoords.x
                                 , cubicVertex->GetY() - localCoords.y };
            double deltaRadius = dif.Distance() - cubicVertex->GetRadius();

            if( iWidenAllAlong )
            {
                for( int i = 0; i < mSelectedPathArray.size(); i++ )
                {
                    mSelectedPathArray[i]->AlterRadius( deltaRadius );
                }
            }
            else
            {
                cubicVertex->SetRadius( cubicVertex->GetRadius() + deltaRadius );
            }

            return cubicVertex->GetBoundingBox( false );
        }
    }

    if( mPickingMode == ePathPickingMode::Vertex )
    {
        if( iPoint->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
        {
            FOdysseyVectorHandleSegment* segmentHandle = static_cast<FOdysseyVectorHandleSegment*>( iPoint );
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segmentHandle->GetOwner());

            segmentHandle->Set( iPoint->GetX() + localVector.x
                              , iPoint->GetY() + localVector.y );

            return cubicSegment->GetBoundingBox( false );
        }

        if( iPoint->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>( iPoint );

            cubicVertex->Set( iPoint->GetX() + localVector.x
                            , iPoint->GetY() + localVector.y );

            return cubicVertex->GetBoundingBox( false );
        }
    }

    if( mPickingMode == ePathPickingMode::SegmentHandle )
    {
        if( iPoint->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
        {
            FOdysseyVectorHandleSegment* segmentHandle = static_cast<FOdysseyVectorHandleSegment*>( iPoint );
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segmentHandle->GetOwner());
            FOdysseyVectorVertex* vertex = cubicSegment->GetVertex( segmentHandle->GetHandleID() );

            segmentHandle->Set( iPoint->GetX() + localVector.x
                              , iPoint->GetY() + localVector.y );

            if( vertex->IsHandleAligned() )
            {
                vertex->AlignHandles( segmentHandle );
            }

            return cubicSegment->GetBoundingBox( false );
        }
    }

    return { 0, 0, 0, 0 };
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseDragVector( FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
/*
    static ::ULIS::FRectI oldInvalidatedArea = { 0, 0, 0, 0 };
    ::ULIS::FRectD localInvalidatedArea = { 0, 0, 0, 0 };
    ::ULIS::FRectI invalidatedArea;
    ::ULIS::FRectI totalInvalidatedArea;
    BLPoint worldAreaP1;
    BLPoint worldAreaP2;

    bool inited = false;
*/

    // Left mouse button clicked
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        mPathEditHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

        for( int i = 0; i < mPickedPointArray.size(); i++ )
        {
            FOdysseyVectorPoint *selectedPoint = mPickedPointArray[i];
            ::ULIS::FRectD rect;

            rect = DragPoint( selectedPoint
                            , iPointInTexture.x
                            , iPointInTexture.y
                            // we don't use iPointInTexture.deltaPosition because for some reason,
                            // the readings are not good when a key is pressed.
                            , iPointInTexture.x - mOldPointInTexture.x
                            , iPointInTexture.y - mOldPointInTexture.y
                            , WidenAllAlong && ( mPickedPointArray.size() == 1 ) );

            //localInvalidatedArea = ( inited == false ) ? rect : localInvalidatedArea | rect;

            //inited = true;
        }

    /*
        worldAreaP1 = cubicPath->GetWorldMatrix().mapPoint( localInvalidatedArea.x, localInvalidatedArea.y );
        worldAreaP2 = cubicPath->GetWorldMatrix().mapPoint( localInvalidatedArea.x + localInvalidatedArea.w
                                                          , localInvalidatedArea.y + localInvalidatedArea.h );

        invalidatedArea = ::ULIS::FRectI::FromMinMax( ::ULIS::FMath::Min(worldAreaP1.x,worldAreaP2.x)
                                                    , ::ULIS::FMath::Min(worldAreaP1.y,worldAreaP2.y)
                                                    , ::ULIS::FMath::Max(worldAreaP1.x,worldAreaP2.x)
                                                    , ::ULIS::FMath::Max(worldAreaP1.y,worldAreaP2.y) );

        totalInvalidatedArea = invalidatedArea | oldInvalidatedArea;

        oldInvalidatedArea = invalidatedArea;
    */
        mOldPointInTexture = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

        iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED );
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpVector( FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mPickedPointArray.size() == 0 )
        {
            // use the pick tool if the Down and Up events were at the same position (no dragging )
            GetEditor()->GetVectorSelectionTool()->OnMouseDown( iPointInTexture, iKey );
            GetEditor()->GetVectorSelectionTool()->OnMouseUp( iPointInTexture, iKey );
        }

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED;
}

ePathPickingMode
UOdysseyPainterEditorVectorPathEditTool::GetPickingMode()
{
    return mPickingMode;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::GetPickingFlags()
{
    return mPickingFlags;
}

#undef LOCTEXT_NAMESPACE
