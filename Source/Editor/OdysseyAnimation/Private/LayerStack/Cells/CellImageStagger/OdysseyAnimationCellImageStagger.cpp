// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStaggerExport.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStaggerImport.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStaggerImageRenderer.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"

#include "Misc/OdysseyDuplicate.h"

#define LOCTEXT_NAMESPACE "FOdysseyAnimationCellImageStagger"

TSharedRef<FOdysseyAnimationCellImageStagger>
FOdysseyAnimationCellImageStagger::Create(UOdysseyAnimationLayer* iLayer, int iLength)
{
    TSharedRef<FOdysseyAnimationCellImageStagger> cell = MakeShared<FOdysseyAnimationCellImageStagger>(iLayer, iLength);
    cell->Init();
    return cell;
}

const FName&
FOdysseyAnimationCellImageStagger::StaticType()
{
    static FName type = TEXT("FOdysseyAnimationCellImageStagger");
    return type;
}

FOdysseyAnimationCellImageStagger::~FOdysseyAnimationCellImageStagger()
{
}

FOdysseyAnimationCellImageStagger::FOdysseyAnimationCellImageStagger(UOdysseyAnimationLayer* iLayer, int iLength)
    : FOdysseyAnimationCell(iLength, iLayer)
    , mBehaviour(eBehaviour::Hold)
    , mReach(0) // <= 0 is Infinite reach
{
}

void
FOdysseyAnimationCellImageStagger::Init()
{
}

TSharedPtr<FOdysseyAnimationCell>
FOdysseyAnimationCellImageStagger::Clone(UOdysseyAnimationLayer* iLayer, int iLength) const
{
    TSharedPtr<FOdysseyAnimationCellImageStagger> cloneCell = MakeShared<FOdysseyAnimationCellImageStagger>(iLayer, 1);
    ::Odyssey::Duplicate(const_cast<FOdysseyAnimationCellImageStagger*>(this), cloneCell.Get());
    cloneCell->mLength = iLength;
    return cloneCell;
}

const FName&
FOdysseyAnimationCellImageStagger::GetType() const
{
    return StaticType();
}

void
FOdysseyAnimationCellImageStagger::Serialize(FArchive& Ar)
{
    FOdysseyAnimationCell::Serialize(Ar);

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellImageStaggerExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellImageStaggerImport::Read( this, Ar ))
        {
            checkf(false, TEXT("Error while loading Stagger Cell"));
        }
    }
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationCellImageStagger::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    return MakeShared<FOdysseyAnimationCellImageStaggerImageRenderer>(SharedThis(this), iFrame, iRenderType, GetImageRenderingRects());
}

int
FOdysseyAnimationCellImageStagger::GetStaggerFrame(int iFrameIndex) const
{
    int cellStartFrame = GetLayer()->GetCellsContainer()->GetCellFrame(SharedThis(this));
    if (cellStartFrame == INDEX_NONE)
        return INDEX_NONE;

    int cellIndex = GetLayer()->GetCellsContainer()->GetCellIndex(SharedThis(this));
    if (cellIndex == INDEX_NONE || cellIndex == 0)
        return INDEX_NONE;

    FInt32Range frameRange = GetLayer()->GetFrameRange();

    int frame = INDEX_NONE;
    switch(mBehaviour)
    {
        case eBehaviour::Hold:
        {
            frame = cellStartFrame - 1;
        }
        break;

        case eBehaviour::Loop:
        {
            int layerStartFrame = frameRange.GetLowerBoundValue();
            int startFrame = mReach <= 0 ? layerStartFrame : FMath::Min(layerStartFrame, int(cellStartFrame - mReach));
            int offset = iFrameIndex % (cellStartFrame - startFrame);
            frame = startFrame + offset;
        }
        break;

        case eBehaviour::PingPong:
        {
            int layerStartFrame = frameRange.GetLowerBoundValue();
            int startFrame = mReach <= 0 ? layerStartFrame : FMath::Min(layerStartFrame, int(cellStartFrame - mReach));

            //If there is only one frame before the stagger cell,
            //we return that one frame because PingPong needs at least 2 frames to work properly
            if (cellStartFrame - startFrame == 1) 
                return startFrame;

            int offset = (iFrameIndex % (cellStartFrame - startFrame - 1));
            int forwardFrame = startFrame + offset + 1;
            int backwardFrame = cellStartFrame - offset - 2;

            int direction = (iFrameIndex / (cellStartFrame - startFrame - 1)) % 2;
            frame = direction == 0 ? backwardFrame : forwardFrame;
        }
        break;

        default:
            break;;
    }
    
    if (!GetLayer()->GetCellsContainer()->HasCellAtFrame(frame))
        return INDEX_NONE;

    return frame;
}

int
FOdysseyAnimationCellImageStagger::GetReach() const
{
    return mReach;
}

FOdysseyAnimationCellImageStagger::eBehaviour
FOdysseyAnimationCellImageStagger::GetBehaviour() const
{
    return mBehaviour;
}

TSharedPtr<FOdysseyAnimationCell>
FOdysseyAnimationCellImageStagger::GetReferenceCellAtFrame(int iFrameIndex, int* oCellFrameIndex) const
{
    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = GetLayer()->GetCellsContainer();
    if (!cellsContainer)
        return nullptr;

    int frame = GetStaggerFrame(iFrameIndex);
    if (frame == INDEX_NONE)
        return nullptr;

    TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCellAtFrame(frame);
    if (!cell)
        return nullptr;

    if (oCellFrameIndex)
        *oCellFrameIndex = cellsContainer->GetCellFrameAtFrame(frame);

    return cell;
}

TArray<FGuid>
FOdysseyAnimationCellImageStagger::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };
    
    int cellFrameIndex = INDEX_NONE;
    TSharedPtr<FOdysseyAnimationCell> cell = GetReferenceCellAtFrame(iFrameIndex, &cellFrameIndex);
    if (!cell)
        return idComposition;

    idComposition.Append(cell->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, cellFrameIndex));
    
    return idComposition;
}

TArray<::ULIS::FRectI>
FOdysseyAnimationCellImageStagger::GetImageRenderingRects() const
{
    return GetLayer()->GetImageRenderingRects();
}

TSharedPtr<FOdysseyAnimationCell>
FOdysseyAnimationCellImageStagger::CreateCellFromFrame(uint32 iFrameIndex) const
{
    int cellFrameIndex = INDEX_NONE;
    TSharedPtr<FOdysseyAnimationCell> cell = GetReferenceCellAtFrame(iFrameIndex, &cellFrameIndex);
    if (!cell)
        return nullptr;

    return cell->CreateCellFromFrame(cellFrameIndex);
}

#undef LOCTEXT_NAMESPACE