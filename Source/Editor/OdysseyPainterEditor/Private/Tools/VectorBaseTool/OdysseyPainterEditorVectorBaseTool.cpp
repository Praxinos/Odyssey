// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Widgets/Tools/SOdysseyPainterEditorVectorEditionMode.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Framework/Commands/GenericCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorBaseTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorBaseTool::~UOdysseyPainterEditorVectorBaseTool()
{
    if( mBaseHUD )
    {
        delete mBaseHUD;
    }
}

UOdysseyPainterEditorVectorBaseTool::UOdysseyPainterEditorVectorBaseTool()
    : mBaseHUD( nullptr )
    , mHasContextMenu( true )
    // to prevent a double mouse down bug detected in
    // FOdysseyPainterEditorViewportClient::InputKey
    // FOdysseyPainterEditorViewportClient::OnStylusStateChanged
    // they sometimes are both called and both trigger 
    // FOdysseyPainterEditorViewportClient::InputKeyWithStrokePoint
    , mDoubleMouseDown_WorkAround( false )
{
}

UOdysseyPainterEditorVectorBaseTool::UOdysseyPainterEditorVectorBaseTool( FOdysseyPainterEditorVectorBaseToolHUD* iBaseHUD )
    : mBaseHUD( iBaseHUD )
    , mHasContextMenu(true)
{
}

//static
bool
UOdysseyPainterEditorVectorBaseTool::DoubleClicked()
{
    uint64 clickTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    static uint64 previousClickTime = 0;
    bool doubleClicked = ( ( clickTime - previousClickTime ) < 200 ) ? true : false;

    previousClickTime = clickTime;

    return doubleClicked;
}

void
UOdysseyPainterEditorVectorBaseTool::GetSelectedVertices( FOdysseyVectorGroupPaint* iScene
                                                        , std::vector<FOdysseyVectorVertex*>& oSelectedVertexArray )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    oSelectedVertexArray.clear();

    vectorEngine->Traverse
    ( iScene
    , iScene
    , 0
    , [ this
      , iScene
      , vectorEngine
      , &oSelectedVertexArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);
 
                  path->GetSelectedVertices( oSelectedVertexArray );
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );
}

bool
UOdysseyPainterEditorVectorBaseTool::IsDragging()
{
    return mDragging;
}

void
UOdysseyPainterEditorVectorBaseTool::GetSegmentHandlesFromVertices( const std::vector<FOdysseyVectorVertex*>& iVertexArray
                                                                  , std::vector<FOdysseyVectorHandleSegment*>& oSegmentHandleArray )
{
    for( FOdysseyVectorVertex* vertex : iVertexArray )
    {
        for( FOdysseyVectorSegment* segment : vertex->GetSegmentList() )
        {
            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorHandleSegment* handle = segment->GetHandle( vertex );

                oSegmentHandleArray.push_back( handle );
            }
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        //Should be done in UnloadVector directly
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = UnloadVector( vectorScene );

            vectorEngine->Signal( signalFlags );

            if( mBaseHUD )
            {
                mBaseHUD->Unload( vectorScene );
                vectorEngine->RemoveHUD( mBaseHUD );
            }
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            vectorEngine->ClearHUD();

            if( mBaseHUD )
            {
                mBaseHUD->Load( vectorScene );

                vectorEngine->AddHUD( mBaseHUD );
                vectorEngine->ResetHUD();
            }

            signalFlags = LoadVector( vectorScene );

            vectorEngine->Signal( signalFlags );
        }
    }
}

uint64
UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                    , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
/*
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        if( iKey == EKeys::C )
        {
            FOdysseyPainterEditor::CopyObjects( iScene );
        }

        if( iKey == EKeys::V )
        {
            FOdysseyPainterEditor::PasteObjects( iScene );
        }

        if( iKey == EKeys::A )
        {
            GetEditor()->SelectAll( iScene );
        }
    }
*/
    if( iKey == EKeys::W )
    {
/*
        uint64 drawingflags = iEngine->GetDrawingFlags();

        iEngine->SetDrawingFlags( drawingflags | FOdysseyVectorEngine::DRAWING_WIREFRAME );
*/
    }

    if( iKey == EKeys::Delete )
    {
        if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
        {
            GetEditor()->DeleteObjects( iScene );
        }

        if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            GetEditor()->DeletePointSelection( iScene );
        }
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyDown( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    bool ret = false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnKeyDownVector( vectorScene, iKey );

            vectorEngine->Signal( signalFlags );
        }
    }

    return false;
}

uint64
UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                  , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
/*
    uint64 drawingflags = iEngine->GetDrawingFlags();

    iEngine->SetDrawingFlags( drawingflags & (~FOdysseyVectorEngine::DRAWING_WIREFRAME) );
*/

    return 0;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyUp( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnKeyUpVector(vectorScene,iKey);

            vectorEngine->Signal( signalFlags );
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                , const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    mDragging = false;

  if( mDoubleMouseDown_WorkAround == false ) // workaround
  {                                          // workaround
    mDoubleMouseDown_WorkAround = true;      // workaround

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnMouseDownVector( vectorScene, iPointInTexture, iKey );

            vectorEngine->Signal( signalFlags );
        }

       return true;
    }
  }  // workaround

    return false;
}

void
UOdysseyPainterEditorVectorBaseTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnMouseHoverVector( vectorScene, iPointInTexture );

            vectorEngine->Signal( signalFlags );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    mDragging = true;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnMouseDragVector( vectorScene, iPointInTexture );

            vectorEngine->Signal( signalFlags );
        }
    }
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                              , const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    bool ret = false;

    mDoubleMouseDown_WorkAround = false; // workaround

    mDragging = false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnMouseUpVector( vectorScene, iPointInTexture, iKey );

            vectorEngine->Signal( signalFlags );

            if( iKey == EKeys::RightMouseButton )
            {
                if( mHasContextMenu )
                {
                    PopupContextMenu();
                }
            }
        }

       return true;
    }

    return false;
}

uint64
UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FName& iPropertyName )
{
    // RestrictToSelection was changed, return redraw flag
    if( iPropertyName == "RestrictToSelectedObjects" )
    {
        iScene->GetEngine()->ResetHUD();
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

void
UOdysseyPainterEditorVectorBaseTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    // redraw
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if (hasVector)
    {
        //Should be done in OnKeyUpVector directly
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = PropertyChangedVector( vectorScene, PropertyChangedEvent.GetPropertyName() );

            vectorEngine->Signal( signalFlags );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Commit()
{

}

void
UOdysseyPainterEditorVectorBaseTool::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();

    Super::BindShortcuts(iToolkit);

    toolkitCommands->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::SelectAll )
    );
/*
    toolkitCommands->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::Delete )
    );
*/
    toolkitCommands->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::Copy )
    );

    toolkitCommands->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::Paste )
    );
}

void
UOdysseyPainterEditorVectorBaseTool::Copy()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            FOdysseyPainterEditor::CopyObjects( vectorScene );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Paste()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            FOdysseyPainterEditor::PasteObjects( vectorScene );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::SelectAll()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            //FOdysseyPainterEditor::SelectAllObjects( vectorScene );
        }
    }
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorBaseTool::CreateTopTabWidget()
{
    return SNew(SWrapBox)
           .InnerSlotPadding(FVector2D(10.f, 3.f))
           .UseAllottedSize(true)
           .HAlign(HAlign_Fill)
           + SWrapBox::Slot()
           .HAlign(HAlign_Fill)
           [
               SNew( SOdysseyPainterEditorVectorEditionMode, GetEditor() )
           ];
}

void
UOdysseyPainterEditorVectorBaseTool::PopupContextMenu()
{
    TSharedPtr<SWidget> contextMenu = CreateContextMenu();

    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
    FSlateApplication::Get().PushMenu( viewportTab->Widget().ToSharedRef(),
                                       FWidgetPath(),
                                       contextMenu.ToSharedRef(),
                                       FSlateApplication::Get().GetCursorPos(),
                                       FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu) );
}

TSharedPtr<SWidget>
UOdysseyPainterEditorVectorBaseTool::CreateContextMenu()
{
    FMenuBuilder menu( true, nullptr );

    ExtendContextMenu( menu );

    return menu.MakeWidget();
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenu( FMenuBuilder& menu )
{
    if( GetEditor()->GetVectorHUDFlags() &  FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        ExtendContextMenuObject( menu );
    }

    if( GetEditor()->GetVectorHUDFlags() &  FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        ExtendContextMenuVertex( menu );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuObject( FMenuBuilder& menu )
{
    //FMenuBuilder menu( true, nullptr );
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

        // Commented-out: sections are not needed here as they would conflict with the section
        // just created by the Edit Menu when this tool's menu appears in the Edit Menu
        // See FOdysseyPainterEditor::AddEditMenuEntry() for details
        //    menu.BeginSection("Context");
        //    {
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.reset-view.name", "Reset View")
                    , LOCTEXT("vector-tool.object-context-menu.reset-view.tooltip", "Reset View")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::ResetView, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.group-paint.name", "Make Paint Group")
                    , LOCTEXT("vector-tool.object-context-menu.group-paint.tooltip", "Make Paint Group")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::MakePaintGroup, vectorScene )));
                /*menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.trim.name", "Trim")
                    , LOCTEXT("vector-tool.object-context-menu.trim.tooltip", "Trim")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Trim, vectorScene)));*/
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.group.name", "Group")
                    , LOCTEXT("vector-tool.object-context-menu.group.tooltip", "Group")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Group, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.ungroup.name", "Ungroup")
                    , LOCTEXT("vector-tool.object-context-menu.ungroup.tooltip", "Ungroup")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Ungroup, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.bring-forward.name", "Bring forward")
                    , LOCTEXT("vector-tool.object-context-menu.bring-forward.tooltip", "Bring forward")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::BringForward, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.send-backward.name", "Send backward")
                    , LOCTEXT("vector-tool.object-context-menu.send-backward.tooltip", "Send backward")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::SendBackward, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.delete-selection.name","Delete Selection")
                    , LOCTEXT("vector-tool.object-context-menu.delete-selection.tooltip","Delete Selection")
                    , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteObjects, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.flip-horizontal.name","Flip Horizontal")
                    , LOCTEXT("vector-tool.object-context-menu.flip-horizontal.tooltip","Flip Horizontal")
                    , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::FlipHorizontal, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.flip-vertical.name","Flip Vertical")
                    , LOCTEXT("vector-tool.object-context-menu.flip-vertical.tooltip","Flip Vertical")
                    , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::FlipVertical, vectorScene )));
                menu.AddMenuEntry(
                    LOCTEXT("vector-tool.object-context-menu.clear-coloring.name", "Clear Coloring")
                    , LOCTEXT("vector-tool.object-context-menu.clear-coloring.tooltip", "Clear Coloring")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ClearColoring, vectorScene )));
                menu.AddMenuEntry(
                    LOCTEXT("vector-tool.object-context-menu.apply-transformations.name", "Apply Transformations")
                    , LOCTEXT("vector-tool.object-context-menu.apply-transformations.tooltip", "Apply Transformations")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ApplyTransformations, vectorScene )));
        //    }
        //    menu.EndSection();
        }
    }

    //return menu.MakeWidget();
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuVertex( FMenuBuilder& menu )
{
    //FMenuBuilder menu( true, nullptr );
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

        // Commented-out: sections are not needed here as they would conflict with the section
        // just created by the Edit Menu when this tool's menu appears in the Edit Menu
        // See FOdysseyPainterEditor::AddEditMenuEntry() for details
        //     menu.BeginSection("Context");
        //     {
            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.vertex-context-menu.delete-selection.name", "Delete Selection")
                , LOCTEXT("vector-tool.vertex-context-menu.delete-selection.tooltip", "Delete Selection")
                , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::DeletePointSelection, vectorScene )));
            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.vertex-context-menu.align-point-selection.name", "Align Point Selection")
                , LOCTEXT("vector-tool.vertex-context-menu.align-point-selection.tooltip", "Align Point Selection")
                , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::AlignPointSelection, vectorScene )));
            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.vertex-context-menu.unalign-point-selection.name", "Unalign Point Selection")
                , LOCTEXT("vector-tool.vertex-context-menu.unalign-point-selection.tooltip", "Unalign Point Selection")
                , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::UnalignPointSelection, vectorScene )));
        //    }
        //    menu.EndSection();
        }
    }

    //return menu.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
