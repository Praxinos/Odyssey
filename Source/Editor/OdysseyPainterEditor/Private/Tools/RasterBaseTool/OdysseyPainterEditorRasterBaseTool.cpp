// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "ISinglePropertyView.h"
#include "OdysseyMediaRaster.h"
#include "PainterEditor/OdysseyPainterEditorRasterSelection.h"

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

TSharedRef<SWidget> UOdysseyPainterEditorRasterBaseTool::CreateTopTabWidget()
{
    return UOdysseyPainterEditorTool::CreateTopTabWidget();
}


TSharedPtr<SWidget> UOdysseyPainterEditorRasterBaseTool::CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle, const TSharedPtr<ISinglePropertyView> iView)
{
    return SNullWidget::NullWidget;
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
                    PopupContextMenu();
                    return true;
                }
            }
        }
    }

    return UOdysseyPainterEditorTool::OnMouseDown( iPointInTexture, iKey );
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

void UOdysseyPainterEditorRasterBaseTool::PopupContextMenu()
{
    TSharedPtr<SWidget> contextMenu = CreateContextMenu();

    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
    FSlateApplication::Get().PushMenu(viewportTab->Widget().ToSharedRef(),
        FWidgetPath(),
        contextMenu.ToSharedRef(),
        FSlateApplication::Get().GetCursorPos(),
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
}

TSharedPtr<SWidget> UOdysseyPainterEditorRasterBaseTool::CreateContextMenu()
{
    FMenuBuilder menu(true, nullptr);

    ExtendContextMenu(menu);

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


void UOdysseyPainterEditorRasterBaseTool::ExtendContextMenu(FMenuBuilder& menu)
{
    //FMenuBuilder menu( true, nullptr );
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();

    if (hasRaster)
    {
        menu.AddMenuEntry(
            LOCTEXT("raster-tool.object-context-menu.select-all.name", "Select All")
            , LOCTEXT("raster-tool.object-context-menu.select-all.tooltip", "Select All")
            , FSlateIcon()
            , FUIAction());

        menu.AddMenuEntry(
            LOCTEXT("raster-tool.object-context-menu.copy-selection.name", "Copy Selection")
            , LOCTEXT("raster-tool.object-context-menu.copy-selection.tooltip", "Copy Selection")
            , FSlateIcon()
            , FUIAction( 
                FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorRasterBaseTool::CopySelection )
              , FCanExecuteAction::CreateLambda([this]() { return (mEditor && !mEditor->RasterSelection()->IsEmpty()); }))
            );

        menu.AddMenuEntry(
            LOCTEXT("raster-tool.object-context-menu.cut-selection.name", "Cut Selection")
            , LOCTEXT("raster-tool.object-context-menu.cut-selection.tooltip", "Cut Selection")
            , FSlateIcon()
            , FUIAction(
                FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::CutSelection)
              , FCanExecuteAction::CreateLambda([this]() { return (mEditor && !mEditor->RasterSelection()->IsEmpty()); }))
        );

        menu.AddMenuEntry(
            LOCTEXT("raster-tool.object-context-menu.paste-selection.name", "Paste Selection")
            , LOCTEXT("raster-tool.object-context-menu.paste-selection.tooltip", "Paste Selection")
            , FSlateIcon()
            , FUIAction(
                FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::PasteSelection)
              , FCanExecuteAction::CreateLambda([this]() { return (mEditor && mEditor->HasCopyBlock()); }))
        );

        menu.AddMenuEntry(
            LOCTEXT("raster-tool.object-context-menu.clear-selection.name", "Clear Selection")
            , LOCTEXT("raster-tool.object-context-menu.clear-selection.tooltip", "Clear Selection")
            , FSlateIcon()
            , FUIAction(
                FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::ClearSelection)
                , FCanExecuteAction::CreateLambda([this]() { return (mEditor && !mEditor->RasterSelection()->IsEmpty()); }))
        );

        menu.AddMenuEntry(
            LOCTEXT("raster-tool.object-context-menu.paste-selection-in-current-layer.name", "Paste Selection In Current Layer")
            , LOCTEXT("raster-tool.object-context-menu.paste-selection-in-current-layer.tooltip", "Paste Selection In Current Layer")
            , FSlateIcon()
            , FUIAction());

        menu.AddMenuEntry(
            LOCTEXT("raster-tool.object-context-menu.invert-selection.name", "Invert Selection")
            , LOCTEXT("raster-tool.object-context-menu.invert-selection.tooltip", "Invert Selection")
            , FSlateIcon()
            , FUIAction());
    }
}


void UOdysseyPainterEditorRasterBaseTool::BindShortcuts(FBaseToolkit* iToolkit)
{

}


void UOdysseyPainterEditorRasterBaseTool::CopySelection()
{
    if( mEditor )
        mEditor->CopyCurrentSelectionToCopyBlock();
}

void UOdysseyPainterEditorRasterBaseTool::CutSelection()
{
    if (mEditor)
        mEditor->CutCurrentSelectionToCopyBlock();
}

void UOdysseyPainterEditorRasterBaseTool::PasteSelection()
{
    if (mEditor)
        mEditor->PasteCopiedBlockToNewLayer();
}

void UOdysseyPainterEditorRasterBaseTool::ClearSelection()
{
    if (mEditor)
        mEditor->RasterSelection()->Clear();
}


#undef LOCTEXT_NAMESPACE
