// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStaggerMutator.h"
#include "Widgets/Input/SSpinBox.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationCellImageStagger::Construct(const FArguments& iArgs, TSharedPtr<FOdysseyAnimationCellImageStagger> iCell, FOdysseyAnimationEditorExtension* iExtension)
{
    mExtension = iExtension;
    mCell = iCell;
    mShowContent = iArgs._ShowContent;
        
    FSlateColor behaviourColor( FOdysseyStyle::GetColor( "Animation.CellImageStagger.BehaviourColor" ) );

    ChildSlot
    .VAlign(VAlign_Center)
    .HAlign(HAlign_Left)
    .Padding(FMargin(3, 0, 0, 0))
    [
        SNew(SHorizontalBox)
        .Visibility(this, &SOdysseyAnimationCellImageStagger::GetContentVisibility)
        .Clipping(EWidgetClipping::ClipToBoundsAlways)
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
                .ColorAndOpacity(behaviourColor)
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(FMargin(5, 0, 0, 0))
        [
            SAssignNew( mReachSpinBox, SSpinBox<int> )
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

	const FSlateBrush* ArrowBrushTop = FOdysseyStyle::GetBrush("Animation.CellImageStagger.ArrowTop");
    const FSlateBrush* ArrowBrushBottom = FOdysseyStyle::GetBrush("Animation.CellImageStagger.ArrowBottom");
    const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
	const float height = AllottedGeometry.GetLocalSize().Y;  
	const float width = AllottedGeometry.GetLocalSize().X;
	float offset = mExtension->Timeline()->GetOffset();
	const float frameSize = mExtension->Timeline()->GetFrameWidth();

    int reach = GetClampedReach();
    int staggerLength = GetStaggerLength();

    bool displayReach = false;
    displayReach |= mReachSpinBox && mReachSpinBox->HasKeyboardFocus();
    displayReach |= mReachSpinBox && mReachSpinBox->IsHovered();

    if (staggerLength != INDEX_NONE && staggerLength > 0)
    {
        FLinearColor staggerLengthColor = FOdysseyStyle::GetColor( "Animation.CellImageStagger.StaggerLengthColor" );
        for (int x = staggerLength * frameSize; x < width; x += staggerLength * frameSize)
        {
            //Repeatiton Limits
            FSlateDrawElement::MakeBox(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry(FVector2D(x, 7.f), FVector2D(1, height - 7.f)),
                GenericBrush,
                ESlateDrawEffect::None,
                staggerLengthColor
            );
        }
    }

    FLinearColor arrowsColor = FOdysseyStyle::GetColor( "Animation.CellImageStagger.ArrowsColor" );

    //Top Arrows
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(FVector2D(3, 0), FVector2D(width - 3.f, 7.f)),
        ArrowBrushTop,
        ESlateDrawEffect::None,
        arrowsColor
    );

    //Bottom Arrows
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(FVector2D(3, height - 7.f), FVector2D(width - 3.f, 7.f)),
        ArrowBrushBottom,
        ESlateDrawEffect::None,
        arrowsColor
    );

    //Plus
    if (displayReach)
    {
        FLinearColor reachColor = FOdysseyStyle::GetColor( "Animation.CellImageStagger.PreviewReachColor" );
        const FSlateBrush* reachBrush = FOdysseyStyle::GetBrush( "Animation.CellImageStagger.ArrowPreviewReach" );
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry(FVector2D(reach * frameSize * -1, (height - 32.f) / 2.f ), FVector2D(reach * frameSize, 32.f)),
            reachBrush,
            ESlateDrawEffect::None,
            reachColor
        );
    }

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
    FScopedTransaction ScopedTransaction(LOCTEXT("cell-image-stagger.set-reach", "Set Stagger Cell Reach"));
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

    //Clear the keyboard focus here because the spinbox keeps it after dragging the value
    //which leads to the reach preview to be displayed after edition
    FSlateApplication::Get().SetKeyboardFocus(AsShared(), EFocusCause::SetDirectly);
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
    iMenuBuilder.BeginSection("Behaviour", LOCTEXT("cell-image-stagger.behaviour-menu.behaviour-section.name", "Behaviour"));
        iMenuBuilder.AddMenuEntry(FOdysseyAnimationEditorCommands::Get().SetStaggerCellBehaviourHold, NAME_None, LOCTEXT("cell-image-stagger.behaviour-menu.hold", "Hold"));
        iMenuBuilder.AddMenuEntry(FOdysseyAnimationEditorCommands::Get().SetStaggerCellBehaviourLoop, NAME_None, LOCTEXT("cell-image-stagger.behaviour-menu.loop", "Loop"));
        iMenuBuilder.AddMenuEntry(FOdysseyAnimationEditorCommands::Get().SetStaggerCellBehaviourPingPong, NAME_None, LOCTEXT("cell-image-stagger.behaviour-menu.pingpong", "PingPong"));
    iMenuBuilder.EndSection();

    //TODO: Reach
}

void
SOdysseyAnimationCellImageStagger::SetBehaviour(FOdysseyAnimationCellImageStagger::eBehaviour iBehaviour)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("cell-image-stagger.transaction.set-behaviour", "Set Stagger Cell Behaviour"));
#endif
    FOdysseyAnimationCellImageStaggerMutator mutator(mCell->GetLayer(), mCell);
    mutator.SetBehaviour(iBehaviour);
}

bool
SOdysseyAnimationCellImageStagger::CanSetBehaviour(FOdysseyAnimationCellImageStagger::eBehaviour iBehaviour) const
{
    //TODO: check if layer is locked
    return true;
}

int
SOdysseyAnimationCellImageStagger::GetClampedReach() const
{
    int cellStartFrame = mCell->GetLayer()->GetCellsContainer()->GetCellFrame(mCell);
    int layerStartFrame = mCell->GetLayer()->GetCellsContainer()->GetOffset();
    int maxReach = cellStartFrame - layerStartFrame;
    int reach = GetReach() <= 0 ? maxReach : FMath::Min(GetReach(), maxReach);

    return reach;
}

int
SOdysseyAnimationCellImageStagger::GetStaggerLength() const
{
    switch(mCell->GetBehaviour())
    {
        case FOdysseyAnimationCellImageStagger::eBehaviour::Hold:
            return INDEX_NONE;
        break;

        case FOdysseyAnimationCellImageStagger::eBehaviour::Loop:
            return GetClampedReach();
        break;

        case FOdysseyAnimationCellImageStagger::eBehaviour::PingPong:
            return GetClampedReach() - 1;
        break;
    }
    return INDEX_NONE;
}

EVisibility
SOdysseyAnimationCellImageStagger::GetContentVisibility() const
{
    return mShowContent.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE