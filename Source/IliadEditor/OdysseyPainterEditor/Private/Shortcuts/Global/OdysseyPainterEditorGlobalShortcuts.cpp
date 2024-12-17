// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyPainterEditorGlobalShortcuts.h"

#include "Models/OdysseyPainterEditorCommands.h"
#include "OdysseyBlockClipboardData.h"
#include "OdysseyEditorModule.h"
#include "OdysseyPainterEditor.h"
#include "Shortcuts/Global/OdysseyPainterEditorGlobalToolsShortcuts.h"

FOdysseyPainterEditorGlobalShortcuts::FOdysseyPainterEditorGlobalShortcuts(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{
    Add(MakeShared<FOdysseyPainterEditorGlobalToolsShortcuts>(iEditor));
}

void FOdysseyPainterEditorGlobalShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    if (!mEditor)
        return;

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().CopyCurrentSelection,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalShortcuts::Action_CopyCurrentSelection),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalShortcuts::CanAction_CopyCurrentSelection)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().CutCurrentSelection,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalShortcuts::Action_CutCurrentSelection),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalShortcuts::CanAction_CutCurrentSelection)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().PasteCurrentSelection,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalShortcuts::Action_PasteCurrentSelection),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalShortcuts::CanAction_PasteCurrentSelection)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().PasteCurrentSelectionInNewLayer,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalShortcuts::Action_PasteCurrentSelectionInNewLayer),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalShortcuts::CanAction_PasteCurrentSelectionInNewLayer)
    );
}

void FOdysseyPainterEditorGlobalShortcuts::Action_CopyCurrentSelection()
{
    if (!mEditor)
        return;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mEditor->GetCurrentMediaProvider().GetMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return;

    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));

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
    odysseyEditorModule.GetClipboard()->SetData(clipboardData);
}

void FOdysseyPainterEditorGlobalShortcuts::Action_CutCurrentSelection()
{
    if (!mEditor)
        return;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mEditor->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return;

    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));

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
    odysseyEditorModule.GetClipboard()->SetData(clipboardData);
}

void FOdysseyPainterEditorGlobalShortcuts::Action_PasteCurrentSelection()
{
    if (!mEditor)
        return;

    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));

    TSharedPtr<FOdysseyBlockClipboardData> clipboardData = odysseyEditorModule.GetClipboard()->GetData<FOdysseyBlockClipboardData>();

    mEditor->GetSource()->PasteBlockToCurrentLayer(clipboardData->GetBlock());
}


void FOdysseyPainterEditorGlobalShortcuts::Action_PasteCurrentSelectionInNewLayer()
{
    if (!mEditor)
        return;

    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));

    TSharedPtr<FOdysseyBlockClipboardData> clipboardData = odysseyEditorModule.GetClipboard()->GetData<FOdysseyBlockClipboardData>();

    mEditor->GetSource()->PasteBlockToNewLayer(clipboardData->GetBlock());
}

bool FOdysseyPainterEditorGlobalShortcuts::CanAction_CopyCurrentSelection()
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

bool FOdysseyPainterEditorGlobalShortcuts::CanAction_CutCurrentSelection()
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

bool FOdysseyPainterEditorGlobalShortcuts::CanAction_PasteCurrentSelection()
{
    if (!mEditor)
        return false;

    if (!mEditor->GetSource())
        return false;

    if (mEditor->GetCurrentMediaProvider().IsLocked())
        return false;

    if (!mEditor->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>())
        return false;

    if( !mEditor->HasCopyBlockClipboard() )
        return false;

    return true;
}

bool FOdysseyPainterEditorGlobalShortcuts::CanAction_PasteCurrentSelectionInNewLayer()
{
    if (!mEditor)
        return false;

    if (!mEditor->GetSource())
        return false;

    if (!mEditor->HasCopyBlockClipboard())
        return false;

    return true;
}
