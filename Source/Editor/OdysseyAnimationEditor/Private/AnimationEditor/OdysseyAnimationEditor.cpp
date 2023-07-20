// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditor.h"

#include "AnimationEditor/OdysseyAnimationEditorGUI.h"
#include "OdysseyLayerFunctionLibrary.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationCellsMutator.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimationTexture.h"
#include "OdysseyMediaRaster.h"
#include "Abilities/IOdysseyAnimationImageRenderingAbility.h"
#include "Abilities/IOdysseyAnimationMediaAbility.h"
#include "ULISLoaderModule.h"


#define LOCTEXT_NAMESPACE "OdysseyAnimationEditor"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditor::~FOdysseyAnimationEditor()
{
	mPlayer->OnStop().RemoveAll(this);
	mPlayer->Stop();
	mAnimation->OnCurrentFrameChanged().RemoveAll(this);
	IOdysseyAnimationImageRenderingAbility::OnCompositionCommited().RemoveAll(this);
	UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
}

FOdysseyAnimationEditor::FOdysseyAnimationEditor() :
	FOdysseyPainterEditor(),
	mAnimation(nullptr),
	mGUI(nullptr),
	mPlayer(nullptr),
    mTexture(),
	mPlaybackFramesPerSecond(0)
{
	UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyAnimationEditor::OnCurrentLayerChanged);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyAnimationEditor::InitData(UObject* iEditedObject)
{
	mAnimation = Cast<UOdysseyAnimation>(iEditedObject);

	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = mAnimation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	mImageRenderingComposition = imageRenderAbility->GetComposition(mAnimation->CurrentFrame, IOdysseyImageRenderer::eRenderType::Render);
	mPlaybackFramesPerSecond = mAnimation->GetFramesPerSecond();

	//Configure a Media player and media texture to be able to display and play the animation
	mPlayer = NewObject<UOdysseyAnimationPlayer>();
    mTexture = NewObject<UOdysseyAnimationTexture>();

	mPlayer->SetAnimation(mAnimation);
	mPlayer->SetRenderType(IOdysseyImageRenderer::eRenderType::Editor);
	mTexture->SetPlayer(mPlayer);
	mTexture->UpdateResource();

	//Seek at current frame 
    mPlayer->SeekToFrame(mAnimation->CurrentFrame);

	//Set Media player and Animation callbacks
	mPlayer->OnStop().AddRaw(this, &FOdysseyAnimationEditor::OnPlayerStop);
	mAnimation->OnCurrentFrameChanged().AddRaw(this, &FOdysseyAnimationEditor::OnCurrentFrameChanged);
	IOdysseyAnimationImageRenderingAbility::OnCompositionCommited().AddRaw(this, &FOdysseyAnimationEditor::OnImageRenderingCompositionCommited);
	IOdysseyAnimationMediaAbility::OnChanged().AddRaw(this, &FOdysseyAnimationEditor::OnLayerStackElementMediaChanged);

	FOdysseyPainterEditor::InitData(iEditedObject);
}

void
FOdysseyAnimationEditor::InitTools()
{
	FOdysseyPainterEditor::InitTools();
	//UpdateToolContext();

	/* mToolContext->GetRasterBlockAttribute().BindRaw(this, &FOdysseyAnimationEditor::GetCurrentRasterBlock);
    mToolContext->GetCanProvideRasterBlockOnDemandAttribute().BindRaw(this, &FOdysseyAnimationEditor::CanProvideRasterBlockOnDemand);
	mToolContext->GetIsRasterBlockReadOnlyAttribute().BindRaw(this, &FOdysseyAnimationEditor::IsRasterBlockReadOnly);
	mToolContext->OnProvideRasterBlockDelegate().BindRaw(this, &FOdysseyAnimationEditor::ProvideRasterBlock);
    mToolContext->GetVectorEngineAttribute().BindRaw(this, &FOdysseyAnimationEditor::GetCurrentVectorEngine); */
}

void
FOdysseyAnimationEditor::Tick(float iDeltaTime)
{
	//UpdateToolContext();
	FOdysseyPainterEditor::Tick(iDeltaTime);
}

/* void
FOdysseyAnimationEditor::UpdateToolContext()
{
	FOdysseyPainterEditorToolContext::FParams toolContextParams;
	toolContextParams.mRasterBlock = GetCurrentRasterBlock();
	toolContextParams.mCanProvideRasterBlockOnDemand = CanProvideRasterBlockOnDemand();
	toolContextParams.mIsRasterBlockReadOnly = IsRasterBlockReadOnly();
	toolContextParams.mOnProvideRasterBlockDelegate.BindRaw(this, &FOdysseyAnimationEditor::ProvideRasterBlock);
	toolContextParams.mVectorEngine = GetCurrentVectorEngine();
	
	mToolContext->Set(toolContextParams);
} */

void
FOdysseyAnimationEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyPainterEditor::BindShortcuts(iToolkit);
}

void
FOdysseyAnimationEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
	FOdysseyPainterEditor::ExtendMenu(iOwner, iMenuName);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UOdysseyAnimation*
FOdysseyAnimationEditor::Animation() const
{
	return mAnimation;
}

UOdysseyAnimationLayerStack*
FOdysseyAnimationEditor::LayerStack() const
{
	return mAnimation->GetLayerStack();
}

UOdysseyAnimationPlayer*
FOdysseyAnimationEditor::Player() const
{
	return mPlayer;
}

FOdysseyAnimationEditorTimeline*
FOdysseyAnimationEditor::Timeline()
{
	return &mTimeline;
}

float
FOdysseyAnimationEditor::PlaybackFramesPerSecond() const
{
	return mPlaybackFramesPerSecond;
}

UTexture*
FOdysseyAnimationEditor::DisplayTexture() const
{
	return mTexture;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyAnimationEditor::GetDisplayBlock()
{
	//TODO: this is used only for picking a color in PainterEditor's viewport tab
	//Find a way to do it without having that method

	return nullptr;
}

void
FOdysseyAnimationEditor::OnLayerStackElementMediaChanged()
{
	SelectDefaultTool(); //Refresh the current tool
    Timeline()->SetSelectedFrames(FInt32Range()); //Clear Selected frames when changing layer
}

FOdysseyMediaProvider
FOdysseyAnimationEditor::GetCurrentMediaProvider()
{
	UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(LayerStack()->CurrentLayer.Get());
	if (!currentLayer)
		return FOdysseyMediaProvider();

	TSharedPtr<IOdysseyAnimationMediaAbility> mediaAbility = currentLayer->GetAbility<IOdysseyAnimationMediaAbility>();
	if ( !mediaAbility )
		return FOdysseyMediaProvider();

	return mediaAbility->GetMediaProvider(Animation()->CurrentFrame);
}

/* TSharedPtr<FOdysseyRasterBlock>
FOdysseyAnimationEditor::GetCurrentRasterBlock() const
{
	UOdysseyAnimationLayerImageRaster* currentLayerRaster = Cast<UOdysseyAnimationLayerImageRaster>(LayerStack()->CurrentLayer.Get());
	if (!currentLayerRaster)
		return nullptr;

	int celFrameIndex = INDEX_NONE;
	TSharedPtr<FOdysseyAnimationCell> cell = currentLayerRaster->GetCellAtFrame(Animation()->CurrentFrame, celFrameIndex);
	if (!cell)
		return nullptr;
	
	TSharedPtr<IOdysseyAnimationImageRasterEditingAbility> rasterEditableAbility = cell->GetAbility<IOdysseyAnimationImageRasterEditingAbility>();
	if ( !rasterEditableAbility )
		return nullptr;
	
	return rasterEditableAbility->GetRasterBlock(celFrameIndex);
} */

bool
FOdysseyAnimationEditor::CanProvideRasterBlockOnDemand() const
{
	UOdysseyAnimationLayerImageRaster* currentLayerRaster = Cast<UOdysseyAnimationLayerImageRaster>(LayerStack()->CurrentLayer.Get());
	if (!currentLayerRaster)
		return false;

	return true;
}

bool
FOdysseyAnimationEditor::IsRasterBlockReadOnly() const
{
	UOdysseyAnimationLayerImageRaster* currentLayerRaster = Cast<UOdysseyAnimationLayerImageRaster>(LayerStack()->CurrentLayer.Get());
	if (!currentLayerRaster)
		return false;

	bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(currentLayerRaster);
	bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(currentLayerRaster);

	return !isActive || isLocked;
}

void
FOdysseyAnimationEditor::ProvideRasterBlock()
{
	UOdysseyAnimationLayerImageRaster* currentLayerRaster = Cast<UOdysseyAnimationLayerImageRaster>(LayerStack()->CurrentLayer.Get());
	if (!currentLayerRaster)
		return;

	FInt32Range range = currentLayerRaster->GetFrameRange();

	if ( mAnimation->CurrentFrame < range.GetLowerBoundValue())
	{
		//Add a frame at current frame and extend it 
		TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(currentLayerRaster, mAnimation->Width(), mAnimation->Height(), mAnimation->Format());
        cell->SetLength(range.GetLowerBoundValue() - mAnimation->CurrentFrame);

		FOdysseyAnimationCellsMutator mutator(currentLayerRaster);
		mutator.Add({cell}, 0);
		mutator.SetOffset(currentLayerRaster->GetOffset() - cell->GetLength());
		mutator.Commit();
	}
	else if ( mAnimation->CurrentFrame > range.GetUpperBoundValue())
	{
		//Add a frame at current frame and extend previous frame to it 
		TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(currentLayerRaster, mAnimation->Width(), mAnimation->Height(), mAnimation->Format());
        cell->SetLength(1);
		
		FOdysseyAnimationCellsMutator mutator(currentLayerRaster);

		int lastCellIndex = currentLayerRaster->GetCellsCount() - 1;
		if ( lastCellIndex >= 0 )
		{
			int cellLength;
			if ( currentLayerRaster->GetCellLength(lastCellIndex, cellLength) )
			{
				mutator.SetLength(lastCellIndex, cellLength + mAnimation->CurrentFrame - range.GetUpperBoundValue() - 1);
			}
		}

		mutator.Add({cell});
		mutator.Commit();
	}
	else
	{
		int cellIndex;
		int cellFrameIndex;
		if(currentLayerRaster->GetCellIndexAtFrame(mAnimation->CurrentFrame, cellIndex, cellFrameIndex))
		{
			if (cellIndex >= 0 && cellFrameIndex != 0)
			{
				TSharedPtr<FOdysseyAnimationCell> currentCell = currentLayerRaster->GetCell(cellIndex);
				TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = currentCell->GetAbility<IOdysseyAnimationImageRenderingAbility>();
				if ( imageRenderAbility )
				{
					//Here we need to break the instance
					//We get the render of the current frame, and create a raster cell to draw on it

					TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(mAnimation->Width(), mAnimation->Height(), mAnimation->Format());
					TSharedPtr<IOdysseyImageRenderer> renderer = imageRenderAbility->BuildRenderer(cellFrameIndex, IOdysseyImageRenderer::eRenderType::Render);
					renderer->Copy(block, block->Rect(), {});
					::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
					ctx.Finish();

					int currentCellLength = cellFrameIndex;
					int newCellLength = currentCell->GetLength() - currentCellLength;

					TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(currentLayerRaster, block);
					cell->SetLength(newCellLength);

					FOdysseyAnimationCellsMutator mutator(currentLayerRaster);
					mutator.SetLength(cellIndex, currentCellLength);
					mutator.Add({ cell }, cellIndex + 1);
					mutator.Commit();
				}
			}
		}
	}
}

FOdysseyVectorEngine*
FOdysseyAnimationEditor::GetCurrentVectorEngine() const
{
	UOdysseyAnimationLayerImageVector* currentLayerVector = Cast<UOdysseyAnimationLayerImageVector>(LayerStack()->CurrentLayer.Get());
	if (!currentLayerVector)
		return nullptr;

	return currentLayerVector->GetEngine();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

FOdysseyAnimationEditorGUI*
FOdysseyAnimationEditor::GetGUI()
{
	if (!mGUI)
		mGUI = MakeShareable(new FOdysseyAnimationEditorGUI(this));
	return mGUI.Get();
}

TSharedPtr<FWorkspaceItem>
FOdysseyAnimationEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyAnimationEditor", "Odyssey Animation2D Editor"));
	TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
	GetGUI()->RegisterTabSpawners(iTabManager, workspaceMenuCategoryRef);
	return workspaceMenuCategory;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyAnimationEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	FOdysseyPainterEditor::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(mPlayer);
	Collector.AddReferencedObject(mTexture);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Events

void
FOdysseyAnimationEditor::OnPlayerStop()
{
	mPlayer->SeekToFrame(mAnimation->CurrentFrame);
}

void
FOdysseyAnimationEditor::OnImageRenderingCompositionCommited(const FGuid& iFrameId)
{
	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = mAnimation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	TArray<FGuid> imageRenderingComposition = imageRenderAbility->GetComposition(mAnimation->CurrentFrame, IOdysseyImageRenderer::eRenderType::Render);
	if ( imageRenderingComposition == mImageRenderingComposition )
		return;

	mImageRenderingComposition = imageRenderingComposition;

	//Reload the tool
	//TODO: we should maybe do this in a different way, it feels a bit weird to unselect and reselect the whole tool
	UOdysseyPainterEditorTool* tool = GetSelectedTool();
	SetSelectedTool(nullptr);
	SetSelectedTool(tool);
}

void
FOdysseyAnimationEditor::OnCurrentFrameChanged(UOdysseyAnimation* iAnimation)
{
	if (iAnimation != mAnimation)
		return;

	//Preload the new current frame for edition
	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = mAnimation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	mImageRenderingComposition = imageRenderAbility->GetComposition(mAnimation->CurrentFrame, IOdysseyImageRenderer::eRenderType::Render);

	//Display the new current frame
	mPlayer->SeekToFrame(mAnimation->CurrentFrame);
	mPlayer->Stop();

	//Reload the tool
	//TODO: we should maybe do this in a different way, it feels a bit weird to unselect and reselect the whole tool
	UOdysseyPainterEditorTool* tool = GetSelectedTool();
	SetSelectedTool(nullptr);
	SetSelectedTool(tool);
}

void
FOdysseyAnimationEditor::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	if ( iLayerStack != LayerStack() )
		return;
	//TODO: Maybe this should be done differently later, but we don't have time for that now
	SelectDefaultTool(); //Refresh the current tool when we change layer
    Timeline()->SetSelectedFrames(FInt32Range()); //Clear Selected frames when changing layer
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Common Actions

void
FOdysseyAnimationEditor::Clear()
{
    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(LayerStack()->CurrentLayer.Get());
	if (currentLayer)
	{		
	#ifdef WITH_EDITOR
		FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Clear"));
	#endif

		TSharedPtr<IOdysseyAnimationMediaAbility> mediaAbility = currentLayer->GetAbility<IOdysseyAnimationMediaAbility>();
		if ( mediaAbility )
		{
			FOdysseyMediaProvider mediaProvider = mediaAbility->GetMediaProvider(Animation()->CurrentFrame);
			if ( mediaProvider.HasMedia<FOdysseyMediaRaster>() )
			{
				TArray<TSharedPtr<FOdysseyMediaRaster>> mediasRaster = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
				for (TSharedPtr<FOdysseyMediaRaster> mediaRaster : mediasRaster)
				{
					if (mediaRaster->IsLocked())
						continue;

					TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRaster->GetRasterBlock();
					if (!rasterBlock)
						continue;

					FOdysseyRasterBlockMutator mutator(rasterBlock);
					mutator.EditTilesFromRects(
						{ ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight()) },
						FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
							[&](const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
							{
								TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = rasterBlock->GetBlock();
								::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
								::ULIS::FEvent eventClear;
								ctx.Clear(*ULISRasterBlock, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear);
								return { eventClear };
							}
						)
					);
					mutator.Commit();
				}
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE