// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "AnimationEditor/OdysseyAnimationEditorSource.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationTexture.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyMediaVector.h"
#include "ULISLoaderModule.h"
#include "Undo/OdysseyVectorUndoEngineClear.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyAnimationCurrentFrameMutator.h"

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
    , mTexture( NewObject<UOdysseyAnimationTexture>() )
	, mPlayer ( NewObject<UOdysseyAnimationPlayer>() )
{
	mPlayer->SetRenderType(IOdysseyImageRenderer::eRenderType::Editor);
}

const FGuid&
FOdysseyAnimationEditorSource::Id() const
{
	return StaticId();
}

int
FOdysseyAnimationEditorSource::Width() const
{
	return mAnimation->Width();
}

int
FOdysseyAnimationEditorSource::Height() const
{
	return mAnimation->Height();
}

void
FOdysseyAnimationEditorSource::Activate()
{
	AddEditedObject(mAnimation);

	FOdysseyObjectEditorUtils::SetPropertyValue(mPlayer, "Animation", mAnimation);
	mTexture->SetPlayer(mPlayer);
	mTexture->UpdateResource();

	//Seek at current frame 
    mPlayer->SeekToFrame(mAnimation->CurrentFrame);
	mPlayer->OnPlay().AddRaw(this, &FOdysseyAnimationEditorSource::OnPlayerPlay);
	mPlayer->OnStop().AddRaw(this, &FOdysseyAnimationEditorSource::OnPlayerStop);

	mAnimation->OnCurrentFrameChanged().AddRaw(this, &FOdysseyAnimationEditorSource::OnCurrentFrameChanged);

    FOdysseyPainterEditorSource::Activate();
}

void
FOdysseyAnimationEditorSource::Inactivate()
{
	RemoveEditedObject(mAnimation);

	mPlayer->OnPlay().RemoveAll(this);
	mPlayer->OnStop().RemoveAll(this);
	mPlayer->Stop();
	FOdysseyObjectEditorUtils::SetPropertyValue(mPlayer, "Animation", nullptr);

	mAnimation->OnCurrentFrameChanged().RemoveAll(this);

    FOdysseyPainterEditorSource::Inactivate();
}

UTexture*
FOdysseyAnimationEditorSource::DisplayTexture() const
{
    return mTexture;
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

UOdysseyAnimationTexture*
FOdysseyAnimationEditorSource::GetAnimationTexture() const
{
	return mTexture;
}

UOdysseyAnimationPlayer*
FOdysseyAnimationEditorSource::GetAnimationPlayer() const
{
	return mPlayer;
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
	Collector.AddReferencedObject(mTexture);
	Collector.AddReferencedObject(mPlayer);
}

void
FOdysseyAnimationEditorSource::OnCurrentFrameChanged(UOdysseyAnimation* iAnimation)
{
	if (iAnimation != mAnimation)
		return;

	//Display the new current frame
	mPlayer->SeekToFrame(mAnimation->CurrentFrame);
	mPlayer->Stop();
}

void
FOdysseyAnimationEditorSource::OnPlayerPlay()
{
	if (!mAnimation)
		return;

	mPlayer->SetRenderType(IOdysseyImageRenderer::eRenderType::Render);
}

void
FOdysseyAnimationEditorSource::OnPlayerStop()
{
	if (!mAnimation)
		return;

	mPlayer->SetRenderType(IOdysseyImageRenderer::eRenderType::Editor);
	mPlayer->SeekToFrame(mAnimation->CurrentFrame);
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
				FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
					[&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
					{
						::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
						::ULIS::FEvent eventClear;
						ctx.Clear(*iBlock, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear);
						return { eventClear };
					}
				)
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

void 
FOdysseyAnimationEditorSource::PasteBlockToNewLayer( TSharedPtr<::ULIS::FBlock> iBlock )
{
	/*
		if (!mCopyBlock)
			return;

		if (GetCurrentMediaProvider().IsLocked())
			return;

		TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaRaster>();
		if (mediaRasters.Num() <= 0)
			return;

		TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();

		LayerStack()->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass(), LayerStack()->CurrentLayer);

		::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mCopyBlock->Format());

		ctx.Blend(
			*mCopyBlock,
			*rasterBlock->GetBlock(),
			mCopyBlock->Rect(),
			::ULIS::FVec2I(0, 0),
			::ULIS::Blend_Normal,
			::ULIS::Alpha_Normal,
			1.f,
			::ULIS::FSchedulePolicy::AsyncCacheEfficient,
			0,
			nullptr,
			nullptr
		);

		ctx.Finish();*/
}

void
FOdysseyAnimationEditorSource::RecordCurrentFrameUndo() const
{
	FOdysseyAnimationCurrentFrameMutator currentFrameMutator(mAnimation);
	currentFrameMutator.Set(mAnimation->CurrentFrame);
	currentFrameMutator.Commit();
}

#undef LOCTEXT_NAMESPACE