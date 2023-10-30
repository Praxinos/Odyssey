// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "Widgets/Tools/SOdysseyPainterEditorVectorEditionMode.h"
#include "Widgets/Layout/SWrapBox.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorBaseTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorBaseTool::~UOdysseyPainterEditorVectorBaseTool()
{
}

UOdysseyPainterEditorVectorBaseTool::UOdysseyPainterEditorVectorBaseTool()
    : mHasContextMenu( true )
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
UOdysseyPainterEditorVectorBaseTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        //Should be done in UnloadVector directly
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = UnloadVector( vectorScene );

            vectorEngine->Signal( signalFlags );
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
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = LoadVector( vectorScene );

            vectorEngine->Signal( signalFlags );
        }
    }
}

uint64
UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( FOdysseyVectorScene* iScene
                                                    , const FKey& iKey )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        if( iKey == EKeys::C )
        {
            FOdysseyPainterEditor::CopyObjectSelection( iScene );
        }

        if( iKey == EKeys::V )
        {
            FOdysseyPainterEditor::PasteObjectSelection( iScene );
        }

        if( iKey == EKeys::A )
        {
            GetEditor()->SelectAll( iScene );
        }
    }

    if( iKey == EKeys::Delete )
    {
        if( GetEditor()->GetVectorEditionFlags() & FOdysseyVectorHUD::VIEW_MODE_OBJECT )
        {
            GetEditor()->DeleteObjectSelection( iScene );
        }

        if( GetEditor()->GetVectorEditionFlags() & FOdysseyVectorHUD::VIEW_MODE_VERTEX )
        {
UE_LOG(LogTemp, Warning, TEXT("UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector to refactor") );
            GetEditor()->DeletePointSelection( iScene, nullptr );
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
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnKeyDownVector( vectorScene, iKey );

            vectorEngine->Signal( signalFlags );
        }
    }

    return false;
}

uint64
UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( FOdysseyVectorScene* iScene
                                                  , const FKey& iKey )
{
    return 0;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyUp( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
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

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnMouseDownVector( vectorScene, iPointInTexture, iKey );

            vectorEngine->Signal( signalFlags );
        }
    }

    return false;
}

void
UOdysseyPainterEditorVectorBaseTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
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

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
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

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
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
    }

    return false;
}

uint64
UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( FOdysseyVectorScene* iScene
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
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
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
    if( GetEditor()->GetVectorEditionFlags() &  FOdysseyVectorHUD::VIEW_MODE_OBJECT )
    {
        ExtendContextMenuObject( menu );
    }

    if( GetEditor()->GetVectorEditionFlags() &  FOdysseyVectorHUD::VIEW_MODE_VERTEX )
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
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

            std::list<FOdysseyVectorObject*>* focusedObjectList = nullptr;

UE_LOG(LogTemp, Warning, TEXT("UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuObject to refactor") );
        // Commented-out: sections are not needed here as they would conflict with the section
        // just created by the Edit Menu when this tool's menu appears in the Edit Menu
        // See FOdysseyPainterEditor::AddEditMenuEntry() for details
        //    menu.BeginSection("Context");
        //    {
                menu.AddMenuEntry(
                      LOCTEXT("ResetView", "Reset View")
                    , LOCTEXT("ResetView", "Reset View")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::ResetView, vectorScene)));
                menu.AddMenuEntry(
                      LOCTEXT("GroupPaint", "Make Paint Group")
                    , LOCTEXT("GroupPaint", "Make Paint Group")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::GroupPaint, vectorScene)));
                /*menu.AddMenuEntry(
                      LOCTEXT("Trim", "Trim")
                    , LOCTEXT("Trim", "Trim")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Trim, vectorScene)));*/
                menu.AddMenuEntry(
                      LOCTEXT("Group", "Group")
                    , LOCTEXT("Group", "Group")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Group, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("Ungroup", "Ungroup")
                    , LOCTEXT("Ungroup", "Ungroup")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Ungroup, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("BringForward", "Bring forward")
                    , LOCTEXT("BringForward", "Bring forward")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::BringForward, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("SendBackward", "Send backward")
                    , LOCTEXT("SendBackward", "Send backward")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::SendBackward, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("DeleteSelection","Delete Selection")
                    , LOCTEXT("DeleteSelection","Delete Selection")
                    , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteObjectSelection, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("FlipHorizontal","Flip Horizontal")
                    , LOCTEXT("FlipHorizontal","Flip Horizontal")
                    , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::FlipHorizontal, vectorScene, focusedObjectList )));
                menu.AddMenuEntry(
                      LOCTEXT("FlipVertical","Flip Vertical")
                    , LOCTEXT("FlipVertical","Flip Vertical")
                    , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::FlipVertical, vectorScene, focusedObjectList )));
                menu.AddMenuEntry(
                    LOCTEXT("ClearColoring", "Clear Coloring")
                    , LOCTEXT("ClearColoring", "Clear Coloring")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::ClearColoring, vectorScene, focusedObjectList )));
                menu.AddMenuEntry(
                    LOCTEXT("ApplyTransformations", "Apply Transformations")
                    , LOCTEXT("ApplyTransformations", "Apply Transformations")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::ApplyTransformations, vectorScene, focusedObjectList )));
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
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            std::list<FOdysseyVectorObject*>* focusedObjectList = /*&GetFocusedObjectList( vectorScene )*/nullptr;
UE_LOG(LogTemp, Warning, TEXT("UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuVertex to refactor") );

        // Commented-out: sections are not needed here as they would conflict with the section
        // just created by the Edit Menu when this tool's menu appears in the Edit Menu
        // See FOdysseyPainterEditor::AddEditMenuEntry() for details
        //     menu.BeginSection("Context");
        //     {
            menu.AddMenuEntry(
                  LOCTEXT("DeleteSelection", "Delete Selection")
                , LOCTEXT("DeleteSelection", "Delete Selection")
                , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeletePointSelection, vectorScene, focusedObjectList )));
            menu.AddMenuEntry(
                  LOCTEXT("AlignPointSelection", "Align Point Selection")
                , LOCTEXT("AlignPointSelection", "Align Point Selection")
                , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::AlignPointSelection, vectorScene, focusedObjectList )));
            menu.AddMenuEntry(
                  LOCTEXT("UnalignPointSelection", "Unalign Point Selection")
                , LOCTEXT("UnalignPointSelection", "Unalign Point Selection")
                , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::UnalignPointSelection, vectorScene, focusedObjectList )));
        //    }
        //    menu.EndSection();
        }
    }

    //return menu.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
