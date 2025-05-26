// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorTextureSource.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerImageVector.h"
#include "OdysseyRasterBlockMutator.h"
#include "ULISLoaderModule.h"
#include "Undo/OdysseyVectorUndoSceneClear.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPixelFormat.h"
#include "ScopedTransaction.h"
#include "TextureCompiler.h"

#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorCell.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Engine/TextureRenderTarget2D.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

const FGuid&
FOdysseyPainterEditorTextureSource::StaticId()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

FOdysseyPainterEditorTextureSource::~FOdysseyPainterEditorTextureSource()
{

}

FOdysseyPainterEditorTextureSource::FOdysseyPainterEditorTextureSource(UTexture2D* iTexture)
    : mTexture(iTexture)
{
}

const FGuid&
FOdysseyPainterEditorTextureSource::Id() const
{
    return StaticId();
}

int
FOdysseyPainterEditorTextureSource::Width() const
{
    return mTexture->Source.GetSizeX();
}

int
FOdysseyPainterEditorTextureSource::Height() const
{
    return mTexture->Source.GetSizeY();
}

void
FOdysseyPainterEditorTextureSource::Activate(FOdysseyPainterEditor* iEditor)
{
    AddEditedObject(mTexture);

    InitTextureUserData();

    UOdysseyTextureLayerStack* layerStack = GetLayerStack();
    if ( layerStack )
        layerStack->ActivateTextureFastUpdate();

    FOdysseyPainterEditorSource::Activate(iEditor);
}

void
FOdysseyPainterEditorTextureSource::Inactivate()
{
    RemoveEditedObject(mTexture);

    UOdysseyTextureLayerStack* layerStack = GetLayerStack();
    if ( layerStack )
        layerStack->InactivateTextureFastUpdate();

    FOdysseyPainterEditorSource::Inactivate();
}

UTexture*
FOdysseyPainterEditorTextureSource::DisplayTexture() const
{
    return mTexture;
}

UTexture2D*
FOdysseyPainterEditorTextureSource::GetTexture() const
{
    return mTexture;
}

UOdysseyTextureLayerStack*
FOdysseyPainterEditorTextureSource::GetLayerStack() const
{
    UOdysseyTextureLayerStackUserData* userData = TextureUserData();
    if (!userData)
        return nullptr;

    return userData->GetLayerStack();
}

UOdysseyTextureLayerStackUserData*
FOdysseyPainterEditorTextureSource::TextureUserData() const
{
    if ( !mTexture )
        return nullptr;

    return Cast<UOdysseyTextureLayerStackUserData>(mTexture->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
}

void
FOdysseyPainterEditorTextureSource::InitTextureUserData()
{
    UOdysseyTextureLayerStackUserData* userData = TextureUserData();
    if (userData)
        return;

    //Init user data
    userData = NewObject<UOdysseyTextureLayerStackUserData>(mTexture, NAME_None, RF_Public);
    userData->InitWithDefaultLayerStack();

    // Notify for changes
    mTexture->AddAssetUserData( userData );
    mTexture->PostEditChange();
}

FOdysseyMediaProvider
FOdysseyPainterEditorTextureSource::GetCurrentMediaProvider()
{
    UOdysseyTextureLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return FOdysseyMediaProvider();

    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return FOdysseyMediaProvider();

    return currentLayer->GetMediaProvider(0);
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Common Actions

void
FOdysseyPainterEditorTextureSource::Clear()
{
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if ( !layerStack )
        return;

    //UOdysseyTextureLayerImageRaster* currentLayerRaster = Cast<UOdysseyTextureLayerImageRaster>(layerStack->GetCurrentLayer());
    //UOdysseyTextureLayerImageVector* currentLayerVector = Cast<UOdysseyTextureLayerImageVector>(layerStack->GetCurrentLayer());

    #ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("actions.clear", "Clear"));
    #endif

    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
    if (!currentLayer)
        return;

    FOdysseyMediaProvider mediaProvider = currentLayer->GetMediaProvider(0);
    if ( mediaProvider.IsLocked() )
        return;

    if ( mediaProvider.HasMedia<FOdysseyMediaRaster>() )
    {
        TArray<TSharedPtr<FOdysseyMediaRaster>> mediasRaster = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
        for (TSharedPtr<FOdysseyMediaRaster> mediaRaster : mediasRaster)
        {
            TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRaster->GetRasterBlock();
            if (!rasterBlock)
                continue;

            FOdysseyRasterBlockMutator mutator(rasterBlock);
            mutator.EditTilesFromRects(
                { ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight()) },
                [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
                {
                    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
                    ::ULIS::FEvent eventClear;
                    ctx.Clear(*iBlock, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear);
                    return { eventClear };
                }
            );
            mutator.Commit();
        }
    }
    else if (mediaProvider.HasMedia<FOdysseyMediaVector>())
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediasVector = mediaProvider.GetOrCreateMedias<FOdysseyMediaVector>();
        uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_ALL;

        for (TSharedPtr<FOdysseyMediaVector> mediaVector : mediasVector)
        {
            FOdysseyVectorCell* vectorCell = mediaVector->GetScene()->GetCell();

            // needed for undos
            if (GUndo)
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSceneClear( mediaVector->GetScene(), notificationFlags );
                GUndo->StoreUndo(GEditor, TUniquePtr<FOdysseyVectorUndo>(undo));
                RecordCurrentFrameUndo();
            }

            vectorCell->SetScene(new FOdysseyVectorGroupPaint("Scene") );
        }

        FOdysseyVectorEngine::Notify( nullptr, notificationFlags );
    }
}

void
FOdysseyPainterEditorTextureSource::ClearFromCopyBlock(TSharedPtr<::ULIS::FBlock> iCopyBlock)
{
    if (!iCopyBlock)
        return;

    UOdysseyLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("actions.cut", "Cut"));
#endif

    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
    if (!currentLayer)
        return;

    FOdysseyMediaProvider mediaProvider = currentLayer->GetMediaProvider(0);
    if (mediaProvider.IsLocked())
        return;

    if (mediaProvider.HasMedia<FOdysseyMediaRaster>())
    {
        TArray<TSharedPtr<FOdysseyMediaRaster>> mediasRaster = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
        for (TSharedPtr<FOdysseyMediaRaster> mediaRaster : mediasRaster)
        {
            TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRaster->GetRasterBlock();
            if (!rasterBlock)
                continue;

            FOdysseyRasterBlockMutator mutator(rasterBlock);
            mutator.EditTilesFromRects(
                { ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight()) },
                [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
                {
                    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
                    ::ULIS::FEvent eventCut;

                    ctx.Blend(
                        *iCopyBlock,
                        *iBlock,
                        iCopyBlock->Rect(),
                        ::ULIS::FVec2I(0, 0),
                        ::ULIS::Blend_Normal,
                        ::ULIS::Alpha_Sub,
                        1.f,
                        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                        0,
                        nullptr,
                        &eventCut
                    );

                    return { eventCut };
                }
            );
            mutator.Commit();
        }
    }
}

void FOdysseyPainterEditorTextureSource::PasteBlockToCurrentLayer(TSharedPtr<::ULIS::FBlock> iBlock)
{
    if (!iBlock)
        return;

    UOdysseyLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("actions.paste", "Paste"));
#endif

    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
    if (!currentLayer)
        return;

    FOdysseyMediaProvider mediaProvider = currentLayer->GetMediaProvider(0);
    if (mediaProvider.IsLocked())
        return;


    if (mediaProvider.HasMedia<FOdysseyMediaRaster>())
    {
        int width = GetLayerStack()->GetTexture()->GetSizeX();
        int height = GetLayerStack()->GetTexture()->GetSizeY();
        ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(GetLayerStack()->GetTexture()->Source.GetFormat());

        TSharedPtr<::ULIS::FBlock> copyBlock = MakeShared<::ULIS::FBlock>(width, height, format);

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
        ctx.Clear(*copyBlock);
        ctx.Finish();

        ctx.ConvertFormat(
            *iBlock,
            *copyBlock,
            iBlock->Rect(),
            ::ULIS::FVec2I(0, 0),
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            0,
            nullptr,
            nullptr);

        ctx.Finish();

        TArray<TSharedPtr<FOdysseyMediaRaster>> mediasRaster = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
        for (TSharedPtr<FOdysseyMediaRaster> mediaRaster : mediasRaster)
        {
            TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRaster->GetRasterBlock();
            if (!rasterBlock)
                continue;

            FOdysseyRasterBlockMutator mutator(rasterBlock);
            mutator.EditTilesFromRects(
                { ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight()) },
                [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
                {
                    ::ULIS::FEvent eventPaste;

                    ctx.Blend(
                        *copyBlock,
                        *iBlock,
                        copyBlock->Rect(),
                        ::ULIS::FVec2I(0, 0),
                        ::ULIS::Blend_Normal,
                        ::ULIS::Alpha_Normal,
                        1.f,
                        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                        0,
                        nullptr,
                        &eventPaste);

                    return { eventPaste };

                }
            );
            mutator.Commit();
        }
    }
}

void
FOdysseyPainterEditorTextureSource::PasteBlockToNewLayer( TSharedPtr<::ULIS::FBlock> iBlock )
{
    if (!iBlock)
        return;

    if (GetCurrentMediaProvider().IsLocked())
        return;

    if (!GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("actions.paste", "Paste"));
#endif
    GetLayerStack()->Modify();

    UOdysseyTextureLayerImageRaster* layer = Cast< UOdysseyTextureLayerImageRaster >(GetLayerStack()->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass()));
    GetLayerStack()->SetCurrentLayer(layer);

    FOdysseyRasterBlockMutator mutator(layer->GetRasterBlock());
    mutator.Copy(iBlock, {});
    mutator.Commit();
}

#undef LOCTEXT_NAMESPACE
