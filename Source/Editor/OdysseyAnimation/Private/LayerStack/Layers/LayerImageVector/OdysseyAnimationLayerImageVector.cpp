// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorImageRenderer.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "OdysseyLayerFunctionLibrary.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorExport.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorImport.h"

#define LOCTEXT_NAMESPACE "UOdysseyAnimationLayerImageVector"

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

UOdysseyAnimationLayerImageVector::~UOdysseyAnimationLayerImageVector()
{
}

UOdysseyAnimationLayerImageVector::UOdysseyAnimationLayerImageVector()
    : mCellsContainer(MakeShared<FOdysseyAnimationCellsContainer>())
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Vector Image Layer");
    Icon = *FOdysseyStyle::GetBrush( "OdysseyLayerStack.LayerVector16");
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
UOdysseyAnimationLayerImageVector::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    return MakeShared<FOdysseyAnimationLayerImageVectorImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects());
}

TArray<FGuid>
UOdysseyAnimationLayerImageVector::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };

    if (iRenderType == IOdysseyImageRenderer::eRenderType::Editor && bIsLightTableActivated && mLightTable->GetDisplayPosition() == EOdysseyLightTableDisplayPosition::UnderLayer )
    {
        idComposition.Append(mLightTable->GetImageRenderingComposition(iRenderType, iFrameIndex));
    }

    int celFrameIndex = mCellsContainer->GetCellFrameAtFrame(iFrameIndex);
    TSharedPtr<FOdysseyAnimationCell> cell = mCellsContainer->GetCellAtFrame(iFrameIndex);
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
    if (!cell || celFrameIndex != 0)
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
	TSharedPtr<IOdysseyMedia> cellMediaVector = GetCellMediaVector(iFrameIndex);
	if (cellMediaVector)
	{
		bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(this);
		bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(this);
		cellMediaVector->IsLocked(!isActive || isLocked);
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

    bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(this);
    bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(this);

    mediaVector->IsLocked(!isActive || isLocked);
    return mediaVector;
}

void
UOdysseyAnimationLayerImageVector::AutoCreateCell(int iFrameIndex)
{
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
        return;
    }
    
    //iFrameIndex is not Out Of Range

    int cellIndex = mCellsContainer->GetCellIndexAtFrame(iFrameIndex);
    if (cellIndex == INDEX_NONE)
        return;

    int cellFrameIndex = mCellsContainer->GetCellFrameAtFrame(iFrameIndex);
    if (cellFrameIndex == INDEX_NONE)
        return;

    if (cellFrameIndex == 0) //is Cell Head
        return;

    //Is Not Head
    if (!bAutoBreakCells) //If breaking a cell is not allowed, return
        return;

    TSharedPtr<FOdysseyAnimationCell> currentCell = mCellsContainer->GetCells()[cellIndex];
    int currentCellLength = cellFrameIndex;
    int newCellLength = currentCell->GetLength() - currentCellLength;

    TSharedPtr<FOdysseyAnimationCell> cell = currentCell->CreateCellFromFrame(cellFrameIndex); //TODO: Find a better name than "Break" to extract a cell from a single frame of another cell

    FOdysseyAnimationCellsMutator mutator(this, mCellsContainer);
    mutator.SetLength(cellIndex, currentCellLength);
    mutator.Add({ cell }, cellIndex + 1);
    mutator.SetLength(cellIndex + 1, newCellLength);
    mutator.Commit();
}

#undef LOCTEXT_NAMESPACE