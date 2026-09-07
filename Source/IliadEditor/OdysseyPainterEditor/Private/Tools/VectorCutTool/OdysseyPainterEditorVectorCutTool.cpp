// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorCutTool/OdysseyPainterEditorVectorCutTool.h"

#include "Editor.h"
#include "ISinglePropertyView.h"
#include "Widgets/Layout/SWrapBox.h"

#include "Tools/VectorCutTool/OdysseyPainterEditorVectorCutToolHUD.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaVector.h"
#include "SOdysseySinglePropertyView.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoErase.h"


#include <chrono>

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorCutTool::~UOdysseyPainterEditorVectorCutTool()
{
}

UOdysseyPainterEditorVectorCutTool::UOdysseyPainterEditorVectorCutTool()
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorCutToolHUD>( this ), false, true )
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.PathCut64"));

    Shapes.AddShapeType( EOdysseyShapeType::kLine, nullptr );
    Shapes.AddShapeType( EOdysseyShapeType::kRectangle, nullptr );
    Shapes.AddShapeType( EOdysseyShapeType::kEllipse, nullptr );
    Shapes.AddShapeType( EOdysseyShapeType::kFreehand, nullptr );

    Shapes.SetActiveShapeType( EOdysseyShapeType::kFreehand );

    mPickHUD = static_cast<FOdysseyPainterEditorVectorCutToolHUD*>( mBaseHUD.Get() );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides


bool
UOdysseyPainterEditorVectorCutTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorCutTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorCutTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
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

void
UOdysseyPainterEditorVectorCutTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                      , const FOdysseyPoint& iPointInTexture )
{
    mIsLayerHovered = !(
        ( iPointInTexture.x < 0 )
        || ( iPointInTexture.y < 0 )
        || ( iPointInTexture.x > iScene->GetLayer()->GetWidth() )
        || ( iPointInTexture.y > iScene->GetLayer()->GetHeight() ) );

    mMouseCursor = mIsLayerHovered ? EMouseCursor::Crosshairs : EMouseCursor::SlashedCircle;
}

bool
UOdysseyPainterEditorVectorCutTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                     , const FOdysseyPoint& iPointInTexture
                                                     , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( ( mIsLayerHovered )
     && ( iKey == EKeys::LeftMouseButton ) )
    {
        mPointArray.clear();

        switch( Shapes.GetActiveShapeType() )
        {
            case EOdysseyShapeType::kLine :
                mPointArray.resize( 2 );

                mPointArray[0] = mPointArray[1] = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );
            break;

            default:
                mPointArray.push_back( ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y ) );
            break;
        }
    }

    //iScene->GetLayer()->RequestRedraw( 0 );

    return true;
}

void
UOdysseyPainterEditorVectorCutTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                     , const FOdysseyPoint& iPointInTexture )
{
    //::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };

    if ( ( mIsLayerHovered )
      && ( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE ) )
    {
        ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };

        switch( Shapes.GetActiveShapeType() )
        {
            case EOdysseyShapeType::kRectangle:
            case EOdysseyShapeType::kEllipse :
            {
                ::ULIS::FVec2D downPoint = mPointArray[0];

                mPointArray.clear();
                mPointArray.push_back( downPoint );
                mPointArray.push_back( point );
            }
            break;

            case EOdysseyShapeType::kFreehand :
                mPointArray.push_back( point );
            break;

            case EOdysseyShapeType::kLine :
                mPointArray[1] = point;
            break;

            default:
            break;
        }
    }

    // force redraw
    //iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

::ULIS::FRectD
UOdysseyPainterEditorVectorCutTool::GenerateMask( bool iStroke )
{
    ::ULIS::FRectD roi = ::ULIS::FRectD::FromXYWH( 0, 0, 0, 0 );

    mPickHUD->ClearMask();

    if( mPointArray.size() > 1 )
    {
        switch( Shapes.GetActiveShapeType() )
        {
            case EOdysseyShapeType::kRectangle:
            {
                double xmin = ::ULIS::FMath::Min( mPointArray[0].x, mPointArray[1].x );
                double ymin = ::ULIS::FMath::Min( mPointArray[0].y, mPointArray[1].y );
                double xmax = ::ULIS::FMath::Max( mPointArray[0].x, mPointArray[1].x );
                double ymax = ::ULIS::FMath::Max( mPointArray[0].y, mPointArray[1].y );
                ::ULIS::FRectD rect = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );

                return mPickHUD->GenerateRectangleMask( rect, iStroke );
            }

            case EOdysseyShapeType::kEllipse:
            {
                ::ULIS::FVec2D diagonal = ::ULIS::FVec2D( mPointArray[1] - mPointArray[0] );

                return mPickHUD->GenerateCircleMask( mPointArray[0].x
                                                   , mPointArray[0].y
                                                   , diagonal.Distance()
                                                   , iStroke );
            }

            case EOdysseyShapeType::kLine:
            {
                return mPickHUD->GenerateLineMask( mPointArray[0], mPointArray[1] );
            }

            case EOdysseyShapeType::kFreehand:
                return mPickHUD->GenerateFreehandMask( mPointArray, iStroke );

            default:
            break;
        }
    }

    return roi;
}

uint64
UOdysseyPainterEditorVectorCutTool::OnMouseUpVectorObjectMode( FOdysseyVectorGroupPaint* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    std::vector<FOdysseyVectorObject*> pickedObjectArray;
    ::ULIS::FRectD roi;

    // deselect all if control key is not pressed
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
    {
        iScene->GetCell()->ClearObjectSelection();
    }

    // dragging occured
    if ( mPointArray.size() > 1 )
    {
        std::vector<FOdysseyVectorObject*> addedObjectArray;
        std::vector<FOdysseyVectorVertex*> addedVertexArray;
        std::vector<FOdysseyVectorSegment*> addedSegmentArray;
        std::vector<FOdysseyVectorVertex*> removedVertexArray;
        std::vector<FOdysseyVectorSegment*> removedSegmentArray;
        std::vector<FOdysseyVectorObject*> removedObjectArray;

        // the mask as strokes
        roi = GenerateMask( true );

        CutPaths( iScene
                , roi
                , addedObjectArray
                , addedVertexArray
                , addedSegmentArray
                , removedVertexArray
                , removedSegmentArray
                , removedObjectArray );

        if( Shapes.GetActiveShapeType() != EOdysseyShapeType::kLine )
        {
            // the mask now becomes a filled area
            roi = GenerateMask( false );

            // we then select the path that were newly added
/*refactor
            iScene->GetCell()->Pick( iScene, roi, pickedObjectArray, FOdysseyVectorObject::PICK_MASK_BASED );
*/
            // when dragging occured, we select all objects lying in the selection area.
            for ( int i = 0; i < pickedObjectArray.size(); i++ )
            {
                iScene->GetCell()->SelectObject( pickedObjectArray[i] );
            }
        }

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-cut-tool.transaction.cut","Cut"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoErase( iScene
                                                                  , addedObjectArray
                                                                  , addedVertexArray
                                                                  , addedSegmentArray
                                                                  , removedObjectArray
                                                                  , removedVertexArray
                                                                  , removedSegmentArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
    }

    return 0;
}

void
UOdysseyPainterEditorVectorCutTool::CutPaths( FOdysseyVectorGroupPaint* iScene
                                            , const ::ULIS::FRectD& iErasureArea
                                            , std::vector<FOdysseyVectorObject*>& oAddedObjectArray
                                            , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                            , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                            , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                            , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                                            , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray )
{
    FOdysseyVectorObject::Traverse
    ( iScene
    , GetEditor()->GetVectorHUDFlags()
    , [ this
      , iScene
      , &iErasureArea
      , &oAddedObjectArray
      , &oAddedVertexArray
      , &oAddedSegmentArray
      , &oRemovedVertexArray
      , &oRemovedSegmentArray
      , &oRemovedObjectArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( iScene->GetCell()->ObjectHasFocus( object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);
                  ::ULIS::FRectD pathWorldBBox = path->GetBBox( false, true );

                  if( FOdysseyVector::IntersectRegions<double>( pathWorldBBox
                                                              , iErasureArea
                                                              , nullptr ) )
                  {
                      if( path->Erase( oAddedObjectArray
                                      , oAddedVertexArray
                                      , oAddedSegmentArray
                                      , oRemovedVertexArray
                                      , oRemovedSegmentArray
                                      , *mPickHUD->GetMask()
                                      , false
                                      , true ) )
                      {
                          oRemovedObjectArray.push_back( path );
                      }
                  }
              }

              return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );

    for( int i = 0; i < oAddedObjectArray.size(); i++ )
    {
        // parent is stored in mParent variable byt the Erase function even though it does not
        // belong to the parent. It kinda sucks but it is easier that way, otherwise Traverse()
        // will have some problems because we change the children list, and Traverse is recursive.
        oAddedObjectArray[i]->GetParent()->AppendChild( oAddedObjectArray[i] );
    }

    // Also here, we remove AFTER the Traverse() has been executed, because traverse is recursive
    // so we can't alter the hierarchy, unless traverse works on copies of the children list but that
    // would be very inefficient.
    for( int i = 0; i < oRemovedObjectArray.size(); i++ )
    {
        if( oRemovedObjectArray[i]->GetChildrenList().size() == 0 )
        {
            oRemovedObjectArray[i]->GetParent()->RemoveChild( oRemovedObjectArray[i] );

            if( oRemovedObjectArray[i]->IsSelected() )
            {
                iScene->GetCell()->UnselectObject( oRemovedObjectArray[i] );
            }
        }
    }

    iScene->UpdateMatrix();

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

bool
UOdysseyPainterEditorVectorCutTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    FOdysseyVectorCell* vectorCell = iScene->GetCell();
    ::ULIS::FRectD roi;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( ( mIsLayerHovered )
     && ( iKey == EKeys::LeftMouseButton ) )
    {
/*refactor
        mCutToolHUD->GenerateMask( true );
*/
        if( ( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
         || ( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX ) )
        {
            OnMouseUpVectorObjectMode( iScene, iPointInTexture, iKey );
        }

        mPointArray.clear();
    }

    // force redraw
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->ResetHUD( iScene );

    //iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return true;
}

std::vector<::ULIS::FVec2D>&
UOdysseyPainterEditorVectorCutTool::GetPointArray()
{
    return mPointArray;
}

void
UOdysseyPainterEditorVectorCutTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);
}

FText
UOdysseyPainterEditorVectorCutTool::GetTooltip() const
{
    return LOCTEXT("vector-cut-tool.tooltip", "Cut Tool");
}

#undef LOCTEXT_NAMESPACE
