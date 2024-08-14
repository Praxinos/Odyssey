// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorImageRenderer.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorExport.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorImport.h"
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "OdysseyAnimation.h"
#include "OdysseyMediaVector.h"
#include "LayerStack/Cells/OdysseyAnimationCellsMutator.h"

#define LOCTEXT_NAMESPACE "Animation"

void
UOdysseyAnimationLayerImageVector::PostInitProperties()
{
    Super::PostInitProperties();

	LayerTypeName = LOCTEXT("layer-image-vector.type", "Vector Image Layer");
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerVector16");

    SupportedCellTypes->Add(FOdysseyAnimationCellImageVector::StaticType());
    SupportedCellTypes->Add(FOdysseyAnimationCellImageStagger::StaticType());
}

void
UOdysseyAnimationLayerImageVector::IsColoredChanged()
{
    ImageRenderingChanged();
}

void
UOdysseyAnimationLayerImageVector::IsWireframeChanged()
{
    ImageRenderingChanged();
}

void
UOdysseyAnimationLayerImageVector::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
    Super::PropertyChanged(iPropertyName);

    if(iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayerImageVector, IsColored))
        IsColoredChanged();
    if(iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayerImageVector, IsWireframe))
        IsWireframeChanged();
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyAnimationLayerImageVector::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;
    
    return MakeShared<FOdysseyAnimationLayerImageVectorImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects(), iFilter);
}

TArray<FGuid>
UOdysseyAnimationLayerImageVector::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };

	bool showLighttable = iRenderType == IOdysseyImageRenderer::eRenderType::Editor && bIsLightTableActivated;
    if (showLighttable && mLightTable->GetDisplayPosition() == EOdysseyLightTableDisplayPosition::UnderLayer )
		idComposition.Append(GetLighttableImageRenderingComposition(iFrameIndex));

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

    int cellIndex = mCellsContainer->GetCellIdexAtFrame(frame);
	if (cellIndex != INDEX_NONE)
	{
    	UOdysseyAnimationCell* cell = Cells[cellIndex];
		int cellFrame = GetCellFrameAtFrame(frame);
        idComposition.Append(cell->GetImageRenderingComposition(iRenderType, cellFrame));
    }

    if (showLighttable && mLightTable->GetDisplayPosition() == EOdysseyLightTableDisplayPosition::AboveLayer )
        idComposition.Append(GetLighttableImageRenderingComposition(iFrameIndex));

    return idComposition;
}

TSharedPtr<IOdysseyMedia>
UOdysseyAnimationLayerImageVector::GetCellMediaVector(uint32 iFrameIndex) const
{
	int cellIndex = GetCellIndexAtFrame(iFrameIndex);
    if (!cellIndex)
        return nullptr;
	
    TSharedPtr<FOdysseyAnimationCell> cell = Cells[cellIndex];
	int cellFrame = GetCellFrameAtFrame(iFrameIndex);
    FOdysseyMediaProvider provider = cell->GetMediaProvider(cellFrame);
    if (!provider.HasMedia<FOdysseyMediaVector>())
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectorList = provider.GetOrCreateMedias<FOdysseyMediaVector>();
    if (mediaVectorList.Num() <= 0)
        return nullptr;

	return mediaVectorList[0];
}

FOdysseyMediaProvider
UOdysseyAnimationLayerImageVector::GetMediaProvider(uint32 iFrameIndex) const
{
	FOdysseyMediaProvider provider;

    bool isActive = GetIsActivatedRecursively();
    bool isLocked = GetIsLockedRecursively();
    provider.IsLocked(!isActive || isLocked);

	TSharedPtr<IOdysseyMedia> cellMediaVector = GetCellMediaVector(iFrameIndex);
	if (cellMediaVector)
	{
		provider.Add(cellMediaVector);
	}
	else
	{
		FOdysseyMediaProvider::FCreateMediaDelegate createMediaVectorDelegate = FOdysseyMediaProvider::FCreateMediaDelegate::CreateUObject(const_cast<UOdysseyAnimationLayerImageVector*>(this), &UOdysseyAnimationLayerImageVector::CreateMediaVector, (int)iFrameIndex);
		provider.Add(FOdysseyMediaVector::StaticId(), createMediaVectorDelegate);
	}
    return provider;
}

TSharedPtr<IOdysseyMedia>
UOdysseyAnimationLayerImageVector::CreateMediaVector(int iFrameIndex)
{
    //Create the cell if needed
    AutoCreateCell(iFrameIndex);
	GetCellMediaVector(iFrameIndex);
}

void
UOdysseyAnimationLayerImageVector::AutoCreateCell(int iFrameIndex)
{
    if (IsLocked)
        return;

    FScopedTransaction transaction(LOCTEXT("layer-image-vector.create-cell-transaction", "Create Cell"));
    UOdysseyAnimation* animation = GetAnimation();
    //Check if iFrameIndex is Out Of Range
	if ( iFrameIndex < range.GetLowerBoundValue())
    {
        //Add a frame at current frame and extend it
		UOdysseyAnimationCell* cell = AddCells(FOdysseyAnimationCellImageVector::StaticClass(), 0);
		FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Length), range.GetLowerBoundValue() - iFrameIndex);
		FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), CellsOffset - cell->Length);
        
        FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
        currentFrameMutator.Set(animation->CurrentFrame);
        currentFrameMutator.Commit();
        return;
    }

    if ( iFrameIndex > range.GetUpperBoundValue())
    {
        int cellLength = Cells.Last()->Length + iFrameIndex - range.GetUpperBoundValue() - 1;
		FOdysseyObjectEditorUtils::SetPropertyValue(Cells.Last(), GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Length), cellLength);
		UOdysseyAnimationCell* cell = AddCells(FOdysseyAnimationCellImageVector::StaticClass(), 0);

        FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
        currentFrameMutator.Set(animation->CurrentFrame);
        currentFrameMutator.Commit();
        return;
    }
}

void
UOdysseyAnimationLayerImageVector::Merge(const TArray<UOdysseyLayer*>& iLayers)
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
	FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), frameRange.GetLowerBoundValue());

    //Get cell ranges from each frame ImageRenderAbility composition
    int startFrame = frameRange.GetLowerBoundValue().IsInclusive() ? frameRange.GetLowerBoundValue() : frameRange.GetLowerBoundValue() + 1;
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

	RemoveCells(Cells);
	TArray<UOdysseyAnimationCell*>> cells = AddCells(UOdysseyAnimationCellImageVector::StaticClass(), 0, cellRanges.Num());
    
   	for (int i = 0; i < cellRanges.Num(); i++)
    {
		const FInt32Range& cellRange = cellRanges[i];
		UOdysseyAnimationCell* cell = cells[i];
		FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Length), cellRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue() + 1);
        
		int frame = cellRange.GetLowerBoundValue();
        FOdysseyVectorGroupPaint* destinationScene = cell->GetEngine()->GetScene();

        for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
        {
            UOdysseyAnimationLayerImageVector* vectorLayer = Cast<UOdysseyAnimationLayerImageVector>(iLayers[layerIndex]);
            if ( !vectorLayer )
                continue;

			int srcCellIndex = vectorLayer->GetCellIndexAtFrame(frame);
			if (srcCellIndex == INDEX_NONE)
				continue;

            UOdysseyAnimationCell* srcCell = Cells[srcCellIndex];
            int srcCellFrame = vectorLayer->GetCellFrameAtFrame(frame);
            while(srcCell && srcCell->IsA<FOdysseyAnimationCellImageStagger>())
            {
                UOdysseyAnimationCellImageStagger* cellStagger = Cast<UOdysseyAnimationCellImageStagger>(srcCell);
                srcCellIndex = cellStagger->GetReferenceCellIndexAtFrame(srcCellFrame, &srcCellFrame);
				srcCell = Cells[srcCellIndex];
            }

            UOdysseyAnimationCellImageVector* cellVector = Cast<FOdysseyAnimationCellImageVector>(srcCell);
            if (!cellVector)
                continue;

            FOdysseyVectorGroupPaint* scene = cellVector->GetEngine()->GetScene();
            for( FOdysseyVectorObject* child : scene->GetChildrenList() )
            {
                FOdysseyVectorObject* copiedChild = child->Copy();
                destinationScene->AppendChild( copiedChild );

                scene->CopyBuckets( destinationScene, false );
            }
        }
    }
    
    for ( UOdysseyAnimationCell* cell : Cells)
    {
        UOdysseyAnimationCellImageVector* vectorCell = Cast<UOdysseyAnimationCellImageVector>(cell);
        FOdysseyVectorEngine* engine = vectorCell->GetEngine();
        FOdysseyVectorGroupPaint* scene = engine->GetScene();
        scene->UpdateMatrix();
        scene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
        engine->Signal( FOdysseyVectorEngine::SIGNAL_ALL );
    }
}

#undef LOCTEXT_NAMESPACE