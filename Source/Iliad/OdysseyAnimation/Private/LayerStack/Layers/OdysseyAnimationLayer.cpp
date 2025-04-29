// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationLayer.h"

#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationCell.h"

#if WITH_EDITOR
#include "UObject/OdysseyObjectEditorUtils.h"
#endif

//===========================

void
UOdysseyAnimationLayer::PostInitProperties()
{
    Super::PostInitProperties();

#if WITH_EDITOR
    //Activate first previous and first next keys in the lighttable by default
    Lighttable.PreviousKeys[0].bIsActivated = true;
    Lighttable.NextKeys[0].bIsActivated = true;
#endif
}

UOdysseyAnimation*
UOdysseyAnimationLayer::GetAnimation() const
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());
    if(!layerStack)
        return nullptr;

    return layerStack->GetAnimation();
}

UOdysseyAnimationLayerStack*
UOdysseyAnimationLayer::GetLayerStack() const
{
    return Cast<UOdysseyAnimationLayerStack>(UOdysseyLayer::GetLayerStack());
}

#if WITH_EDITOR
FOdysseyAnimationLightTable
UOdysseyAnimationLayer::GetLighttable() const
{
    return Lighttable;
}

bool
UOdysseyAnimationLayer::HasLighttable() const
{
    return bHasLighttable;
}

void
UOdysseyAnimationLayer::SetLighttable(FOdysseyAnimationLightTable Value)
{
    Lighttable = Value;
    RenderingCompositionChanged(); //Composition could change if lighttable or a key is activated/inactivated
    RenderingChanged(); //ImageRendering changes without a composition change when any other param is changed
    mOnLightTableChanged.Broadcast();
}

void
UOdysseyAnimationLayer::SetLighttableInteractive(FOdysseyAnimationLightTable Value)
{
    Lighttable = Value;
    RenderingCompositionChanged(true); //Composition could change if lighttable or a key is activated/inactivated
    RenderingChanged(true); //ImageRendering changes without a composition change when any other param is changed
    mOnLightTableChanged.Broadcast();
}

FSimpleMulticastDelegate&
UOdysseyAnimationLayer::OnLightTableChanged()
{
    return mOnLightTableChanged;
}

void
UOdysseyAnimationLayer::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();

    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable)))
    {
        RenderingCompositionChanged(); //Composition could change if lighttable or a key is activated/inactivated
        RenderingChanged(); //ImageRendering changes without a composition change when any other param is changed
        mOnLightTableChanged.Broadcast();
    }
}

TArray<FGuid>
UOdysseyAnimationLayer::GetLighttableImageRenderingComposition(int iFrameIndex) const
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

TArray<FName>
UOdysseyAnimationLayer::GetRows() const
{
    TArray<FName> rows = UOdysseyLayer::GetRows();
    rows.Add("Lighttable");
    rows.Add("OutOfPegs");

    return rows;
}

int
UOdysseyAnimationLayer::GetRowHeight(FName iSubRowName) const
{
    if (iSubRowName == "Lighttable")
        return 40;

    if (iSubRowName == "OutOfPegs")
        return 20;

    return Super::GetRowHeight(iSubRowName);
}

bool
UOdysseyAnimationLayer::IsRowVisible(FName iSubRowName) const
{
    if (iSubRowName == "Lighttable")
        return ShouldDisplayOptions() && bHasLighttable && Lighttable.bIsActivated;

    if (iSubRowName == "OutOfPegs")
        return ShouldDisplayOptions() && bHasLighttable && Lighttable.bIsActivated;

    return Super::IsRowVisible(iSubRowName);
}

#endif
