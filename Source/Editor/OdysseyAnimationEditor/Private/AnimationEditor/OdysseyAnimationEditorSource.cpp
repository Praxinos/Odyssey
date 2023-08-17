// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "AnimationEditor/OdysseyAnimationEditorSource.h"
#include "OdysseyAnimationTexture.h"

const FGuid&
FOdysseyAnimationEditorSource::StaticId()
{
	static FGuid id;
	return id;
}

FOdysseyAnimationEditorSource::~FOdysseyAnimationEditorSource()
{

}

FOdysseyAnimationEditorSource::FOdysseyAnimationEditorSource(UOdysseyAnimation* iAnimation)
    : mAnimation( iAnimation )
    , mTexture( NewObject<UOdysseyAnimationTexture>() )
{
}

const FGuid&
FOdysseyAnimationEditorSource::Id() const
{
	return StaticId();
}

void
FOdysseyAnimationEditorSource::Activate()
{
    FOdysseyPainterEditorSource::Activate();
}

void
FOdysseyAnimationEditorSource::Inactivate()
{
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

FOdysseyMediaProvider
FOdysseyAnimationEditorSource::GetCurrentMediaProvider()
{
	UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(GetLayerStack()->CurrentLayer.Get());
	if (!currentLayer)
		return FOdysseyMediaProvider();

	TSharedPtr<IOdysseyAnimationMediaAbility> mediaAbility = currentLayer->GetAbility<IOdysseyAnimationMediaAbility>();
	if ( !mediaAbility )
		return FOdysseyMediaProvider();

	return mediaAbility->GetMediaProvider(mAnimation->CurrentFrame);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyAnimationEditorSource::AddReferencedObjects(FReferenceCollector& Collector)
{
	FOdysseyPainterEditorSource::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(mTexture);
}
