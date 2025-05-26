// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "ISinglePropertyView.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "Shortcuts/Global/OdysseyPainterEditorGlobalShortcuts.h"
#include "PropertyHandle.h"
#include <chrono>

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterBaseTool::~UOdysseyPainterEditorRasterBaseTool()
{
}

UOdysseyPainterEditorRasterBaseTool::UOdysseyPainterEditorRasterBaseTool()
    : mHasContextMenu( true )
{
}

void
UOdysseyPainterEditorRasterBaseTool::ExtendMenu(TSharedRef<FExtender> iExtender)
{
    TSharedPtr<FUICommandList> commandList = MakeShared<FUICommandList>();
    iExtender->AddMenuExtension(
        "OdysseyEdit",
        EExtensionHook::After,
        commandList,
        FMenuExtensionDelegate::CreateLambda(
            [this](FMenuBuilder& iBuilder)
            {
                if (!IsActivated())
                    return;

                iBuilder.BeginSection("ToolOptions", LOCTEXT("raster-base-tool.edit-menu.tool-options", "Tool Options"));
                {
                    ExtendMenu( iBuilder );
                }
                iBuilder.EndSection();
            }
        )
    );

}

//static
bool
UOdysseyPainterEditorRasterBaseTool::DoubleClicked()
{
    uint64 clickTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    static uint64 previousClickTime = 0;
    bool doubleClicked = ( ( clickTime - previousClickTime ) < 200 ) ? true : false;

    previousClickTime = clickTime;

    return doubleClicked;
}

TSharedPtr<SWidget> UOdysseyPainterEditorRasterBaseTool::CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle, const TSharedPtr<ISinglePropertyView> iView)
{
    if (!iPropertyHandle)
        return nullptr;

    TSharedRef<SWidget> nameWidget = iPropertyHandle->CreatePropertyNameWidget();
    TSharedRef<SWidget> valueWidget = iPropertyHandle->CreatePropertyValueWidget(false);

    iView->SetVisibility(EVisibility::Collapsed);

    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            //PATCH:
            iView.ToSharedRef()
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(0.f, 0.f, 3.f, 0.f)
        [
            nameWidget
        ]
        + SHorizontalBox::Slot()
        [
            valueWidget
        ];
}

bool
UOdysseyPainterEditorRasterBaseTool::OnKeyDown( const FKey& iKey )
{
    return UOdysseyPainterEditorTool::OnKeyDown( iKey );
}

bool
UOdysseyPainterEditorRasterBaseTool::OnKeyUp( const FKey& iKey )
{
    return UOdysseyPainterEditorTool::OnKeyUp( iKey );
}

bool
UOdysseyPainterEditorRasterBaseTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                , const FKey& iKey )
{
    return UOdysseyPainterEditorTool::OnMouseDown( iPointInTexture, iKey );
}

bool
UOdysseyPainterEditorRasterBaseTool::OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    bool ret = false;

    if (hasRaster)
    {
        TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRaster = mediaProvider.GetMedias<FOdysseyMediaRaster>();

        if (mediaRaster.Num())
        {
            if (iKey == EKeys::RightMouseButton)
            {
                if (mHasContextMenu)
                {
                    PopupContextMenu(iPointInTexture, iKey);
                    return true;
                }
            }
        }
    }

    return UOdysseyPainterEditorTool::OnMouseClick(iPointInTexture, iKey);
}

bool
UOdysseyPainterEditorRasterBaseTool::OnMouseUp(const FOdysseyPoint& iPointInTexture
    , const FKey& iKey)
{
    return UOdysseyPainterEditorTool::OnMouseUp(iPointInTexture, iKey);
}

void
UOdysseyPainterEditorRasterBaseTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{

}

void UOdysseyPainterEditorRasterBaseTool::PopupContextMenu(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    TSharedPtr<SWidget> contextMenu = CreateContextMenu(iPointInTexture, iKey);

    TSharedPtr<SWindow> window = FSlateApplication::Get().GetActiveTopLevelWindow();
    if (!window)
        return;

    FSlateApplication::Get().PushMenu( window.ToSharedRef(),
        FWidgetPath(),
        contextMenu.ToSharedRef(),
        FSlateApplication::Get().GetCursorPos(),
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
}

TSharedPtr<SWidget> UOdysseyPainterEditorRasterBaseTool::CreateContextMenu(const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    FMenuBuilder menu(true, nullptr);

    ExtendContextMenu(menu, iPointInTexture, iKey);

    return menu.MakeWidget();
}


void
UOdysseyPainterEditorRasterBaseTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{

}

void
UOdysseyPainterEditorRasterBaseTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    UOdysseyPainterEditorTool::PostEditChangeProperty( PropertyChangedEvent );
}

void
UOdysseyPainterEditorRasterBaseTool::BuildSelectionMenu(FMenuBuilder& iMenu)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    if (!mediaProvider.HasMedia<FOdysseyMediaRaster>())
        return;

    iMenu.AddMenuEntry(
        LOCTEXT("raster-tool.object-context-menu.select-all.name", "Select All")
        , LOCTEXT("raster-tool.object-context-menu.select-all.tooltip", "Select All")
        , FSlateIcon()
        , FUIAction(
            FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::SelectAll)
            , FCanExecuteAction::CreateLambda([this]() { return mEditor != nullptr; }))
    );

    iMenu.AddMenuEntry(
        LOCTEXT("raster-tool.object-context-menu.clear-selection.name", "Clear Selection")
        , LOCTEXT("raster-tool.object-context-menu.clear-selection.tooltip", "Clear Selection")
        , FSlateIcon()
        , FUIAction(
            FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::ClearSelection)
            , FCanExecuteAction::CreateLambda([this]() { return (mEditor && !mEditor->RasterSelection()->IsEmpty()); }))
    );

    iMenu.AddMenuEntry(
        LOCTEXT("raster-tool.object-context-menu.copy-selection.name", "Copy Selection")
        , LOCTEXT("raster-tool.object-context-menu.copy-selection.tooltip", "Copy Selection")
        , FSlateIcon()
        , FUIAction(
            FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::CopySelection)
            , FCanExecuteAction::CreateLambda([this]() { return (mEditor && !mEditor->RasterSelection()->IsEmpty()); }))
    );

    iMenu.AddMenuEntry(
        LOCTEXT("raster-tool.object-context-menu.cut-selection.name", "Cut Selection")
        , LOCTEXT("raster-tool.object-context-menu.cut-selection.tooltip", "Cut Selection")
        , FSlateIcon()
        , FUIAction(
            FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::CutSelection)
            , FCanExecuteAction::CreateLambda([this]() { return (mEditor && !mEditor->RasterSelection()->IsEmpty()); }))
    );

    iMenu.AddMenuEntry(
        LOCTEXT("raster-tool.object-context-menu.paste-selection.name", "Paste Selection")
        , LOCTEXT("raster-tool.object-context-menu.paste-selection.tooltip", "Paste Selection")
        , FSlateIcon()
        , FUIAction(
            FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::PasteSelection)
            , FCanExecuteAction::CreateLambda([this]() { return (mEditor && mEditor->HasCopyBlockClipboard()); }))
    );


    iMenu.AddMenuEntry(
        LOCTEXT("raster-tool.object-context-menu.paste-selection-in-new-layer.name", "Paste Selection In New Layer")
        , LOCTEXT("raster-tool.object-context-menu.paste-selection-in-new-layer.tooltip", "Paste Selection In New Layer")
        , FSlateIcon()
        , FUIAction(
            FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::PasteSelectionInNewLayer)
            , FCanExecuteAction::CreateLambda([this]() { return (mEditor && mEditor->HasCopyBlockClipboard()); }))
    );

    iMenu.AddMenuEntry(
        LOCTEXT("raster-tool.object-context-menu.invert-selection.name", "Invert Selection")
        , LOCTEXT("raster-tool.object-context-menu.invert-selection.tooltip", "Invert Selection")
        , FSlateIcon()
        , FUIAction(
            FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::InvertSelection)
            , FCanExecuteAction::CreateLambda([this]() { return (mEditor && !mEditor->RasterSelection()->IsEmpty()); }))
    );
}

void UOdysseyPainterEditorRasterBaseTool::ExtendContextMenu(FMenuBuilder& iMenu, const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    iMenu.BeginSection( "RasterSelection", LOCTEXT("raster-tool.context-menu.raster-selection-section.name", "Selection") );
        BuildSelectionMenu(iMenu);
    iMenu.EndSection();
}

void UOdysseyPainterEditorRasterBaseTool::ExtendMenu(FMenuBuilder& iMenu)
{
    BuildSelectionMenu(iMenu);
}


void UOdysseyPainterEditorRasterBaseTool::BindShortcuts(FBaseToolkit* iToolkit)
{

}

void UOdysseyPainterEditorRasterBaseTool::SelectAll()
{
    if (mEditor)
    {
        TArray<FVector2D> polyPoints;
        polyPoints.Add(FVector2D(0, 0));
        polyPoints.Add(FVector2D(mEditor->RasterSelection()->GetBlock()->Width(), 0));
        polyPoints.Add(FVector2D(mEditor->RasterSelection()->GetBlock()->Width(), mEditor->RasterSelection()->GetBlock()->Height()));
        polyPoints.Add(FVector2D(0, mEditor->RasterSelection()->GetBlock()->Height()));

        mEditor->RasterSelection()->Add(polyPoints);
    }
}

void UOdysseyPainterEditorRasterBaseTool::CopySelection()
{
    TSharedPtr<FOdysseyPainterEditorGlobalShortcuts> shortcuts = MakeShared<FOdysseyPainterEditorGlobalShortcuts>(mEditor);
    if (shortcuts->CanAction_CopyCurrentSelection())
        shortcuts->Action_CopyCurrentSelection();
}

void UOdysseyPainterEditorRasterBaseTool::CutSelection()
{
    TSharedPtr<FOdysseyPainterEditorGlobalShortcuts> shortcuts = MakeShared<FOdysseyPainterEditorGlobalShortcuts>(mEditor);
    if (shortcuts->CanAction_CutCurrentSelection())
        shortcuts->Action_CutCurrentSelection();
}

void UOdysseyPainterEditorRasterBaseTool::PasteSelection()
{
    TSharedPtr<FOdysseyPainterEditorGlobalShortcuts> shortcuts = MakeShared<FOdysseyPainterEditorGlobalShortcuts>(mEditor);
    if (shortcuts->CanAction_PasteCurrentSelection())
        shortcuts->Action_PasteCurrentSelection();
}

void UOdysseyPainterEditorRasterBaseTool::PasteSelectionInNewLayer()
{
    TSharedPtr<FOdysseyPainterEditorGlobalShortcuts> shortcuts = MakeShared<FOdysseyPainterEditorGlobalShortcuts>(mEditor);
    if (shortcuts->CanAction_PasteCurrentSelectionInNewLayer())
        shortcuts->Action_PasteCurrentSelectionInNewLayer();
}

void UOdysseyPainterEditorRasterBaseTool::ClearSelection()
{
    if (mEditor)
        mEditor->RasterSelection()->Clear();
}

void UOdysseyPainterEditorRasterBaseTool::InvertSelection()
{
    if (mEditor)
        mEditor->RasterSelection()->Invert();
}


#undef LOCTEXT_NAMESPACE
