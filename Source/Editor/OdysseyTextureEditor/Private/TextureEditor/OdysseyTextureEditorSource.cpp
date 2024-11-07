// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditorSource.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "OdysseyRasterBlockMutator.h"
#include "ULISLoaderModule.h"
#include "Undo/OdysseyVectorUndoEngineClear.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPixelFormat.h"
#include "ScopedTransaction.h"
#include "TextureCompiler.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

const FGuid&
FOdysseyTextureEditorSource::StaticId()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

FOdysseyTextureEditorSource::~FOdysseyTextureEditorSource()
{

}

FOdysseyTextureEditorSource::FOdysseyTextureEditorSource(UTexture2D* iTexture)
    : mTexture(iTexture)
{
}

const FGuid&
FOdysseyTextureEditorSource::Id() const
{
    return StaticId();
}

int
FOdysseyTextureEditorSource::Width() const
{
    return mTexture->Source.GetSizeX();
}

int
FOdysseyTextureEditorSource::Height() const
{
    return mTexture->Source.GetSizeY();
}

void
FOdysseyTextureEditorSource::Activate()
{
    AddEditedObject(mTexture);

    InitTextureUserData();

    UOdysseyTextureLayerStack* layerStack = GetLayerStack();
    if ( layerStack )
        layerStack->ActivateTextureFastUpdate();

    FOdysseyPainterEditorSource::Activate();
}

void
FOdysseyTextureEditorSource::Inactivate()
{
    RemoveEditedObject(mTexture);

    UOdysseyTextureLayerStack* layerStack = GetLayerStack();
    if ( layerStack )
        layerStack->InactivateTextureFastUpdate();

    FOdysseyPainterEditorSource::Inactivate();
}

UTexture*
FOdysseyTextureEditorSource::DisplayTexture() const
{
    return mTexture;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyTextureEditorSource::GetDisplayBlock()
{
    UOdysseyTextureLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return nullptr;

    return layerStack->GetSurface()->Block();
}

UTexture2D*
FOdysseyTextureEditorSource::GetTexture() const
{
    return mTexture;
}

UOdysseyTextureLayerStack*
FOdysseyTextureEditorSource::GetLayerStack() const
{
    UOdysseyTextureLayerStackUserData* userData = TextureUserData();
    if (!userData)
        return nullptr;
    
    return userData->GetLayerStack();
}

UOdysseyTextureLayerStackUserData*
FOdysseyTextureEditorSource::TextureUserData() const
{
    if ( !mTexture )
        return nullptr;

    return Cast<UOdysseyTextureLayerStackUserData>(mTexture->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
}

void
FOdysseyTextureEditorSource::InitTextureUserData()
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
FOdysseyTextureEditorSource::GetCurrentMediaProvider()
{
    UOdysseyTextureLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return FOdysseyMediaProvider();

    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return FOdysseyMediaProvider();

    return currentLayer->GetMediaProvider(0);
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Common Actions

void
FOdysseyTextureEditorSource::Clear()
{
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if ( !layerStack )
        return;

    //UOdysseyTextureLayerImageRaster* currentLayerRaster = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CurrentLayer.Get());
    //UOdysseyTextureLayerImageVector* currentLayerVector = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    #ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("actions.clear", "Clear"));
    #endif
    
    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
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
                [&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
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
        for (TSharedPtr<FOdysseyMediaVector> mediaVector : mediasVector)
        {
            FOdysseyVectorEngine* vectorEngine = mediaVector->GetScene()->GetEngine();
            // needed for undos
            if (GUndo)
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoEngineClear(vectorEngine);
                GUndo->StoreUndo(GEditor, TUniquePtr<FOdysseyVectorUndo>(undo));
                RecordCurrentFrameUndo();
            }

            vectorEngine->SetScene(new FOdysseyVectorGroupPaint("Scene"));
            vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_ALL );
        }
    }
}

void 
FOdysseyTextureEditorSource::ClearFromCopyBlock(TSharedPtr<::ULIS::FBlock> iCopyBlock)
{
    if (!iCopyBlock)
        return;

    UOdysseyLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("actions.cut", "Cut"));
#endif

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
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
                [&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
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

void FOdysseyTextureEditorSource::PasteBlockToCurrentLayer(TSharedPtr<::ULIS::FBlock> iBlock)
{
    if (!iBlock)
        return;

    UOdysseyLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("actions.paste", "Paste"));
#endif

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
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
                [&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
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
FOdysseyTextureEditorSource::PasteBlockToNewLayer( TSharedPtr<::ULIS::FBlock> iBlock )
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
    FOdysseyObjectEditorUtils::SetPropertyValue(GetLayerStack(), GET_MEMBER_NAME_CHECKED(UOdysseyLayerStack, CurrentLayer), layer);

    FOdysseyRasterBlockMutator mutator(layer->GetRasterBlock());
    mutator.Copy(iBlock, {});
    mutator.Commit();
}

#undef LOCTEXT_NAMESPACE
