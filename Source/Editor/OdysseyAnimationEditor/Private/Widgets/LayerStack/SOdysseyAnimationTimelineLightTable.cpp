// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTable.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableKey.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "Tools/OutOfPegsTool/OdysseyAnimationEditorOutOfPegsTool.h"
#include "OdysseyAnimation.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineSection.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineScrollBox.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"

void
SOdysseyAnimationTimelineLightTable::Construct(const FArguments& InArgs, UOdysseyAnimationLayer* iLayer, FOdysseyAnimationEditorExtension* iExtension)
{
	UOdysseyAnimation::OnCurrentFrameChanged().AddSP(SharedThis(this), &SOdysseyAnimationTimelineLightTable::OnCurrentFrameChanged);
	FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate().AddSP(this, &SOdysseyAnimationTimelineLightTable::OnImageRenderingChanged);

	mLayer = iLayer;
	mExtension = iExtension;

	TSharedRef<SHorizontalBox> horizontalBox = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SOdysseyAnimationTimelineSection, mExtension)
			.WidthInFrames_Lambda(
				[this]()
				{
					if (!mCurrentCell)
						return 0;

					int firstCellIndex = FMath::Max(mCurrentCell->IndexInLayer - 10, 0);
					UOdysseyAnimationCell* cell = mLayer->GetCells()[firstCellIndex];
					int width = cell->GetFrameRange().GetLowerBoundValue();
					return width;
				}
			)
			.Content()
			[
				SNullWidget::NullWidget
			]
		];

	//Previous cells
	for (int i = 9; i >= 0; i--)
	{
		horizontalBox->AddSlot()
		.AutoWidth()
		[
			SNew(SOdysseyAnimationTimelineSection, mExtension)
			.HAlign(HAlign_Left)
			.WidthInFrames_Lambda(
				[this, i]()
				{
					if (!mCurrentCell)
						return 0;

					const TArray<UOdysseyAnimationCell*>& cells = mLayer->GetCells();
					int cellIndex = mCurrentCell->IndexInLayer - i - 1;
					if (cellIndex < 0 || cellIndex >= cells.Num())
						return 0;

					return cells[cellIndex]->Exposure;
				}
			)
			[

				SNew(SOdysseyAnimationTimelineLightTableKey, mExtension)
				.Visibility_Lambda(
					[this, i]()
					{
						if (!mCurrentCell)
							return EVisibility::Collapsed;

						int cellIndex = mCurrentCell->IndexInLayer - i - 1;
						if (cellIndex < 0 || cellIndex >= mLayer->GetCells().Num())
							return EVisibility::Collapsed;

						return EVisibility::Visible;
					}
				)
				.Key_Lambda([this, i]() { return mLayer->Lighttable.PreviousKeys[i];})
				.Cell_Lambda(
					[this, i]() -> UOdysseyAnimationCell*
					{
						if (!mCurrentCell)
							return nullptr;

						const TArray<UOdysseyAnimationCell*>& cells = mLayer->GetCells();
						int cellIndex = mCurrentCell->IndexInLayer - i - 1;
						if (cellIndex < 0 || cellIndex >= cells.Num())
							return nullptr;

						return cells[cellIndex];
					}
				)
				.OnChanged_Lambda(
					[this, i](FOdysseyAnimationLightTableKey iKey)
					{
						FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
						lighttable.PreviousKeys[i] = iKey;
						FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::Interactive);
					}
				)
				.OnCommited_Lambda(
					[this, i](FOdysseyAnimationLightTableKey iKey)
					{
						FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
						lighttable.PreviousKeys[i] = iKey;
						FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::ValueSet);
					}
				)
			]
		];
	}

	//Current Cell
	horizontalBox->AddSlot()
	.AutoWidth()
	[
		SNew(SOdysseyAnimationTimelineSection, mExtension)
		.Visibility_Lambda(
			[this]()
			{
				if (!mCurrentCell)
					return EVisibility::Collapsed;

				return EVisibility::Visible;
			}
		)
		.WidthInFrames_Lambda(
			[this]()
			{
				if (!mCurrentCell)
					return 0;

				return mCurrentCell->Exposure;
			}
		)
		[
			SNullWidget::NullWidget
		]
	];

	//Next cells
	for (int i = 0; i <= 9; i++)
	{
		horizontalBox->AddSlot()
		.AutoWidth()
		[
			SNew(SOdysseyAnimationTimelineSection, mExtension)
			.HAlign(HAlign_Left)
			.WidthInFrames_Lambda(
				[this, i]()
				{
					if (!mCurrentCell)
						return 0;

					const TArray<UOdysseyAnimationCell*>& cells = mLayer->GetCells();
					int cellIndex = mCurrentCell->IndexInLayer + i + 1;
					if (cellIndex < 0 || cellIndex >= cells.Num())
						return 0;

					return cells[cellIndex]->Exposure;
				}
			)
			[

				SNew(SOdysseyAnimationTimelineLightTableKey, mExtension)
				.Visibility_Lambda(
					[this, i]()
					{
						if (!mCurrentCell)
							return EVisibility::Collapsed;

						int cellIndex = mCurrentCell->IndexInLayer + i + 1;
						if (cellIndex < 0 || cellIndex >= mLayer->GetCells().Num())
							return EVisibility::Collapsed;

						return EVisibility::Visible;
					}
				)
				.Key_Lambda([this, i]() { return mLayer->Lighttable.NextKeys[i];})
				.Cell_Lambda(
					[this, i]() -> UOdysseyAnimationCell*
					{
						if (!mCurrentCell)
							return nullptr;

						const TArray<UOdysseyAnimationCell*>& cells = mLayer->GetCells();
						int cellIndex = mCurrentCell->IndexInLayer + i + 1;
						if (cellIndex < 0 || cellIndex >= cells.Num())
							return nullptr;

						return cells[cellIndex];
					}
				)
				.OnChanged_Lambda(
					[this, i](FOdysseyAnimationLightTableKey iKey)
					{
						FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
						lighttable.NextKeys[i] = iKey;
						FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::Interactive);
					}
				)
				.OnCommited_Lambda(
					[this, i](FOdysseyAnimationLightTableKey iKey)
					{
						FOdysseyAnimationLightTable lighttable = mLayer->Lighttable;
						lighttable.NextKeys[i] = iKey;
						FOdysseyObjectEditorUtils::SetPropertyValue(mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, Lighttable), lighttable, EPropertyChangeType::ValueSet);
					}
				)
			]
		];
	}

	ChildSlot
	[
		SNew(SBox)
        .HeightOverride(FOptionalSize(mDesiredHeight))
		[
			SNew(SOdysseyAnimationTimelineScrollBox, mExtension)
			+ SOdysseyAnimationTimelineScrollBox::Slot()
			[
				horizontalBox
			]
		]
	];

	Update();
}

void
SOdysseyAnimationTimelineLightTable::OnCurrentFrameChanged(UOdysseyAnimation* iAnimation)
{
	if (iAnimation != mLayer->GetAnimation())
		return;

	Update();
}

void
SOdysseyAnimationTimelineLightTable::OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationTimelineLightTable::OnImageRenderingChanged);
	if (iEvent.IsInteractive())
		return;

	if (iEvent.GetType() != FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange)
		return;

	UOdysseyAnimation* animation = mLayer->GetAnimation();
	if (!animation)
		return;

	TArray<FGuid> composition = mLayer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Editor, animation->CurrentFrame);
	if (!composition.Contains(iEvent.GetId()))
		return;

	Update();
}

void
SOdysseyAnimationTimelineLightTable::Update()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(SOdysseyAnimationTimelineLightTable::Update);
	UOdysseyAnimation* animation = mLayer->GetAnimation();
	if (!animation)
		return;

	int currentFrame = animation->CurrentFrame;
	mCurrentCell = mLayer->GetCellAtFrame(currentFrame);
}
