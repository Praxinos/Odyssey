// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStaggerMutator.h"
#include "Widgets/Input/SSpinBox.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationCellImageStagger"

void
SOdysseyAnimationCellImageStagger::Construct(const FArguments& iArgs, TSharedPtr<FOdysseyAnimationCellImageStagger> iCell)
{
    mCell = iCell;

    SetClipping( EWidgetClipping::ClipToBoundsAlways );

    ChildSlot
    .VAlign(VAlign_Center)
    .HAlign(HAlign_Left)
    .Padding(FMargin(3, 0, 0, 0))
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SComboButton)
            //.IsFocusable(true)
            .HasDownArrow(false)
            .OnGetMenuContent(this, &SOdysseyAnimationCellImageStagger::GetBehaviourMenuContent)
            .ContentPadding(FMargin(0, 2, 0, 2))
            .ButtonContent()
            [
                SNew(SImage)
                .Image(this, &SOdysseyAnimationCellImageStagger::GetBehaviourBrush)
            ]   
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(FMargin(5, 0, 0, 0))
        [
            SNew( SSpinBox<int> )
            .Value(this, &SOdysseyAnimationCellImageStagger::GetReach )
            .OnValueChanged( this, &SOdysseyAnimationCellImageStagger::OnReachValueChanged )
            .OnValueCommitted( this, &SOdysseyAnimationCellImageStagger::OnReachValueCommited )
            .OnBeginSliderMovement( this, &SOdysseyAnimationCellImageStagger::OnReachBeginSliderMovement )
            .OnEndSliderMovement( this, &SOdysseyAnimationCellImageStagger::OnReachEndSliderMovement )
            .LinearDeltaSensitivity( 15 )
            .Delta(1)
            /* SNew(STextBlock)
            .Text(this, &SOdysseyAnimationCellImageStagger::GetReachText) */
        ]
    ];
}

int32
SOdysseyAnimationCellImageStagger::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Draw a current frame
    
	LayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	const FSlateBrush* ArrowBrushTop = FOdysseyStyle::GetBrush("Animation.CellImageStaggerArrowTop");
    const FSlateBrush* ArrowBrushBottom = FOdysseyStyle::GetBrush("Animation.CellImageStaggerArrowBottom");
	const float height = AllottedGeometry.GetLocalSize().Y;  
	const float width = AllottedGeometry.GetLocalSize().X;

    //Plus
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(FVector2D(3, 0), FVector2D(width - 3.f, 7.f)),
        ArrowBrushTop
    );

    //Plus
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(FVector2D(3, height - 7.f), FVector2D(width - 3.f, 7.f)),
        ArrowBrushBottom
    );

	return LayerId;
}

const FSlateBrush*
SOdysseyAnimationCellImageStagger::GetBehaviourBrush() const
{
    switch(mCell->GetBehaviour())
    {
        case FOdysseyAnimationCellImageStagger::eBehaviour::Hold:
            return FOdysseyStyle::GetBrush("Animation.CellImageStagger.Behaviour.Hold");
        break;

        case FOdysseyAnimationCellImageStagger::eBehaviour::Loop:
            return FOdysseyStyle::GetBrush("Animation.CellImageStagger.Behaviour.Loop");
        break;

        case FOdysseyAnimationCellImageStagger::eBehaviour::PingPong:
            return FOdysseyStyle::GetBrush("Animation.CellImageStagger.Behaviour.PingPong");
        break;
    }
    return nullptr;
}

int
SOdysseyAnimationCellImageStagger::GetReach() const
{
    if (mIsEditingReach)
        return mReachData.mReach;
    return mCell->GetReach();
}

void
SOdysseyAnimationCellImageStagger::OnReachValueChanged(int iReach)
{
    mReachData.mReach = FMath::Max(0, iReach);
    //return mCell->GetReach()
}

void
SOdysseyAnimationCellImageStagger::OnReachValueCommited(int iReach, ETextCommit::Type iType)
{    
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("OdysseyAnimationCellImageStagger::SetReach", "Set Stagger Cell Reach"));
#endif
    FOdysseyAnimationCellImageStaggerMutator mutator(mCell->GetLayer(), mCell);
    mutator.SetReach(FMath::Max(0, iReach));
}

void
SOdysseyAnimationCellImageStagger::OnReachBeginSliderMovement()
{
    mReachData.mReach = mCell->GetReach();
    mIsEditingReach = true;
}

void
SOdysseyAnimationCellImageStagger::OnReachEndSliderMovement(int iReach)
{
    mReachData.mReach = mCell->GetReach();
    mIsEditingReach = false;
}

TSharedRef<SWidget>
SOdysseyAnimationCellImageStagger::GetBehaviourMenuContent()
{
    TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
    MapActions(commandList);
    FMenuBuilder menuBuilder(true, commandList);
    BuildContextMenu(menuBuilder);

    return menuBuilder.MakeWidget();
}

void
SOdysseyAnimationCellImageStagger::MapActions(TSharedPtr<FUICommandList> iCommandList)
{
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().SetStaggerCellBehaviourHold,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::SetBehaviour, FOdysseyAnimationCellImageStagger::eBehaviour::Hold),
        FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::CanSetBehaviour, FOdysseyAnimationCellImageStagger::eBehaviour::Hold)
    );

    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().SetStaggerCellBehaviourLoop,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::SetBehaviour, FOdysseyAnimationCellImageStagger::eBehaviour::Loop),
        FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::CanSetBehaviour, FOdysseyAnimationCellImageStagger::eBehaviour::Loop)
    );

    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().SetStaggerCellBehaviourPingPong,
        FExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::SetBehaviour, FOdysseyAnimationCellImageStagger::eBehaviour::PingPong),
        FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::CanSetBehaviour, FOdysseyAnimationCellImageStagger::eBehaviour::PingPong)
    );
}

void
SOdysseyAnimationCellImageStagger::BuildContextMenu(FMenuBuilder& iMenuBuilder)
{  
    iMenuBuilder.BeginSection("Behaviour", LOCTEXT("AnimationCellImageStagger-BehaviourMenu-BehaviourSection", "Behaviour"));
        iMenuBuilder.AddMenuEntry(FOdysseyAnimationEditorCommands::Get().SetStaggerCellBehaviourHold, NAME_None, LOCTEXT("AnimationCellImageStagger-BehaviourMenu-Behaviour-Hold", "Hold"));
        iMenuBuilder.AddMenuEntry(FOdysseyAnimationEditorCommands::Get().SetStaggerCellBehaviourLoop, NAME_None, LOCTEXT("AnimationCellImageStagger-BehaviourMenu-Behaviour-Loop", "Loop"));
        iMenuBuilder.AddMenuEntry(FOdysseyAnimationEditorCommands::Get().SetStaggerCellBehaviourPingPong, NAME_None, LOCTEXT("AnimationCellImageStagger-BehaviourMenu-Behaviour-PingPong", "PingPong"));
    iMenuBuilder.EndSection();

    //TODO: Reach
}

void
SOdysseyAnimationCellImageStagger::SetBehaviour(FOdysseyAnimationCellImageStagger::eBehaviour iBehaviour)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("OdysseyAnimationCellImageStagger::SetBehaviour", "Set Stagger Cell Behaviour"));
#endif
    FOdysseyAnimationCellImageStaggerMutator mutator(mCell->GetLayer(), mCell);
    mutator.SetBehaviour(iBehaviour);
    //TODO: Undo + Mutator
}

bool
SOdysseyAnimationCellImageStagger::CanSetBehaviour(FOdysseyAnimationCellImageStagger::eBehaviour iBehaviour) const
{
    //TODO: check if layer is locked
    return true;
}

#undef LOCTEXT_NAMESPACE