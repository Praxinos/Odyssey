// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorSource.h"
#include "OdysseyAnimation.h"
//#include "OdysseyAnimationTexture.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyMediaVector.h"
#include "ULISLoaderModule.h"
#include "Undo/OdysseyVectorUndoEngineClear.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "UObject/OdysseyObjectEditorUtils.h"


#define LOCTEXT_NAMESPACE "AnimationEditor"

const FGuid&
FOdysseyAnimationEditorSource::StaticId()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

FOdysseyAnimationEditorSource::~FOdysseyAnimationEditorSource()
{

}

FOdysseyAnimationEditorSource::FOdysseyAnimationEditorSource(UOdysseyAnimation* iAnimation)
    : mAnimation( iAnimation )
    //, mTexture( NewObject<UOdysseyAnimationTexture>() )
    , mPlayer ( NewObject<UOdysseyAnimationPlayer>() )
{
}

const FGuid&
FOdysseyAnimationEditorSource::Id() const
{
    return StaticId();
}

int
FOdysseyAnimationEditorSource::Width() const
{
    return mAnimation->GetWidth();
}

int
FOdysseyAnimationEditorSource::Height() const
{
    return mAnimation->GetHeight();
}

void
FOdysseyAnimationEditorSource::Activate()
{
    AddEditedObject(mAnimation);

    FOdysseyObjectEditorUtils::SetPropertyValue(mPlayer, GET_MEMBER_NAME_CHECKED( UOdysseyAnimationPlayer, Animation), mAnimation);
    //FOdysseyObjectEditorUtils::SetPropertyValue(mTexture, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationTexture, Player), mPlayer);
    //mTexture->UpdateResource();

    //Seek at current frame
    ActivatePlayer(GetAnimationPlayer());

    mAnimation->OnCurrentFrameChanged().AddRaw(this, &FOdysseyAnimationEditorSource::OnCurrentFrameChanged);

    FOdysseyPainterEditorSource::Activate();
}

void
FOdysseyAnimationEditorSource::ActivatePlayer(UOdysseyAnimationPlayer* iPlayer)
{
    iPlayer->SetRenderType(IOdysseyImageRenderer::eRenderType::Editor);
    iPlayer->SeekToFrame(mAnimation->CurrentFrame);
    iPlayer->OnPlay().AddRaw(this, &FOdysseyAnimationEditorSource::OnPlayerPlay);
    iPlayer->OnStop().AddRaw(this, &FOdysseyAnimationEditorSource::OnPlayerStop);
}

void
FOdysseyAnimationEditorSource::Inactivate()
{
    RemoveEditedObject(mAnimation);

    InactivatePlayer(GetAnimationPlayer());

    FOdysseyObjectEditorUtils::SetPropertyValue(mPlayer, GET_MEMBER_NAME_CHECKED( UOdysseyAnimationPlayer, Animation), nullptr);

    mAnimation->OnCurrentFrameChanged().RemoveAll(this);

    FOdysseyPainterEditorSource::Inactivate();
}

void
FOdysseyAnimationEditorSource::InactivatePlayer(UOdysseyAnimationPlayer* iPlayer)
{
    iPlayer->SetRenderType(IOdysseyImageRenderer::eRenderType::Render);
    iPlayer->OnPlay().RemoveAll(this);
    iPlayer->OnStop().RemoveAll(this);
    iPlayer->Stop();
}

UTexture*
FOdysseyAnimationEditorSource::DisplayTexture() const
{
    return GetAnimationPlayer()->GetTexture();
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyAnimationEditorSource::GetDisplayBlock()
{
    //TODO: this is used only for picking a color in PainterEditor's viewport tab
    //Find a way to do it without having that method

    return nullptr;
}

UOdysseyAnimation*
FOdysseyAnimationEditorSource::GetAnimation() const
{
    return mAnimation;
}

UOdysseyAnimationLayerStack*
FOdysseyAnimationEditorSource::GetLayerStack() const
{
    if (!mAnimation)
        return nullptr;

    return mAnimation->GetLayerStack();
}

/* UOdysseyAnimationTexture*
FOdysseyAnimationEditorSource::GetAnimationTexture() const
{
    return mTexture;
} */

UOdysseyAnimationPlayer*
FOdysseyAnimationEditorSource::GetAnimationPlayer() const
{
    return mExternalPlayer ? mExternalPlayer : mPlayer;
}

void
FOdysseyAnimationEditorSource::SetExternalPlayer(UOdysseyAnimationPlayer* iPlayer)
{
    if (IsActivated())
    {
        if (mExternalPlayer)
        {
            InactivatePlayer(mExternalPlayer);
        }
        else
        {
            InactivatePlayer(mPlayer);
        }
    }

    mExternalPlayer = iPlayer;

    if (IsActivated())
    {
        if (mExternalPlayer)
        {
            ActivatePlayer(mExternalPlayer);
        }
        else
        {
            ActivatePlayer(mPlayer);
        }
    }
}

FOdysseyMediaProvider
FOdysseyAnimationEditorSource::GetCurrentMediaProvider()
{
    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(GetLayerStack()->CurrentLayer.Get());
    if (!currentLayer)
        return FOdysseyMediaProvider();

    return currentLayer->GetMediaProvider(mAnimation->CurrentFrame);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyAnimationEditorSource::AddReferencedObjects(FReferenceCollector& Collector)
{
    FOdysseyPainterEditorSource::AddReferencedObjects(Collector);
    //Collector.AddReferencedObject(mTexture);
    Collector.AddReferencedObject(mPlayer);
    Collector.AddReferencedObject(mExternalPlayer);
}

void
FOdysseyAnimationEditorSource::OnCurrentFrameChanged(UOdysseyAnimation* iAnimation)
{
    if (iAnimation != mAnimation)
        return;

    //Display the new current frame
    GetAnimationPlayer()->SeekToFrame(mAnimation->CurrentFrame);
    GetAnimationPlayer()->Stop();
}

void
FOdysseyAnimationEditorSource::OnPlayerPlay()
{
    if (!mAnimation)
        return;

    GetAnimationPlayer()->SetRenderType(IOdysseyImageRenderer::eRenderType::Render);
}

void
FOdysseyAnimationEditorSource::OnPlayerStop()
{
    if (!mAnimation)
        return;

    GetAnimationPlayer()->SetRenderType(IOdysseyImageRenderer::eRenderType::Editor);
    GetAnimationPlayer()->SeekToFrame(mAnimation->CurrentFrame);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Common Actions

void
FOdysseyAnimationEditorSource::Clear()
{
    FText transactionName = LOCTEXT("actions.clear", "Clear");

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(GetLayerStack()->CurrentLayer.Get());
    if (!currentLayer)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(transactionName);
#endif
    FOdysseyMediaProvider mediaProvider = currentLayer->GetMediaProvider(mAnimation->CurrentFrame);
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

            FOdysseyAnimationCurrentFrameMutator currentFrameMutator(mAnimation);
            currentFrameMutator.Set(mAnimation->CurrentFrame);
            currentFrameMutator.Commit();
        }
    }
    else if (mediaProvider.HasMedia<FOdysseyMediaVector>())
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediasVector = mediaProvider.GetOrCreateMedias<FOdysseyMediaVector>();
        for (TSharedPtr<FOdysseyMediaVector> mediaVector : mediasVector)
        {
            FOdysseyVectorEngine* vectorEngine = mediaVector->GetScene()->GetEngine();
            // needed for undos
            GEditor->BeginTransaction(transactionName);
            if (GUndo)
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoEngineClear(vectorEngine);

                GUndo->StoreUndo(GEditor, TUniquePtr<FOdysseyVectorUndo>(undo));

                FOdysseyAnimationCurrentFrameMutator currentFrameMutator(mAnimation);
                currentFrameMutator.Set(mAnimation->CurrentFrame);
                currentFrameMutator.Commit();
            }
            GEditor->EndTransaction();

            vectorEngine->SetScene(new FOdysseyVectorGroupPaint("Scene"));
            vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_ALL );
        }
    }
}

void FOdysseyAnimationEditorSource::ClearFromCopyBlock(TSharedPtr<::ULIS::FBlock> iCopyBlock)
{
    FText transactionName = LOCTEXT("actions.cut", "Cut");

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(GetLayerStack()->CurrentLayer.Get());
    if (!currentLayer)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(transactionName);
#endif
    FOdysseyMediaProvider mediaProvider = currentLayer->GetMediaProvider(mAnimation->CurrentFrame);
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

            FOdysseyAnimationCurrentFrameMutator currentFrameMutator(mAnimation);
            currentFrameMutator.Set(mAnimation->CurrentFrame);
            currentFrameMutator.Commit();
        }
    }
}

void FOdysseyAnimationEditorSource::PasteBlockToCurrentLayer(TSharedPtr<::ULIS::FBlock> iBlock)
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

    FOdysseyMediaProvider mediaProvider = currentLayer->GetMediaProvider(mAnimation->CurrentFrame);
    if (mediaProvider.IsLocked())
        return;


    if (mediaProvider.HasMedia<FOdysseyMediaRaster>())
    {
        int width = GetLayerStack()->GetAnimation()->GetWidth();
        int height = GetLayerStack()->GetAnimation()->GetHeight();
        ::ULIS::eFormat format = GetLayerStack()->GetAnimation()->GetFormat();

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
FOdysseyAnimationEditorSource::PasteBlockToNewLayer( TSharedPtr<::ULIS::FBlock> iBlock )
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

    UOdysseyAnimationLayerImageRaster* layer = Cast< UOdysseyAnimationLayerImageRaster >(GetLayerStack()->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    layer->Modify();

    GetLayerStack()->CurrentLayer = layer;

    FPropertyChangedEvent PropertyChangedEvent(UOdysseyLayerStack::StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UOdysseyLayerStack, CurrentLayer)), EPropertyChangeType::ValueSet);
    GetLayerStack()->PostEditChangeProperty(PropertyChangedEvent);

    UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>(layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass(), mAnimation->CurrentFrame));
    cell->Modify();

    FOdysseyRasterBlockMutator blockMutator(cell->GetRasterBlock(), false);
    blockMutator.EditTilesFromRects(
        { cell->GetRasterBlock()->GetRect() },
        [&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(cell->GetRasterBlock()->GetFormat());
            ::ULIS::FEvent eventClear;
            ctx.Clear(*iBlock, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear);
            ctx.Finish();
            ctx.Copy(
                *iBlock,
                *cell->GetRasterBlock()->GetBlock()
            );
            ctx.Finish();
            return {};
        }
    );
    blockMutator.Commit();

    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(mAnimation);
    currentFrameMutator.Set(mAnimation->CurrentFrame);
    currentFrameMutator.Commit();
}

void
FOdysseyAnimationEditorSource::RecordCurrentFrameUndo() const
{
    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(mAnimation);
    currentFrameMutator.Set(mAnimation->CurrentFrame);
    currentFrameMutator.Commit();
}

#undef LOCTEXT_NAMESPACE
