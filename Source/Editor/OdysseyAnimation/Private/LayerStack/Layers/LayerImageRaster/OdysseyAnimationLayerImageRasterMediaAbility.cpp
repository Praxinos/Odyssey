// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterMediaAbility.h"
#include "OdysseyLayerFunctionLibrary.h"
#include "OdysseyRectUtils.h"

FOdysseyAnimationLayerImageRasterMediaAbility::FOdysseyAnimationLayerImageRasterMediaAbility(UOdysseyAnimationLayerImageRaster* iLayer)
    : mLayer(iLayer)
{
}

TSharedPtr<IOdysseyMedia>
FOdysseyAnimationLayerImageRasterMediaAbility::GetCellMediaRaster(uint32 iFrameIndex) const
{
	int celFrameIndex = INDEX_NONE;
    TSharedPtr<FOdysseyAnimationCell> cell = mLayer->GetCellAtFrame(iFrameIndex, celFrameIndex);
    if (!cell || celFrameIndex != 0)
        return nullptr;

    TSharedPtr<IOdysseyAnimationMediaAbility> cellAbility = cell->GetAbility<IOdysseyAnimationMediaAbility>();
    if (!cellAbility)
        return nullptr;

    FOdysseyMediaProvider provider = cellAbility->GetMediaProvider(celFrameIndex);
    if (!provider.HasMedia<FOdysseyMediaRaster>())
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasterList = provider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasterList.Num() <= 0)
        return nullptr;

	return mediaRasterList[0];
}

FOdysseyMediaProvider
FOdysseyAnimationLayerImageRasterMediaAbility::GetMediaProvider(uint32 iFrameIndex) const
{
	FOdysseyMediaProvider provider;
	TSharedPtr<IOdysseyMedia> cellMediaRaster = GetCellMediaRaster(iFrameIndex);
	if (cellMediaRaster)
	{
		bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(mLayer);
		bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(mLayer);
		cellMediaRaster->IsLocked(!isActive || isLocked);
		provider.Add(cellMediaRaster);
	}
	else
	{
		FOdysseyMediaProvider::FCreateMediaDelegate createMediaRasterDelegate = FOdysseyMediaProvider::FCreateMediaDelegate::CreateRaw(this, &FOdysseyAnimationLayerImageRasterMediaAbility::CreateMediaRaster, (int)iFrameIndex);
		provider.Add(FOdysseyMediaRaster::StaticId(), createMediaRasterDelegate);
	}
    return provider;
}

TSharedPtr<IOdysseyMedia>
FOdysseyAnimationLayerImageRasterMediaAbility::CreateMediaRaster(int iFrameIndex) const
{
    //Create the cell if needed
    AutoCreateCell(iFrameIndex);

    int celFrameIndex = INDEX_NONE;
    TSharedPtr<FOdysseyAnimationCell> cell = mLayer->GetCellAtFrame(iFrameIndex, celFrameIndex);
    if (!cell)
        return nullptr;

    TSharedPtr<IOdysseyAnimationMediaAbility> cellAbility = cell->GetAbility<IOdysseyAnimationMediaAbility>();
    if (!cellAbility)
        return nullptr;

    FOdysseyMediaProvider provider = cellAbility->GetMediaProvider(celFrameIndex);
    if (!provider.HasMedia<FOdysseyMediaRaster>())
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasterList = provider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasterList.Num() <= 0)
        return nullptr;

    TSharedPtr<FOdysseyMediaRaster> mediaRaster = mediaRasterList[0];

    bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(mLayer);
    bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(mLayer);

    mediaRaster->IsLocked(!isActive || isLocked);
    return mediaRaster;
}

void
FOdysseyAnimationLayerImageRasterMediaAbility::AutoCreateCell(int iFrameIndex) const
{
    UOdysseyAnimation* animation = mLayer->GetAnimation();
	FInt32Range range = mLayer->GetFrameRange();
	if ( iFrameIndex < range.GetLowerBoundValue())
	{
		//Add a frame at current frame and extend it 
		TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(mLayer, animation->Width(), animation->Height(), animation->Format());
        cell->SetLength(range.GetLowerBoundValue() - iFrameIndex);

		FOdysseyAnimationCellsMutator mutator(mLayer);
		mutator.Add({cell}, 0);
		mutator.SetOffset(mLayer->GetOffset() - cell->GetLength());
		mutator.Commit();
	}
	else if ( iFrameIndex > range.GetUpperBoundValue())
	{
		//Add a frame at current frame and extend previous frame to it 
		TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(mLayer, animation->Width(), animation->Height(), animation->Format());
        cell->SetLength(1);
		
		FOdysseyAnimationCellsMutator mutator(mLayer);

		int lastCellIndex = mLayer->GetCellsCount() - 1;
		if ( lastCellIndex >= 0 )
		{
			int cellLength;
			if ( mLayer->GetCellLength(lastCellIndex, cellLength) )
			{
				mutator.SetLength(lastCellIndex, cellLength + iFrameIndex - range.GetUpperBoundValue() - 1);
			}
		}

		mutator.Add({cell});
		mutator.Commit();
	}
	else
	{
		int cellIndex;
		int cellFrameIndex;
		if(mLayer->GetCellIndexAtFrame(iFrameIndex, cellIndex, cellFrameIndex))
		{
			if (cellIndex >= 0 && cellFrameIndex != 0)
			{
				TSharedPtr<FOdysseyAnimationCell> currentCell = mLayer->GetCell(cellIndex);
				//Here we need to break the instance
				//We get the render of the current frame, and create a raster cell to draw on it

				TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), animation->Format());
				TSharedPtr<IOdysseyImageRenderer> renderer = currentCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, cellFrameIndex);
				renderer->Copy(block, block->Rect(), {});
				::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
				ctx.Finish();

				int currentCellLength = cellFrameIndex;
				int newCellLength = currentCell->GetLength() - currentCellLength;

				TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(mLayer, block);
				cell->SetLength(newCellLength);

				FOdysseyAnimationCellsMutator mutator(mLayer);
				mutator.SetLength(cellIndex, currentCellLength);
				mutator.Add({ cell }, cellIndex + 1);
				mutator.Commit();
			}
		}
	}
}
