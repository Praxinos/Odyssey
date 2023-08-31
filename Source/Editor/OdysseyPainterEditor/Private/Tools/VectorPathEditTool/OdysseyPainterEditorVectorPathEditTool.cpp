
// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditToolHUD.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
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
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoEdit64");

    mPathEditHUD = new FOdysseyPainterEditorVectorPathEditToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathEditTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathEditTool::LoadVector( vectorEngine, vectorScene );
}

void
UOdysseyPainterEditorVectorPathEditTool::Unload()
{
    
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathEditTool::UnloadVector( vectorEngine, vectorScene );
}

bool
UOdysseyPainterEditorVectorPathEditTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorPathEditTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mPathEditHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathEditTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
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

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyDown( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

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

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyUp( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

    // first reset display mode
    mPickingMode = ePathPickingMode::Vertex;
    mPickingFlags = FOdysseyVectorPath::PICK_POINT;

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
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
    std::list<FOdysseyVectorObject*>::iterator it;

    mPickedPointArray.clear();

    for( it = childrenList.begin(); it != childrenList.end(); ++it )
    {
        FOdysseyVectorObject* child = *it;

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
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownDeletePoint( FOdysseyVectorEngine* iEngine
                                                               , FOdysseyVectorScene* iScene
                                                               , const FOdysseyPoint& iPointInTexture
                                                               , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
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

    for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject  = *it;

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);

            mPickedPointArray.reserve( 10 );

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

        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            GroupPaintDeletePoint( paintGroup
                                 , removedVertexArray
                                 , removedSegmentArray
                                 , removedPathArray
                                 , addedSegmentArray
                                 , PickingRadius
                                 , iPointInTexture );
        }
    }

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

static void
GroupPaintPickPoint( FOdysseyVectorGroupPaint* iPaintGroup
                   , std::vector<FOdysseyVectorPoint*>& iPickedPointArray
                   , double iSelectionRadius
                   , uint64 iSelectionFlags
                   , const FOdysseyPoint& iPointInTexture )
{
    std::list<FOdysseyVectorObject*>& childrenList = iPaintGroup->GetChildrenList();
    std::list<FOdysseyVectorObject*>::iterator it;

    for( it = childrenList.begin(); it != childrenList.end(); ++it )
    {
        FOdysseyVectorObject* childObject = *it;

        if( childObject->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(childObject);

            PathPickPoint( path, iPickedPointArray, iSelectionRadius, iSelectionFlags, iPointInTexture );
        }
    }
}


void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownPickPoint( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    mPickedPointArray.clear();

    for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject  = *it;

        if( selectedObject->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);

            PathPickPoint( path, mPickedPointArray, PickingRadius, mPickingFlags, iPointInTexture );
        }

        if( selectedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            GroupPaintPickPoint( paintGroup, mPickedPointArray, PickingRadius, mPickingFlags, iPointInTexture );
        }
    }

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
            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("VectorPathEditTool","Vector Path Edit Tool"));
            if( GUndo )
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPointPosition( iScene, mPickedPointArray );

                GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
            }
            GEditor->EndTransaction();
        }
    }
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    mOldPointInTexture = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

    if( FSlateApplication::Get().GetModifierKeys().IsAltDown() )
    {
        if( mPickingFlags & FOdysseyVectorPath::PICK_POINT )
        {
            OnMouseDownDeletePoint( iEngine, iScene, iPointInTexture, iKey );
        }
    }
    else
    {
        OnMouseDownPickPoint( iEngine, iScene, iPointInTexture, iKey );
    }

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW 
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );

    return true;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    bool ret = UOdysseyPainterEditorVectorPathEditTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
    GEditor->EndTransaction();
    return ret;
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
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

    // we also detect picking mode in hover events because we are not sure that the tool has keyboard focus.
    // It should not use too much CPU time.
    //DetectPickingMode();

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathEditTool::OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );
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

void
UOdysseyPainterEditorVectorPathEditTool::AlterVertexRadius( FOdysseyVectorVertex* vertex
                                                          , FOdysseyVectorSegment* iFromSegment
                                                          , double iDeltaRadius )
{
    vertex->SetRadius( vertex->GetRadius() + iDeltaRadius );

    if( WidenAllAlong )
    {
        for( FOdysseyVectorSegment* segment : vertex->GetSegmentList() )
        {
            if( segment != iFromSegment )
            {
                FOdysseyVectorVertex* otherVertex = segment->GetOtherVertex( vertex );

                AlterVertexRadius( otherVertex, segment, iDeltaRadius );
            }
        }
    }
}

::ULIS::FRectD
UOdysseyPainterEditorVectorPathEditTool::DragPoint( FOdysseyVectorPoint *iPoint
                                                  , double iWorldX
                                                  , double iWorldY
                                                  , double iDeltaX
                                                  , double iDeltaY )
{
    FOdysseyVectorObject* object = GetPointParentObject( iPoint );
    BLPoint localCoords = object->GetInverseWorldMatrix().mapPoint( iWorldX, iWorldY );
    BLPoint localVector = object->GetInverseWorldMatrix().mapVector( iDeltaX, iDeltaY );

    if( mPickingMode == ePathPickingMode::VertexHandle  )
    {
        FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>( iPoint );
        ::ULIS::FVec2D dif = { cubicVertex->GetX() - localCoords.x
                             , cubicVertex->GetY() - localCoords.y };

        AlterVertexRadius( cubicVertex, nullptr, dif.Distance() - cubicVertex->GetRadius() );

        return cubicVertex->GetBoundingBox( false );
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

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();
/*
    static ::ULIS::FRectI oldInvalidatedArea = { 0, 0, 0, 0 };
    ::ULIS::FRectD localInvalidatedArea = { 0, 0, 0, 0 };
    ::ULIS::FRectI invalidatedArea;
    ::ULIS::FRectI totalInvalidatedArea;
    BLPoint worldAreaP1;
    BLPoint worldAreaP2;

    bool inited = false;
*/

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
                        , iPointInTexture.y - mOldPointInTexture.y );

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

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathEditTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    if( mPickedPointArray.size() == 0 )
    {
        // use the pick tool if the Down and Up events were at the same position (no dragging )
        GetEditor()->GetVectorPickTool()->OnMouseDown( iPointInTexture, iKey );
        GetEditor()->GetVectorPickTool()->OnMouseUp( iPointInTexture, iKey );
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return true;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorPathEditTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
}

void
UOdysseyPainterEditorVectorPathEditTool::Commit()
{

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

void
UOdysseyPainterEditorVectorPathEditTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;
    
    PropertyChanged( PropertyChangedEvent.GetPropertyName() );

    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathEditTool::PropertyChanged( const FName& iPropertyName )
{
}

#undef LOCTEXT_NAMESPACE
