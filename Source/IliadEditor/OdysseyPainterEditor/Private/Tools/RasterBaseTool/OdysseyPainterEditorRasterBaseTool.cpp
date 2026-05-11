// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyBlockClipboardData.h"
#include "OdysseyCoreEditorModule.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "ISinglePropertyView.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "OdysseyRasterBlock.h"
#include "Shortcuts/Global/OdysseyPainterEditorGlobalShortcuts.h"
#include "PropertyHandle.h"
#include <chrono>

#include "ULISLoaderModule.h"


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
            FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::ClearCurrentSelection)
            , FCanExecuteAction::CreateLambda([this]() { return (mEditor && !mEditor->RasterSelection()->IsEmpty()); }))
    );

    iMenu.AddMenuEntry(
        LOCTEXT("raster-tool.object-context-menu.copy-selection.name", "Copy Selection")
        , LOCTEXT("raster-tool.object-context-menu.copy-selection.tooltip", "Copy Selection")
        , FSlateIcon()
        , FUIAction(
            FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::CopyCurrentSelection)
            , FCanExecuteAction::CreateLambda([this]() { return (mEditor && !mEditor->RasterSelection()->IsEmpty()); }))
    );

    iMenu.AddMenuEntry(
        LOCTEXT("raster-tool.object-context-menu.cut-selection.name", "Cut Selection")
        , LOCTEXT("raster-tool.object-context-menu.cut-selection.tooltip", "Cut Selection")
        , FSlateIcon()
        , FUIAction(
            FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::CutCurrentSelection)
            , FCanExecuteAction::CreateLambda([this]() { return (mEditor && !mEditor->RasterSelection()->IsEmpty()); }))
    );

    iMenu.AddMenuEntry(
        LOCTEXT("raster-tool.object-context-menu.paste-selection.name", "Paste Selection")
        , LOCTEXT("raster-tool.object-context-menu.paste-selection.tooltip", "Paste Selection")
        , FSlateIcon()
        , FUIAction(
            FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::PasteCurrentSelection)
            , FCanExecuteAction::CreateLambda([this]() { return (mEditor && mEditor->HasCopyBlockClipboard()); }))
    );


    iMenu.AddMenuEntry(
        LOCTEXT("raster-tool.object-context-menu.paste-selection-in-new-layer.name", "Paste Selection In New Layer")
        , LOCTEXT("raster-tool.object-context-menu.paste-selection-in-new-layer.tooltip", "Paste Selection In New Layer")
        , FSlateIcon()
        , FUIAction(
            FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::PasteCurrentSelectionInNewLayer)
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


void UOdysseyPainterEditorRasterBaseTool::BindShortcuts(TSharedPtr<FUICommandList> iCommandList)
{
    Super::BindShortcuts(iCommandList);

    iCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::CopyCurrentSelection),
        FCanExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::CanCopyCurrentSelection)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::CutCurrentSelection),
        FCanExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::CanCutCurrentSelection)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::PasteCurrentSelection),
        FCanExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::CanPasteCurrentSelection)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().PasteCurrentSelectionInNewLayer,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::PasteCurrentSelectionInNewLayer),
        FCanExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::CanPasteCurrentSelectionInNewLayer)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().SelectAll,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::SelectAll)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ClearCurrentSelection,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::ClearCurrentSelection)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().InvertSelection,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterBaseTool::InvertSelection)
    );
}

bool
UOdysseyPainterEditorRasterBaseTool::UsesRasterSelection() const
{
    return true;
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

bool UOdysseyPainterEditorRasterBaseTool::CanCopyCurrentSelection()
{
    if (!mEditor)
        return false;

    if (!mEditor->GetSource())
        return false;

    if (mEditor->GetCurrentMediaProvider().IsLocked())
        return false;

    if (!mEditor->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>())
        return false;

    if (mEditor->RasterSelection()->IsEmpty())
        return false;

    return true;
}

bool UOdysseyPainterEditorRasterBaseTool::CanCutCurrentSelection()
{
    if (!mEditor)
        return false;

    if (!mEditor->GetSource())
        return false;

    if (mEditor->GetCurrentMediaProvider().IsLocked())
        return false;

    if (!mEditor->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>())
        return false;

    if (mEditor->RasterSelection()->IsEmpty())
        return false;

    return true;
}


bool UOdysseyPainterEditorRasterBaseTool::CanPasteCurrentSelection()
{
    if (!mEditor)
        return false;

    if (!mEditor->GetSource())
        return false;

    if (mEditor->GetCurrentMediaProvider().IsLocked())
        return false;

    if (!mEditor->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>())
        return false;

    if (!mEditor->HasCopyBlockClipboard())
        return false;

    return true;
}

bool UOdysseyPainterEditorRasterBaseTool::CanPasteCurrentSelectionInNewLayer()
{
    if (!mEditor)
        return false;

    if (!mEditor->GetSource())
        return false;

    if (!mEditor->HasCopyBlockClipboard())
        return false;

    return true;
}

void UOdysseyPainterEditorRasterBaseTool::CopyCurrentSelection()
{
    if (!mEditor)
        return;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mEditor->GetCurrentMediaProvider().GetMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return;

    FOdysseyCoreEditorModule& odysseyCoreEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyCoreEditorModule>(TEXT("OdysseyCoreEditor"));

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = rasterBlock->GetBlock();

    ::ULIS::FRectI boundingBox = rasterBlock->GetRect();

    TSharedPtr<::ULIS::FBlock> copyBlock = MakeShared<::ULIS::FBlock>(boundingBox.w, boundingBox.h, rasterBlock->GetFormat());

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
    ::ULIS::FEvent clearEvent, copyEvent;
    ctx.Clear(*copyBlock);
    ctx.Finish();

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> maskBlock = mEditor->RasterSelection()->GetBlock();

    if (maskBlock)
    {
        ctx.Copy(
            *block,
            *copyBlock,
            boundingBox,
            ::ULIS::FVec2I(0, 0),
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            0,
            nullptr,
            &copyEvent
        );

        ctx.FilterInto(
            [](const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels)
            {
                for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
                {
                    iDstPixel.SetAlphaF(iDstPixel.AlphaF() * iSrcPixel.GreyF());
                }
            }
            , *maskBlock
            , *copyBlock
            , boundingBox
            , ::ULIS::FVec2I(0, 0)
            , ::ULIS::FSchedulePolicy::MultiScanlines
            , 1
            , &copyEvent
            , nullptr
        );

        ctx.Finish();
    }

    TSharedPtr<FOdysseyBlockClipboardData> clipboardData = MakeShared<FOdysseyBlockClipboardData>(copyBlock);
    odysseyCoreEditorModule.GetClipboard()->SetData(clipboardData);
}


void UOdysseyPainterEditorRasterBaseTool::CutCurrentSelection()
{
    if (!mEditor)
        return;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mEditor->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return;

    FOdysseyCoreEditorModule& odysseyCoreEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyCoreEditorModule>(TEXT("OdysseyCoreEditor"));

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = rasterBlock->GetBlock();

    ::ULIS::FRectI boundingBox = rasterBlock->GetRect();

    TSharedPtr<::ULIS::FBlock> copyBlock = MakeShared<::ULIS::FBlock>(boundingBox.w, boundingBox.h, rasterBlock->GetFormat());

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
    ::ULIS::FEvent clearEvent, copyEvent;
    ctx.Clear(*copyBlock);
    ctx.Finish();

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> maskBlock = mEditor->RasterSelection()->GetBlock();

    if (maskBlock)
    {
        ctx.Copy(
            *block,
            *copyBlock,
            boundingBox,
            ::ULIS::FVec2I(0, 0),
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            0,
            nullptr,
            &copyEvent
        );

        ctx.FilterInto(
            [](const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels)
            {
                for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
                {
                    iDstPixel.SetAlphaF(iDstPixel.AlphaF() * iSrcPixel.GreyF());
                }
            }
            , *maskBlock
            , *copyBlock
            , boundingBox
            , ::ULIS::FVec2I(0, 0)
            , ::ULIS::FSchedulePolicy::MultiScanlines
            , 1
            , &copyEvent
            , nullptr
        );

        ctx.Finish();
    }

    mEditor->GetSource()->ClearFromCopyBlock(copyBlock);

    TSharedPtr<FOdysseyBlockClipboardData> clipboardData = MakeShared<FOdysseyBlockClipboardData>(copyBlock);
    odysseyCoreEditorModule.GetClipboard()->SetData(clipboardData);
}

void UOdysseyPainterEditorRasterBaseTool::PasteCurrentSelection()
{
    if (!mEditor)
        return;

    FOdysseyCoreEditorModule& odysseyCoreEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyCoreEditorModule>(TEXT("OdysseyCoreEditor"));

    TSharedPtr<FOdysseyBlockClipboardData> clipboardData = odysseyCoreEditorModule.GetClipboard()->GetData<FOdysseyBlockClipboardData>();

    mEditor->GetSource()->PasteBlockToCurrentLayer(clipboardData->GetBlock());
}


void UOdysseyPainterEditorRasterBaseTool::PasteCurrentSelectionInNewLayer()
{
    if (!mEditor)
        return;

    FOdysseyCoreEditorModule& odysseyCoreEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyCoreEditorModule>(TEXT("OdysseyCoreEditor"));

    TSharedPtr<FOdysseyBlockClipboardData> clipboardData = odysseyCoreEditorModule.GetClipboard()->GetData<FOdysseyBlockClipboardData>();

    mEditor->GetSource()->PasteBlockToNewLayer(clipboardData->GetBlock());
}

void UOdysseyPainterEditorRasterBaseTool::ClearCurrentSelection()
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
