// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/Input/SSpinBox.h"
#include "OdysseyStyleSet.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyAnimationCurrentFrameMutator.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"
#include "UObject/OdysseyObjectEditorUtils.h"

void
SOdysseyAnimationCellImageStagger::Construct(const FArguments& iArgs, UOdysseyAnimationCellImageStagger* iCell, FOdysseyAnimationEditorExtension* iExtension)
{
    mSetReachTransactionName = (LOCTEXT("cell-image-stagger.set-reach", "Set Stagger Cell Reach"));
    mExtension = iExtension;
    mCell = iCell;
    mShowContent = iArgs._ShowContent;

    FSlateColor behaviourColor( FOdysseyStyle::GetColor( "Animation.CellImageStagger.BehaviourColor" ) );

    ChildSlot
    .VAlign(VAlign_Center)
    .HAlign(HAlign_Left)
    .Padding(FMargin(2, 0, 0, 0))
    [
        SNew(SHorizontalBox)
        .Visibility(this, &SOdysseyAnimationCellImageStagger::GetContentVisibility)
        .Clipping(EWidgetClipping::ClipToBoundsAlways)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SComboButton)
            .ButtonStyle(&FAppStyle::Get().GetWidgetStyle< FButtonStyle >( "SimpleButton" ))
            .HasDownArrow(false)
            .OnGetMenuContent(this, &SOdysseyAnimationCellImageStagger::GetBehaviourMenuContent)
            .ButtonContent()
            [
                SNew(SImage)
                .Image(this, &SOdysseyAnimationCellImageStagger::GetBehaviourBrush)
                .ColorAndOpacity(behaviourColor)
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SAssignNew( mReachSpinBox, SSpinBox<int> )
            .Style(&FOdysseyStyle::GetWidgetStyle< FSpinBoxStyle >( "Animation.CellImageStagger.Reach.SpinBoxStyle" ))
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
                AllottedGeometry.ToPaintGeometry( FVector2D(1, height - 7.f), FSlateLayoutTransform(1.0, TransformPoint(1.0, FVector2D(x, 7.f)) ) ),
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
        AllottedGeometry.ToPaintGeometry( FVector2D(width - 3.f, 7.f), FSlateLayoutTransform(1.0, TransformPoint(1.0, FVector2D(3, 0)) ) ),
        ArrowBrushTop,
        ESlateDrawEffect::None,
        arrowsColor
    );

    //Bottom Arrows
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry( FVector2D(width - 3.f, 7.f), FSlateLayoutTransform(1.0, TransformPoint(1.0, FVector2D(3, height - 7.f)) ) ),
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
            AllottedGeometry.ToPaintGeometry( FVector2D(reach * frameSize, 32.f), FSlateLayoutTransform(1.0, TransformPoint(1.0, FVector2D(reach * frameSize * -1, (height - 32.f) / 2.f )) ) ),
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
    switch(mCell->Behaviour)
    {
        case EOdysseyAnimationCellImageStaggerBehaviour::Loop:
            return FOdysseyStyle::GetBrush("Animation.CellImageStagger.Behaviour.Loop");
        break;

        case EOdysseyAnimationCellImageStaggerBehaviour::PingPong:
            return FOdysseyStyle::GetBrush("Animation.CellImageStagger.Behaviour.PingPong");
        break;
    }
    return nullptr;
}

int
SOdysseyAnimationCellImageStagger::GetReach() const
{
    return mCell->Reach;
}

void
SOdysseyAnimationCellImageStagger::OnReachValueChanged(int iReach)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCellImageStagger, Reach), FMath::Max(0, iReach), EPropertyChangeType::Interactive);
}

void
SOdysseyAnimationCellImageStagger::OnReachValueCommited(int iReach, ETextCommit::Type iType)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(mSetReachTransactionName);
#endif
    FOdysseyObjectEditorUtils::SetPropertyValue(mCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCellImageStagger, Reach), FMath::Max(0, iReach), EPropertyChangeType::ValueSet);

    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(mCell->GetAnimation());
    currentFrameMutator.Set(mCell->GetFrameRange().GetLowerBoundValue());
    currentFrameMutator.Commit();
}

void
SOdysseyAnimationCellImageStagger::OnReachBeginSliderMovement()
{
#ifdef WITH_EDITOR
    GEditor->BeginTransaction(mSetReachTransactionName);
#endif
}

void
SOdysseyAnimationCellImageStagger::OnReachEndSliderMovement(int iReach)
{
#ifdef WITH_EDITOR
    GEditor->EndTransaction();
#endif
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
}

void
SOdysseyAnimationCellImageStagger::BuildContextMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.BeginSection("Behaviour", LOCTEXT("cell-image-stagger.behaviour-menu.behaviour-section.name", "Behaviour"));
        iMenuBuilder.AddMenuEntry(
            LOCTEXT("cell-image-stagger.behaviour-menu.loop", "Loop"),
            TAttribute<FText>(),
            FSlateIcon("OdysseyStyle", "Animation.CellImageStagger.Behaviour.Loop"),
            FUIAction(
                FExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::SetBehaviour, EOdysseyAnimationCellImageStaggerBehaviour::Loop),
                FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::CanSetBehaviour, EOdysseyAnimationCellImageStaggerBehaviour::Loop),
                FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCellImageStagger::IsBehaviour, EOdysseyAnimationCellImageStaggerBehaviour::Loop)
            ),
            NAME_None,
            EUserInterfaceActionType::RadioButton
        );
        iMenuBuilder.AddMenuEntry(
            LOCTEXT("cell-image-stagger.behaviour-menu.pingpong", "PingPong"),
            TAttribute<FText>(),
            FSlateIcon("OdysseyStyle", "Animation.CellImageStagger.Behaviour.PingPong"),
            FUIAction(
                FExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::SetBehaviour, EOdysseyAnimationCellImageStaggerBehaviour::PingPong),
                FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::CanSetBehaviour, EOdysseyAnimationCellImageStaggerBehaviour::PingPong),
                FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCellImageStagger::IsBehaviour, EOdysseyAnimationCellImageStaggerBehaviour::PingPong)
            ),
            NAME_None,
            EUserInterfaceActionType::RadioButton
        );
    iMenuBuilder.EndSection();

    //TODO: Reach
}

void
SOdysseyAnimationCellImageStagger::SetBehaviour(EOdysseyAnimationCellImageStaggerBehaviour iBehaviour)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("cell-image-stagger.transaction.set-behaviour", "Set Stagger Cell Behaviour"));
#endif
    FOdysseyObjectEditorUtils::SetPropertyValue(mCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCellImageStagger, Behaviour), iBehaviour);

    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(mCell->GetAnimation());
    currentFrameMutator.Set(mCell->GetFrameRange().GetLowerBoundValue());
    currentFrameMutator.Commit();
}

bool
SOdysseyAnimationCellImageStagger::CanSetBehaviour(EOdysseyAnimationCellImageStaggerBehaviour iBehaviour) const
{
    return !mCell->GetLayer()->IsLockedRecursively();
}

bool
SOdysseyAnimationCellImageStagger::IsBehaviour(EOdysseyAnimationCellImageStaggerBehaviour iBehaviour) const
{
    return mCell->Behaviour == iBehaviour;
}

int
SOdysseyAnimationCellImageStagger::GetClampedReach() const
{
    int cellStartFrame = mCell->GetFrameRange().GetLowerBoundValue();
    int layerStartFrame = mCell->GetLayer()->GetFrameRange().GetLowerBoundValue();
    int maxReach = cellStartFrame - layerStartFrame;
    int reach = GetReach() <= 0 ? maxReach : FMath::Min(GetReach(), maxReach);

    return reach;
}

int
SOdysseyAnimationCellImageStagger::GetStaggerLength() const
{
    switch(mCell->Behaviour)
    {
        case EOdysseyAnimationCellImageStaggerBehaviour::Loop:
            return GetClampedReach();
        break;

        case EOdysseyAnimationCellImageStaggerBehaviour::PingPong:
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
