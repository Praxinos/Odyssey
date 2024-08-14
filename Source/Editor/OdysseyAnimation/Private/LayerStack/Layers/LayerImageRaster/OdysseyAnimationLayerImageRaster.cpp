// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImageRenderer.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "OdysseyRasterBlockMutator.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterExport.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImport.h"
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "OdysseyAnimation.h"
#include "OdysseyRasterBlock.h"
#include "LayerStack/Cells/OdysseyAnimationCellsMutator.h"
#include "OdysseyMediaRaster.h"

#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimationLayerImageRaster::~UOdysseyAnimationLayerImageRaster()
{
}

UOdysseyAnimationLayerImageRaster::UOdysseyAnimationLayerImageRaster()
    : mCellsContainer(MakeShared<FOdysseyAnimationCellsContainer>())
{
	LayerTypeName = LOCTEXT("layer-image-raster.type", "Raster Image Layer");
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerBitmap16");

    mCellsContainer->AddSupportedType(FOdysseyAnimationCellImageRaster::StaticType());
    mCellsContainer->AddSupportedType(FOdysseyAnimationCellImageStagger::StaticType());
}

void
UOdysseyAnimationLayerImageRaster::OnCreated_Implementation()
{
}

FInt32Range
UOdysseyAnimationLayerImageRaster::GetFrameRange() const
{
    return mCellsContainer->GetFrameRange();
}

TSharedPtr<FOdysseyAnimationLightTable>
UOdysseyAnimationLayerImageRaster::GetLightTable() const
{
    return mLightTable;
}

TSharedPtr<FOdysseyAnimationCellsContainer>
UOdysseyAnimationLayerImageRaster::GetCellsContainer() const
{
    return mCellsContainer;
}

void
UOdysseyAnimationLayerImageRaster::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    if (IsLocked)
        return;

    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    //Get all frame ranges and combine them
    TArray<FInt32Range> frameRanges = {};
    for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
    {
        UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iLayers[layerIndex]);
        if ( !layer )
            continue;

        frameRanges.Add(layer->GetFrameRange());
    }
    FInt32Range frameRange = FInt32Range::Hull(frameRanges);

    //Deduce offset from frame ranges
    int offset = frameRange.GetLowerBoundValue();

    //Get cell ranges from each frame ImageRenderAbility composition
    int startFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetLowerBoundValue() : frameRange.GetLowerBoundValue() + 1;
	int endFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetUpperBoundValue() : frameRange.GetUpperBoundValue() - 1;
    TArray<FGuid> previousIds;
    TArray<FInt32Range> cellRanges;
    for (int frameIndex = startFrame; frameIndex <= endFrame; frameIndex++)
    {
        TArray<FGuid> currentIds;
        for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
        {
            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iLayers[layerIndex]);
            if ( !layer )
                continue;

            currentIds.Append(layer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, frameIndex));
        }

        //Do we need a new cell
        if (currentIds != previousIds)
        {
            //do we already have a cell at this position
            cellRanges.Add(FInt32Range::Inclusive(frameIndex, frameIndex));
            previousIds = currentIds;
        }
        else
        {
            cellRanges.Last().SetUpperBoundValue(frameIndex);
        }
    }

    TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    
    for (const FInt32Range& cellRange : cellRanges)
    {
        int cellLength = cellRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue() + 1;
        TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(this, cellLength, animation->Width(), animation->Height(), animation->Format());

        TSharedPtr<FOdysseyRasterBlock> rasterBlock = cell->GetRasterBlock();
        FOdysseyRasterBlockMutator blockMutator(rasterBlock);

        int frame = cellRange.GetLowerBoundValue();
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight());

        blockMutator.EditTilesFromRects(
            { rect },
            FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
                [&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
                {
                    TArray<::ULIS::FEvent> lastEvent;
                    for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
                    {
                        UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iLayers[layerIndex]);
                        if ( !layer )
                            continue;

                        TSharedPtr<IOdysseyImageRenderer> renderer = layer->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, frame);
                        renderer->Init();

                        FOdysseyImageRendererBlendParams params(iBlock, { rect });
                        params.mBlendMode = (::ULIS::eBlendMode)layer->BlendMode;
                        params.mOpacity = layer->Opacity;
                        lastEvent = renderer->Blend(params, lastEvent);
                    }

                    return lastEvent;
                }
            )
        );

        cells.Add(cell);
    }

    ctx.Finish();
    
    FOdysseyAnimationCellsMutator mutator(this, mCellsContainer);
    mutator.Remove(0, mCellsContainer->GetCells().Num());
    mutator.Add(cells);
    mutator.SetOffset(offset);
    mutator.Commit();
}

void
UOdysseyAnimationLayerImageRaster::OnCellsChanged()
{
    //OnCellsChanged().Broadcast(this);
    ImageRenderingCompositionChanged();
    UOdysseyLayer::OnMediaChanged().Broadcast();
}

TSharedPtr<FOdysseyAnimationCell>
UOdysseyAnimationLayerImageRaster::CreateCell( const FName& iCellType, bool iForSerialization)
{
    if (iCellType == FOdysseyAnimationCellImageRaster::StaticType())
    {
        if (iForSerialization)
            return MakeShared<FOdysseyAnimationCellImageRaster>(this, 1);

        UOdysseyAnimation* animation = GetAnimation();
        return FOdysseyAnimationCellImageRaster::Create(this, 1, animation->Width(), animation->Height(), animation->Format());
    }
    else if (iCellType == FOdysseyAnimationCellImageStagger::StaticType())
    {
        if (iForSerialization)
            return FOdysseyAnimationCellImageStagger::Create(this, 1);
    }
    return nullptr;
}

void
UOdysseyAnimationLayerImageRaster::PostInitProperties()
{
    Super::PostInitProperties();

    mCellsContainer->CreateCellDelegate().BindUObject(this, &UOdysseyAnimationLayerImageRaster::CreateCell);
    mCellsContainer->OnCellsChanged().AddUObject(this, &UOdysseyAnimationLayerImageRaster::OnCellsChanged);
    mLightTable = MakeShared<FOdysseyAnimationLightTable>(this);
}

void
UOdysseyAnimationLayerImageRaster::PostDuplicate(bool bDuplicateForPIE)
{
    Super::PostDuplicate(bDuplicateForPIE);
    mCellsContainer->PostDuplicate();
}

void
UOdysseyAnimationLayerImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationLayerImageRasterExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationLayerImageRasterImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            mCellsContainer->Serialize(Ar);
        }
    }
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyAnimationLayerImageRaster::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;
    
    return MakeShared<FOdysseyAnimationLayerImageRasterImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects(), iFilter);
}

TArray<FGuid>
UOdysseyAnimationLayerImageRaster::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };

    if (iRenderType == IOdysseyImageRenderer::eRenderType::Editor && bIsLightTableActivated && mLightTable->GetDisplayPosition() == EOdysseyLightTableDisplayPosition::UnderLayer )
    {
        idComposition.Append(mLightTable->GetImageRenderingComposition(iRenderType, iFrameIndex));
    }

    int frame = iFrameIndex;
    FInt32Range frameRange = GetFrameRange();
    if (iFrameIndex < frameRange.GetLowerBoundValue())
    {
        frame = GetPreBehaviourFrame(PreBehaviour, iFrameIndex);
    }
    else if (iFrameIndex > frameRange.GetUpperBoundValue())
    {
        frame = GetPostBehaviourFrame(PostBehaviour, iFrameIndex);
    }

    int celFrameIndex = mCellsContainer->GetCellFrameAtFrame(frame);
    TSharedPtr<FOdysseyAnimationCell> cell = mCellsContainer->GetCellAtFrame(frame);
    if (cell)
    {
        idComposition.Append(cell->GetImageRenderingComposition(iRenderType, celFrameIndex));
    }

    if (iRenderType == IOdysseyImageRenderer::eRenderType::Editor && bIsLightTableActivated && mLightTable->GetDisplayPosition() == EOdysseyLightTableDisplayPosition::AboveLayer )
    {
        idComposition.Append(mLightTable->GetImageRenderingComposition(iRenderType, iFrameIndex));
    }

    return idComposition;
}

TSharedPtr<IOdysseyMedia>
UOdysseyAnimationLayerImageRaster::GetCellMediaRaster(uint32 iFrameIndex) const
{
	int celFrameIndex = mCellsContainer->GetCellFrameAtFrame(iFrameIndex);
    TSharedPtr<FOdysseyAnimationCell> cell = mCellsContainer->GetCellAtFrame(iFrameIndex);
    if (!cell)
        return nullptr;

    FOdysseyMediaProvider provider = cell->GetMediaProvider(celFrameIndex);
    if (!provider.HasMedia<FOdysseyMediaRaster>())
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasterList = provider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasterList.Num() <= 0)
        return nullptr;

	return mediaRasterList[0];
}

FOdysseyMediaProvider
UOdysseyAnimationLayerImageRaster::GetMediaProvider(uint32 iFrameIndex) const
{
	FOdysseyMediaProvider provider;

    bool isActive = GetIsActivatedRecursively();
    bool isLocked = GetIsLockedRecursively();
    provider.IsLocked(!isActive || isLocked);

	TSharedPtr<IOdysseyMedia> cellMediaRaster = GetCellMediaRaster(iFrameIndex);
	if (cellMediaRaster)
	{
		provider.Add(cellMediaRaster);
	}
	else
	{
		FOdysseyMediaProvider::FCreateMediaDelegate createMediaRasterDelegate = FOdysseyMediaProvider::FCreateMediaDelegate::CreateUObject(const_cast<UOdysseyAnimationLayerImageRaster*>(this), &UOdysseyAnimationLayerImageRaster::CreateMediaRaster, (int)iFrameIndex);
		provider.Add(FOdysseyMediaRaster::StaticId(), createMediaRasterDelegate);
	}
    return provider;
}

TSharedPtr<IOdysseyMedia>
UOdysseyAnimationLayerImageRaster::CreateMediaRaster(int iFrameIndex)
{
    //Create the cell if needed
    AutoCreateCell(iFrameIndex);

    int celFrameIndex = mCellsContainer->GetCellFrameAtFrame(iFrameIndex);
    TSharedPtr<FOdysseyAnimationCell> cell = mCellsContainer->GetCellAtFrame(iFrameIndex);
    if (!cell)
        return nullptr;

    FOdysseyMediaProvider provider = cell->GetMediaProvider(celFrameIndex);
    if (!provider.HasMedia<FOdysseyMediaRaster>())
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasterList = provider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasterList.Num() <= 0)
        return nullptr;

    TSharedPtr<FOdysseyMediaRaster> mediaRaster = mediaRasterList[0];
    return mediaRaster;
}

void
UOdysseyAnimationLayerImageRaster::AutoCreateCell(int iFrameIndex)
{
    if (IsLocked)
        return;

    FScopedTransaction transaction(LOCTEXT("layer-image-raster.create-cell-transaction", "Create Cell"));

    UOdysseyAnimation* animation = GetAnimation();
    //Check if iFrameIndex is Out Of Range
    FInt32Range range = mCellsContainer->GetFrameRange();
    if ( iFrameIndex < range.GetLowerBoundValue())
    {
        int length = range.GetLowerBoundValue() - iFrameIndex;

        //Add a frame at current frame and extend it 
        TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(this, length, animation->Width(), animation->Height(), animation->Format());

        FOdysseyAnimationCellsMutator mutator(this, mCellsContainer);
        mutator.Add({cell}, 0);
        mutator.SetOffset(mCellsContainer->GetOffset() - length);
        mutator.Commit();

        FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
        currentFrameMutator.Set(animation->CurrentFrame);
        currentFrameMutator.Commit();
        return;
    }

    if ( iFrameIndex > range.GetUpperBoundValue())
    {
        //Add a frame at current frame and extend previous frame to it 
        TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(this, 1, animation->Width(), animation->Height(), animation->Format());
        
        int cellCount = mCellsContainer->GetCells().Num();
        int lastCellLength = mCellsContainer->GetCells().Last()->GetLength();

        FOdysseyAnimationCellsMutator mutator(this, mCellsContainer);
        mutator.SetLength( cellCount - 1, lastCellLength + iFrameIndex - range.GetUpperBoundValue() - 1);
        mutator.Add({cell});
        mutator.Commit();
        
        FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
        currentFrameMutator.Set(animation->CurrentFrame);
        currentFrameMutator.Commit();
        return;
    }
}

#undef LOCTEXT_NAMESPACE