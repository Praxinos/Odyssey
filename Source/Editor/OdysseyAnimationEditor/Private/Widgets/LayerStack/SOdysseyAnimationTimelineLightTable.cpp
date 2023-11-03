// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTable.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableKey.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationTimelineLightTable"

SOdysseyAnimationTimelineLightTable::SOdysseyAnimationTimelineLightTable()
	: mLayer(nullptr)
	, mExtension(nullptr)
	, mCurrentCellIndex(INDEX_NONE)
{
}

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
					if (mCurrentCellIndex == INDEX_NONE)
						return 0;

					TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();
					int firstCellIndex = FMath::Max(mCurrentCellIndex - 10, 0);
					int width = cellsContainer->GetCellFrame(cellsContainer->GetCells()[firstCellIndex]);
					return width;
				}
			)
			.HeightInScreenUnits(this, &SOdysseyAnimationTimelineLightTable::GetLightTableCellHeight)
			.Content()
			[
				SNullWidget::NullWidget
			]
		];

	//Previous cells
	for (int i = 0; i < 10; i++)
	{
		horizontalBox->AddSlot()
		.AutoWidth()
		[
			SNew(SOdysseyAnimationTimelineSection, mExtension)
			.Visibility(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyVisibility, i - 10)
			.WidthInFrames(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyWidthInFrames, i - 10)
			.HeightInScreenUnits(this, &SOdysseyAnimationTimelineLightTable::GetLightTableCellHeight)
			[
				SNew(SOdysseyAnimationTimelineLightTableKey, mLayer, i - 10)
			]
		];
	}

	//Current Cell
	horizontalBox->AddSlot()
	.AutoWidth()
	[
		SNew(SOdysseyAnimationTimelineSection, mExtension)
		.Visibility(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyVisibility, 0)
		.WidthInFrames(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyWidthInFrames, 0)
		.HeightInScreenUnits(this, &SOdysseyAnimationTimelineLightTable::GetLightTableCellHeight)
		[
			SNullWidget::NullWidget
		]
	];

	//Next cells
	for (int i = 1; i <= 10; i++)
	{
		horizontalBox->AddSlot()
		.AutoWidth()
		[
			SNew(SOdysseyAnimationTimelineSection, mExtension)
			.Visibility(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyVisibility, i)
			.WidthInFrames(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyWidthInFrames, i)
			.HeightInScreenUnits(this, &SOdysseyAnimationTimelineLightTable::GetLightTableCellHeight)
			[
				SNew(SOdysseyAnimationTimelineLightTableKey, mLayer, i)
			]
		];
	}

	ChildSlot
	[
		SNew(SOdysseyAnimationTimelineScrollBox, mExtension)
        + SOdysseyAnimationTimelineScrollBox::Slot()
        [
			horizontalBox
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
	UOdysseyAnimation* animation = mLayer->GetAnimation();
	if (!animation)
		return;

	int currentFrame = animation->CurrentFrame;
	TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();
	mCurrentCellIndex = cellsContainer->GetCellIndexAtFrame(currentFrame);
}

float
SOdysseyAnimationTimelineLightTable::GetLightTableCellHeight() const
{
	return 50; //TODO:
}

float
SOdysseyAnimationTimelineLightTable::GetLightTableKeyWidthInFrames(int iCellOffset) const
{
	if (mCurrentCellIndex == INDEX_NONE)
		return 0;

	TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();

	int cellIndex = mCurrentCellIndex + iCellOffset;
	if (cellIndex < 0 || cellIndex >= cellsContainer->GetCells().Num())
		return 0;

	int width = cellsContainer->GetCells()[cellIndex]->GetLength();
	return width;
}

EVisibility
SOdysseyAnimationTimelineLightTable::GetLightTableKeyVisibility(int iCellOffset) const
{
	if (mCurrentCellIndex == INDEX_NONE)
		return EVisibility::Collapsed;

	TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();

	int cellIndex = mCurrentCellIndex + iCellOffset;
	if (cellIndex < 0 || cellIndex >= cellsContainer->GetCells().Num())
		return EVisibility::Collapsed;

	return EVisibility::Visible;
}

#undef LOCTEXT_NAMESPACE
