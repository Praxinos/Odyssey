// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationCellImageStagger.h"

#include "Editor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ScopedTransaction.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SSpinBox.h"

#include "OdysseyStyle.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationCellImageStagger::Construct(const FArguments& iArgs, UOdysseyLayerCellImageStagger* iCell)
{
    mSetReachTransactionName = (LOCTEXT("cell-image-stagger.set-reach", "Set Stagger Cell Reach"));
    mCell = iCell;
    mShowContent = iArgs._ShowContent;
    mTimelinePosition = iArgs._TimelinePosition;
    mOnTransactCurrentFrame = iArgs._OnTransactCurrentFrame;

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
    const float frameSize = mTimelinePosition->GetFrameSize();

    int reach = GetClampedReach();
    int staggerLength = GetStaggerLength();

    bool displayReach = false;
    displayReach |= mReachSpinBox && mReachSpinBox->HasKeyboardFocus();
    displayReach |= mReachSpinBox && mReachSpinBox->IsHovered();

    if (staggerLength != INDEX_NONE && staggerLength > 0)
    {
        switch(mCell->GetBehaviour())
        {
            case EOdysseyLayerCellImageStaggerBehaviour::Loop:
            case EOdysseyLayerCellImageStaggerBehaviour::PingPong:
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
            break;

            default:
            break;
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
    switch(mCell->GetBehaviour())
    {
        case EOdysseyLayerCellImageStaggerBehaviour::Loop:
            return FOdysseyStyle::GetBrush("Animation.CellImageStagger.Behaviour.Loop");
        break;

        case EOdysseyLayerCellImageStaggerBehaviour::PingPong:
            return FOdysseyStyle::GetBrush("Animation.CellImageStagger.Behaviour.PingPong");
        break;

        case EOdysseyLayerCellImageStaggerBehaviour::Random:
            return FOdysseyStyle::GetBrush("Animation.CellImageStagger.Behaviour.Random");
        break;
    }
    return nullptr;
}

int
SOdysseyAnimationCellImageStagger::GetReach() const
{
    return mCell->GetReach();
}

void
SOdysseyAnimationCellImageStagger::OnReachValueChanged(int iReach)
{
    mCell->SetReach(FMath::Max(0, iReach), true);
}

void
SOdysseyAnimationCellImageStagger::OnReachValueCommited(int iReach, ETextCommit::Type iType)
{
#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(mSetReachTransactionName);
#endif
    mCell->SetReach(FMath::Max(0, iReach), false);
    mOnTransactCurrentFrame.ExecuteIfBound(mCell->GetFrameRange().GetLowerBoundValue());
}

void
SOdysseyAnimationCellImageStagger::OnReachBeginSliderMovement()
{
#if WITH_EDITOR
    GEditor->BeginTransaction(mSetReachTransactionName);
#endif
}

void
SOdysseyAnimationCellImageStagger::OnReachEndSliderMovement(int iReach)
{
#if WITH_EDITOR
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
                FExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::SetBehaviour, EOdysseyLayerCellImageStaggerBehaviour::Loop),
                FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::CanSetBehaviour, EOdysseyLayerCellImageStaggerBehaviour::Loop),
                FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCellImageStagger::IsBehaviour, EOdysseyLayerCellImageStaggerBehaviour::Loop)
            ),
            NAME_None,
            EUserInterfaceActionType::RadioButton
        );
        iMenuBuilder.AddMenuEntry(
            LOCTEXT("cell-image-stagger.behaviour-menu.pingpong", "PingPong"),
            TAttribute<FText>(),
            FSlateIcon("OdysseyStyle", "Animation.CellImageStagger.Behaviour.PingPong"),
            FUIAction(
                FExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::SetBehaviour, EOdysseyLayerCellImageStaggerBehaviour::PingPong),
                FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::CanSetBehaviour, EOdysseyLayerCellImageStaggerBehaviour::PingPong),
                FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCellImageStagger::IsBehaviour, EOdysseyLayerCellImageStaggerBehaviour::PingPong)
            ),
            NAME_None,
            EUserInterfaceActionType::RadioButton
        );
        iMenuBuilder.AddMenuEntry(
            LOCTEXT("cell-image-stagger.behaviour-menu.random", "Random"),
            TAttribute<FText>(),
            FSlateIcon("OdysseyStyle", "Animation.CellImageStagger.Behaviour.Random"),
            FUIAction(
                FExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::SetBehaviour, EOdysseyLayerCellImageStaggerBehaviour::Random),
                FCanExecuteAction::CreateRaw(this, &SOdysseyAnimationCellImageStagger::CanSetBehaviour, EOdysseyLayerCellImageStaggerBehaviour::Random),
                FIsActionChecked::CreateRaw(this, &SOdysseyAnimationCellImageStagger::IsBehaviour, EOdysseyLayerCellImageStaggerBehaviour::Random)
            ),
            NAME_None,
            EUserInterfaceActionType::RadioButton
        );
    iMenuBuilder.EndSection();
}

void
SOdysseyAnimationCellImageStagger::SetBehaviour(EOdysseyLayerCellImageStaggerBehaviour iBehaviour)
{
#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("cell-image-stagger.transaction.set-behaviour", "Set Stagger Cell Behaviour"));
#endif
    mCell->SetBehaviour(iBehaviour);

    mOnTransactCurrentFrame.ExecuteIfBound(mCell->GetFrameRange().GetLowerBoundValue());
}

bool
SOdysseyAnimationCellImageStagger::CanSetBehaviour(EOdysseyLayerCellImageStaggerBehaviour iBehaviour) const
{
    return mCell->GetLayer()->IsEditable();
}

bool
SOdysseyAnimationCellImageStagger::IsBehaviour(EOdysseyLayerCellImageStaggerBehaviour iBehaviour) const
{
    return mCell->GetBehaviour() == iBehaviour;
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
    switch(mCell->GetBehaviour())
    {
        case EOdysseyLayerCellImageStaggerBehaviour::Loop:
        case EOdysseyLayerCellImageStaggerBehaviour::Random:
            return GetClampedReach();
        break;

        case EOdysseyLayerCellImageStaggerBehaviour::PingPong:
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
