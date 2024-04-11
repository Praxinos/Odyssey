
// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "Undo/OdysseyVectorUndoVertexRadius.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"
#include "Undo/OdysseyVectorUndoPathEdit.h"
#include "Undo/OdysseyVectorUndoVertexAlignment.h"
#include "ISinglePropertyView.h"
#include "Widgets/Layout/SWrapBox.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

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
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathEdit64");

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
/*
    TSharedPtr< SViewport > viewportWidget; // to force keyboard focus on mouse hover.
                                            // Prevents the user from having to click at least once in the viewport.
    // we need the focus on the viewport for keyboard 
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
    viewportWidget = viewportTab->GetViewport()->GetViewportWidget();

    // we need the focus on the viewport for keyboard 
    FSlateApplication::Get().SetKeyboardFocus( viewportWidget );
*/
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FKey& iKey )
{
    uint64 retFlags = 0;


    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl ) )
    {
        mPickingMode   = ePathPickingMode::SegmentHandle;
        mPickingFlags  = FOdysseyVectorPath::PICK_HANDLE_SEGMENT
                       | FOdysseyVectorPath::PICK_VERTEX ;

        retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    }

    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftShift ) || ( iKey == EKeys::RightShift ) )
    {
        mPickingMode  = ePathPickingMode::VertexHandle;
        mPickingFlags = FOdysseyVectorPath::PICK_HANDLE_VERTEX;

        retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    }

    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftAlt ) || ( iKey == EKeys::RightAlt ) )
    {
        mPickingMode  = ePathPickingMode::Vertex;
        mPickingFlags = FOdysseyVectorPath::PICK_VERTEX;

        retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownGlobalVector( iScene, iKey )
         | retFlags;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 retFlags = 0;

    // note, we cannot use FSlateApplication::Get().GetModifierKeys()
    // because the keys are already released. For consistency we do
    // the same in the KeyDown event even though we could use 
    // FSlateApplication::Get().GetModifierKeys()
    if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl )
      || ( iKey == EKeys::LeftShift   ) || ( iKey == EKeys::RightShift   )
      || ( iKey == EKeys::LeftAlt     ) || ( iKey == EKeys::RightAlt     ) )
    {
        retFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    }

    // first reset display mode
    mPickingMode = ePathPickingMode::Vertex;
    mPickingFlags = FOdysseyVectorPath::PICK_VERTEX;

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpGlobalVector( iScene, iKey )
         | retFlags;
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
    bool hasHit = false;

    removedPathArray.reserve( 10 );
    removedVertexArray.reserve( 10 );
    removedSegmentArray.reserve( 10 );
    addedSegmentArray.reserve( 10 );

    vectorEngine->Traverse
    ( iScene
    , 0
    , [ this
      , iScene
      , vectorEngine
      , &hasHit
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
                  // work on fresh new arrays to prevent double selection or something.
                  std::vector<FOdysseyVectorVertex*> pickedVertexArray;
                  std::vector<FOdysseyVectorHandleSegment*> pickedHandleSegmentArray;

                  path->PickPoint( iPointInTexture.x
                                 , iPointInTexture.y
                                 , PickingRadius
                                 , pickedVertexArray
                                 , pickedHandleSegmentArray // will be empty
                                 , FOdysseyVectorPath::PICK_VERTEX );

                  FOdysseyVectorPath::DeleteVertex( path
                                                  , pickedVertexArray
                                                  , removedVertexArray
                                                  , removedSegmentArray
                                                  , removedPathArray
                                                  , addedSegmentArray );

                  if( pickedVertexArray.size() )
                  {
                      hasHit = true;
                  }
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );

    if( hasHit )
    {
        // mPickedVertexArray is used as a boolean in MouseUp to trigger or not the picking mode
        mPickedVertexArray = removedVertexArray;

        for( int i = 0; i < removedPathArray.size(); i++ )
        {
            FOdysseyVectorPath* path = removedPathArray[i];

            path->GetParent()->RemoveChild( path );
        }

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // updated invalidated objects

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-path-edit-tool.transaction.delete-point","Vector Path Edit Tool"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                      , removedPathArray
                                                                      , removedVertexArray
                                                                      , removedSegmentArray
                                                                      , addedPathArray
                                                                      , addedVertexArray
                                                                      , addedSegmentArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
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
        GEditor->BeginTransaction(LOCTEXT("vector-path-edit-tool.transaction.align-point-selection","Align Point Selection"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexAlignment( iScene, mPickedVertexArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
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
                GEditor->BeginTransaction(LOCTEXT("vector-path-edit-tool.transaction.edit-vertex-handle","Vector Path Edit Tool"));
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

                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
                    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
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
                GEditor->BeginTransaction(LOCTEXT("vector-path-edit-tool.transaction.edit-vertex","Vector Path Edit Tool"));
                if( GUndo )
                {
                    FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathEdit( iScene
                                                                             , mPickedVertexArray
                                                                             , alteredSegmentArray );

                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
                    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
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
                GEditor->BeginTransaction(LOCTEXT("vector-path-edit-tool.transaction.edit-segment-handle","Vector Path Edit Tool"));
                if( GUndo )
                {
                    FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathEdit( iScene
                                                                             , mPickedVertexArray // empty
                                                                             , alteredSegmentArray );

                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
                    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
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
    FOdysseyVectorPath* path = segment->GetOwnerAsPath();
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
    FOdysseyVectorPath* path = iVertex->GetOwnerAsPath();
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
    double pointInTextureX = iPointInTexture.x;
    double pointInTextureY = iPointInTexture.y;
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
        std::vector<FOdysseyVectorPoint*> snappedPointArray;

        mPathEditHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

        /*
        // snapping
        mPathEditHUD->PickPoints( iPointInTexture.x
                                , iPointInTexture.y
                                , PickingRadius
                                , snappedPointArray );
        if( snappedPointArray.size() )
        {
            if( snappedPointArray[0]->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                FOdysseyVectorVertex* snappedVertex = static_cast<FOdysseyVectorVertex*>(snappedPointArray[0]);
                ::ULIS::FVec2D snappedVertexWorldCoords = snappedVertex->GetWorldCoords();

                pointInTextureX = snappedVertexWorldCoords.x;
                pointInTextureY = snappedVertexWorldCoords.y;
            }
        }
        */

        for( int i = 0; i < mPickedVertexArray.size(); i++ )
        {
            FOdysseyVectorVertex *vertex = mPickedVertexArray[i];
            ::ULIS::FRectD rect;

            rect = DragVertex( vertex
                             , pointInTextureX
                             , pointInTextureY
                             // we don't use iPointInTexture.deltaPosition because for some reason,
                             // the readings are not good when a key is pressed.
                             , pointInTextureX - mOldPointInTexture.x
                             , pointInTextureY - mOldPointInTexture.y
                             , WidenAllAlong && ( mPickedVertexArray.size() == 1 ) );

            //localInvalidatedArea = ( inited == false ) ? rect : localInvalidatedArea | rect;

            //inited = true;
        }

        for( int i = 0; i < mPickedHandleArray.size(); i++ )
        {
            FOdysseyVectorHandleSegment *handle = mPickedHandleArray[i];
            ::ULIS::FRectD rect;

            rect = DragSegmentHandle( handle
                                    , pointInTextureX
                                    , pointInTextureY
                                    // we don't use iPointInTexture.deltaPosition because for some reason,
                                    // the readings are not good when a key is pressed.
                                    , pointInTextureX - mOldPointInTexture.x
                                    , pointInTextureY - mOldPointInTexture.y
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
        mOldPointInTexture = ::ULIS::FVec2D( pointInTextureX, pointInTextureY );

        iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED );
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

void
UOdysseyPainterEditorVectorPathEditTool::PickObjects( FOdysseyVectorGroupPaint* iScene
                                                    , double iX
                                                    , double iY )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorObject*> pickedObjectArray;
    ::ULIS::FRectD roi;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-path-edit-tool.transaction.select-object","Vector Path Edit Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( iScene );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

          mPathEditHUD->ClearMask();
    roi = mPathEditHUD->GenerateMask( iX, iY, PickingRadius );
    // TODO: pass the mask image as arg to Pick function
    vectorEngine->SetBLMask( mPathEditHUD->GetMask() );

    // deselect all if control key is not pressed
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
    {
        vectorEngine->ClearObjectSelection();
    }

    // dragging occured
    vectorEngine->Pick( iScene, roi, pickedObjectArray, FOdysseyVectorObject::PICK_MASK_BASED );

    // when dragging occured, we select all objects lying in the selection area.
    for ( int i = 0; i < pickedObjectArray.size(); i++ )
    {
        vectorEngine->SelectObject( pickedObjectArray[i] );
    }

    vectorEngine->SetBLMask( nullptr );
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    uint64 ret = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

        if( ( mPickedVertexArray.size() == 0 ) && ( mPickedHandleArray.size() == 0 ) )
        {
            PickObjects( iScene, iPointInTexture.x, iPointInTexture.y );

            ret |= FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED;
        }

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

        vectorEngine->ResetHUD();
    }

    return ret;
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

TSharedRef<SWidget>
UOdysseyPainterEditorVectorPathEditTool::CreateTopTabWidget()
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;
    const TSharedPtr<ISinglePropertyView> radiusPropertyView = propertyEditorModule.CreateSingleProperty(this, "PickingRadius", defaultPropertyParams);
    const TSharedPtr<ISinglePropertyView> widenPropertyView = propertyEditorModule.CreateSingleProperty(this, "WidenAllAlong", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> radiusHandle = radiusPropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> widenHandle = widenPropertyView->GetPropertyHandle();

    return SNew(SUniformWrapPanel)
        .SlotPadding(FVector2D(3.f, 0.f))
        .EvenRowDistribution(true)
        .HAlign(HAlign_Left)
        + SUniformWrapPanel::Slot()
        [
            SNew( SOdysseyPainterEditorVectorEditionMode, GetEditor() )
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(radiusHandle, radiusPropertyView).ToSharedRef()
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(widenHandle, widenPropertyView).ToSharedRef()
        ];
}

FText
UOdysseyPainterEditorVectorPathEditTool::GetTooltip() const
{
    return LOCTEXT("vector-path-edit-tool.tooltip", "Path Edit Tool");
}

#undef LOCTEXT_NAMESPACE
