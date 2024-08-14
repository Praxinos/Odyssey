// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStaggerImport.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStaggerImageRenderer.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "UObject/OdysseyObjectEditorUtils.h"

void
UOdysseyAnimationCellImageStagger::OldSerialize(FArchive& Ar)
{
    Super::OldSerialize(Ar);
    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellImageStaggerImport::Read( this, Ar ))
        {
            checkf(false, TEXT("Error while loading Stagger Cell"));
        }
    }
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyAnimationCellImageStagger::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;
    
    return MakeShared<FOdysseyAnimationCellImageStaggerImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects(), iFilter);
}

int
UOdysseyAnimationCellImageStagger::GetStaggerFrame(int iFrameIndex) const
{
	FInt32Range frameRange = GetFrameRange();
    if (frameRange.IsEmpty())
        return INDEX_NONE;

    int cellStartFrame = frameRange.GetLowerBoundValue();
    int frame = INDEX_NONE;
    switch(Behaviour)
    {
        case EOdysseyAnimationCellImageStaggerBehaviour::Loop:
        {
            int layerStartFrame = frameRange.GetLowerBoundValue();
            int startFrame = Reach <= 0 ? layerStartFrame : FMath::Max(layerStartFrame, int(cellStartFrame - Reach));
            int offset = iFrameIndex % (cellStartFrame - startFrame);
            frame = startFrame + offset;
        }
        break;

        case EOdysseyAnimationCellImageStaggerBehaviour::PingPong:
        {
            int layerStartFrame = frameRange.GetLowerBoundValue();
            int startFrame = Reach <= 0 ? layerStartFrame : FMath::Max(layerStartFrame, int(cellStartFrame - Reach));

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
    
    if (!GetLayer()->HasCellAtFrame(frame))
        return INDEX_NONE;

    return frame;
}

UOdysseyAnimationCell*
UOdysseyAnimationCellImageStagger::GetReferenceCellAtFrame(int Frame) const
{
	if (!GetLayer())
		return nullptr;

    int staggerFrame = GetStaggerFrame(Frame);
    if (staggerFrame == INDEX_NONE)
        return nullptr;

	return GetLayer()->GetCellAtFrame(staggerFrame);
}

TArray<FGuid>
UOdysseyAnimationCellImageStagger::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };
    
    UOdysseyAnimationCell* cell = GetReferenceCellAtFrame(iFrameIndex);
    if (!cell)
        return idComposition;

    int cellFrame = iFrameIndex - cell->GetFrameRange().GetLowerBoundValue();

    idComposition.Append(cell->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, cellFrame));
    
    return idComposition;
}

TArray<::ULIS::FRectI>
UOdysseyAnimationCellImageStagger::GetImageRenderingRects() const
{
    return GetLayer()->GetImageRenderingRects();
}

UOdysseyAnimationCell*
UOdysseyAnimationCellImageStagger::Break(int Frame)
{
	if (Frame <= 0 || Frame >= Length)
		return nullptr;

	UOdysseyAnimationCell* cell = this;
	do
	{
		UOdysseyAnimationCellImageStagger* cellStagger = Cast<UOdysseyAnimationCellImageStagger>(cell);
		int staggerFrame = cellStagger->GetStaggerFrame(Frame);
		cell = cellStagger->GetReferenceCellAtFrame(Frame);
	}
	while(cell && cell->IsA<UOdysseyAnimationCellImageStagger>());

	if (!cell)
		return nullptr;

	UOdysseyAnimationCell* copiedCell = GetLayer()->CopyCell(cell, IndexInLayer + 1);
	FOdysseyObjectEditorUtils::SetPropertyValue(copiedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Length), Length - Frame);
	FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Length), Frame);

	return copiedCell;
}