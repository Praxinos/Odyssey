// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorAnimationFlipSystem.h"

#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyPainterEditorAnimationProjectSettings.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimation.h"
#include "OdysseyPainterEditorAnimationUserSettings.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyStyle.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SComboButton.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

#define ALL_CELLMARKS_INDEX -2 //avoids conflict between ALL_CELLMARKS_INDEX and INDEX_NONE (which is -1)

class FOdysseyPainterEditorAnimationFlipDetailCustomization : public IPropertyTypeCustomization
{
public:
    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;
    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;

public:
    FOdysseyAnimationFlipConfiguration* GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const;
    FText GetCellMarkName(int iMarkId) const;
    const FSlateBrush* GetCellMarkSymbolIcon(int iMarkId) const;
    FSlateColor GetCellMarkColor(int iMarkId) const;

    TSharedRef<SWidget> GetLimitsCellMarkMenuContent();
    TSharedRef<SWidget> GetKeysCellMarkMenuContent();
    TSharedRef<SWidget> CreateLimitsCellMarkButtonWidget();
    TSharedRef<SWidget> CreateKeysCellMarkButtonWidget();
    TSharedRef<SWidget> CreateCellMarkMenuWidget(int iMarkId);

    FText GetSelectedLimitsCellMarkName() const;
    FText GetSelectedKeysCellMarkName() const;

    EVisibility GetLimitsCustomVisibility() const;
    EVisibility GetLimitsCellMarkVisibility() const;
    EVisibility GetKeysCellMarkVisibility() const;

public:
    FOdysseyAnimationFlipConfiguration* mFlipConfiguration = nullptr;
};

void
FOdysseyPainterEditorAnimationFlipDetailCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)

    ioHeaderRow
    .NameContent()
    [
        iStructPropertyHandle->CreatePropertyNameWidget()
    ];
}

void
FOdysseyPainterEditorAnimationFlipDetailCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils )
{
    mFlipConfiguration = GetEditStruct( iStructPropertyHandle );

    uint32 num_children;
    FPropertyAccess::Result result = iStructPropertyHandle->GetNumChildren( num_children );

    for( uint32 i = 0; i < num_children; i++ )
    {
        TSharedPtr<IPropertyHandle> handle = iStructPropertyHandle->GetChildHandle( i );
        if( !handle.IsValid() )
            continue;

        if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FOdysseyAnimationFlipConfiguration, LimitsCellMark ) )
        {
            ioChildBuilder.AddCustomRow( LOCTEXT( "flip-detail-customization.limits-cell-mark.custum-row.name", "Limits Cell Mark" ) )
            .Visibility(TAttribute<EVisibility>(this, &FOdysseyPainterEditorAnimationFlipDetailCustomization::GetLimitsCellMarkVisibility))
            .NameContent()
            [
                handle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            //.HAlign( HAlign_Fill )
            [
                SNew( SComboButton )
                .OnGetMenuContent(this, &FOdysseyPainterEditorAnimationFlipDetailCustomization::GetLimitsCellMarkMenuContent)
                .ButtonContent()
                [
                    CreateLimitsCellMarkButtonWidget()
                ]
            ];
        }
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FOdysseyAnimationFlipConfiguration, KeysCellMark ) )
        {
            ioChildBuilder.AddCustomRow( LOCTEXT( "flip-detail-customization.keys-cell-mark.custum-row.name", "Keys Cell Mark" ) )
            .Visibility(TAttribute<EVisibility>(this, &FOdysseyPainterEditorAnimationFlipDetailCustomization::GetKeysCellMarkVisibility))
            .NameContent()
            [
                handle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            //.HAlign( HAlign_Fill )
            [
                SNew( SComboButton )
                .OnGetMenuContent(this, &FOdysseyPainterEditorAnimationFlipDetailCustomization::GetKeysCellMarkMenuContent)
                .ButtonContent()
                [
                    CreateKeysCellMarkButtonWidget()
                ]
            ];
        }
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FOdysseyAnimationFlipConfiguration, LeftLimit ) )
        {
            ioChildBuilder.AddProperty( handle.ToSharedRef() ).Visibility(TAttribute<EVisibility>(this, &FOdysseyPainterEditorAnimationFlipDetailCustomization::GetLimitsCustomVisibility));
        }
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FOdysseyAnimationFlipConfiguration, RightLimit ) )
        {
            ioChildBuilder.AddProperty( handle.ToSharedRef() ).Visibility(TAttribute<EVisibility>(this, &FOdysseyPainterEditorAnimationFlipDetailCustomization::GetLimitsCustomVisibility));
        }
        else
        {
            ioChildBuilder.AddProperty( handle.ToSharedRef() );
        }
    }
}

EVisibility
FOdysseyPainterEditorAnimationFlipDetailCustomization::GetLimitsCustomVisibility() const
{
    return mFlipConfiguration->Limits == EOdysseyAnimationFlipLimits::Custom ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
FOdysseyPainterEditorAnimationFlipDetailCustomization::GetLimitsCellMarkVisibility() const
{
    return mFlipConfiguration->Limits == EOdysseyAnimationFlipLimits::CellMarks ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
FOdysseyPainterEditorAnimationFlipDetailCustomization::GetKeysCellMarkVisibility() const
{
    return mFlipConfiguration->Keys == EOdysseyAnimationFlipKeys::CellMarks ? EVisibility::Visible : EVisibility::Collapsed;
}

FOdysseyAnimationFlipConfiguration*
FOdysseyPainterEditorAnimationFlipDetailCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FOdysseyAnimationFlipConfiguration*> editStruct;

    if( iStructPropertyHandle->IsValidHandle() )
        iStructPropertyHandle->AccessRawData( reinterpret_cast<TArray<void*>&>( editStruct ) );

    if( editStruct.Num() == 1 )
        return editStruct[0];

    return nullptr;
}

TSharedRef<SWidget>
FOdysseyPainterEditorAnimationFlipDetailCustomization::GetLimitsCellMarkMenuContent()
{
    // create zoom menu
    FMenuBuilder menuBuilder( true, nullptr );
    {
        UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();

        menuBuilder.AddMenuEntry(
            FUIAction(
                FExecuteAction::CreateLambda([this]() { mFlipConfiguration->LimitsCellMark = ALL_CELLMARKS_INDEX; }),
                FCanExecuteAction::CreateLambda([]() { return true; }),
                FIsActionChecked::CreateLambda([this]() { return mFlipConfiguration->LimitsCellMark == ALL_CELLMARKS_INDEX; })
            ),
            CreateCellMarkMenuWidget(ALL_CELLMARKS_INDEX),
            NAME_None,
            TAttribute<FText>(),
            EUserInterfaceActionType::RadioButton
        );

        menuBuilder.AddSeparator();

        for (int i = 0; i < settings->AnimationCellsMarks.Num(); i++ )
        {
            menuBuilder.AddMenuEntry(
                FUIAction(
                    FExecuteAction::CreateLambda([this, i]() { mFlipConfiguration->LimitsCellMark = i; }),
                    FCanExecuteAction::CreateLambda([]() { return true; }),
                    FIsActionChecked::CreateLambda([this, i]() { return mFlipConfiguration->LimitsCellMark == i; })
                ),
                CreateCellMarkMenuWidget(i),
                NAME_None,
                TAttribute<FText>(),
                EUserInterfaceActionType::RadioButton
            );
        }
    }

    return menuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FOdysseyPainterEditorAnimationFlipDetailCustomization::GetKeysCellMarkMenuContent()
{
    // create zoom menu
    FMenuBuilder menuBuilder( true, nullptr );
    {
        UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();

        menuBuilder.AddMenuEntry(
            FUIAction(
                FExecuteAction::CreateLambda([this]() { mFlipConfiguration->KeysCellMark = ALL_CELLMARKS_INDEX; }),
                FCanExecuteAction::CreateLambda([]() { return true; }),
                FIsActionChecked::CreateLambda([this]() { return mFlipConfiguration->KeysCellMark == ALL_CELLMARKS_INDEX; })
            ),
            CreateCellMarkMenuWidget(ALL_CELLMARKS_INDEX),
            NAME_None,
            TAttribute<FText>(),
            EUserInterfaceActionType::RadioButton
        );

        menuBuilder.AddSeparator();

        for (int i = 0; i < settings->AnimationCellsMarks.Num(); i++ )
        {
            menuBuilder.AddMenuEntry(
                FUIAction(
                    FExecuteAction::CreateLambda([this, i]() { mFlipConfiguration->KeysCellMark = i; }),
                    FCanExecuteAction::CreateLambda([]() { return true; }),
                    FIsActionChecked::CreateLambda([this, i]() { return mFlipConfiguration->KeysCellMark == i; })
                ),
                CreateCellMarkMenuWidget(i),
                NAME_None,
                TAttribute<FText>(),
                EUserInterfaceActionType::RadioButton
            );
        }
    }

    return menuBuilder.MakeWidget();
}

FText
FOdysseyPainterEditorAnimationFlipDetailCustomization::GetCellMarkName(int iMarkId) const
{
    if (iMarkId == ALL_CELLMARKS_INDEX)
        return LOCTEXT( "flip-detail-customization.all-cell-marks", "All Cell Marks" );

    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    if (iMarkId < 0 || iMarkId >= settings->AnimationCellsMarks.Num())
        return LOCTEXT( "flip-detail-customization.invalid-cell-mark", "Invalid Value" );

    return FText::FromName(settings->AnimationCellsMarks[iMarkId].Name);
}

const FSlateBrush*
FOdysseyPainterEditorAnimationFlipDetailCustomization::GetCellMarkSymbolIcon(int iMarkId) const
{
    if (iMarkId == ALL_CELLMARKS_INDEX)
        return nullptr;

    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[iMarkId];
    const FSlateBrush* icon = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
    switch(markSettings.Symbol)
    {
        case EOdysseyAnimationCellMarkSymbol::Triangle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Triangle"); break;
        case EOdysseyAnimationCellMarkSymbol::FilledTriangle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Triangle"); break;
        case EOdysseyAnimationCellMarkSymbol::Circle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Circle"); break;
        case EOdysseyAnimationCellMarkSymbol::FilledCircle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Circle"); break;
        case EOdysseyAnimationCellMarkSymbol::Diamond: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Diamond"); break;
        case EOdysseyAnimationCellMarkSymbol::FilledDiamond: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Diamond"); break;
        case EOdysseyAnimationCellMarkSymbol::Star: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Star"); break;
        case EOdysseyAnimationCellMarkSymbol::FilledStar: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Star"); break;
        case EOdysseyAnimationCellMarkSymbol::Cross: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Cross"); break;
        case EOdysseyAnimationCellMarkSymbol::Checkmark: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Checkmark"); break;
    }

    return icon;
}

FSlateColor
FOdysseyPainterEditorAnimationFlipDetailCustomization::GetCellMarkColor(int iMarkId) const
{
    if (iMarkId == ALL_CELLMARKS_INDEX)
        return FSlateColor();

    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[iMarkId];
    return markSettings.Color;
}

TSharedRef<SWidget>
FOdysseyPainterEditorAnimationFlipDetailCustomization::CreateCellMarkMenuWidget(int iMarkId)
{
    return SNew(SHorizontalBox)
    + SHorizontalBox::Slot()
    .Padding(FMargin(0, 0, 4, 0))
    .AutoWidth()
    [
        SNew(SImage)
        .Image(this, &FOdysseyPainterEditorAnimationFlipDetailCustomization::GetCellMarkSymbolIcon, iMarkId)
        .ColorAndOpacity(this, &FOdysseyPainterEditorAnimationFlipDetailCustomization::GetCellMarkColor, iMarkId)
    ]
    + SHorizontalBox::Slot()
    .AutoWidth()
    [
        SNew(STextBlock)
        .Text(this, &FOdysseyPainterEditorAnimationFlipDetailCustomization::GetCellMarkName, iMarkId)
    ];
}

TSharedRef<SWidget>
FOdysseyPainterEditorAnimationFlipDetailCustomization::CreateLimitsCellMarkButtonWidget()
{
    return SNew(SHorizontalBox)
    + SHorizontalBox::Slot()
    .Padding(FMargin(0, 0, 4, 0))
    .AutoWidth()
    [
        SNew(SImage)
        .Image_Lambda([this](){ return GetCellMarkSymbolIcon(mFlipConfiguration->LimitsCellMark); })
        .ColorAndOpacity_Lambda([this](){ return GetCellMarkColor(mFlipConfiguration->LimitsCellMark); })
    ]
    + SHorizontalBox::Slot()
    .AutoWidth()
    [
        SNew(STextBlock)
        .Text_Lambda([this](){ return GetCellMarkName(mFlipConfiguration->LimitsCellMark); })
    ];
}

TSharedRef<SWidget>
FOdysseyPainterEditorAnimationFlipDetailCustomization::CreateKeysCellMarkButtonWidget()
{
    return SNew(SHorizontalBox)
    + SHorizontalBox::Slot()
    .Padding(FMargin(0, 0, 4, 0))
    .AutoWidth()
    [
        SNew(SImage)
        .Image_Lambda([this](){ return GetCellMarkSymbolIcon(mFlipConfiguration->KeysCellMark); })
        .ColorAndOpacity_Lambda([this](){ return GetCellMarkColor(mFlipConfiguration->KeysCellMark); })
    ]
    + SHorizontalBox::Slot()
    .AutoWidth()
    [
        SNew(STextBlock)
        .Text_Lambda([this](){ return GetCellMarkName(mFlipConfiguration->KeysCellMark); })
    ];
}

FText
FOdysseyPainterEditorAnimationFlipDetailCustomization::GetSelectedLimitsCellMarkName() const
{
    return GetCellMarkName(mFlipConfiguration->LimitsCellMark);
}

FText
FOdysseyPainterEditorAnimationFlipDetailCustomization::GetSelectedKeysCellMarkName() const
{
    return GetCellMarkName(mFlipConfiguration->KeysCellMark);
}

//==========================================================

void
FOdysseyPainterEditorAnimationFlipSystem::RegisterDetailCustomization()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomPropertyTypeLayout(
        FOdysseyAnimationFlipConfiguration::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateLambda(
            []()
            {
                return MakeShareable( new FOdysseyPainterEditorAnimationFlipDetailCustomization() );
            }
        )
    );
}

void
FOdysseyPainterEditorAnimationFlipSystem::UnregisterDetailCustomization()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.UnregisterCustomPropertyTypeLayout( FOdysseyAnimationFlipConfiguration::StaticStruct()->GetFName() );
}

FOdysseyPainterEditorAnimationFlipSystem::~FOdysseyPainterEditorAnimationFlipSystem()
{

}

FOdysseyPainterEditorAnimationFlipSystem::FOdysseyPainterEditorAnimationFlipSystem(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{

}

void
FOdysseyPainterEditorAnimationFlipSystem::StartFlipping(const FOdysseyAnimationFlipConfiguration& iFlipConfiguration)
{
    mAnimation = mEditor->GetAnimation();
    if (!mAnimation)
        return;

    if (mIsFlipping)
        return;

    mFlipConfiguration = iFlipConfiguration;
    mIsFlipping = true;
    mStartFrame = mEditor->GetCurrentFrame();

    mEditor->GetAnimationPlayer()->BeginScrub();
    mInitialRenderType = mEditor->GetAnimationPlayer()->GetRenderType();
    mEditor->GetAnimationPlayer()->SeekToFrame(mStartFrame);
}

void
FOdysseyPainterEditorAnimationFlipSystem::EndFlipping()
{
    if (!mIsFlipping)
        return;

    mIsFlipping = false;

    if (mFlipConfiguration.Rollback)
        mEditor->GetAnimationPlayer()->SeekToFrame(mStartFrame);

    mEditor->GetAnimationPlayer()->EndScrub();
}

bool
FOdysseyPainterEditorAnimationFlipSystem::IsFlipping() const
{
    return mIsFlipping;
}

uint64
FOdysseyPainterEditorAnimationFlipSystem::GetRenderType() const
{
    uint64 renderType = EOdysseyRenderingType::Render;
    if (mEditor->GetAnimationPlayer()->GetCursorFrame().FrameNumber.Value != mStartFrame && mFlipConfiguration.OutOfPegs)
        renderType |= EOdysseyRenderingType::OutOfPegs;

    return renderType;
}

void
FOdysseyPainterEditorAnimationFlipSystem::FlipTo(int iDelta)
{
    int frame = INDEX_NONE;
    GetKeyFrame(iDelta, frame);

    if (frame == INDEX_NONE)
        return;

    mEditor->GetAnimationPlayer()->SeekToFrame(frame);
}

bool
FOdysseyPainterEditorAnimationFlipSystem::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    EndFlipping();
    return false; //false means Unreal will continue as if we did nothing
}

bool
FOdysseyPainterEditorAnimationFlipSystem::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
    if (mIsFlipping)
    {
        FVector2D mousePosition = MouseEvent.GetScreenSpacePosition();
        FVector2D mouseDelta = mousePosition - mMousePositionReference;
        float directiondelta = mouseDelta.X;
        switch (mFlipConfiguration.Direction)
        {
            case EOdysseyAnimationFlipDirection::Horizontal:
                directiondelta = mouseDelta.X;
            break;

            case EOdysseyAnimationFlipDirection::HorizontalInverted:
                directiondelta = -mouseDelta.X;
            break;

            case EOdysseyAnimationFlipDirection::Vertical:
                directiondelta = mouseDelta.Y;
            break;

            case EOdysseyAnimationFlipDirection::VerticalInverted:
                directiondelta = -mouseDelta.Y;
            break;
        }

        const float minStep = 1;
        const float maxStep = 100;
        float step = (1.0f - mFlipConfiguration.Sensibility / 100.f) * maxStep + minStep;
        int delta = FMath::RoundToZero(directiondelta / step);

        FlipTo(delta);

        return true;
    }

    mMousePositionReference = MouseEvent.GetScreenSpacePosition();
    return false;
}

void
FOdysseyPainterEditorAnimationFlipSystem::GetKeyFrame(int iDelta, int& oFrame)
{
    int leftLimit;
    int rightLimit;
    GetLimits(mFlipConfiguration.Limits, leftLimit, rightLimit);

    switch(mFlipConfiguration.Keys)
    {
        case EOdysseyAnimationFlipKeys::AllFrames:
        {
            if (rightLimit != INDEX_NONE)
            {
                if (mFlipConfiguration.Loop)
                {
                    oFrame = mStartFrame;
                    while(oFrame + iDelta > rightLimit)
                    {
                        iDelta -= rightLimit - oFrame + 1;
                        oFrame = leftLimit;
                    }

                    while(oFrame + iDelta < leftLimit)
                    {
                        iDelta += oFrame - leftLimit + 1;
                        oFrame = rightLimit;
                    }
                    oFrame += iDelta;
                }
                else
                {
                    oFrame = FMath::Clamp(mStartFrame + iDelta, leftLimit, rightLimit);
                }
            }
            else
            {
                oFrame = FMath::Max(leftLimit, mStartFrame + iDelta);
            }
        }
        break;

        case EOdysseyAnimationFlipKeys::Cells:
        {
            UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(mAnimation->GetLayerStack());
            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
            bool useLayerLeftLimit = layer->GetCells().Num() < 2;
            bool useLayerRightLimit = useLayerLeftLimit;

            useLayerLeftLimit |= layer->GetPreBehaviour() == EOdysseyLayerImagePostBehaviour::None || layer->GetPreBehaviour() == EOdysseyLayerImagePostBehaviour::Hold;
            useLayerRightLimit |= layer->GetPostBehaviour() == EOdysseyLayerImagePostBehaviour::None || layer->GetPostBehaviour() == EOdysseyLayerImagePostBehaviour::Hold;

            int layerLeftLimit;
            int layerRightLimit;
            GetLimits(EOdysseyAnimationFlipLimits::Layer, layerLeftLimit, layerRightLimit);

            if (useLayerLeftLimit && leftLimit == 0) //left limit is never equal to INDEX_NONE, but it also works with 0 in this case
                leftLimit = layerLeftLimit;

            if (useLayerRightLimit && rightLimit == INDEX_NONE)
                rightLimit = layerRightLimit;

            TArray<FGuid> lastFrameComposition = layer->GetRenderingComposition(EOdysseyRenderingType::Render, mStartFrame);
            oFrame = mStartFrame;
            while(iDelta > 0)
            {
                int initialFrame = oFrame;
                TArray<FGuid> frameComposition;
                do
                {
                    oFrame++;

                    if(rightLimit != INDEX_NONE)
                    {
                        if (oFrame > rightLimit)
                        {
                            if (mFlipConfiguration.Loop)
                            {
                                oFrame = leftLimit;
                            }
                            else
                            {
                                oFrame = initialFrame;
                                return;
                            }
                        }
                    }

                    frameComposition = layer->GetRenderingComposition(EOdysseyRenderingType::Render, oFrame);
                }
                while(oFrame != initialFrame && frameComposition == lastFrameComposition);

                lastFrameComposition = frameComposition;
                int frame = oFrame;

                //detect if we cross the start frame or not before the next cell
                while(frame < mStartFrame)
                {
                    if (frame == mStartFrame)
                    {
                        oFrame = mStartFrame;
                        break;
                    }

                    frameComposition = layer->GetRenderingComposition(EOdysseyRenderingType::Render, frame + 1);
                    if (frameComposition != lastFrameComposition)
                        break;

                    frame++;
                }
                iDelta--;
            }

            while(iDelta < 0)
            {
                int initialFrame = oFrame;
                TArray<FGuid> frameComposition;
                do
                {
                    oFrame--;
                    if(oFrame < leftLimit)
                    {
                        if (mFlipConfiguration.Loop && rightLimit != INDEX_NONE)
                        {
                            oFrame = rightLimit;
                        }
                        else
                        {
                            oFrame = initialFrame;
                            return;
                        }
                    }
                    frameComposition = layer->GetRenderingComposition(EOdysseyRenderingType::Render, oFrame);
                }
                while(oFrame != initialFrame && frameComposition == lastFrameComposition);

                //Detect if we cross the start frame or not before the next cell
                //And navigate to the start of the cell
                lastFrameComposition = frameComposition;
                while(oFrame > leftLimit)
                {
                    if (oFrame == mStartFrame)
                        break;

                    frameComposition = layer->GetRenderingComposition(EOdysseyRenderingType::Render, oFrame - 1);
                    if (frameComposition != lastFrameComposition)
                        break;

                    oFrame--;
                }

                iDelta++;
            }
        }
        break;

        case EOdysseyAnimationFlipKeys::CellMarks:
        {
            UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(mAnimation->GetLayerStack());
            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
            const TArray<UOdysseyLayerCell*>& cells = layer->GetCells();
            FInt32Range frameRange = layer->GetFrameRange();

            int layerLeftLimit;
            int layerRightLimit;
            GetLimits(EOdysseyAnimationFlipLimits::Layer, layerLeftLimit, layerRightLimit);

            leftLimit = FMath::Max(leftLimit, layerLeftLimit);
            rightLimit = rightLimit != INDEX_NONE ? FMath::Min(rightLimit, layerRightLimit) : layerRightLimit;
            oFrame = mStartFrame;

            if (leftLimit > frameRange.GetUpperBoundValue() || rightLimit < frameRange.GetLowerBoundValue())
                return;

            int startCellIndex = INDEX_NONE;
            UOdysseyLayerCell* cell = layer->GetCellAtFrame(mStartFrame);
            if (cell)
                startCellIndex = cell->GetIndexInLayer();

            TArray<int> keyFrames;
            int currentKeyFrame = INDEX_NONE;
            if (mStartFrame < frameRange.GetLowerBoundValue())
            {
                keyFrames.Add(mStartFrame);
                currentKeyFrame = 0;
            }

            const TArray<FInt32Range>& cellsFrameRanges = layer->GetCellsFrameRanges();
            for (int i = 0; i < cells.Num(); i++)
            {
                const FInt32Range& cellRange = cellsFrameRanges[i];
                if (cellRange.GetUpperBoundValue() < leftLimit)
                    continue;

                if (cellRange.GetLowerBoundValue() > rightLimit)
                    continue;

                if (i == startCellIndex)
                {
                    keyFrames.Add(mStartFrame);
                    currentKeyFrame = keyFrames.Num() - 1;
                    continue;
                }

                if (cells[i])
                {
                    int markId = cells[i]->GetMark();
                    if (markId != INDEX_NONE && (markId == mFlipConfiguration.KeysCellMark || mFlipConfiguration.KeysCellMark == ALL_CELLMARKS_INDEX) )
                    {
                        keyFrames.Add(FMath::Clamp(cellRange.GetLowerBoundValue(), leftLimit, rightLimit));
                    }
                }
            }

            if (mStartFrame > frameRange.GetUpperBoundValue())
            {
                keyFrames.Add(mStartFrame);
                currentKeyFrame = keyFrames.Num() - 1;
            }

            while(iDelta > 0)
            {
                if (currentKeyFrame + 1 >= keyFrames.Num())
                {
                    //Loop or Stop
                    if (mFlipConfiguration.Loop)
                    {
                        currentKeyFrame = 0;
                        oFrame = keyFrames[currentKeyFrame];
                        iDelta--;
                        continue;
                    }
                    return;
                }

                currentKeyFrame++;
                oFrame = keyFrames[currentKeyFrame];
                iDelta--;
            }

            while(iDelta < 0)
            {
                if (currentKeyFrame - 1 < 0)
                {
                    //Loop or Stop
                    if (mFlipConfiguration.Loop)
                    {
                        currentKeyFrame = keyFrames.Num() - 1;
                        oFrame = keyFrames[currentKeyFrame];
                        iDelta++;
                        continue;
                    }
                    return;
                }

                currentKeyFrame--;
                oFrame = keyFrames[currentKeyFrame];
                iDelta++;
            }
        }
        break;
    }
}

void
FOdysseyPainterEditorAnimationFlipSystem::GetLimits(EOdysseyAnimationFlipLimits iLimits, int& oLeftLimit, int& oRightLimit)
{
    oLeftLimit = INDEX_NONE; //No Limit
    oRightLimit = INDEX_NONE; //No Limit

    switch(iLimits)
    {
        case EOdysseyAnimationFlipLimits::None:
            break;

        case EOdysseyAnimationFlipLimits::Animation:
        {
            FInt32Range frameRange = mAnimation->GetFrameRange();
            oLeftLimit = frameRange.GetLowerBoundValue();
            oRightLimit = frameRange.GetUpperBoundValue();
        }
        break;

        case EOdysseyAnimationFlipLimits::Layer:
        {
            UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(mAnimation->GetLayerStack());
            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
            FInt32Range frameRange = layer->GetFrameRange();
            oLeftLimit = frameRange.GetLowerBoundValue();
            oRightLimit = frameRange.GetUpperBoundValue();
        }
        break;

        case EOdysseyAnimationFlipLimits::CellMarks:
        {
            UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(mAnimation->GetLayerStack());
            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());

            int startCellIndex = INDEX_NONE;
            UOdysseyLayerCell* startCell = layer->GetCellAtFrame(mStartFrame);
            if (startCell)
                startCellIndex = startCell->GetIndexInLayer();

            if (startCellIndex == INDEX_NONE)
            {
                FInt32Range frameRange = layer->GetFrameRange();
                if (mStartFrame < frameRange.GetLowerBoundValue() )
                    startCellIndex = -1;

                if (mStartFrame > frameRange.GetUpperBoundValue() )
                    startCellIndex = layer->GetCells().Num();
            }

            //Search RightLimit
            const TArray<UOdysseyLayerCell*>& cells = layer->GetCells();
            for (int i = startCellIndex + 1; i < cells.Num(); i++)
            {
                UOdysseyLayerCell* cell = cells[i];
                if (!cell)
                    continue;

                if (cell->GetMark() == mFlipConfiguration.LimitsCellMark || ( mFlipConfiguration.LimitsCellMark == ALL_CELLMARKS_INDEX && cell->GetMark() != INDEX_NONE) )
                {
                    oRightLimit = cell->GetFrameRange().GetLowerBoundValue();
                    break;
                }
            }

            for (int i = startCellIndex - 1; i >= 0; i--)
            {
                UOdysseyLayerCell* cell = cells[i];
                if (!cell)
                    continue;

                if (cell->GetMark() == mFlipConfiguration.LimitsCellMark || ( mFlipConfiguration.LimitsCellMark == ALL_CELLMARKS_INDEX && cell->GetMark() != INDEX_NONE) )
                {
                    oLeftLimit = cell->GetFrameRange().GetLowerBoundValue();
                    break;
                }
            }
        }
        break;

        case EOdysseyAnimationFlipLimits::Custom:
        {
            oLeftLimit = FMath::Max(0, mStartFrame - mFlipConfiguration.LeftLimit);
            oRightLimit = mStartFrame + mFlipConfiguration.RightLimit;
        }
        break;
    }

    if (oLeftLimit != INDEX_NONE)
        oLeftLimit = FMath::Min(oLeftLimit, mStartFrame);
    else
        oLeftLimit = 0;

    if (oRightLimit != INDEX_NONE)
        oRightLimit = FMath::Max(oRightLimit, mStartFrame);
}

void
FOdysseyPainterEditorAnimationFlipSystem::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{

}

#undef LOCTEXT_NAMESPACE
