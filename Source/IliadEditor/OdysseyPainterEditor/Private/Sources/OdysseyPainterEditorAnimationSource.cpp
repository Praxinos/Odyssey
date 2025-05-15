// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorAnimationSource.h"
#include "OdysseyAnimation.h"
//#include "OdysseyAnimationTexture.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ULISLoaderModule.h"
#include "Undo/OdysseyVectorUndoSceneClear.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyAnimationPlayer.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Engine/TextureRenderTarget2D.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "OdysseyRasterBlock.h"
#include <ULIS>


#define LOCTEXT_NAMESPACE "AnimationEditor"

const FGuid&
FOdysseyPainterEditorAnimationSource::StaticId()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

FOdysseyPainterEditorAnimationSource::~FOdysseyPainterEditorAnimationSource()
{
}

FOdysseyPainterEditorAnimationSource::FOdysseyPainterEditorAnimationSource(UOdysseyAnimation* iAnimation)
    : mAnimation( iAnimation )
    //, mTexture( NewObject<UOdysseyAnimationTexture>() )
    , mPlayer ( NewObject<UOdysseyAnimationPlayer>() )
    , mEditor( nullptr )
{
    mPlayer->SetUsePreBehaviour(false);
    mPlayer->SetUsePostBehaviour(false);
    mPlayer->SetIgnoreAnimationBounds(true);
}

const FGuid&
FOdysseyPainterEditorAnimationSource::Id() const
{
    return StaticId();
}

int
FOdysseyPainterEditorAnimationSource::Width() const
{
    return mAnimation->GetWidth();
}

int
FOdysseyPainterEditorAnimationSource::Height() const
{
    return mAnimation->GetHeight();
}

void
FOdysseyPainterEditorAnimationSource::Activate(FOdysseyPainterEditor* iEditor)
{
    mEditor = iEditor;

    AddEditedObject(mAnimation);

    mPlayer->SetAnimation(mAnimation);

    //Seek at current frame
    ActivatePlayer(GetAnimationPlayer());
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddRaw(this, &FOdysseyPainterEditorAnimationSource::OnRenderingChanged);

    FOdysseyPainterEditorSource::Activate(iEditor);
}

void
FOdysseyPainterEditorAnimationSource::ActivatePlayer(UOdysseyAnimationPlayer* iPlayer)
{
    iPlayer->SetRenderType(EOdysseyRenderingType::Editor);
    iPlayer->SeekToFrame(0);
    iPlayer->OnCurrentFrameChanged().AddRaw(this, &FOdysseyPainterEditorAnimationSource::OnCurrentFrameChanged);
}

void
FOdysseyPainterEditorAnimationSource::Inactivate()
{
    RemoveEditedObject(mAnimation);

    InactivatePlayer(GetAnimationPlayer());
    mPlayer->SetAnimation(nullptr);
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);

    FOdysseyPainterEditorSource::Inactivate();

    mEditor = nullptr;
}

void
FOdysseyPainterEditorAnimationSource::InactivatePlayer(UOdysseyAnimationPlayer* iPlayer)
{
    iPlayer->OnCurrentFrameChanged().RemoveAll(this);
    iPlayer->SetRenderType(EOdysseyRenderingType::Render);
    iPlayer->Stop();
}

UTexture*
FOdysseyPainterEditorAnimationSource::DisplayTexture() const
{
    return GetAnimationPlayer()->GetRenderTarget();
}

UOdysseyAnimation*
FOdysseyPainterEditorAnimationSource::GetAnimation() const
{
    return mAnimation;
}

UOdysseyAnimationLayerStack*
FOdysseyPainterEditorAnimationSource::GetLayerStack() const
{
    if (!mAnimation)
        return nullptr;

    return Cast<UOdysseyAnimationLayerStack>(mAnimation->GetLayerStack());
}

UOdysseyAnimationPlayer*
FOdysseyPainterEditorAnimationSource::GetAnimationPlayer() const
{
    return mExternalPlayer ? mExternalPlayer : mPlayer;
}

void
FOdysseyPainterEditorAnimationSource::SetExternalPlayer(UOdysseyAnimationPlayer* iPlayer)
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
FOdysseyPainterEditorAnimationSource::GetCurrentMediaProvider()
{
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return FOdysseyMediaProvider();

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return FOdysseyMediaProvider();

    UOdysseyAnimationPlayer* player = GetAnimationPlayer();
    if (!player)
        return FOdysseyMediaProvider();

    return currentLayer->GetMediaProvider(player->GetCurrentFrame().FrameNumber.Value);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyPainterEditorAnimationSource::AddReferencedObjects(FReferenceCollector& Collector)
{
    FOdysseyPainterEditorSource::AddReferencedObjects(Collector);
    //Collector.AddReferencedObject(mTexture);
    Collector.AddReferencedObject(mPlayer);
    Collector.AddReferencedObject(mExternalPlayer);
}

void
FOdysseyPainterEditorAnimationSource::OnCurrentFrameChanged()
{
    UOdysseyAnimationPlayer* player = GetAnimationPlayer();
    if (!player)
        return;

    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return;

    //Preload the new current frame for edition
    TArray<FGuid> imageRenderingComposition = animation->GetRenderingComposition(EOdysseyRenderingType::Render, player->GetCurrentFrame().FrameNumber.Value);
    if ( imageRenderingComposition == mImageRenderingComposition )
        return;

    mImageRenderingComposition = imageRenderingComposition;
    mEditor->SanitizeCurrentTool();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Common Actions

void
FOdysseyPainterEditorAnimationSource::Clear()
{
    FText transactionName = LOCTEXT("actions.clear", "Clear");

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(GetLayerStack()->GetCurrentLayer());
    if (!currentLayer)
        return;

    UOdysseyAnimationPlayer* player = GetAnimationPlayer();
    if (!player)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(transactionName);
#endif
    FOdysseyMediaProvider mediaProvider = currentLayer->GetMediaProvider(player->GetCurrentFrame().FrameNumber.Value);
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

            FOdysseyAnimationCurrentFrameMutator currentFrameMutator(player);
            currentFrameMutator.Set(player->GetCurrentFrame().FrameNumber.Value);
            currentFrameMutator.Commit();
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
            GEditor->BeginTransaction(transactionName);
            if (GUndo)
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSceneClear( vectorCell->GetScene(), notificationFlags );

                GUndo->StoreUndo(GEditor, TUniquePtr<FOdysseyVectorUndo>(undo));

                FOdysseyAnimationCurrentFrameMutator currentFrameMutator(player);
                currentFrameMutator.Set(player->GetCurrentFrame().FrameNumber.Value);
                currentFrameMutator.Commit();
            }
            GEditor->EndTransaction();

            vectorCell->SetScene( new FOdysseyVectorGroupPaint("Scene") );
            vectorCell->GetLayer()->RequestRedraw( vectorCell, 0 );
        }

        FOdysseyVectorEngine::Notify( nullptr, notificationFlags );
    }
}

void FOdysseyPainterEditorAnimationSource::ClearFromCopyBlock(TSharedPtr<::ULIS::FBlock> iCopyBlock)
{
    FText transactionName = LOCTEXT("actions.cut", "Cut");

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(GetLayerStack()->GetCurrentLayer());
    if (!currentLayer)
        return;

    UOdysseyAnimationPlayer* player = GetAnimationPlayer();
    if (!player)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(transactionName);
#endif
    FOdysseyMediaProvider mediaProvider = currentLayer->GetMediaProvider(player->GetCurrentFrame().FrameNumber.Value);
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

            FOdysseyAnimationCurrentFrameMutator currentFrameMutator(player);
            currentFrameMutator.Set(player->GetCurrentFrame().FrameNumber.Value);
            currentFrameMutator.Commit();
        }
    }
}

void FOdysseyPainterEditorAnimationSource::PasteBlockToCurrentLayer(TSharedPtr<::ULIS::FBlock> iBlock)
{
    if (!iBlock)
        return;

    UOdysseyLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return;

    UOdysseyAnimationPlayer* player = GetAnimationPlayer();
    if (!player)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("actions.paste", "Paste"));
#endif

    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
    if (!currentLayer)
        return;

    FOdysseyMediaProvider mediaProvider = currentLayer->GetMediaProvider(player->GetCurrentFrame().FrameNumber.Value);
    if (mediaProvider.IsLocked())
        return;


    if (mediaProvider.HasMedia<FOdysseyMediaRaster>())
    {
        int width = GetLayerStack()->GetAnimation()->GetWidth();
        int height = GetLayerStack()->GetAnimation()->GetHeight();

        ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
        switch(GetLayerStack()->GetAnimation()->GetFormat())
        {
            case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8; break;
            case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF; break;
        }

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
FOdysseyPainterEditorAnimationSource::PasteBlockToNewLayer( TSharedPtr<::ULIS::FBlock> iBlock )
{
    if (!iBlock)
        return;

    if (GetCurrentMediaProvider().IsLocked())
        return;

    if (!GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>())
        return;

    UOdysseyAnimationPlayer* player = GetAnimationPlayer();
    if (!player)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("actions.pasteInNewLayer", "Paste In New Layer"));
#endif
    GetLayerStack()->Modify();

    UOdysseyAnimationLayerImageRaster* layer = Cast< UOdysseyAnimationLayerImageRaster >(GetLayerStack()->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    layer->Modify();

    GetLayerStack()->SetCurrentLayer(layer);

    UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>(layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass(), player->GetCurrentFrame().FrameNumber.Value));
    cell->Modify();

    FOdysseyMediaProvider mediaProvider = GetLayerStack()->GetCurrentLayer()->GetMediaProvider(player->GetCurrentFrame().FrameNumber.Value);

    if (mediaProvider.HasMedia<FOdysseyMediaRaster>())
    {
        int width = GetLayerStack()->GetAnimation()->GetWidth();
        int height = GetLayerStack()->GetAnimation()->GetHeight();

        ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
        switch (GetLayerStack()->GetAnimation()->GetFormat())
        {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8; break;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF; break;
        }

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

    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(player);
    currentFrameMutator.Set(player->GetCurrentFrame().FrameNumber.Value);
    currentFrameMutator.Commit();
}

void
FOdysseyPainterEditorAnimationSource::RecordCurrentFrameUndo() const
{
    UOdysseyAnimationPlayer* player = GetAnimationPlayer();
    if (!player)
        return;

    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(player);
    currentFrameMutator.Set(player->GetCurrentFrame().FrameNumber.Value);
    currentFrameMutator.Commit();
}

void
FOdysseyPainterEditorAnimationSource::OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyPainterEditor::OnRenderingChanged);
    if (iEvent.IsInteractive() || iEvent.GetType() != FOdysseyRenderingChangedEvent::eEventType::kCompositionChange)
        return;

    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return;

    UOdysseyAnimationPlayer* player = GetAnimationPlayer();
    if (!player)
        return;

    TArray<FGuid> imageRenderingComposition = animation->GetRenderingComposition(EOdysseyRenderingType::Render, player->GetCurrentFrame().FrameNumber.Value);
    if ( imageRenderingComposition == mImageRenderingComposition )
        return;

    mImageRenderingComposition = imageRenderingComposition;

    mEditor->SanitizeCurrentTool();
}

#undef LOCTEXT_NAMESPACE
