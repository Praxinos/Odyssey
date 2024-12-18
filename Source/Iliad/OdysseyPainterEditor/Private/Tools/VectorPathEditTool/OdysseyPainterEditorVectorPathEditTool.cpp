// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "OdysseyMediaVector.h"
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
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorPathEditTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

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
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                              , const FKey& iKey
                                                              , uint64& oSignalFlags )
{
    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl )
      || ( iKey == EKeys::LeftCommand ) || ( iKey == EKeys::RightCommand ) )
    {
        mPickingMode   = ePathPickingMode::SegmentHandle;
        mPickingFlags  = FOdysseyVectorPath::PICK_HANDLE_SEGMENT
                       | FOdysseyVectorPath::PICK_VERTEX ;
        iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );
        return true;
    }

    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftShift ) || ( iKey == EKeys::RightShift ) )
    {
        mPickingMode  = ePathPickingMode::VertexHandle;
        mPickingFlags = FOdysseyVectorPath::PICK_HANDLE_VERTEX;
        iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );
        return true;
    }

    // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
    // with the events processing in the OnKeyUpGlobalVector(), we do like that.
    if ( ( iKey == EKeys::LeftAlt ) || ( iKey == EKeys::RightAlt ) )
    {
        mPickingMode  = ePathPickingMode::Alter;
        mPickingFlags = FOdysseyVectorPath::PICK_VERTEX;
        iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );
        return true;
    }

    return false;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FKey& iKey
                                                            , uint64& oSignalFlags )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // note, we cannot use FSlateApplication::Get().GetModifierKeys()
    // because the keys are already released. For consistency we do
    // the same in the KeyDown event even though we could use
    // FSlateApplication::Get().GetModifierKeys()
    if ( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl )
      || ( iKey == EKeys::LeftCommand ) || ( iKey == EKeys::RightCommand )
      || ( iKey == EKeys::LeftShift   ) || ( iKey == EKeys::RightShift   )
      || ( iKey == EKeys::LeftAlt     ) || ( iKey == EKeys::RightAlt     ) )
    {
        // redraw
        iScene->GetEngine()->Invalidate( 0 );
    }

    // first reset display mode
    mPickingMode = ePathPickingMode::Vertex;
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

    if( FOdysseyVector::IntersectRegions<double>( rect, iPath->GetBBox(false), &isxRect ) )
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
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorVertex*> addedVertexArray;
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 notificationFlags = 0;

    // crashes if I don't reserve. Why that ?
    removedSegmentArray.reserve(50);
    addedSegmentArray.reserve(50);
    addedVertexArray.reserve(50);

    // traverse recursively on objects determined by the HUD
    vectorEngine->Traverse
    ( iScene
    , 0
    , [ this
        , iScene
        , vectorEngine
        , &iPointInTexture
        , &addedVertexArray
        , &addedSegmentArray
        , &removedSegmentArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
        {
            if( vectorEngine->ObjectHasFocus( iScene, object, traversalFlags )  )
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

                return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
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
                                                                    , addedSegmentArray
                                                                    , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // update invalidated objects

    return notificationFlags;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpDeletePoint( FOdysseyVectorGroupPaint* iScene
                                                             , const std::vector<FOdysseyVectorPoint*>& iPickedPointArray )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorPath*> removedPathArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorPath*> addedPathArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray; // not filled, here just for the undo record
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::vector<FAlteredPathRecord> alteredPathRecordArray;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS;
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

        iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // updated invalidated objects

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
                                                                      , addedSegmentArray
                                                                      , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
    }

    return notificationFlags;
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
    uint64 notificationFlags = 0;

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

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

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
                                                                  , addedSegmentArray
                                                                  , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    return notificationFlags;
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

uint64
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownPickPoint( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE;

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
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexAlignment( iScene
                                                                             , mPickedVertexArray
                                                                             , notificationFlags );

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

                        undo = new FOdysseyVectorUndoVertexRadius( iScene, mSelectedPathArray, notificationFlags );
                    }
                    else
                    {
                        undo = new FOdysseyVectorUndoVertexRadius( iScene, mPickedVertexArray, notificationFlags );
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
                                                                             , alteredSegmentArray
                                                                             , notificationFlags );

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
                                                                             , alteredSegmentArray
                                                                             , notificationFlags );

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

    return notificationFlags;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey
                                                          , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    uint64 notificationFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mOldPointInTexture = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

        mPathEditHUD->SetCutLineP0( iPointInTexture.x, iPointInTexture.y );
        mPathEditHUD->SetCutLineP1( iPointInTexture.x, iPointInTexture.y );

        switch( mPickingMode )
        {
            case ePathPickingMode::Alter :
            // dealt with in OnMouseUpVector
            break;

            default:
                notificationFlags = OnMouseDownPickPoint( iScene, iPointInTexture, iKey );
            break;
        }
    }

    // redraw
    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE
                                   | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
    oSignalFlags = notificationFlags;

    return true;
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , uint64& oSignalFlags )
{
    std::vector<FOdysseyVectorPoint*>& hoveredPointArray = mPathEditHUD->GetHoveredPointArray();
    uint64 notificationFlags = 0;

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

    // This populates mPathEditHUD::mHoveredPointArray
    mPathEditHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    // redraw
    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );
    oSignalFlags = notificationFlags;
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
    FOdysseyVectorVertex* vertex = iHandle->GetAttachedVertex();
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

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , uint64& oSignalFlags )
{
    uint64 notificationFlags = 0;
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

        if( ( mPickingMode == ePathPickingMode::Vertex        )
         || ( mPickingMode == ePathPickingMode::VertexHandle  ) )
        {
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
        }

        if( ( mPickingMode == ePathPickingMode::SegmentHandle )
        ||  ( mPickingMode == ePathPickingMode::Vertex        ) )
        {
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

        iScene->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                      | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
    }

    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );
    oSignalFlags = notificationFlags;
}

uint64
UOdysseyPainterEditorVectorPathEditTool::PickObjects( FOdysseyVectorGroupPaint* iScene
                                                    , double iX
                                                    , double iY )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorObject*> pickedObjectArray;
    ::ULIS::FRectD roi;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-path-edit-tool.transaction.select-object","Vector Path Edit Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( iScene, notificationFlags );

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

    return notificationFlags;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey
                                                        , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    uint64 notificationFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

        switch( mPickingMode )
        {
            case ePathPickingMode::Alter :
            {
                std::vector<FOdysseyVectorPoint*>& hoveredPointArray = mPathEditHUD->GetHoveredPointArray();

                // This populates mPathEditHUD::mHoveredPointArray
                mPathEditHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

                if( hoveredPointArray.size() )
                {
                    notificationFlags |= OnMouseUpDeletePoint( iScene, hoveredPointArray );
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
                        notificationFlags |= OnMouseUpAddPoint( iScene
                                                              , iPointInTexture
                                                              , pickedSegmentArray );
                    }
                    else
                    {
                        notificationFlags |= OnMouseUpCutPaths( iScene, iPointInTexture );
                    }
                }
            }
            break;

            default:
            {
                // If nothing was selected, we pick an object
                if( ( mPickedVertexArray.size() == 0 ) && ( mPickedHandleArray.size() == 0 ) )
                {
                    notificationFlags |= PickObjects( iScene, iPointInTexture.x, iPointInTexture.y );
                }
            }
            break;
        }

        iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

        vectorEngine->ResetHUD();
    }

    // redraw
    iScene->GetEngine()->Invalidate( 0 );
    oSignalFlags = notificationFlags;
    return true;
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
          LOCTEXT("vector-tool.object-context-menu.reset-view.name", "Subdivide segments")
        , LOCTEXT("vector-tool.object-context-menu.reset-view.tooltip", "Subdivide segments")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Subdivide, GetEditor(), iScene )));
    }
// the caller will call menu.EndSection()
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
