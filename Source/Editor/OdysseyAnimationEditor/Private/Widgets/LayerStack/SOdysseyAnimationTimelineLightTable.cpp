// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTable.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableKey.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"

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
					int firstCellIndex = FMath::Max(mCurrentCellIndex - mLayer->GetLightTable()->GetRange(), 0);
					int width = cellsContainer->GetCellFrame(cellsContainer->GetCells()[firstCellIndex]);
					return width;
				}
			)
			.Content()
			[
				SNullWidget::NullWidget
			]
		];

	//Previous cells
	for (int i = -mLayer->GetLightTable()->GetRange(); i <= -1; i++)
	{
		horizontalBox->AddSlot()
		.AutoWidth()
		[
			SNew(SOdysseyAnimationTimelineSection, mExtension)
			.WidthInFrames(1)
			.Visibility(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyVisibility, i)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				[
					SNew(SOdysseyAnimationTimelineLightTableKey, mLayer, i)
					.IsActivated(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyIsActivated, i)
				]
			]
		];

		horizontalBox->AddSlot()
		.AutoWidth()
		[
			SNew(SOdysseyAnimationTimelineSection, mExtension)
			.WidthInFrames(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyRemainingLength, i)
			.Visibility(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyVisibility, i)
			[
				SNullWidget::NullWidget
			]
		];
	}

	//Current Cell
	horizontalBox->AddSlot()
	.AutoWidth()
	[
		SNew(SOdysseyAnimationTimelineSection, mExtension)
		.Visibility(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyVisibility, 0)
		.WidthInFrames(this, &SOdysseyAnimationTimelineLightTable::GetCurrentCellLength)
		[
			SNullWidget::NullWidget
		]
	];

	//Next cells
	for (int i = 1; i <= mLayer->GetLightTable()->GetRange(); i++)
	{
		
		horizontalBox->AddSlot()
		.AutoWidth()
		[
			SNew(SOdysseyAnimationTimelineSection, mExtension)
			.WidthInFrames(1)
			.Visibility(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyVisibility, i)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				[
					SNew(SOdysseyAnimationTimelineLightTableKey, mLayer, i)
					.IsActivated(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyIsActivated, i)
				]
			]
		];

		horizontalBox->AddSlot()
		.AutoWidth()
		[
			SNew(SOdysseyAnimationTimelineSection, mExtension)
			.WidthInFrames(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyRemainingLength, i)
			.Visibility(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyVisibility, i)
			[
				SNullWidget::NullWidget
			]
		];
	}

	ChildSlot
	[
		SNew(SBox)
        .HeightOverride(FOptionalSize(SOdysseyAnimationTimelineLightTableKey::mDesiredHeight))
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
SOdysseyAnimationTimelineLightTable::GetLightTableKeyRemainingLength(int iCellOffset) const
{
	if (mCurrentCellIndex == INDEX_NONE)
		return 0;

	TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();

	int cellIndex = mCurrentCellIndex + iCellOffset;
	if (cellIndex < 0 || cellIndex >= cellsContainer->GetCells().Num())
		return 0;

	int width = cellsContainer->GetCells()[cellIndex]->GetLength() - 1;
	return width;
}

float
SOdysseyAnimationTimelineLightTable::GetCurrentCellLength() const
{
	if (mCurrentCellIndex == INDEX_NONE)
		return 0;

	TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();

	int cellIndex = mCurrentCellIndex;
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

bool
SOdysseyAnimationTimelineLightTable::GetLightTableKeyIsActivated(int iCellOffset) const
{
	return mLayer->GetLightTable()->GetKeyIsActivated(iCellOffset);
}
