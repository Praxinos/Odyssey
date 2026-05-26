// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"

#include "Editor.h"
#include "ISinglePropertyView.h"
#include "Styling/StyleColors.h"
#include "ToolMenu.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "Widgets/Layout/SBox.h"

#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserToolHUD.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyVector.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyPainterEditorSource.h"
#include "Undo/OdysseyVectorUndoErase.h"
#include "SOdysseySinglePropertyView.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorEraserTool::~UOdysseyPainterEditorVectorEraserTool()
{
}

UOdysseyPainterEditorVectorEraserTool::UOdysseyPainterEditorVectorEraserTool()
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorEraserToolHUD>( this ), false, true )
    , mEditionMode( eVectorEraserEditionMode::Default )
    , SplitPath( true )
    , Radius( 20.0f )
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.Eraser64"));

    mEraserHUD = static_cast<FOdysseyPainterEditorVectorEraserToolHUD*>( mBaseHUD.Get() );
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

bool
UOdysseyPainterEditorVectorEraserTool::HasRadius() const
{
    return true;
}

void
UOdysseyPainterEditorVectorEraserTool::SetRadius(float iRadius)
{
    Radius = iRadius;
}

float
UOdysseyPainterEditorVectorEraserTool::GetRadius() const
{
    return Radius;
}

uint64
UOdysseyPainterEditorVectorEraserTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
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
UOdysseyPainterEditorVectorEraserTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorEraserTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FKeyEvent& InKeyEvent )
{
    if( InKeyEvent.IsRepeat() == false )
    {
        FKey key = InKeyEvent.GetKey();

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftShift ) || ( key == EKeys::RightShift ) )
        {
            mEditionMode  = eVectorEraserEditionMode::Section;

            return true;
        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftAlt ) || ( key == EKeys::RightAlt ) )
        {
            mEditionMode  = eVectorEraserEditionMode::Path;

            return true;
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorEraserTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FKeyEvent& InKeyEvent )
{
    // first reset display mode
    mEditionMode = eVectorEraserEditionMode::Default;

    return false;
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();

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
                                                         , const FOdysseyPoint& iPointInTexture )
{
    mEraserHUD->SetPosition( iPointInTexture.x, iPointInTexture.y );
}

void
UOdysseyPainterEditorVectorEraserTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture )
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
        ::ULIS::FRectD paintGroupWorldBBox = paintGroup->GetBBox( true, true );

        if( FOdysseyVector::IntersectRegions<double>( paintGroupWorldBBox
                                                    , iErasureArea
                                                    , nullptr ) )
        {
            // Note: this piece of code only works in the "texture coordinates system".
            // The picking system used in the selection tool e.g works in the Viewport coordinates system
            // and should be the one used here.  Thus the code below should be refactored to use a
            // "viewport mask" to be able to erase objects outside the canvas
            paintGroup->EraseSections( oAddedObjectArray
                                     , oAddedVertexArray
                                     , oAddedSegmentArray
                                     , oRemovedObjectArray
                                     , oRemovedVertexArray
                                     , oRemovedSegmentArray
                                     , *mEraserHUD->GetMask()
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

    iScene->GetCell()->InvalidateRect(); // force refreshing the whole screen to clear the erasure mask

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
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
                      if( mEditionMode == eVectorEraserEditionMode::Default )
                      {
                          // Note: this piece of code only works in the "texture coordinates system".
                          // The picking system used in the selection tool e.g works in the Viewport coordinates system
                          // and should be the one used here.  Thus the code below should be refactored to use a
                          // "viewport mask" to be able to erase objects outside the canvas
                          if( path->Erase( oAddedObjectArray
                                         , oAddedVertexArray
                                         , oAddedSegmentArray
                                         , oRemovedVertexArray
                                         , oRemovedSegmentArray
                                         , *mEraserHUD->GetMask()
                                         , false
                                         , SplitPath ) )
                          {
                              oRemovedObjectArray.push_back( path );
                          }
                      }

                      if( mEditionMode == eVectorEraserEditionMode::Path )
                      {
                          // Note: this piece of code only works in the "texture coordinates system".
                          // The picking system used in the selection tool e.g works in the Viewport coordinates system
                          // and should be the one used here.  Thus the code below should be refactored to use a
                          // "viewport mask" to be able to erase objects outside the canvas
                          if( path->Pick( iScene
                                        , iErasureArea
                                        , *mEraserHUD->GetMask() ) )
                          {
                              oRemovedObjectArray.push_back( path );
                          }
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

    iScene->GetCell()->InvalidateRect(); // force refreshing the whole screen to clear the erasure mask

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    ::ULIS::FRectD erasureArea = ::ULIS::FRectD::FromMinMax( mMin.x - Radius
                                                           , mMin.y - Radius
                                                           , mMax.x + Radius
                                                           , mMax.y + Radius );

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
/*
        // TODO: pass the mask image as arg to Pick function
        iScene->GetCell()->SetBLMask( mEraserHUD->GetMask() );
*/

        if ( ( mEditionMode == eVectorEraserEditionMode::Default )
          || ( mEditionMode == eVectorEraserEditionMode::Path    ) )
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

        if ( mEditionMode == eVectorEraserEditionMode::Section )
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
                                                                  , removedSegmentArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
    }

    return true;
}

void
UOdysseyPainterEditorVectorEraserTool::SetEditionMode( eVectorEraserEditionMode iMode )
{
    mEditionMode = iMode;
}

const FSlateBrush*
UOdysseyPainterEditorVectorEraserTool::GetBackgroundColor( eVectorEraserEditionMode iMode ) const
{
    static FSlateColorBrush selected = FSlateColorBrush( FStyleColors::Select );

    return ( iMode == mEditionMode ) ? &selected : nullptr;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorEraserTool::CreateModifierSegmentControl()
{
    return SNew(SSegmentedControl<eVectorEraserEditionMode>)
           .Value_Lambda( [this]{ return mEditionMode; } )
           .SupportsEmptySelection( false )
           .SupportsMultiSelection( false )
           .IsEnabled( false ) // currently not clickable - Info only
           .UniformPadding( FMargin( 2, 0, 2, 0 ) )
           .OnValueChanged( SSegmentedControl<eVectorEraserEditionMode>::FOnValueChanged::CreateUObject( this, &UOdysseyPainterEditorVectorEraserTool::SetEditionMode ) )
           // DEFAULT
           + SSegmentedControl<eVectorEraserEditionMode>::Slot( eVectorEraserEditionMode::Default )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.EraserEraseDefault20") )
           .ToolTip( LOCTEXT("vector-eraser-tool.erasure-mode.default.name", "Default") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorEraserTool::GetBackgroundColor, eVectorEraserEditionMode::Default  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.EraserEraseDefault20") )
               ]
           ]
           // SHIFT
           + SSegmentedControl<eVectorEraserEditionMode>::Slot( eVectorEraserEditionMode::Section )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.EraserEraseToIntersection20") )
           .ToolTip( LOCTEXT("vector-eraser-tool.erasure-mode.shift.name", "Erase to intersection (SHIFT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorEraserTool::GetBackgroundColor, eVectorEraserEditionMode::Section  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.EraserEraseToIntersection20") )
               ]
           ]
           // ALT
           + SSegmentedControl<eVectorEraserEditionMode>::Slot( eVectorEraserEditionMode::Path )
           //.Icon( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.EraserEraseWholePath20") )
           .ToolTip( LOCTEXT("vector-eraser-tool.erasure-mode.alt.name", "Erase whole path (ALT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UOdysseyPainterEditorVectorEraserTool::GetBackgroundColor, eVectorEraserEditionMode::Path  )
               [
                   SNew(SImage)
                   .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsShortcuts.EraserEraseWholePath20") )
               ]
           ];
}

void
UOdysseyPainterEditorVectorEraserTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            CreateModifierSegmentControl(),
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorEraserTool, Radius ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorEraserTool, SplitPath ), FSinglePropertyParams())
                .InnerPadding(10.f)
            ],
            FText()
        )
    );
}

FText
UOdysseyPainterEditorVectorEraserTool::GetTooltip() const
{
    return LOCTEXT("vector-eraser-tool.tooltip", "Eraser Tool");
}

#undef LOCTEXT_NAMESPACE
