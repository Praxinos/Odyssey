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
#include "OdysseyLayerFunctionLibrary.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorExport.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorImport.h"
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "OdysseyAnimation.h"
#include "OdysseyMediaVector.h"
#include "LayerStack/Cells/OdysseyAnimationCellsMutator.h"

#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimationLayerImageVector::FOnBlendModeChanged&
UOdysseyAnimationLayerImageVector::OnBlendModeChanged()
{
    static FOnBlendModeChanged onBlendModeChanged;
    return onBlendModeChanged;
}

UOdysseyAnimationLayerImageVector::FOnOpacityChanged&
UOdysseyAnimationLayerImageVector::OnOpacityChanged()
{
    static FOnOpacityChanged onOpacityChanged;
    return onOpacityChanged;
}

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

UOdysseyAnimationLayerImageVector::~UOdysseyAnimationLayerImageVector()
{
}

UOdysseyAnimationLayerImageVector::UOdysseyAnimationLayerImageVector()
    : mCellsContainer(MakeShared<FOdysseyAnimationCellsContainer>())
{
	LayerTypeName = LOCTEXT("layer-image-vector.type", "Vector Image Layer");
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerVector16");

    mCellsContainer->AddSupportedType(FOdysseyAnimationCellImageVector::StaticType());
    mCellsContainer->AddSupportedType(FOdysseyAnimationCellImageStagger::StaticType());
}

void
UOdysseyAnimationLayerImageVector::OnCreated_Implementation()
{
}

FInt32Range
UOdysseyAnimationLayerImageVector::GetFrameRange() const
{
    return mCellsContainer->GetFrameRange();
}

TSharedPtr<FOdysseyAnimationLightTable>
UOdysseyAnimationLayerImageVector::GetLightTable() const
{
    return mLightTable;
}

bool
UOdysseyAnimationLayerImageVector::GetIsLightTableActivated() const
{
    return bIsLightTableActivated;
}

TSharedPtr<FOdysseyAnimationCellsContainer>
UOdysseyAnimationLayerImageVector::GetCellsContainer() const
{
    return mCellsContainer;
}

void
UOdysseyAnimationLayerImageVector::IsLightTableActivatedChanged()
{   
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    ImageRenderingChanged();
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
UOdysseyAnimationLayerImageVector::OpacityChanged()
{
    OnOpacityChanged().Broadcast(this);

    ImageRenderingChanged();
}

void
UOdysseyAnimationLayerImageVector::BlendModeChanged()
{
    OnBlendModeChanged().Broadcast(this);
    
    ImageRenderingChanged();
}

void
UOdysseyAnimationLayerImageVector::OnCellsChanged()
{
    //OnCellsChanged().Broadcast(this);
    ImageRenderingCompositionChanged();
    UOdysseyLayer::OnMediaChanged().Broadcast();
}

void
UOdysseyAnimationLayerImageVector::PropertyChanged(const FName& iPropertyName)
{
    Super::PropertyChanged(iPropertyName);

    if(iPropertyName == "IsColored")
        IsColoredChanged();
    if(iPropertyName == "IsWireframe")
        IsWireframeChanged();
    if (iPropertyName == "BlendMode")
        BlendModeChanged();
    if (iPropertyName == "Opacity")
        OpacityChanged();
    if (iPropertyName == "bIsLightTableActivated")
        IsLightTableActivatedChanged();
}

TSharedPtr<FOdysseyAnimationCell>
UOdysseyAnimationLayerImageVector::CreateCell( const FName& iCellType, bool iForSerialization)
{
    if (iCellType == FOdysseyAnimationCellImageVector::StaticType())
    {
        if (iForSerialization)
            return MakeShared<FOdysseyAnimationCellImageVector>(this, 1);

        UOdysseyAnimation* animation = GetAnimation();
        return FOdysseyAnimationCellImageVector::Create(this, 1, animation->Width(), animation->Height());
    }
    else if (iCellType == FOdysseyAnimationCellImageStagger::StaticType())
    {
        if (iForSerialization)
            return FOdysseyAnimationCellImageStagger::Create(this, 1);
    }
    return nullptr;
}

void
UOdysseyAnimationLayerImageVector::PostInitProperties()
{
    Super::PostInitProperties();

    mCellsContainer->CreateCellDelegate().BindUObject(this, &UOdysseyAnimationLayerImageVector::CreateCell);
    mCellsContainer->OnCellsChanged().AddUObject(this, &UOdysseyAnimationLayerImageVector::OnCellsChanged);
    mLightTable = MakeShared<FOdysseyAnimationLightTable>(this);
}

void
UOdysseyAnimationLayerImageVector::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationLayerImageVectorExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationLayerImageVectorImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            mCellsContainer->Serialize(Ar);
        }
    }
}

/* void
UOdysseyAnimationLayerImageVector::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	if ( iLayerStack != GetLayerStack() )
		return;

    bool isCurrentLayer = iLayerStack->CurrentLayer.Get() == this;
    const TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = mCellsContainer->GetCells();
    for (TSharedPtr<FOdysseyAnimationCell> cell : cells)
    {
        if (cell->GetType() != FOdysseyAnimationCellImageVector::StaticType())
            continue;

        TSharedPtr<FOdysseyAnimationCellImageVector> cellImageVector = StaticCastSharedPtr<FOdysseyAnimationCellImageVector>(cell);
        cellImageVector->GetVectorBlock()->SetRenderHUD(isCurrentLayer);
    }
} */

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

::ULIS::eBlendMode
UOdysseyAnimationLayerImageVector::GetImageRenderingBlendMode() const
{
    return (::ULIS::eBlendMode)BlendMode;
}

float
UOdysseyAnimationLayerImageVector::GetImageRenderingOpacity() const
{
    return Opacity;
}

TSharedPtr<IOdysseyMedia>
UOdysseyAnimationLayerImageVector::GetCellMediaVector(uint32 iFrameIndex) const
{
	int celFrameIndex = mCellsContainer->GetCellFrameAtFrame(iFrameIndex);
    TSharedPtr<FOdysseyAnimationCell> cell = mCellsContainer->GetCellAtFrame(iFrameIndex);
    if (!cell)
        return nullptr;

    FOdysseyMediaProvider provider = cell->GetMediaProvider(celFrameIndex);
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

    bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(this);
    bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(this);
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

    int celFrameIndex = mCellsContainer->GetCellFrameAtFrame(iFrameIndex);
    TSharedPtr<FOdysseyAnimationCell> cell = mCellsContainer->GetCellAtFrame(iFrameIndex);
    if (!cell)
        return nullptr;

    FOdysseyMediaProvider provider = cell->GetMediaProvider(celFrameIndex);
    if (!provider.HasMedia<FOdysseyMediaVector>())
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectorList = provider.GetOrCreateMedias<FOdysseyMediaVector>();
    if (mediaVectorList.Num() <= 0)
        return nullptr;

    TSharedPtr<FOdysseyMediaVector> mediaVector = mediaVectorList[0];
    return mediaVector;
}

void
UOdysseyAnimationLayerImageVector::AutoCreateCell(int iFrameIndex)
{
    if (IsLocked)
        return;

    FScopedTransaction transaction(LOCTEXT("layer-image-vector.create-cell-transaction", "Create Cell"));
    UOdysseyAnimation* animation = GetAnimation();
    //Check if iFrameIndex is Out Of Range
    FInt32Range range = mCellsContainer->GetFrameRange();
    if ( iFrameIndex < range.GetLowerBoundValue())
    {
        if (!bAutoAddCells) //If out of range is not allowed, return
            return;

        int length = range.GetLowerBoundValue() - iFrameIndex;

        //Add a frame at current frame and extend it 
        TSharedPtr<FOdysseyAnimationCellImageVector> cell = FOdysseyAnimationCellImageVector::Create(this, length, animation->Width(), animation->Height());

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
        if (!bAutoAddCells) //If out of range is not allowed, return
            return;

        //Add a frame at current frame and extend previous frame to it 
        TSharedPtr<FOdysseyAnimationCellImageVector> cell = FOdysseyAnimationCellImageVector::Create(this, 1, animation->Width(), animation->Height());
        
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
    
    FOdysseyAnimationCellsMutator mutator(this, mCellsContainer);
    mutator.Remove(0, mCellsContainer->GetCells().Num());
    
    for (const FInt32Range& cellRange : cellRanges)
    {
        int cellLength = cellRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue() + 1;
        TSharedPtr<FOdysseyAnimationCellImageVector> cell = FOdysseyAnimationCellImageVector::Create(this, cellLength, animation->Width(), animation->Height());
        int frame = cellRange.GetLowerBoundValue();

        FOdysseyVectorGroupPaint* destinationScene = cell->GetEngine()->GetScene();

        for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
        {
            UOdysseyAnimationLayerImageVector* vectorLayer = Cast<UOdysseyAnimationLayerImageVector>(iLayers[layerIndex]);
            if ( !vectorLayer )
                continue;

            TSharedPtr<FOdysseyAnimationCell> srcCell = vectorLayer->GetCellsContainer()->GetCellAtFrame(frame);
            int srcCellFrame = vectorLayer->GetCellsContainer()->GetCellFrameAtFrame(frame);
            while(srcCell && srcCell->GetType() == FOdysseyAnimationCellImageStagger::StaticType())
            {
                TSharedPtr<FOdysseyAnimationCellImageStagger> cellStagger = StaticCastSharedPtr<FOdysseyAnimationCellImageStagger>(srcCell);
                srcCell = cellStagger->GetReferenceCellAtFrame(srcCellFrame, &srcCellFrame);
            }

            TSharedPtr<FOdysseyAnimationCellImageVector> cellVector = StaticCastSharedPtr<FOdysseyAnimationCellImageVector>(srcCell);
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

        cells.Add(cell);
    }
    
    mutator.Add(cells);
    mutator.SetOffset(offset);
    mutator.Commit();
    
    for ( TSharedPtr<FOdysseyAnimationCell> cell : mCellsContainer->GetCells())
    {
        TSharedPtr<FOdysseyAnimationCellImageVector> vectorCell = StaticCastSharedPtr<FOdysseyAnimationCellImageVector>(cell);
        FOdysseyVectorEngine* engine = vectorCell->GetEngine();
        FOdysseyVectorGroupPaint* scene = engine->GetScene();
        scene->UpdateMatrix();
        scene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
        engine->Signal( FOdysseyVectorEngine::SIGNAL_ALL );
    }
}

#undef LOCTEXT_NAMESPACE
