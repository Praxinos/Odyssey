// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayer.h"

#include "OdysseyLayerStack.h"
#include "OdysseyLayerCell.h"
#include "Misc/TransactionObjectEvent.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "OdysseyBlendShader.h"
#include "OdysseyBlendColorShader.h"

#if WITH_EDITOR
UOdysseyLayer::FOnDisplayChildrenChanged&
UOdysseyLayer::OnDisplayChildrenChanged()
{
    static FOnDisplayChildrenChanged onDisplayChildrenChanged;
    return onDisplayChildrenChanged;
}

UOdysseyLayer::FOnDisplayOptionsChanged&
UOdysseyLayer::OnDisplayOptionsChanged()
{
    static FOnDisplayOptionsChanged onDisplayOptionsChanged;
    return onDisplayOptionsChanged;
}

FSimpleMulticastDelegate&
UOdysseyLayer::OnMediaChanged()
{
    static FSimpleMulticastDelegate onMediaChanged;
    return onMediaChanged;
}
#endif

FSimpleMulticastDelegate&
UOdysseyLayer::OnCellsChanged()
{
    return mOnCellsChanged;
}

void
UOdysseyLayer::PostInitProperties()
{
    Super::PostInitProperties();

#if WITH_EDITOR
    //Activate first previous and first next keys in the lighttable by default
    Lighttable.PreviousKeys[0].bIsActivated = true;
    Lighttable.NextKeys[0].bIsActivated = true;
#endif
}

void
UOdysseyLayer::InvalidateCellsFrameRanges()
{
    mCellsFrameRanges.Empty();
}

FInt32Range
UOdysseyLayer::GetFrameRange() const
{
    TArray<FInt32Range> ranges;
    const TArray<UOdysseyLayer*> layers = Children;
    for (UOdysseyLayer* layer : layers)
    {
        ranges.Add(layer->GetFrameRange());
    }

    if (!Cells.IsEmpty())
    {
        const TArray<FInt32Range>& cellFrameRanges = GetCellsFrameRanges();
        ranges.Add(FInt32Range::Inclusive( cellFrameRanges[0].GetLowerBoundValue(), cellFrameRanges.Last().GetUpperBoundValue() ));
    }

    return FInt32Range::Hull(ranges);
}

UOdysseyLayer*
UOdysseyLayer::GetParent() const
{
    return Parent;
}

TArray<UOdysseyLayer*>
UOdysseyLayer::GetParents() const
{
    TArray<UOdysseyLayer*> parents;
    UOdysseyLayer* parent = Parent;
    while ( parent )
    {
        parents.Add(parent);
        parent = parent->Parent;
    }
    return parents;
}

const TArray<UOdysseyLayer*>&
UOdysseyLayer::GetChildren() const
{
    return Children;
}

TArray<UOdysseyLayer*>
UOdysseyLayer::GetChildrenRecursively(EGetLayerChildrenMethod Method) const
{
    TArray<UOdysseyLayer*> children;
    if ( Method == EGetLayerChildrenMethod::DepthFirst )
    {
        for ( UOdysseyLayer* child : Children )
        {
            children.Add(child);
            children.Append(child->GetChildrenRecursively(Method));
        }
    }
    else if ( Method == EGetLayerChildrenMethod::BreadthFirst )
    {
        children = Children;
        for ( UOdysseyLayer* child : Children )
            children.Append(child->GetChildrenRecursively(Method));
    }
    return children;
}

int
UOdysseyLayer::GetIndexInParent() const
{
    if ( !Parent )
        return INDEX_NONE;
    return Parent->Children.Find(const_cast<UOdysseyLayer*>(this));
}

bool
UOdysseyLayer::IsChildOf(UOdysseyLayer* Layer) const
{
    return GetParents().Find(Layer) != INDEX_NONE;
}

TSet<UClass*>
UOdysseyLayer::GetMergeDefaultLayerTypes() const
{
    return { GetClass() };
}

TSet<UClass*>
UOdysseyLayer::GetMergeLayerTypesFromTypes(TSet<UClass*> iLayerTypes) const
{
    return { GetClass() };
}

UOdysseyLayerStack*
UOdysseyLayer::GetLayerStack() const
{
    return Cast<UOdysseyLayerStack>(GetOuter());
}

UClass*
UOdysseyLayer::GetDefaultCellClass() const
{
    return DefaultCellClass;
}

FText
UOdysseyLayer::GetLayerTypeName() const
{
    return LayerTypeName;
}


FText
UOdysseyLayer::GetDescription() const
{
    return Description;
}

FText
UOdysseyLayer::GetDefaultName() const
{
    return DefaultName;
}

bool
UOdysseyLayer::CanHaveChildren() const
{
    return bCanHaveChildren;
}

void
UOdysseyLayer::Merge(const TArray<UOdysseyLayer*>& Layers)
{
}

void
UOdysseyLayer::CellsChanged()
{
    UpdateCellsIndexInLayer();
    InvalidateCellsFrameRanges();
    mOnCellsChanged.Broadcast();
    RenderingCompositionChanged();
#if WITH_EDITOR
    OnMediaChanged().Broadcast();
#endif
}

#if WITH_EDITOR
FOdysseyMediaProvider
UOdysseyLayer::GetMediaProvider(uint32 iFrameIndex) const
{
    return FOdysseyMediaProvider();
}
#endif

int
UOdysseyLayer::GetPreBehaviourFrame(EOdysseyLayerImagePostBehaviour iBehaviour, int iFrame) const
{
    FInt32Range frameRange = GetFrameRange();

    //PreBehaviour
    int frame = iFrame;
    switch(iBehaviour)
    {
        default:
        case EOdysseyLayerImagePostBehaviour::None:
        break;

        case EOdysseyLayerImagePostBehaviour::Hold:
        {
            frame = frameRange.GetLowerBoundValue();
        }
        break;
        case EOdysseyLayerImagePostBehaviour::Loop:
        {
            int offsetFromStart = frameRange.GetLowerBoundValue() - iFrame;
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;
            frame = frameRange.GetUpperBoundValue() - ((offsetFromStart - 1) % layerLength);
        }
        break;
        case EOdysseyLayerImagePostBehaviour::PingPong:
        {
            int offsetFromStart = frameRange.GetLowerBoundValue() - iFrame;
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;

            if (layerLength <= 1)
            {
                frame = frameRange.GetLowerBoundValue();
                break;
            }

            int offset = ((offsetFromStart - 1) % (layerLength - 1)) + 1;
            int forwardFrame = frameRange.GetLowerBoundValue() + offset;
            int backwardFrame = frameRange.GetUpperBoundValue() - offset;

            int direction = layerLength > 0 ? ((offsetFromStart - 1) / (layerLength - 1)) % 2 : 0;
            frame = direction == 0 ? forwardFrame : backwardFrame;
        }
        break;
    }

    return frame;
}

int
UOdysseyLayer::GetPostBehaviourFrame(EOdysseyLayerImagePostBehaviour iBehaviour, int iFrame) const
{
    //PreBehaviour
    FInt32Range frameRange = GetFrameRange();

    int frame = iFrame;
    switch(iBehaviour)
    {
        default:
        case EOdysseyLayerImagePostBehaviour::None:
        break;

        case EOdysseyLayerImagePostBehaviour::Hold:
        {
            frame = frameRange.GetUpperBoundValue();
        }
        break;
        case EOdysseyLayerImagePostBehaviour::Loop:
        {
            int offsetFromEnd = iFrame - frameRange.GetUpperBoundValue();
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;
            frame = frameRange.GetLowerBoundValue() + (offsetFromEnd - 1) % layerLength;
        }
        break;
        case EOdysseyLayerImagePostBehaviour::PingPong:
        {
            int offsetFromEnd = iFrame - frameRange.GetUpperBoundValue();
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;

            if (layerLength <= 1)
            {
                frame = frameRange.GetLowerBoundValue();
                break;
            }

            int offset = ((offsetFromEnd - 1) % (layerLength - 1)) + 1;
            int forwardFrame = frameRange.GetLowerBoundValue() + offset;
            int backwardFrame = frameRange.GetUpperBoundValue() - offset;

            int direction = layerLength > 0 ? ((offsetFromEnd - 1) / (layerLength - 1)) % 2 : 0;
            frame = direction == 0 ? backwardFrame : forwardFrame;
        }
        break;
    }

    return frame;
}

const TArray<UOdysseyLayerCell*>&
UOdysseyLayer::GetCells() const
{
    return Cells;
}

UOdysseyLayerCell*
UOdysseyLayer::GetCellAtFrame(int Frame) const
{
    int cellIndex = GetCellIndexAtFrame(Frame);
    if (cellIndex == INDEX_NONE)
        return nullptr;

    return Cells[cellIndex];
}

int
UOdysseyLayer::GetCellIndexAtFrame(int Frame) const
{
    //TODO: Binary search using GetFrameRanges()
    //TODO: Use Prolly Search, as most of the time in animation the data is approximately uniformly distributed
    //see : https://www.dolthub.com/blog/2024-05-13-binary-vs-prolly/

    if( Frame < CellsOffset )
        return INDEX_NONE;

    int frameIndex = CellsOffset;
    for (int i = 0; i < Cells.Num(); i++)
    {
        UOdysseyLayerCell* cell = Cells[i];
        int cellExposure = 1;
        if (cell)
            cellExposure = cell->GetExposure();

        if ( frameIndex + cellExposure - 1 >= Frame)
            return i;

        frameIndex += cellExposure;
    }

    return INDEX_NONE;
}

bool
UOdysseyLayer::HasCellAtFrame(int Frame) const
{
    return !!GetCellAtFrame(Frame);
}

const TArray<FInt32Range>&
UOdysseyLayer::GetCellsFrameRanges() const
{
    if (mCellsFrameRanges.Num() != Cells.Num())
    {
        mCellsFrameRanges.Empty();
        uint32 startFrame = CellsOffset;
        for (TObjectPtr<UOdysseyLayerCell> cell : Cells)
        {
            int cellExposure = 1;
            if (cell)
                cellExposure = cell->GetExposure();

            FInt32Range frameRange = FInt32Range::Inclusive(startFrame, startFrame + cellExposure - 1);
            mCellsFrameRanges.Add(frameRange);
            startFrame += cellExposure;
        }
    }

    return mCellsFrameRanges;
}

#if WITH_EDITOR
void
UOdysseyLayer::AddNullCell(int Index)
{
    AddNullCells(Index);
}

void
UOdysseyLayer::AddNullCells(int Index, int Count)
{
    if (Index < 0 )
    {
        Index = Cells.Num();
    }
    else
    {
        Index = FMath::Clamp(Index, 0, Cells.Num());
    }

    TArray<UOdysseyLayerCell*> cells;
    for (int i = 0; i < Count; i++)
    {
        //Create the Layer
        cells.Add(nullptr);
    }

    Cells.Insert(cells, Index);
    CellsChanged();
}
#endif

UOdysseyLayerCell*
UOdysseyLayer::AddCell(TSubclassOf<UOdysseyLayerCell> CellType, int Index)
{
    TArray<UOdysseyLayerCell*> cells = AddCells(CellType, Index);
    if (cells.IsEmpty())
        return nullptr;

    return cells[0];
}

TArray<UOdysseyLayerCell*>
UOdysseyLayer::AddCells(TSubclassOf<UOdysseyLayerCell> CellType, int Index, int Count)
{
    UClass* cellType = CellType.Get();

    //No cellType
    if ( !cellType )
        return {};

    //cellType Not supported
    if (!SupportedCellTypes.Contains(cellType))
        return {};

    if (Index < 0 )
    {
        Index = Cells.Num();
    }
    else
    {
        Index = FMath::Clamp(Index, 0, Cells.Num());
    }

    TArray<UOdysseyLayerCell*> cells;
    for (int i = 0; i < Count; i++)
    {
        //Create the Layer
        UOdysseyLayerCell* cell = NewObject<UOdysseyLayerCell>(this, cellType, NAME_None, RF_Public | RF_Transactional);
        if (!cell )
            return cells;

        cells.Add(cell);
    }

    Cells.Insert(cells, Index);
    CellsChanged();

    return cells;
}

void
UOdysseyLayer::RemoveCell(UOdysseyLayerCell* Cell)
{
    if (!Cell) //if you need to remove a null cell use RemoveCellAtIndex
        return;

    RemoveCells({Cell});
    checkf(Cell->GetLayer() == this, TEXT("Layer does not contain the cell to remove"));
}

void
UOdysseyLayer::RemoveCells(const TArray<UOdysseyLayerCell*>& iCells)
{
    TArray<UOdysseyLayerCell*> cells;
    for (UOdysseyLayerCell* cell : iCells)
    {
        if (!cell) //if you need to remove a null cell use RemoveCellAtIndex
            continue;

        if (!ensureMsgf(cell->GetLayer() == this, TEXT("Layer does not contain the cell to remove")))
            continue;

        cell->IndexInLayer = INDEX_NONE;
        cells.Add(cell);
    }

    Cells.RemoveAll([&](UOdysseyLayerCell* iCell) { return cells.Contains(iCell); });
    CellsChanged();
}

void
UOdysseyLayer::RemoveCellAtIndex(int Index)
{
    if (Index < 0 || Index >= Cells.Num())
        return;

    if (Cells[Index])
        Cells[Index]->IndexInLayer = INDEX_NONE;

    Cells.RemoveAt(Index);
    CellsChanged();
}

void
UOdysseyLayer::RemoveAllCells()
{
    Cells.Empty();
    CellsChanged();
}

void
UOdysseyLayer::UpdateCellsIndexInLayer()
{
    for (int i = 0; i < Cells.Num(); i++)
    {
        if (!Cells[i])
            continue;

        Cells[i]->IndexInLayer = i;
    }
}

UOdysseyLayerCell*
UOdysseyLayer::CopyCell(UOdysseyLayerCell* Cell, int Index)
{
    //No Layer
    if(!Cell)
        return nullptr;

    if (Index < 0 )
    {
        Index = Cells.Num();
    }
    else
    {
        Index = FMath::Clamp(Index, 0, Cells.Num());
    }

    //Duplicate the cell
    FObjectDuplicationParameters params(Cell, this);
    UOdysseyLayerCell* cellCopy = Cast<UOdysseyLayerCell>(StaticDuplicateObjectEx(params));

    Cells.Insert(cellCopy, Index);
    CellsChanged();

    return cellCopy;
}

TArray<UOdysseyLayerCell*>
UOdysseyLayer::CopyCells(TArray<UOdysseyLayerCell*> iCells, int Index)
{
    TArray<UOdysseyLayerCell*> cellCopies;

    if (Index < 0 )
    {
        Index = Cells.Num();
    }
    else
    {
        Index = FMath::Clamp(Index, 0, Cells.Num());
    }

    //Sanitize Layers array
    iCells.RemoveAll(
        [this](const UOdysseyLayerCell* iCell)
        {
            return !iCell;
        }
    );

    //No Layers
    if (iCells.IsEmpty())
        return cellCopies;

    for (UOdysseyLayerCell* cell : iCells)
    {
        FObjectDuplicationParameters params(cell, this);
        UOdysseyLayerCell* cellCopy = Cast<UOdysseyLayerCell>(StaticDuplicateObjectEx(params));
        cellCopies.Add(cellCopy);
    }

    Cells.Insert(cellCopies, Index);
    CellsChanged();

    return cellCopies;
}

FText
UOdysseyLayer::GetLayerName() const
{
    return Name;
}

bool
UOdysseyLayer::IsActivated() const
{
    return bIsActivated;
}

bool
UOdysseyLayer::IsActivatedRecursively() const
{
    const UOdysseyLayer* layer = this;
    while(layer)
    {
        if (!layer->bIsActivated)
            return false;
        layer = layer->Parent;
    }

    return true;
}

#if WITH_EDITOR

bool
UOdysseyLayer::IsLocked() const
{
    return bIsLocked;
}

bool
UOdysseyLayer::IsLockedRecursively() const
{
    const UOdysseyLayer* layer = this;
    while(layer)
    {
        if (layer->bIsLocked)
            return true;
        layer = layer->Parent;
    }

    return false;
}

bool
UOdysseyLayer::ShouldDisplayChildren() const
{
    return bDisplayChildren;
}

bool
UOdysseyLayer::ShouldDisplayOptions() const
{
    return bDisplayOptions;
}
#endif

EOdysseyBlendingMode
UOdysseyLayer::GetBlendMode() const
{
    return BlendMode;
}

float
UOdysseyLayer::GetOpacity() const
{
    return Opacity;
}

int
UOdysseyLayer::GetCellsOffset() const
{
    return CellsOffset;
}

EOdysseyLayerImagePostBehaviour
UOdysseyLayer::GetPreBehaviour() const
{
    return PreBehaviour;
}

EOdysseyLayerImagePostBehaviour
UOdysseyLayer::GetPostBehaviour() const
{
    return PostBehaviour;
}

const TArray<TSubclassOf<UOdysseyLayerCell>>&
UOdysseyLayer::GetSupportedCellTypes() const
{
    return SupportedCellTypes;
}


TArray<FGuid>
UOdysseyLayer::GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrame) const
{
    TArray<FGuid> idComposition = { GetRenderingId() };

#if WITH_EDITOR
    bool showLighttable = iRenderType == EOdysseyRenderingType::Editor && Lighttable.bIsActivated;
    if ( showLighttable && Lighttable.DisplayPosition == EOdysseyLighttableDisplayPosition::UnderLayer )
        idComposition.Append(GetLighttableImageRenderingComposition(iFrame));
#endif

    if (bCanHaveChildren)
    {
        const TArray<UOdysseyLayer*>& children = GetChildren();
        for (UOdysseyLayer* child : children)
        {
            if (!child->bIsActivated)
                continue;

            idComposition.Append(child->GetRenderingComposition(iRenderType, iFrame));
        }
    }
    else
    {
        int frame = iFrame;
        FInt32Range frameRange = GetFrameRange();
        if ( iFrame < frameRange.GetLowerBoundValue() )
        {
            frame = GetPreBehaviourFrame(PreBehaviour, iFrame);
        }
        else if ( iFrame > frameRange.GetUpperBoundValue() )
        {
            frame = GetPostBehaviourFrame(PostBehaviour, iFrame);
        }

        UOdysseyLayerCell* cell = GetCellAtFrame(frame);
        if ( cell )
        {
            int cellFrame = frame - cell->GetFrameRange().GetLowerBoundValue();
            idComposition.Append(cell->GetRenderingComposition(iRenderType, cellFrame));
        }
    }

#if WITH_EDITOR
    if ( showLighttable && Lighttable.DisplayPosition == EOdysseyLighttableDisplayPosition::AboveLayer )
        idComposition.Append(GetLighttableImageRenderingComposition(iFrame));
#endif

    return idComposition;
}

FIntRect
UOdysseyLayer::GetDefaultRenderRect() const
{
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if(!layerStack)
        return FIntRect(0, 0, 0, 0);

    return layerStack->GetDefaultRenderRect();
}

void
UOdysseyLayer::SetCellsOffset(int Value)
{
    CellsOffset = Value;
    InvalidateCellsFrameRanges();
    RenderingCompositionChanged();

#if WITH_EDITOR
    OnMediaChanged().Broadcast();
#endif
}

void
UOdysseyLayer::SetPreBehaviour(EOdysseyLayerImagePostBehaviour Value)
{
    PreBehaviour = Value;
    RenderingCompositionChanged();
}

void
UOdysseyLayer::SetPostBehaviour(EOdysseyLayerImagePostBehaviour Value)
{
    PostBehaviour = Value;
    RenderingCompositionChanged();
}

void
UOdysseyLayer::AddChild(UOdysseyLayer* Layer, int IndexInParent)
{
    AddChildren({Layer}, IndexInParent);
}

void
UOdysseyLayer::AddChildren(TArray<UOdysseyLayer*> Layers, int IndexInParent)
{
    for (UOdysseyLayer* layer : Layers)
        ensure(!layer->Parent);

    //Sanitize Layers array
    Layers.RemoveAll(
        [this](const UOdysseyLayer* iLayer)
        {
            return !iLayer || iLayer->Parent != nullptr;
        }
    );

    if (IndexInParent == INDEX_NONE)
    {
        Children.Append(Layers);
    }
    else
    {
        IndexInParent = FMath::Clamp(IndexInParent, 0, Children.Num());
        Children.Insert(Layers, IndexInParent);
    }

    for (UOdysseyLayer* layer : Layers)
        layer->Parent = this;

    RenderingCompositionChanged();

    UOdysseyLayerStack* layerStack = GetLayerStack();
    if ( !layerStack )
        return;

    layerStack->HierarchyChanged();
}

void
UOdysseyLayer::RemoveChild(UOdysseyLayer* Layer)
{
    RemoveChildren({Layer});
}

void
UOdysseyLayer::RemoveChildren(TArray<UOdysseyLayer*> Layers)
{
    //Sanitize Layers array
    Layers.RemoveAll(
        [this](const UOdysseyLayer* iLayer)
        {
            return !iLayer || !Children.Contains(iLayer);
        }
    );

    //No Layers
    if (Layers.Num() <= 0)
        return;

    for (UOdysseyLayer* layer : Layers)
    {
        layer->Parent = nullptr;
        Children.Remove(layer);
    }

    RenderingCompositionChanged();
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if ( !layerStack )
        return;

    layerStack->HierarchyChanged();
}

void
UOdysseyLayer::SetLayerName(FText Value)
{
    Name = Value;
}

void
UOdysseyLayer::SetIsActivated(bool Value)
{
    bIsActivated = Value;
    if (Parent)
        Parent->RenderingCompositionChanged();
}

#if WITH_EDITOR
void
UOdysseyLayer::SetIsLocked(bool Value)
{
    bIsLocked = Value;
}

const FSlateIcon&
UOdysseyLayer::GetIcon() const
{
    return Icon;
}

const FSlateIcon&
UOdysseyLayer::GetIconExpanded() const
{
    return IconExpanded;
}

void
UOdysseyLayer::SetDisplayChildren(bool Value)
{
    bDisplayChildren = Value;
    OnDisplayChildrenChanged().Broadcast(this);
}

void
UOdysseyLayer::SetDisplayOptions(bool Value)
{
    bDisplayOptions = Value;
    OnDisplayOptionsChanged().Broadcast(this);
}
#endif

void
UOdysseyLayer::SetBlendMode(EOdysseyBlendingMode Value)
{
    BlendMode = Value;
    RenderingChanged();
}

void
UOdysseyLayer::SetOpacity(float Value)
{
    Opacity = Value;
    RenderingChanged();
}

#if WITH_EDITOR
void
UOdysseyLayer::SetOpacityInteractive(float Value)
{
    Opacity = Value;
    RenderingChanged(true);
}

void
UOdysseyLayer::SetCellsOffsetInteractive(float Value)
{
    CellsOffset = Value;
    InvalidateCellsFrameRanges();
    RenderingCompositionChanged(true);
    OnMediaChanged().Broadcast();
}
#endif

bool
UOdysseyLayer::BuildRenderPipeline(
    FFrameNumber iFrame,
    EOdysseyRenderingType iType,
    FOdysseyTextureRenderFunction& oRenderFunction
) const
{
    FOdysseyTextureRenderFunction renderFunction;
    if ( bCanHaveChildren )
    {
        if (!BuildRenderChildrenPipeline(iFrame,iType,renderFunction))
            return false;
    }
    else
    {
        FInt32Range frameRange = GetFrameRange();
        int frame = iFrame.Value;
        if ( frame < frameRange.GetLowerBoundValue() )
        {
            frame = GetPreBehaviourFrame(PreBehaviour, frame);
        }
        else if ( frame > frameRange.GetUpperBoundValue() )
        {
            frame = GetPostBehaviourFrame(PostBehaviour, frame);
        }

        if ( frame == INDEX_NONE )
            return false;

        UOdysseyLayerCell* cell = GetCellAtFrame(iFrame.Value);
        if ( !cell )
            return false;

        frame -= cell->GetFrameRange().GetLowerBoundValue();

        if(!cell->BuildRenderPipeline( frame, iType, renderFunction ))
            return false;
    }

#if WITH_EDITOR
    bool showLighttable = iType == EOdysseyRenderingType::Editor && Lighttable.bIsActivated;
    EOdysseyLighttableDisplayPosition lighttablePosition = Lighttable.DisplayPosition;

    FOdysseyTextureRenderFunction lighttableRenderFunction;
    if (showLighttable)
        showLighttable = BuildLighttableRenderPipeline(iFrame, iType, lighttableRenderFunction);

    oRenderFunction = [this, renderFunction, showLighttable, lighttablePosition, lighttableRenderFunction](
            FRDGBuilder& iGraphBuilder,
            ERHIFeatureLevel::Type iFeatureLevel,
            FRDGTextureRef iDestinationTexture,
            const FIntRect& iSrcRect,
            const FIntRect& iDstRect,
            const FMatrix& iSrcTransform
        )
        {
            AddClearRenderTargetPass(iGraphBuilder, iDestinationTexture, FLinearColor::Transparent, iDstRect);

            FRDGTextureDesc renderTextureDesc = FRDGTextureDesc::Create2D(
                iDestinationTexture->Desc.Extent,
                iDestinationTexture->Desc.Format,
                FClearValueBinding::Transparent,
                ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
            );

            FRDGTextureRef renderTexture = iGraphBuilder.CreateTexture(renderTextureDesc, TEXT("UOdysseyLayer::renderTexture"));

            renderFunction(
                iGraphBuilder,
                iFeatureLevel,
                renderTexture,
                iSrcRect,
                iDstRect,
                iSrcTransform
            );

            FRDGTextureRef lighttableTexture = nullptr;
            if ( showLighttable )
            {
                FRDGTextureDesc lighttableTextureDesc = FRDGTextureDesc::Create2D(
                    iDestinationTexture->Desc.Extent,
                    iDestinationTexture->Desc.Format,
                    FClearValueBinding::Transparent,
                    ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
                );

                lighttableTexture = iGraphBuilder.CreateTexture(lighttableTextureDesc, TEXT("UOdysseyLayer::LighttableTexture"));

                lighttableRenderFunction(
                    iGraphBuilder,
                    iFeatureLevel,
                    lighttableTexture,
                    iSrcRect,
                    iDstRect,
                    iSrcTransform
                );
            }

            if ( showLighttable && lighttablePosition == EOdysseyLighttableDisplayPosition::UnderLayer )
            {
                FOdysseyBlendShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    iDestinationTexture,
                    lighttableTexture,
                    iDestinationTexture,
                    iDstRect,
                    iDstRect,
                    FMatrix::Identity,
                    EOdysseyBlendingMode::kNormal,
                    EOdysseyAlphaMode::kNormal,
                    1.0f,
                    EOdysseyAntiAliasing::AnisotropicLinear
                );
            }

            FOdysseyBlendShader::BlendRect(
                iGraphBuilder,
                iFeatureLevel,
                iDestinationTexture,
                renderTexture,
                iDestinationTexture,
                iDstRect,
                iDstRect,
                FMatrix::Identity,
                EOdysseyBlendingMode::kNormal,
                EOdysseyAlphaMode::kNormal,
                1.0f,
                EOdysseyAntiAliasing::AnisotropicLinear
            );

            if ( showLighttable && lighttablePosition == EOdysseyLighttableDisplayPosition::AboveLayer )
            {
                FOdysseyBlendShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    iDestinationTexture,
                    lighttableTexture,
                    iDestinationTexture,
                    iDstRect,
                    iDstRect,
                    FMatrix::Identity,
                    EOdysseyBlendingMode::kNormal,
                    EOdysseyAlphaMode::kNormal,
                    1.0f,
                    EOdysseyAntiAliasing::AnisotropicLinear
                );
            }
        };
    return true;
#else
    oRenderFunction = renderFunction;
    return true;
#endif
}

bool
UOdysseyLayer::BuildLighttableRenderPipeline(
    FFrameNumber iFrame,
    EOdysseyRenderingType iType,
    FOdysseyTextureRenderFunction& oRenderFunction
) const
{
    UOdysseyLayerCell* cell = GetCellAtFrame(iFrame.Value);
    if (!cell)
        return false;

    struct FLighttableKeyRenderParams
    {
        FOdysseyTextureRenderFunction RenderFunction;
        float Opacity;
        FLinearColor Color;
        float Contrast;
    };

    TArray<FLighttableKeyRenderParams> keysRenderParams;
    for (int i = 9; i >= 0; i--)
    {
        if (Lighttable.PreviousKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->GetIndexInLayer() - i - 1;
            if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
            {
                UOdysseyLayerCell* keyCell = GetCells()[keyCellIndex];
                if (keyCell)
                {
                    int frame = iFrame.Value;
                    frame -= cell->GetFrameRange().GetLowerBoundValue();
                    FOdysseyTextureRenderFunction renderFunction;
                    if (keyCell->BuildRenderPipeline(frame, EOdysseyRenderingType::Render, renderFunction))
                    {
                        keysRenderParams.Add(
                            {
                                renderFunction,
                                Lighttable.PreviousKeys[i].Opacity / 100.f,
                                Lighttable.PreviousKeysColor,
                                Lighttable.PreviousKeysContrast / 100.f
                            }
                        );
                    }
                }
            }
        }

        if (Lighttable.NextKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->GetIndexInLayer() + i + 1;
            if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
            {
                UOdysseyLayerCell* keyCell = GetCells()[keyCellIndex];
                if (keyCell)
                {
                    int frame = iFrame.Value;
                    frame -= cell->GetFrameRange().GetLowerBoundValue();
                    FOdysseyTextureRenderFunction renderFunction;
                    if (keyCell->BuildRenderPipeline(frame, EOdysseyRenderingType::Render, renderFunction))
                    {
                        keysRenderParams.Add(
                            {
                                renderFunction,
                                Lighttable.NextKeys[i].Opacity / 100.f,
                                Lighttable.NextKeysColor,
                                Lighttable.NextKeysContrast / 100.f
                            }
                        );
                    }
                }
            }
        }
    }

    if (keysRenderParams.IsEmpty())
        return false;

    oRenderFunction = [this, keysRenderParams](
            FRDGBuilder& iGraphBuilder,
            ERHIFeatureLevel::Type iFeatureLevel,
            FRDGTextureRef iDestinationTexture,
            const FIntRect& iSrcRect,
            const FIntRect& iDstRect,
            const FMatrix& iSrcTransform
        )
        {
            AddClearRenderTargetPass(iGraphBuilder, iDestinationTexture, FLinearColor::Transparent, iDstRect);

            for ( const FLighttableKeyRenderParams& keyRenderParams : keysRenderParams )
            {
                FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
                    iDestinationTexture->Desc.Extent,
                    iDestinationTexture->Desc.Format,
                    FClearValueBinding::Transparent,
                    ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
                );

                FRDGTextureRef keyTexture = iGraphBuilder.CreateTexture(desc, TEXT("UOdysseyLayer::Lighttable::keyTexture"));
                keyRenderParams.RenderFunction(iGraphBuilder, iFeatureLevel, keyTexture, iSrcRect, iDstRect, iSrcTransform);

                //Turn to black and white
                FOdysseyBlendColorShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    keyTexture,
                    FLinearColor::Black,
                    keyTexture,
                    iDstRect,
                    EOdysseyBlendingMode::kSaturation,
                    EOdysseyAlphaMode::kBack,
                    1.f
                );

                //Apply Contrast
                FOdysseyBlendColorShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    keyTexture,
                    FLinearColor::Black,
                    keyTexture,
                    iDstRect,
                    EOdysseyBlendingMode::kLuminosity,
                    EOdysseyAlphaMode::kBack,
                    1.f - keyRenderParams.Contrast
                );

                FOdysseyBlendColorShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    keyTexture,
                    keyRenderParams.Color,
                    keyTexture,
                    iDstRect,
                    EOdysseyBlendingMode::kScreen,
                    EOdysseyAlphaMode::kBack,
                    1.f
                );

                FOdysseyBlendShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    iDestinationTexture,
                    keyTexture,
                    iDestinationTexture,
                    iDstRect,
                    iDstRect,
                    FMatrix::Identity,
                    EOdysseyBlendingMode::kNormal,
                    EOdysseyAlphaMode::kNormal,
                    keyRenderParams.Opacity,
                    EOdysseyAntiAliasing::AnisotropicLinear
                );
            }
        };

    return true;
}

bool
UOdysseyLayer::BuildRenderChildrenPipeline(
    FFrameNumber iFrame,
    EOdysseyRenderingType iType,
    FOdysseyTextureRenderFunction& oRenderFunction
) const
{
    struct FChildRenderParams
    {
        FOdysseyTextureRenderFunction RenderFunction;
        EOdysseyBlendingMode BlendMode;
        float Opacity;
    };

    TArray<FChildRenderParams> childrenRenderParams;
    for ( int i = Children.Num() - 1; i >= 0; i-- )
    {
        UOdysseyLayer* child = Children[i];
        if ( !child->IsActivated() )
            continue;

        FOdysseyTextureRenderFunction childRenderFunction;
        if (child->BuildRenderPipeline(iFrame, iType, childRenderFunction))
        {
            childrenRenderParams.Add(
                {
                    childRenderFunction,
                    child->GetBlendMode(),
                    child->GetOpacity()
                }
            );
        }
    }

    if (childrenRenderParams.IsEmpty())
        return false;

    oRenderFunction = [childrenRenderParams](
            FRDGBuilder& iGraphBuilder,
            ERHIFeatureLevel::Type iFeatureLevel,
            FRDGTextureRef iDestinationTexture,
            const FIntRect& iSrcRect,
            const FIntRect& iDstRect,
            const FMatrix& iSrcTransform
        )
        {
            AddClearRenderTargetPass(iGraphBuilder, iDestinationTexture, FLinearColor::Transparent, iDstRect);

            FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
                iDestinationTexture->Desc.Extent,
                iDestinationTexture->Desc.Format,
                FClearValueBinding::Transparent,
                ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
            );

            FRDGTextureRef childTexture = iGraphBuilder.CreateTexture(desc, TEXT("UOdysseyLayerRoot::childTexture"));

            for ( const FChildRenderParams& childRenderParams : childrenRenderParams )
            {
                childRenderParams.RenderFunction(iGraphBuilder, iFeatureLevel, childTexture, iSrcRect, iDstRect, iSrcTransform);

                FOdysseyBlendShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    iDestinationTexture,
                    childTexture,
                    iDestinationTexture,
                    iDstRect,
                    iDstRect,
                    FMatrix::Identity,
                    childRenderParams.BlendMode,
                    EOdysseyAlphaMode::kNormal,
                    childRenderParams.Opacity,
                    EOdysseyAntiAliasing::AnisotropicLinear
                );
            }
        };

    return true;
}

#if WITH_EDITOR

TArray<FName>
UOdysseyLayer::GetRows() const
{
    return {
        "Main",
        "Blend",
        "Lighttable",
        "OutOfPegs"
    };
}

int
UOdysseyLayer::GetRowHeight(FName iSubRowName) const
{
    if (iSubRowName == "Main")
        return 20;

    if (iSubRowName == "Blend")
        return 20;

    if (iSubRowName == "Lighttable")
        return 40;

    if (iSubRowName == "OutOfPegs")
        return 20;

    return 0;
}

bool
UOdysseyLayer::IsRowVisible(FName iSubRowName) const
{
    if (iSubRowName == "Main")
        return true;

    if (iSubRowName == "Blend")
        return bDisplayOptions;

    if (iSubRowName == "Lighttable")
        return ShouldDisplayOptions() && bHasLighttable && Lighttable.bIsActivated;

    if (iSubRowName == "OutOfPegs")
        return ShouldDisplayOptions() && bHasLighttable && Lighttable.bIsActivated;

    return 0;
}

FMargin
UOdysseyLayer::GetRowPadding(FName iSubRowName) const
{
    return FMargin(0.f, 0.f, 0.f, 2.f);
}

void
UOdysseyLayer::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();

    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Cells)))
    {
        CellsChanged();
        mOnCellsChanged.Broadcast();
        OnMediaChanged().Broadcast();
    }

    if ( changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, CellsOffset) ) )
    {
        InvalidateCellsFrameRanges();
        OnMediaChanged().Broadcast();
    }

    if ( changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Children)))
    {
        UOdysseyLayerStack* layerStack = GetLayerStack();
        if ( !layerStack )
            return;

        layerStack->HierarchyChanged();
    }

    if ( changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, bIsActivated) ) )
    {
        if (Parent)
            Parent->RenderingCompositionChanged();
    }

    if ( changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Children))
        || changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Cells))
        || changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, CellsOffset))
        || changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, PreBehaviour))
        || changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, PostBehaviour)))
    {
        RenderingCompositionChanged();
    }

    if ( changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, BlendMode))
        || changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Opacity)))
    {
        RenderingChanged();
    }

    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Lighttable)))
    {
        RenderingCompositionChanged(); //Composition could change if lighttable or a key is activated/inactivated
        RenderingChanged(); //ImageRendering changes without a composition change when any other param is changed
        mOnLighttableChanged.Broadcast();
    }
}


FOdysseyLighttable
UOdysseyLayer::GetLighttable() const
{
    return Lighttable;
}

bool
UOdysseyLayer::HasLighttable() const
{
    return bHasLighttable;
}

void
UOdysseyLayer::SetLighttable(FOdysseyLighttable Value)
{
    Lighttable = Value;
    RenderingCompositionChanged(); //Composition could change if lighttable or a key is activated/inactivated
    RenderingChanged(); //ImageRendering changes without a composition change when any other param is changed
    mOnLighttableChanged.Broadcast();
}

void
UOdysseyLayer::SetLighttableInteractive(FOdysseyLighttable Value)
{
    Lighttable = Value;
    RenderingCompositionChanged(true); //Composition could change if lighttable or a key is activated/inactivated
    RenderingChanged(true); //ImageRendering changes without a composition change when any other param is changed
    mOnLighttableChanged.Broadcast();
}

FSimpleMulticastDelegate&
UOdysseyLayer::OnLighttableChanged()
{
    return mOnLighttableChanged;
}

TArray<FGuid>
UOdysseyLayer::GetLighttableImageRenderingComposition(int iFrameIndex) const
{
    UOdysseyLayerCell* cell = GetCellAtFrame(iFrameIndex);
    if (!cell)
        return {};

    TArray<FGuid> idComposition = {};
    for (int i = 9; i >= 0; i--)
    {
        if (Lighttable.PreviousKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->GetIndexInLayer() - i - 1;
            if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
            {
                UOdysseyLayerCell* keyCell = GetCells()[keyCellIndex];
                if (keyCell)
                    idComposition.Append(keyCell->GetRenderingComposition(EOdysseyRenderingType::Render, 0));
            }
        }

        if (Lighttable.NextKeys[i].bIsActivated)
        {
            int keyCellIndex = cell->GetIndexInLayer() + i + 1;
            if (keyCellIndex >= 0 && keyCellIndex < Cells.Num())
            {
                UOdysseyLayerCell* keyCell = GetCells()[keyCellIndex];
                if (keyCell)
                    idComposition.Append(keyCell->GetRenderingComposition(EOdysseyRenderingType::Render, 0));
            }
        }
    }

    return idComposition;
}

#endif
