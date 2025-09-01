// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVector.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "Undo/OdysseyVectorUndoVertexRadius.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"
#include "Undo/OdysseyVectorUndoPathEdit.h"
#include "Undo/OdysseyVectorUndoVertexAlignment.h"
#include "ISinglePropertyView.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyMediaVector.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyPainterEditorSource.h"
#include "SOdysseySinglePropertyView.h"
#include "Widgets/Input/SSegmentedControl.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathEditTool::~UOdysseyPainterEditorVectorPathEditTool()
{
}

UOdysseyPainterEditorVectorPathEditTool::UOdysseyPainterEditorVectorPathEditTool()
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorPathEditToolHUD>( this ), false, true )
    , mPickingFlags ( FOdysseyVectorPath::PICK_VERTEX )
    , mEditionMode  ( eVectorPathEditEditionMode::Vertex )
    , PickingRadius(10.0f)
    , WidenAllAlong( true )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathEdit64");

    mPathEditHUD = static_cast<FOdysseyPainterEditorVectorPathEditToolHUD*>( mBaseHUD.Get() );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPathEditTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorPathEditTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    // Note: we use mworkingLayer because is not called in the same context as Load, so there could be
    // an orphan cell here in the case of a cell deletion for example.
    mWorkingLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    if( iScene->GetCell()->GetLayer() == mWorkingLayer )
    {
        mWorkingLayer->RequestRedraw( iScene->GetCell(), 0 );
    }

    return 0;
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
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FKeyEvent& InKeyEvent )
{
    if( InKeyEvent.IsRepeat() == false )
    {
        FKey key = InKeyEvent.GetKey();

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftControl ) || ( key == EKeys::RightControl )
          || ( key == EKeys::LeftCommand ) || ( key == EKeys::RightCommand ) )
        {
            mEditionMode   = eVectorPathEditEditionMode::SegmentHandle;
            mPickingFlags  = FOdysseyVectorPath::PICK_HANDLE_SEGMENT
                           | FOdysseyVectorPath::PICK_VERTEX ;

            iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

            return true;
        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftShift ) || ( key == EKeys::RightShift ) )
        {
            mEditionMode  = eVectorPathEditEditionMode::VertexHandle;
            mPickingFlags = FOdysseyVectorPath::PICK_HANDLE_VERTEX;

            iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

            return true;
        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftAlt ) || ( key == EKeys::RightAlt ) )
        {
            mEditionMode  = eVectorPathEditEditionMode::Alter;
            mPickingFlags = FOdysseyVectorPath::PICK_VERTEX;

            iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

            return true;
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FKeyEvent& InKeyEvent )
{
    FKey key = InKeyEvent.GetKey();

    // note, we cannot use FSlateApplication::Get().GetModifierKeys()
    // because the keys are already released. For consistency we do
    // the same in the KeyDown event even though we could use
    // FSlateApplication::Get().GetModifierKeys()
    if ( ( key == EKeys::LeftControl ) || ( key == EKeys::RightControl )
      || ( key == EKeys::LeftCommand ) || ( key == EKeys::RightCommand )
      || ( key == EKeys::LeftShift   ) || ( key == EKeys::RightShift   )
      || ( key == EKeys::LeftAlt     ) || ( key == EKeys::RightAlt     ) )
    {
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
    }

    // first reset display mode
    mEditionMode = eVectorPathEditEditionMode::Vertex;
    mPickingFlags = FOdysseyVectorPath::PICK_VERTEX;

    return false;
}

// temporary structure to store the path that will have their vertices removed.
// the path are retrived via the quadtree that is built by the HUD. Each quad
// stores points (vertices), and we retrieve the path from those vertices.
// then we can pass the data to FOdysseyVectorPath::DeleteVertex()
struct FAlteredPathRecord
{
    FOdysseyVectorPath* path;
    std::vector<FOdysseyVectorVertex*> vertexArray;

    FAlteredPathRecord( FOdysseyVectorPath* iPath )
    {
        path = iPath;
        vertexArray.reserve( 10 );
    }
};

static void
CutPath( FOdysseyVectorPath* iPath
       , const ::ULIS::FVec2D& iCutLineP0
       , const ::ULIS::FVec2D& iCutLineP1
       , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
       , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
       , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray )
{
    BLMatrix2D& inverseWorldMatrix = iPath->GetInverseWorldMatrix();
    BLPoint localP0 = inverseWorldMatrix.mapPoint( iCutLineP0.x, iCutLineP0.y );
    BLPoint localP1 = inverseWorldMatrix.mapPoint( iCutLineP1.x, iCutLineP1.y );
    double xmin = ::ULIS::FMath::Min( localP0.x, localP1.x );
    double ymin = ::ULIS::FMath::Min( localP0.y, localP1.y );
    double xmax = ::ULIS::FMath::Max( localP0.x, localP1.x );
    double ymax = ::ULIS::FMath::Max( localP0.y, localP1.y );
    ::ULIS::FRectD rect = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );
    ::ULIS::FRectD isxRect;

    if( FOdysseyVector::IntersectRegions<double>( rect, iPath->GetBBox( false, false ), &isxRect ) )
    {
        iPath->Cut( ::ULIS::FVec2D( localP0.x, localP0.y )
                  , ::ULIS::FVec2D( localP1.x, localP1.y )
                  , oAddedVertexArray
                  , oAddedSegmentArray
                  , oRemovedSegmentArray );
    }

    //path->Invalidate();
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpCutPaths( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    std::vector<FOdysseyVectorVertex*> addedVertexArray;
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;

    // crashes if I don't reserve. Why that ?
    removedSegmentArray.reserve(50);
    addedSegmentArray.reserve(50);
    addedVertexArray.reserve(50);

    // traverse recursively on objects determined by the HUD
    FOdysseyVectorObject::Traverse
    ( iScene
    , 0
    , [ this
        , iScene
        , &iPointInTexture
        , &addedVertexArray
        , &addedSegmentArray
        , &removedSegmentArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
        {
            if( iScene->GetCell()->ObjectHasFocus( object, traversalFlags )  )
            {
                if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                {
                    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                    CutPath( path
                           , mPathEditHUD->GetCutLineP0()
                           , mPathEditHUD->GetCutLineP1()
                           , addedVertexArray
                           , addedSegmentArray
                           , removedSegmentArray );
                }

                return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
            }

            return 0;
        } );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-path-cut-tool.transaction.cut-path","Vector Path Cut Tool"));
    if( GUndo )
    {
        std::vector<FOdysseyVectorPath*> removedPathArray; // empty on purpose.
        std::vector<FOdysseyVectorVertex*> removedVertexArray; // empty on purpose.
        std::vector<FOdysseyVectorPath*> addedPathArray; // empty on purpose.

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

    // update. note: as the scene is invalidated, it will request a redraw as well
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpDeletePoint( FOdysseyVectorGroupPaint* iScene
                                                             , const std::vector<FOdysseyVectorPoint*>& iPickedPointArray )
{
    std::vector<FOdysseyVectorPath*> removedPathArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorPath*> addedPathArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray; // not filled, here just for the undo record
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::vector<FAlteredPathRecord> alteredPathRecordArray;
    bool hasHit = false;

    alteredPathRecordArray.reserve( 10 );

    removedPathArray.reserve( 10 );
    removedVertexArray.reserve( 10 );
    removedSegmentArray.reserve( 10 );
    addedSegmentArray.reserve( 10 );

    for( FOdysseyVectorPoint* point : iPickedPointArray )
    {
        if( point->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(point);
            FOdysseyVectorPath* path = vertex->GetOwnerAsPath();
            std::vector<FAlteredPathRecord>::iterator it = std::find_if( alteredPathRecordArray.begin()
                                                                       , alteredPathRecordArray.end()
                                                                       , [path]( const FAlteredPathRecord& alteredPathRecord ) -> bool
                                                                         {
                                                                             return ( alteredPathRecord.path == path );
                                                                         } );
            FAlteredPathRecord* alteredPathRecord = ( it == alteredPathRecordArray.end() ) ? &alteredPathRecordArray.emplace_back( path )
                                                                                           : &(*it);

            alteredPathRecord->vertexArray.push_back( vertex );
        }
    }

    if( alteredPathRecordArray.size() )
    {
        for( FAlteredPathRecord& alteredPathRecord : alteredPathRecordArray )
        {
            FOdysseyVectorPath::DeleteVertex( alteredPathRecord.path
                                            , alteredPathRecord.vertexArray
                                            , removedVertexArray
                                            , removedSegmentArray
                                            , removedPathArray
                                            , addedSegmentArray );
        }

        for( int i = 0; i < removedPathArray.size(); i++ )
        {
            FOdysseyVectorPath* path = removedPathArray[i];

            path->GetParent()->RemoveChild( path );
        }

        // Update. note: as the scene is invalidated, it will request a redraw as well
        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

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

    return 0;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpAddPoint( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const std::vector<FOdysseyVectorSegment*>& iPickedSegmentArray )
{
    std::vector<FOdysseyVectorPath*> removedPathArray; // not filled, here just for the undo record
    std::vector<FOdysseyVectorVertex*> removedVertexArray; // not filled, here just for the undo record
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorPath*> addedPathArray; // not filled, here just for the undo record
    std::vector<FOdysseyVectorVertex*> addedVertexArray;
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;

    for( FOdysseyVectorSegment* pickedSegment : iPickedSegmentArray )
    {
        BLPoint point = pickedSegment->GetOwner()->GetInverseWorldMatrix().mapPoint( iPointInTexture.x
                                                                                   , iPointInTexture.y );
        ::ULIS::FVec2D projectedPoint;
        double projectedPointT = pickedSegment->ProjectConstrained( ::ULIS::FVec2D( point.x
                                                                                  , point.y )
                                                                    , projectedPoint );
        std::vector<FOdysseyVectorVertex*> newVertexArray;
        std::vector<FOdysseyVectorSegment*> newSegmentArray;

        pickedSegment->Split( projectedPoint
                            , projectedPointT
                            , newVertexArray
                            , newSegmentArray );

        pickedSegment->GetOwnerAsPath()->RemoveSegment( pickedSegment );
            // record for undo
        removedSegmentArray.push_back( pickedSegment );

        for( FOdysseyVectorVertex* vertex : newVertexArray )
        {
            pickedSegment->GetOwnerAsPath()->AddVertex( vertex );
            // record for undo
            addedVertexArray.push_back( vertex );
        }

        for( FOdysseyVectorSegment* segment : newSegmentArray )
        {
            pickedSegment->GetOwnerAsPath()->AddSegment( segment );
            // record for undo
            addedSegmentArray.push_back( segment );
        }
    }

    // Update. note: as the scene is invalidated, it will request a redraw as well
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-path-edit-tool.transaction.add-point","Vector Path Edit Tool"));
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

    return 0;
}

void
UOdysseyPainterEditorVectorPathEditTool::GetPathsFromSelection( FOdysseyVectorGroupPaint* iScene
                                                              , std::vector<FOdysseyVectorPath*>& oPathArray )
{
    FOdysseyVectorObject::Traverse
    ( iScene
    , 0
    , [ iScene
      , &oPathArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( iScene->GetCell()->ObjectHasFocus( object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                  oPathArray.push_back( path );
              }

              return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
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

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownPickPoint( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    mSelectedPathArray.clear();
    mPickedVertexArray.clear();
    mPickedVertexPositionArray.clear();
    mPickedVertexRadiusArray.clear();
    mPickedHandleArray.clear();
    mPickedHandlePositionArray.clear();
    mSegmentAdjustmentArray.clear();

    FOdysseyVectorObject::Traverse
    ( iScene
    , 0
    , [ this
      , iScene
      , &iPointInTexture ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( iScene->GetCell()->ObjectHasFocus( object, traversalFlags ) )
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

              return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );

    // Link or Unlink segment handles
    if( ( mEditionMode == eVectorPathEditEditionMode::SegmentHandle ) &&  ( mPickedVertexArray.size() == 1 ) )
    {
        FOdysseyVectorVertex* vertex = mPickedVertexArray[0];

        //-------------- undo ---------------//
        GEditor->BeginTransaction(LOCTEXT("vector-path-edit-tool.transaction.align-point-selection","Align Point Selection"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexAlignment( iScene
                                                                            , mPickedVertexArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
        //---------- end of undo ------------//

        vertex->SetHandleAligned( vertex->IsHandleAligned() ? false : true );
    }
    // Else, save point coordinates before changing them
    else
    {
        switch( mEditionMode )
        {
            case eVectorPathEditEditionMode::VertexHandle :
                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("vector-path-edit-tool.transaction.edit-vertex-handle","Vector Path Edit Tool"));
                if( GUndo )
                {
                    FOdysseyVectorUndo* undo;

                    if( WidenAllAlong && ( mPickedVertexArray.size() == 1 ) )
                    {
                        GetPathsFromSelection( iScene, mSelectedPathArray );

                        undo = new FOdysseyVectorUndoVertexRadius( iScene
                                                                 , mSelectedPathArray );
                    }
                    else
                    {
                        undo = new FOdysseyVectorUndoVertexRadius( iScene
                                                                 , mPickedVertexArray );
                    }

                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
                }
                GEditor->EndTransaction();
            break;

            case eVectorPathEditEditionMode::Vertex :
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

            case eVectorPathEditEditionMode::SegmentHandle :
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


    // remember vertex position at mouse down
    mPickedVertexPositionArray.reserve( mPickedVertexArray.size() );
    mPickedVertexRadiusArray.reserve( mPickedVertexArray.size() );
    for( FOdysseyVectorVertex* vertex : mPickedVertexArray )
    {
        mPickedVertexPositionArray.emplace_back( vertex->GetX(), vertex->GetY() );
        mPickedVertexRadiusArray.emplace_back( vertex->GetRadius() );
    }

    // remember handle position at mouse down
    mPickedHandlePositionArray.reserve( mPickedHandleArray.size() );
    for( FOdysseyVectorHandleSegment* handle : mPickedHandleArray )
    {
        mPickedHandlePositionArray.emplace_back( handle->GetX(), handle->GetY() );
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                              | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );

    return 0;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mPointInTextureAtDown = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

        mPathEditHUD->SetCutLineP0( iPointInTexture.x, iPointInTexture.y );
        mPathEditHUD->SetCutLineP1( iPointInTexture.x, iPointInTexture.y );

        switch( mEditionMode )
        {
            case eVectorPathEditEditionMode::Alter :
            // dealt with in OnMouseUpVector
            break;

            default:
                OnMouseDownPickPoint( iScene, iPointInTexture, iKey );
            break;
        }
    }

    // redraw
//    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    // This populates mPathEditHUD::mHoveredPointArray
    mPathEditHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );
}

void
UOdysseyPainterEditorVectorPathEditTool::DragVertexHandle( FOdysseyVectorVertex *iVertex
                                                         , double iOriginalRadius
                                                         , const ::ULIS::FVec2D& iPointInTexture
                                                         , bool iWidenAllAlong )
{
    FOdysseyVectorPath* path = iVertex->GetOwnerAsPath();
    ::ULIS::FVec2D localMouse = FOdysseyVector::MapPoint( path->GetInverseWorldMatrix()
                                                        , iPointInTexture );
    double ratio = ( ::ULIS::FVec2D( iVertex->GetX() - localMouse.x
                                   , iVertex->GetY() - localMouse.y ).Distance() ) / iVertex->GetRadius();

    if( iWidenAllAlong )
    {
        for( int i = 0; i < mSelectedPathArray.size(); i++ )
        {
            mSelectedPathArray[i]->AlterRadius( ratio  );
        }
    }
    else
    {
        iVertex->SetRadius( iVertex->GetRadius() * ratio );
    }
}

void
UOdysseyPainterEditorVectorPathEditTool::DragSegmentHandle( FOdysseyVectorHandleSegment *iHandle
                                                          , double iHandleX
                                                          , double iHandleY
                                                          , double iDeltaX
                                                          , double iDeltaY )
{
    FOdysseyVectorSegment* segment = iHandle->GetOwner();
    FOdysseyVectorPath* path = segment->GetOwnerAsPath();
    FOdysseyVectorVertex* vertex = iHandle->GetAttachedVertex();
    BLPoint localVector = path->GetInverseWorldMatrix().mapVector( iDeltaX, iDeltaY );

    iHandle->Set( iHandleX + localVector.x
                , iHandleY + localVector.y );
}

void
UOdysseyPainterEditorVectorPathEditTool::DragVertex( FOdysseyVectorVertex *iVertex
                                                   , double iVertexX
                                                   , double iVertexY
                                                   , double iDeltaX
                                                   , double iDeltaY )
{
    FOdysseyVectorPath* path = iVertex->GetOwnerAsPath();
    BLPoint localVector = path->GetInverseWorldMatrix().mapVector( iDeltaX, iDeltaY );

    iVertex->Set( iVertexX + localVector.x
                , iVertexY + localVector.y );
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    double deltaX = iPointInTexture.x - mPointInTextureAtDown.x;
    double deltaY = iPointInTexture.y - mPointInTextureAtDown.y;

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

        mPathEditHUD->SetCutLineP1( iPointInTexture.x, iPointInTexture.y );
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

        if( mEditionMode == eVectorPathEditEditionMode::VertexHandle  )
        {
            for( int i = 0; i < mPickedVertexArray.size(); i++ )
            {
                FOdysseyVectorVertex *vertex = mPickedVertexArray[i];

                DragVertexHandle( vertex
                                , mPickedVertexRadiusArray[i]
                                , ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y )
                                , WidenAllAlong && ( mPickedVertexArray.size() == 1 ) );
            }
        }

        if( mEditionMode == eVectorPathEditEditionMode::Vertex        )
        {
            for( int i = 0; i < mPickedVertexArray.size(); i++ )
            {
                FOdysseyVectorVertex *vertex = mPickedVertexArray[i];

                DragVertex( vertex
                          , mPickedVertexPositionArray[i].x
                          , mPickedVertexPositionArray[i].y
                          // we don't use iPointInTexture.deltaPosition because for some reason,
                          // the readings are not good when a key is pressed.
                          , deltaX
                          , deltaY );
            }
        }

        if( ( mEditionMode == eVectorPathEditEditionMode::SegmentHandle )
        ||  ( mEditionMode == eVectorPathEditEditionMode::Vertex        ) )
        {
            for( int i = 0; i < mPickedHandleArray.size(); i++ )
            {
                FOdysseyVectorHandleSegment *handle = mPickedHandleArray[i];

                DragSegmentHandle( handle
                                 , mPickedHandlePositionArray[i].x
                                 , mPickedHandlePositionArray[i].y
                                 // we don't use iPointInTexture.deltaPosition because for some reason,
                                 // the readings are not good when a key is pressed.
                                 , deltaX
                                 , deltaY );

                //localInvalidatedArea = ( inited == false ) ? rect : localInvalidatedArea | rect;

                //inited = true;
            }
        }

        // adjust handle length to keep the same ratio as before the editing
        if( mEditionMode == eVectorPathEditEditionMode::Vertex )
        {
            for( FSegmentAdjustment& segmentAdjustment : mSegmentAdjustmentArray )
            {
                segmentAdjustment.Adjust();
            }
        }

        // Update. Note: as the seen is invalidated, it will request a redraw
        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                  | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
    }
}

uint64
UOdysseyPainterEditorVectorPathEditTool::PickObjects( FOdysseyVectorGroupPaint* iScene
                                                    , double iX
                                                    , double iY )
{
    FOdysseyVectorCell* vectorCell = iScene->GetCell();
    std::vector<FOdysseyVectorObject*> pickedObjectArray;
    ::ULIS::FRectD roi;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-path-edit-tool.transaction.select-object","Vector Path Edit Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( vectorCell->GetLayer()
                                                                     , vectorCell );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

          mPathEditHUD->ClearMask();
    roi = mPathEditHUD->GenerateMask( iX, iY, PickingRadius );
    // TODO: pass the mask image as arg to Pick function
    vectorCell->SetBLMask( mPathEditHUD->GetMask() );

    // deselect all if control key is not pressed
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
    {
        vectorCell->ClearObjectSelection();
    }

    // dragging occured
    vectorCell->Pick( iScene, roi, pickedObjectArray, FOdysseyVectorObject::PICK_MASK_BASED );

    // when dragging occured, we select all objects lying in the selection area.
    for ( int i = 0; i < pickedObjectArray.size(); i++ )
    {
        vectorCell->SelectObject( pickedObjectArray[i] );
    }

    vectorCell->SetBLMask( nullptr );

    return 0;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        switch( mEditionMode )
        {
            case eVectorPathEditEditionMode::Alter :
            {
                std::vector<FOdysseyVectorPoint*>& hoveredPointArray = mPathEditHUD->GetHoveredPointArray();

                // This populates mPathEditHUD::mHoveredPointArray
                mPathEditHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

                if( hoveredPointArray.size() )
                {
                    OnMouseUpDeletePoint( iScene, hoveredPointArray );
                }
                else
                {
                    std::vector<FOdysseyVectorSegment*> pickedSegmentArray;

                    // check if we picked any segment
                    PickSegments( iScene
                                , iPointInTexture.x
                                , iPointInTexture.y
                                , PickingRadius
                                , true
                                , false
                                , pickedSegmentArray );

                    if( pickedSegmentArray.size() )
                    {
                        OnMouseUpAddPoint( iScene
                                         , iPointInTexture
                                         , pickedSegmentArray );
                    }
                    else
                    {
                        OnMouseUpCutPaths( iScene, iPointInTexture );
                    }
                }
            }
            break;

            default:
            {
                // If nothing was selected, we pick an object
                if( ( mPickedVertexArray.size() == 0 ) && ( mPickedHandleArray.size() == 0 ) )
                {
                    PickObjects( iScene, iPointInTexture.x, iPointInTexture.y );
                }
            }
            break;
        }

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
    }

    return true;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::GetPickingFlags()
{
    return mPickingFlags;
}

void
UOdysseyPainterEditorVectorPathEditTool::ExtendContextMenuObject( FOdysseyVectorGroupPaint* iScene
                                                                , FMenuBuilder& menu
                                                                , uint64 iObjectMenuFlags )
{
    UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuObject( iScene, menu, iObjectMenuFlags );

// We must first end the section that might be created by the caller function
// See FOdysseyPainterEditor::AddEditMenuEntry() for details
    menu.EndSection();
    menu.BeginSection("Tool specific", TAttribute(FText::FromString("Tool/Specific")));
    {
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.object-context-menu.subdivide-segments.name", "Subdivide segments")
        , LOCTEXT("vector-tool.object-context-menu.subdivide-segments.tooltip", "Subdivide segments")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Subdivide, GetEditor(), iScene )));
    }
// the caller will call menu.EndSection()
}

eVectorPathEditEditionMode
UOdysseyPainterEditorVectorPathEditTool::GetEditionMode()
{
    return mEditionMode;
}

void
UOdysseyPainterEditorVectorPathEditTool::SetEditionMode( eVectorPathEditEditionMode iMode )
{
    mEditionMode = iMode;
}

const FSlateBrush*
UOdysseyPainterEditorVectorPathEditTool::GetBackgroundColor( eVectorPathEditEditionMode iMode ) const
{
    static FSlateColorBrush selected = FSlateColorBrush( FStyleColors::Select );

    return ( iMode == mEditionMode ) ? &selected : nullptr;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorPathEditTool::CreateModifierSegmentControl()
{
    return SNew(SSegmentedControl<eVectorPathEditEditionMode>)
           .Value_Lambda( [this]{ return mEditionMode; } )
           .SupportsEmptySelection( false )
           .SupportsMultiSelection( false )
           .IsEnabled( false ) // currently not clickable - Info only
           .UniformPadding( FMargin( 2, 0, 2, 0 ) )
           .OnValueChanged( SSegmentedControl<eVectorPathEditEditionMode>::FOnValueChanged::CreateUObject( this, &UOdysseyPainterEditorVectorPathEditTool::SetEditionMode ) )
           // DEFAULT
           + SSegmentedControl<eVectorPathEditEditionMode>::Slot( eVectorPathEditEditionMode::Vertex )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathEdit16") )
           .ToolTip( LOCTEXT("vector-path-edit-tool.edition-mode.default.name", "Default") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorPathEditTool::GetBackgroundColor, eVectorPathEditEditionMode::Vertex  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.PathEditMoveVertex20") )
               ]
           ]
           // CTRL
           + SSegmentedControl<eVectorPathEditEditionMode>::Slot( eVectorPathEditEditionMode::SegmentHandle )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathEdit16") )
#if PLATFORM_WINDOWS
           .ToolTip( LOCTEXT("vector-path-edit-tool.edition-mode.ctrl.name", "Deform Segment (CTRL)") )
#endif
#if PLATFORM_MAC
           .ToolTip( LOCTEXT("vector-path-edit-tool.edition-mode.cmd.name", "Deform Segment (CMD)") )
#endif
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorPathEditTool::GetBackgroundColor, eVectorPathEditEditionMode::SegmentHandle  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.PathEditDeformSegment20") )
               ]
           ]
           // SHIFT
           + SSegmentedControl<eVectorPathEditEditionMode>::Slot( eVectorPathEditEditionMode::VertexHandle )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathEdit16") )
           .ToolTip( LOCTEXT("vector-path-edit-tool.edition-mode.shift.name", "Widen Vertex (SHIFT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorPathEditTool::GetBackgroundColor, eVectorPathEditEditionMode::VertexHandle  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.PathEditWidenVertex20") )
               ]
           ]
           // ALT
           + SSegmentedControl<eVectorPathEditEditionMode>::Slot( eVectorPathEditEditionMode::Alter )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathEdit16") )
           .ToolTip( LOCTEXT("vector-path-edit-tool.edition-mode.alt.name", "Add/Remove Vertex (ALT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorPathEditTool::GetBackgroundColor, eVectorPathEditEditionMode::Alter  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.PathEditAddRemoveVertex20") )
               ]
           ];
}

void
UOdysseyPainterEditorVectorPathEditTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    //TOptional<eVectorPathEditEditionMode> val;

    Super::ExtendToolbar(iBuilder);

    iBuilder.BeginSection( NAME_None );

    iBuilder.AddWidget(
        CreateModifierSegmentControl()
    );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorPathEditTool, PickingRadius ), FSinglePropertyParams())
            .InnerPadding(10.f)
            .ValueWidthOverride(100.f)
        ]
    );

/*
    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .MinWidth( 16 )
            [
                SNew(SImage)
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathEdit16") )
            ]
            + SHorizontalBox::Slot()
            .MinWidth( 16 )
            [
                SNew(SImage)
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathEdit16") )
            ]
            + SHorizontalBox::Slot()
            .MinWidth( 16 )
            [
                SNew(SImage)
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathEdit16") )
            ]
            + SHorizontalBox::Slot()
            .MinWidth( 16 )
            [
                SNew(SImage)
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathEdit16") )
            ]
        ]
    );
*/

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorPathEditTool, WidenAllAlong ), FSinglePropertyParams())
            .InnerPadding(10.f)
        ]
    );

    iBuilder.EndSection();
}

FText
UOdysseyPainterEditorVectorPathEditTool::GetTooltip() const
{
    return LOCTEXT("vector-path-edit-tool.tooltip", "Path Edit Tool");
}

#undef LOCTEXT_NAMESPACE
