// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainerImport.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorImageRenderer.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorImport.h"
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "OdysseyAnimation.h"
#include "OdysseyMediaVector.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimationLayerImageVector::FOnIsColoredChanged&
UOdysseyAnimationLayerImageVector::OnIsColoredChanged()
{
    static FOnIsColoredChanged onIsColoredChanged;
    return onIsColoredChanged;
}

UOdysseyAnimationLayerImageVector::FOnIsWireframeChanged&
UOdysseyAnimationLayerImageVector::OnIsWireframeChanged()
{
    static FOnIsWireframeChanged onIsWireframeChanged;
    return onIsWireframeChanged;
}

void
UOdysseyAnimationLayerImageVector::PostInitProperties()
{
    Super::PostInitProperties();

	LayerTypeName = LOCTEXT("layer-image-vector.type", "Vector Image Layer");
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerVector16");
	DefaultCellClass = UOdysseyAnimationCellImageVector::StaticClass();

    SupportedCellTypes.Add(UOdysseyAnimationCellImageVector::StaticClass());
    SupportedCellTypes.Add(UOdysseyAnimationCellImageStagger::StaticClass());
}

struct FOdysseyAnimationLayerImageVectorObjectVersion
{
	enum Type
	{
		// Before any version changes were made
		BeforeCustomVersionWasAdded,

		// Reworked how anim blueprint root nodes are recovered
		RefactorCellsToUObject,

		// -----<new versions can be added above this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	// The GUID for this custom version number
	const static FGuid GUID;

private:
	FOdysseyAnimationLayerImageVectorObjectVersion() {}
};

const FGuid FOdysseyAnimationLayerImageVectorObjectVersion::GUID(0x8799329A, 0x214C4880, 0xB9229B09, 0x30C8D2BB);
FDevVersionRegistration GRegisterOdysseyAnimationLayerImageVectorObjectVersion(FOdysseyAnimationLayerImageVectorObjectVersion::GUID, FOdysseyAnimationLayerImageVectorObjectVersion::LatestVersion, TEXT("OdysseyAnimationLayerImageVector"));

void
UOdysseyAnimationLayerImageVector::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

	Ar.UsingCustomVersion(FOdysseyAnimationLayerImageVectorObjectVersion::GUID);

    if( Ar.IsLoading() && Ar.CustomVer(FOdysseyAnimationLayerImageVectorObjectVersion::GUID) < FOdysseyAnimationLayerImageVectorObjectVersion::RefactorCellsToUObject)
    {
        if (!FOdysseyAnimationLayerImageVectorImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
			//DEPRECATED: Keep for compatibility with Odyssey 2.0 and prior
            if (!FOdysseyAnimationCellsContainerImport::Read( this, Ar ))
			{
				//Old Style No Chunk Loading
				//Load or Save the offset
				Ar << CellsOffset;

				//Empty Cells to prepare for loading
				Cells.Empty();

				//Load or Save number of cells
				int32 numCells = 0;
				Ar << numCells;

				for ( int i = 0; i < numCells; i++ )
				{
					//Load the cell type
                    FName cellType;
                    Ar << cellType;

					if (cellType == TEXT("FOdysseyAnimationCellImageVector"))
					{
						UOdysseyAnimationCellImageVector* cell = NewObject<UOdysseyAnimationCellImageVector>(this, UOdysseyAnimationCellImageVector::StaticClass(), NAME_None, RF_Public | RF_Transactional);
						Cells.Add(cell);
						cell->OldSerialize(Ar);
					}
					else if (cellType == TEXT("FOdysseyAnimationCellImageStagger"))
					{
						UOdysseyAnimationCellImageStagger* cell = NewObject<UOdysseyAnimationCellImageStagger>(this, UOdysseyAnimationCellImageStagger::StaticClass(), NAME_None, RF_Public | RF_Transactional);
						Cells.Add(cell);
						cell->OldSerialize(Ar);
					}
				}
				UpdateCellsIndexInLayer();
			}
        }
    }
}

void
UOdysseyAnimationLayerImageVector::IsColoredChanged()
{
    OnIsColoredChanged().Broadcast(this);

    ImageRenderingChanged();
}

void
UOdysseyAnimationLayerImageVector::IsWireframeChanged()
{
    OnIsWireframeChanged().Broadcast(this);

    ImageRenderingChanged();
}

void
UOdysseyAnimationLayerImageVector::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
    Super::PropertyChanged(iPropertyName, iMemberPropertyName, iIsInteractive);

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

	bool showLighttable = iRenderType == IOdysseyImageRenderer::eRenderType::Editor && Lighttable.bIsActivated;
    if (showLighttable && Lighttable.DisplayPosition == EOdysseyLightTableDisplayPosition::UnderLayer )
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

    UOdysseyAnimationCell* cell = GetCellAtFrame(frame);
	if (cell)
	{
		int cellFrame = frame - cell->GetFrameRange().GetLowerBoundValue();
        idComposition.Append(cell->GetImageRenderingComposition(iRenderType, cellFrame));
    }

    if (showLighttable && Lighttable.DisplayPosition == EOdysseyLightTableDisplayPosition::AboveLayer )
        idComposition.Append(GetLighttableImageRenderingComposition(iFrameIndex));

    return idComposition;
}

TSharedPtr<IOdysseyMedia>
UOdysseyAnimationLayerImageVector::GetCellMediaVector(uint32 iFrameIndex) const
{
	UOdysseyAnimationCell* cell = GetCellAtFrame(iFrameIndex);
    if (!cell)
        return nullptr;
	
	int cellFrame = iFrameIndex - cell->GetFrameRange().GetLowerBoundValue();
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

    bool isActive = IsActivatedRecursively();
    bool isLocked = IsLockedRecursively();
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
	return GetCellMediaVector(iFrameIndex);
}

void
UOdysseyAnimationLayerImageVector::AutoCreateCell(int iFrameIndex)
{
    if (IsLocked)
        return;

    FScopedTransaction transaction(LOCTEXT("layer-image-vector.create-cell-transaction", "Create Cell"));

    UOdysseyAnimation* animation = GetAnimation();

	FInt32Range range = GetFrameRange();

    //Check if iFrameIndex is Out Of Range
	if ( iFrameIndex < range.GetLowerBoundValue())
    {
		Modify();
        //Add a frame at current frame and extend it
		UOdysseyAnimationCell* cell = AddCell(UOdysseyAnimationCellImageVector::StaticClass(), 0);
		FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), range.GetLowerBoundValue() - iFrameIndex);
		FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), CellsOffset - cell->Exposure);
        
        FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
        currentFrameMutator.Set(animation->CurrentFrame);
        currentFrameMutator.Commit();
        return;
    }

    if ( iFrameIndex > range.GetUpperBoundValue())
    {
		Modify();
        int cellExposure = Cells.Last()->Exposure + iFrameIndex - range.GetUpperBoundValue() - 1;
		FOdysseyObjectEditorUtils::SetPropertyValue(Cells.Last(), GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), cellExposure);
		UOdysseyAnimationCell* cell = AddCell(UOdysseyAnimationCellImageVector::StaticClass());

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
    int startFrame = frameRange.GetLowerBound().IsInclusive() ? frameRange.GetLowerBoundValue() : frameRange.GetLowerBoundValue() + 1;
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
	TArray<UOdysseyAnimationCell*> cells = AddCells(UOdysseyAnimationCellImageVector::StaticClass(), 0, cellRanges.Num());
    
   	for (int i = 0; i < cellRanges.Num(); i++)
    {
		const FInt32Range& cellRange = cellRanges[i];
		UOdysseyAnimationCellImageVector* cell = Cast<UOdysseyAnimationCellImageVector>(cells[i]);
		FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), cellRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue() + 1);
        
		int frame = cellRange.GetLowerBoundValue();
        FOdysseyVectorGroupPaint* destinationScene = cell->GetEngine()->GetScene();

        for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
        {
            UOdysseyAnimationLayerImageVector* vectorLayer = Cast<UOdysseyAnimationLayerImageVector>(iLayers[layerIndex]);
            if ( !vectorLayer )
                continue;

			UOdysseyAnimationCell* srcCell = vectorLayer->GetCellAtFrame(frame);
			if (!srcCell)
				continue;

			if (srcCell && srcCell->IsA<UOdysseyAnimationCellImageStagger>())
			{
				int staggerFrame = frame;
				while(srcCell && srcCell->IsA<UOdysseyAnimationCellImageStagger>())
				{
					UOdysseyAnimationCellImageStagger* cellStagger = Cast<UOdysseyAnimationCellImageStagger>(srcCell);
					int srcCellFrame = staggerFrame - srcCell->GetFrameRange().GetLowerBoundValue();
					
					staggerFrame = cellStagger->GetReferenceFrameAtFrame(srcCellFrame);
					srcCell = vectorLayer->GetCellAtFrame(staggerFrame);
				}
			}

            UOdysseyAnimationCellImageVector* cellVector = Cast<UOdysseyAnimationCellImageVector>(srcCell);
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

void
UOdysseyAnimationLayerImageVector::IsWireframeBlueprintSetter(bool Value)
{
	FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayerImageVector, IsWireframe), Value);
}

void
UOdysseyAnimationLayerImageVector::IsColoredBlueprintSetter(bool Value)
{
	FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayerImageVector, IsColored), Value);
}

#undef LOCTEXT_NAMESPACE
