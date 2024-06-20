// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTable.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableKey.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "Tools/OutOfPegsTool/OdysseyAnimationEditorOutOfPegsTool.h"
#include "OdysseyAnimation.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineSection.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineScrollBox.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyPainterEditor.h"

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
			CreateKeyWidget(i)
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
			CreateKeyWidget(i)
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

TSharedRef<SWidget>
SOdysseyAnimationTimelineLightTable::CreateKeyWidget(int iCellOffset)
{
	//const FButtonStyle* outOfPegButtonStyle = &FOdysseyStyle::GetWidgetStyle<FButtonStyle>("Button.TransparentNoPadding");
	const FCheckBoxStyle* checkboxStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.ToggleButton");

	return SNew(SOdysseyAnimationTimelineSection, mExtension)
		.WidthInFrames(1)
		.Visibility(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyVisibility, iCellOffset)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			[
				SNew(SBox)
        		.HeightOverride(FOptionalSize(SOdysseyAnimationTimelineLightTableKey::mDesiredSliderHeight))
				[
					SNew(SOdysseyAnimationTimelineLightTableKey, mLayer, iCellOffset)
					.IsActivated(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyIsActivated, iCellOffset)
				]
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				SNew(SCheckBox)
				.IsEnabled(this, &SOdysseyAnimationTimelineLightTable::GetLightTableKeyIsActivated, iCellOffset)
				.Style( checkboxStyle )
				.OnCheckStateChanged(this, &SOdysseyAnimationTimelineLightTable::OnOutOfPegsCheckStateChanged, iCellOffset)
				.IsChecked(this, &SOdysseyAnimationTimelineLightTable::IsOutOfPegsChecked, iCellOffset)
				.Padding(FMargin(2.f))
				[
					SNew(SImage)
					.Image(this, &SOdysseyAnimationTimelineLightTable::GetOutOfPegsButtonImage, iCellOffset)
				]
			]
		];
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

const FSlateBrush*
SOdysseyAnimationTimelineLightTable::GetOutOfPegsButtonImage(int iCellOffset) const
{
	if (mCurrentCellIndex == INDEX_NONE)
		return nullptr;

	TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();

	int cellIndex = mCurrentCellIndex + iCellOffset;
	if (cellIndex < 0 || cellIndex >= cellsContainer->GetCells().Num())
		return nullptr;

	TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCells()[cellIndex];
	if (cell->IsOutOfPegs())
		return FOdysseyStyle::GetBrush("Animation.LightTable.OutOfPegs.Button.On");

	return FOdysseyStyle::GetBrush("Animation.LightTable.OutOfPegs.Button.Off");
}

void
SOdysseyAnimationTimelineLightTable::OnOutOfPegsCheckStateChanged(ECheckBoxState iValue, int iCellOffset)
{
	FOdysseyPainterEditor* editor = mExtension->GetEditor();
	if (!editor)
		return;

	if (iValue == ECheckBoxState::Checked)
	{
		if (mCurrentCellIndex == INDEX_NONE)
			return;

		TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();

		int cellIndex = mCurrentCellIndex + iCellOffset;
		if (cellIndex < 0 || cellIndex >= cellsContainer->GetCells().Num())
			return;

		TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCells()[cellIndex];
		mExtension->GetOutOfPegsTool()->SetCell(cell);
		editor->ActivateTemporaryTool(mExtension->GetOutOfPegsTool());
	}
	else
	{
		editor->InactivateTemporaryTool();
	}
}

ECheckBoxState
SOdysseyAnimationTimelineLightTable::IsOutOfPegsChecked(int iCellOffset) const
{
	FOdysseyPainterEditor* editor = mExtension->GetEditor();
	if (!editor)
		return ECheckBoxState::Unchecked;

	UOdysseyPainterEditorTool* tool = editor->GetCurrentTool();
	if (!tool)
		return ECheckBoxState::Unchecked;

	if (mCurrentCellIndex == INDEX_NONE)
		return ECheckBoxState::Unchecked;

	TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();

	int cellIndex = mCurrentCellIndex + iCellOffset;
	if (cellIndex < 0 || cellIndex >= cellsContainer->GetCells().Num())
		return ECheckBoxState::Unchecked;

	TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCells()[cellIndex];

	bool isToolActive = tool->IsA(UOdysseyAnimationEditorOutOfPegsTool::StaticClass());
	if (isToolActive)
	{
		UOdysseyAnimationEditorOutOfPegsTool* outOfPegsTool = Cast<UOdysseyAnimationEditorOutOfPegsTool>(tool);
		if (outOfPegsTool->GetCell() == cell)
			return ECheckBoxState::Checked;
	}
	
	return ECheckBoxState::Unchecked;
}