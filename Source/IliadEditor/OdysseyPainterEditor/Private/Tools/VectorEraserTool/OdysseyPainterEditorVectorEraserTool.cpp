// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserToolHUD.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
#include "OdysseyVector.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "Undo/OdysseyVectorUndoErase.h"
#include "SOdysseySinglePropertyView.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorEraserTool::~UOdysseyPainterEditorVectorEraserTool()
{
}

UOdysseyPainterEditorVectorEraserTool::UOdysseyPainterEditorVectorEraserTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorEraserToolHUD( this ), false )
    , SplitPath( true )
    , Radius( 20.0f )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Eraser64");

    mEraserHUD = static_cast<FOdysseyPainterEditorVectorEraserToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorEraserTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorEraserTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorEraserTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey
                                                        , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    mMin.x = mMax.x = iPointInTexture.x;
    mMin.y = mMax.y = iPointInTexture.y;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mEraserHUD->BlendMask( true );
        mEraserHUD->ClearMask();
        mEraserHUD->FillCircle( iPointInTexture.x, iPointInTexture.y );
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorEraserTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , uint64& oSignalFlags )
{
    double diameter = Radius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)Radius
                          , (int)iPointInTexture.y - (int)Radius
                          , (int)diameter
                          , (int)diameter };

    mEraserHUD->SetPosition( iPointInTexture.x, iPointInTexture.y );
/*
    if( rect.x < 0 ) rect.x = 0;
    if( rect.y < 0 ) rect.y = 0;

    rect = rect & layerStack->GetSurface()->Block()->Rect();

    if( rect.Area() )
    {*/

    /*}*/
    // Calling Update via Root will request a redraw even if root is not invalidated
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorEraserTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , uint64& oSignalFlags )
{
    if( iPointInTexture.x < mMin.x ) mMin.x = iPointInTexture.x;
    if( iPointInTexture.y < mMin.y ) mMin.y = iPointInTexture.y;
    if( iPointInTexture.x > mMax.x ) mMax.x = iPointInTexture.x;
    if( iPointInTexture.y > mMax.y ) mMax.y = iPointInTexture.y;

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        mEraserHUD->SetPosition( iPointInTexture.x, iPointInTexture.y );
        mEraserHUD->StrokeLine( ::ULIS::FVec2D( iPointInTexture.x - iPointInTexture.deltaPosition.X
                                              , iPointInTexture.y - iPointInTexture.deltaPosition.Y )
                              , ::ULIS::FVec2D( iPointInTexture.x
                                              , iPointInTexture.y ) );
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorEraserTool::EraseSections( FOdysseyVectorGroupPaint* iScene
                                                    , const ::ULIS::FRectD& iErasureArea
                                                    , std::vector<FOdysseyVectorObject*>& oAddedObjectArray
                                                    , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                                    , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                                    , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray
                                                    , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                                    , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray )
{
    std::vector<FOdysseyVectorGroupPaint*> oPaintGroupArray;

    FOdysseyVectorObject::Traverse
    ( iScene
    , 0
    , [ this
      , iScene
      , &iErasureArea
      , &oPaintGroupArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          // build the list of impacted paintgroups, no duplicates
          if( iScene->GetCell()->ObjectHasFocus( object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
              {
                  FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

                  if( std::find ( oPaintGroupArray.begin()
                                , oPaintGroupArray.end()
                                , paintGroup ) == oPaintGroupArray.end() )
                  {
                      oPaintGroupArray.push_back( paintGroup );
                  }
              }

              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);
                  FOdysseyVectorObject* parent = path->GetParent();

                  if( parent->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
                  {
                      FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(parent);

                      if( std::find ( oPaintGroupArray.begin()
                                    , oPaintGroupArray.end()
                                    , paintGroup ) == oPaintGroupArray.end() )
                      {
                          oPaintGroupArray.push_back( paintGroup );
                      }
                  }
              }

              return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );

    // proceed
    for( FOdysseyVectorGroupPaint* paintGroup : oPaintGroupArray )
    {
        ::ULIS::FRectD paintGroupWorldBBox = paintGroup->GetBBox( false, true );

        if( FOdysseyVector::IntersectRegions<double>( paintGroupWorldBBox
                                                    , iErasureArea
                                                    , nullptr ) )
        {
            paintGroup->EraseSections( oAddedObjectArray
                                     , oAddedVertexArray
                                     , oAddedSegmentArray
                                     , oRemovedObjectArray
                                     , oRemovedVertexArray
                                     , oRemovedSegmentArray
                                     , SplitPath );
        }
    }

    for( int i = 0; i < oAddedObjectArray.size(); i++ )
    {
        // parent is stored in mParent variable byt the Erase function even though it does not
        // belong to the parent. It kinda sucks but it is easier that way, otherwise Traverse()
        // will have some problems because we change the children list, and Traverse is recursive.
        oAddedObjectArray[i]->GetParent()->AppendChild( oAddedObjectArray[i] );

        //oAddedObjectArray[i]->Invalidate();
    }

    // Also here, we remove AFTER the Traverse() has been executed, because traverse is recursive
    // so we can't alter the hierarchy, unles traverse works on copies of the children list but that
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
    iScene->GetLayer()->RequestRedraw( 0 );
}

void
UOdysseyPainterEditorVectorEraserTool::ErasePaths( FOdysseyVectorGroupPaint* iScene
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
                                     , false
                                     , SplitPath ) )
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

        //oAddedObjectArray[i]->Invalidate();
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
    iScene->GetLayer()->RequestRedraw( 0 );
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey
                                                      , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    ::ULIS::FRectD erasureArea = ::ULIS::FRectD::FromMinMax( mMin.x - Radius
                                                           , mMin.y - Radius
                                                           , mMax.x + Radius
                                                           , mMax.y + Radius );
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        std::vector<FOdysseyVectorObject*> addedObjectArray;
        std::vector<FOdysseyVectorVertex*> addedVertexArray;
        std::vector<FOdysseyVectorSegment*> addedSegmentArray;
        std::vector<FOdysseyVectorObject*> removedObjectArray;
        std::vector<FOdysseyVectorVertex*> removedVertexArray;
        std::vector<FOdysseyVectorSegment*> removedSegmentArray;
        ::ULIS::FRectD roi;

        mEraserHUD->BlendMask( false );
        // TODO: pass the mask image as arg to Pick function
        iScene->GetCell()->SetBLMask( mEraserHUD->GetMask() );

        if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
        {
            EraseSections( iScene
                         , erasureArea
                         , addedObjectArray
                         , addedVertexArray
                         , addedSegmentArray
                         , removedObjectArray
                         , removedVertexArray
                         , removedSegmentArray );
        }
        else
        {
            ErasePaths( iScene
                      , erasureArea
                      , addedObjectArray
                      , addedVertexArray
                      , addedSegmentArray
                      , removedVertexArray
                      , removedSegmentArray
                      , removedObjectArray );
        }

        iScene->GetCell()->SetBLMask( nullptr );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-eraser-tool.transaction.erase","Erase"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoErase( iScene
                                                                  , addedObjectArray
                                                                  , addedVertexArray
                                                                  , addedSegmentArray
                                                                  , removedObjectArray
                                                                  , removedVertexArray
                                                                  , removedSegmentArray
                                                                  , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
   }

    oSignalFlags = notificationFlags;

    return true;
}

void
UOdysseyPainterEditorVectorEraserTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    Super::ExtendToolbar(iBuilder);

    iBuilder.BeginSection( NAME_None );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorEraserTool, Radius ), FSinglePropertyParams())
            .InnerPadding(10.f)
            .ValueWidthOverride(100.f)
        ]
    );

    iBuilder.AddWidget(
        SNew(SBox)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorEraserTool, SplitPath ), FSinglePropertyParams())
            .InnerPadding(10.f)
        ]
    );

    iBuilder.EndSection();
}

FText
UOdysseyPainterEditorVectorEraserTool::GetTooltip() const
{
    return LOCTEXT("vector-eraser-tool.tooltip", "Eraser Tool");
}

#undef LOCTEXT_NAMESPACE
