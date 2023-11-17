
// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditToolHUD.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "Undo/OdysseyVectorUndoVertexRadius.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"
#include "Undo/OdysseyVectorUndoPathEdit.h"
#include "Undo/OdysseyVectorUndoVertexAlignment.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathEditTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathEditTool::~UOdysseyPainterEditorVectorPathEditTool()
{
}

UOdysseyPainterEditorVectorPathEditTool::UOdysseyPainterEditorVectorPathEditTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorPathEditToolHUD( this ), false )
    , mPickingFlags ( FOdysseyVectorPath::PICK_VERTEX )
    , mPickingMode  ( ePathPickingMode::Vertex )
    , PickingRadius(10.0f)
    , WidenAllAlong( true )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoEdit64");

    mPathEditHUD = static_cast<FOdysseyPainterEditorVectorPathEditToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPathEditTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorPathEditTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    TSharedPtr< SViewport > viewportWidget; // to force keyboard focus on mouse hover.
                                            // Prevents the user from having to click at least once in the viewport.
    // we need the focus on the viewport for keyboard 
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
    viewportWidget = viewportTab->GetViewport()->GetViewportWidget();

    // we need the focus on the viewport for keyboard 
    FSlateApplication::Get().SetKeyboardFocus( viewportWidget );

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FKey& iKey )
{
    // then detect which keys are pressed and set display mode
    if ( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        mPickingMode   = ePathPickingMode::SegmentHandle;
        mPickingFlags  = FOdysseyVectorPath::PICK_HANDLE_SEGMENT
                       | FOdysseyVectorPath::PICK_VERTEX ;
    }

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        mPickingMode  = ePathPickingMode::VertexHandle;
        mPickingFlags = FOdysseyVectorPath::PICK_HANDLE_VERTEX;
    }

    if ( FSlateApplication::Get().GetModifierKeys().IsAltDown() )
    {
        mPickingMode  = ePathPickingMode::Vertex;
        mPickingFlags = FOdysseyVectorPath::PICK_VERTEX;
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( iScene, iKey )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                      , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // first reset display mode
    mPickingMode = ePathPickingMode::Vertex;
    mPickingFlags = FOdysseyVectorPath::PICK_VERTEX;

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( iScene, iKey )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownDeletePoint( FOdysseyVectorGroupPaint* iScene
                                                               , const FOdysseyPoint& iPointInTexture
                                                               , const FKey& iKey )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
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

    mPickedHandleArray.clear();
    mPickedVertexArray.clear();
    mPickedVertexArray.reserve( 10 );

    vectorEngine->Traverse
    ( iScene
    , iScene
    , 0
    , [ this
      , iScene
      , vectorEngine
      , &iPointInTexture
      , &removedPathArray
      , &removedVertexArray
      , &removedSegmentArray
      , &addedSegmentArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                  path->PickPoint( iPointInTexture.x
                                 , iPointInTexture.y
                                 , PickingRadius
                                 , mPickedVertexArray
                                 , mPickedHandleArray
                                 , FOdysseyVectorPath::PICK_VERTEX );

                  FOdysseyVectorPath::DeleteVertex( path
                                                  , mPickedVertexArray
                                                  , removedVertexArray
                                                  , removedSegmentArray
                                                  , removedPathArray
                                                  , addedSegmentArray );
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );

    if( mPickedVertexArray.size() )
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

void
UOdysseyPainterEditorVectorPathEditTool::GetPathsFromSelection( FOdysseyVectorGroupPaint* iScene
                                                              , std::vector<FOdysseyVectorPath*>& oPathArray )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    vectorEngine->Traverse
    ( iScene
    , iScene
    , 0
    , [ iScene
      , vectorEngine
      , &oPathArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);
 
                  oPathArray.push_back( path );
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );
}

// static
void
UOdysseyPainterEditorVectorPathEditTool::BuildSegmentAdjustments( const std::vector<FOdysseyVectorSegment*>& iSegmentArray
                                                                , std::vector<FSegmentAdjustment>& oSegmentAdjustmentArray )
{
    oSegmentAdjustmentArray.reserve( iSegmentArray.size() );

    for( FOdysseyVectorSegment* segment : iSegmentArray )
    {
        oSegmentAdjustmentArray.emplace_back( segment );
    }
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownPickPoint( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    mSelectedPathArray.clear();
    mPickedVertexArray.clear();
    mPickedHandleArray.clear();
    mSegmentAdjustmentArray.clear();

    vectorEngine->Traverse
    ( iScene
    , iScene
    , 0
    , [ this
      , iScene
      , vectorEngine
      , &iPointInTexture ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                  path->PickPoint( iPointInTexture.x
                                 , iPointInTexture.y
                                 , PickingRadius
                                 , mPickedVertexArray
                                 , mPickedHandleArray
                                 , mPickingFlags );
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );

    // Link or Unlink segment handles
    if( ( mPickingMode == ePathPickingMode::SegmentHandle ) &&  ( mPickedVertexArray.size() == 1 ) )
    {
        FOdysseyVectorVertex* vertex = mPickedVertexArray[0];

        //-------------- undo ---------------//
        GEditor->BeginTransaction(LOCTEXT("AlignPointSelection","Align Point Selection"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexAlignment( iScene, mPickedVertexArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction(); 
        //---------- end of undo ------------//

        vertex->SetHandleAligned( vertex->IsHandleAligned() ? false : true );

        if( vertex->IsHandleAligned() )
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

                    if( WidenAllAlong && ( mPickedVertexArray.size() == 1 ) )
                    {
                        GetPathsFromSelection( iScene, mSelectedPathArray );

                        undo = new FOdysseyVectorUndoVertexRadius( iScene, mSelectedPathArray );
                    }
                    else
                    {
                        undo = new FOdysseyVectorUndoVertexRadius( iScene, mPickedVertexArray );
                    }

                    GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
                }
                GEditor->EndTransaction();
            break;

            case ePathPickingMode::Vertex :
            {
                std::vector<FOdysseyVectorSegment*> alteredSegmentArray;

                // static call
                FOdysseyVectorVertex::ArrayToSegmentArray( mPickedVertexArray, alteredSegmentArray );
                // static call
                BuildSegmentAdjustments( alteredSegmentArray, mSegmentAdjustmentArray );

                // Control points must move with the point. Store them in the iPickedHandleArray
                for( FOdysseyVectorVertex* vertex : mPickedVertexArray )
                {
                    for( FOdysseyVectorSegment* segment : vertex->GetSegmentList() )
                    {
                        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
                        {
                            FOdysseyVectorHandleSegment* handle = segment->GetHandle( vertex );

                            mPickedHandleArray.push_back( handle );
                        }
                    }
                }

                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("VectorPathEditTool","Vector Path Edit Tool"));
                if( GUndo )
                {
                    FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathEdit( iScene
                                                                             , mPickedVertexArray
                                                                             , alteredSegmentArray );

                    GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
                }
                GEditor->EndTransaction();
                //---------- end of undo
            }
            break;

            case ePathPickingMode::SegmentHandle :
            {
                std::vector<FOdysseyVectorSegment*> alteredSegmentArray;
                std::vector<FOdysseyVectorVertex*> connectedVertexArray;

                // static call
                FOdysseyVectorHandleSegment::ArrayToVertexArray( mPickedHandleArray, connectedVertexArray );
                // static call
                FOdysseyVectorVertex::ArrayToSegmentArray( connectedVertexArray, alteredSegmentArray );

                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("VectorPathEditTool","Vector Path Edit Tool"));
                if( GUndo )
                {
                    FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathEdit( iScene
                                                                             , mPickedVertexArray // empty
                                                                             , alteredSegmentArray );

                    GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
                }
                GEditor->EndTransaction();
                //---------- end of undo
            }
            break;

            default:
            break;
        }
    }
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mOldPointInTexture = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

        if( FSlateApplication::Get().GetModifierKeys().IsAltDown() )
        {
            if( mPickingFlags & FOdysseyVectorPath::PICK_VERTEX )
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
UOdysseyPainterEditorVectorPathEditTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
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

::ULIS::FRectD
UOdysseyPainterEditorVectorPathEditTool::DragSegmentHandle( FOdysseyVectorHandleSegment *iHandle
                                                          , double iWorldX
                                                          , double iWorldY
                                                          , double iDeltaX
                                                          , double iDeltaY
                                                          , bool iRealign )
{
    FOdysseyVectorSegment* segment = iHandle->GetOwner();
    FOdysseyVectorPath* path = segment->GetPath();
    FOdysseyVectorVertex* vertex = segment->GetVertex( iHandle->GetHandleID() );
    BLPoint localCoords = path->GetInverseWorldMatrix().mapPoint( iWorldX, iWorldY );
    BLPoint localVector = path->GetInverseWorldMatrix().mapVector( iDeltaX, iDeltaY );

    iHandle->Set( iHandle->GetX() + localVector.x
                , iHandle->GetY() + localVector.y );

    if( iRealign )
    {
        if( vertex->IsHandleAligned() )
        {
            vertex->AlignHandles( iHandle );
        }
    }

    return segment->GetBoundingBox( false );
}

::ULIS::FRectD
UOdysseyPainterEditorVectorPathEditTool::DragVertex( FOdysseyVectorVertex *iVertex
                                                   , double iWorldX
                                                   , double iWorldY
                                                   , double iDeltaX
                                                   , double iDeltaY
                                                   , bool iWidenAllAlong )
{
    FOdysseyVectorPath* path = iVertex->GetPath();
    BLPoint localCoords = path->GetInverseWorldMatrix().mapPoint( iWorldX, iWorldY );
    BLPoint localVector = path->GetInverseWorldMatrix().mapVector( iDeltaX, iDeltaY );

    if( mPickingMode == ePathPickingMode::VertexHandle  )
    {
        ::ULIS::FVec2D dif = { iVertex->GetX() - localCoords.x
                             , iVertex->GetY() - localCoords.y };
        double deltaRadius = dif.Distance() - iVertex->GetRadius();

        if( iWidenAllAlong )
        {
            for( int i = 0; i < mSelectedPathArray.size(); i++ )
            {
                mSelectedPathArray[i]->AlterRadius( deltaRadius );
            }
        }
        else
        {
            iVertex->SetRadius( iVertex->GetRadius() + deltaRadius );
        }

        return iVertex->GetBoundingBox( false );
    }

    if( mPickingMode == ePathPickingMode::Vertex )
    {
        iVertex->Set( iVertex->GetX() + localVector.x
                    , iVertex->GetY() + localVector.y );

        return iVertex->GetBoundingBox( false );
    }

    return { 0, 0, 0, 0 };
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
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

        for( int i = 0; i < mPickedVertexArray.size(); i++ )
        {
            FOdysseyVectorVertex *vertex = mPickedVertexArray[i];
            ::ULIS::FRectD rect;

            rect = DragVertex( vertex
                             , iPointInTexture.x
                             , iPointInTexture.y
                             // we don't use iPointInTexture.deltaPosition because for some reason,
                             // the readings are not good when a key is pressed.
                             , iPointInTexture.x - mOldPointInTexture.x
                             , iPointInTexture.y - mOldPointInTexture.y
                             , WidenAllAlong && ( mPickedVertexArray.size() == 1 ) );

            //localInvalidatedArea = ( inited == false ) ? rect : localInvalidatedArea | rect;

            //inited = true;
        }

        for( int i = 0; i < mPickedHandleArray.size(); i++ )
        {
            FOdysseyVectorHandleSegment *handle = mPickedHandleArray[i];
            ::ULIS::FRectD rect;

            rect = DragSegmentHandle( handle
                                    , iPointInTexture.x
                                    , iPointInTexture.y
                                    // we don't use iPointInTexture.deltaPosition because for some reason,
                                    // the readings are not good when a key is pressed.
                                    , iPointInTexture.x - mOldPointInTexture.x
                                    , iPointInTexture.y - mOldPointInTexture.y
                                    , ( mPickingMode == ePathPickingMode::SegmentHandle ) ? true : false );

            //localInvalidatedArea = ( inited == false ) ? rect : localInvalidatedArea | rect;

            //inited = true;
        }

        // adjust handle length to keep the same ratio as before the editing
        if( mPickingMode == ePathPickingMode::Vertex )
        {
            for( FSegmentAdjustment& segmentAdjustment : mSegmentAdjustmentArray )
            {
                segmentAdjustment.Adjust();
            }
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
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

        if( ( mPickedVertexArray.size() == 0 ) && ( mPickedHandleArray.size() == 0 ) )
        {
            // use the pick tool if the Down and Up events were at the same position (no dragging )
            GetEditor()->GetVectorSelectionTool()->OnMouseDown( iPointInTexture, iKey );
            GetEditor()->GetVectorSelectionTool()->OnMouseUp( iPointInTexture, iKey );
        }

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

        vectorEngine->ResetHUD();
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
