// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "TextureEditor/OdysseyTextureEditorSource.h"
#include "Undo/OdysseyVectorUndoEngineClear.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorSource"

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

void
FOdysseyTextureEditorSource::Activate()
{
    InitTextureUserData();

	UOdysseyTextureLayerStack* layerStack = GetLayerStack();
	if ( layerStack )
    {
		mLayerStackPreloadHandle = layerStack->Preload();
		layerStack->ActivateTextureFastUpdate();
    }

    FOdysseyPainterEditorSource::Activate();
}

void
FOdysseyTextureEditorSource::Inactivate()
{
	UOdysseyTextureLayerStack* layerStack = GetLayerStack();
	if ( layerStack )
    {
        mLayerStackPreloadHandle = nullptr;
		layerStack->InactivateTextureFastUpdate();
    }

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

    UOdysseyTextureLayerImageRaster* currentLayerRaster = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CurrentLayer.Get());
    UOdysseyTextureLayerImageVector* currentLayerVector = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

	if (currentLayerRaster)
	{		
	#ifdef WITH_EDITOR
		FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Clear Canvas"));
	#endif
		
		TSharedPtr<FOdysseyRasterBlock> rasterBlock = currentLayerRaster->GetRasterBlock();
		FOdysseyRasterBlockMutator mutator(rasterBlock);
		mutator.EditTilesFromRects(
			{ ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight()) },
			FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
				[&](const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
				{
					TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = rasterBlock->GetBlock();
					::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
					::ULIS::FEvent clearEvent;
					ctx.Clear(*ULISRasterBlock, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &clearEvent);
					return { clearEvent };
				}
			)
		);
		mutator.Commit();
	}

    if( currentLayerVector )
    {
        FOdysseyVectorEngine* vectorEngine = currentLayerVector->GetEngine();

        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("ClearVectorScene", "Clear Vector Scene"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoEngineClear( vectorEngine );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        vectorEngine->SetScene( new FOdysseyVectorScene("Scene") );

        currentLayerVector->RenderImageChanged(false);
    }
}

#undef LOCTEXT_NAMESPACE