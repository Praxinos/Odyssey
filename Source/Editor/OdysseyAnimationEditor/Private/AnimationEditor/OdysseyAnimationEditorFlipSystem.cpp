#include "OdysseyAnimationEditorFlipSystem.h"
#include "OdysseyAnimationEditorUserSettings.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SComboButton.h"
#include "AnimationEditor/OdysseyAnimationEditorProjectSettings.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"

#include "AnimationEditor/OdysseyAnimationEditorCommands.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

#define ALL_CELLMARKS_INDEX -2 //avoids conflict between ALL_CELLMARKS_INDEX and INDEX_NONE (which is -1)

class FOdysseyAnimationEditorFlipDetailCustomization : public IPropertyTypeCustomization
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
FOdysseyAnimationEditorFlipDetailCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)

    ioHeaderRow
    .NameContent()
    [
        iStructPropertyHandle->CreatePropertyNameWidget()
    ];
}

void
FOdysseyAnimationEditorFlipDetailCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils )
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
            .Visibility(TAttribute<EVisibility>(this, &FOdysseyAnimationEditorFlipDetailCustomization::GetLimitsCellMarkVisibility))
            .NameContent()
            [
                handle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            //.HAlign( HAlign_Fill )
            [
                SNew( SComboButton )
                .OnGetMenuContent(this, &FOdysseyAnimationEditorFlipDetailCustomization::GetLimitsCellMarkMenuContent)
                .ButtonContent()
                [
                    CreateLimitsCellMarkButtonWidget()
                ]
            ];
        }
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FOdysseyAnimationFlipConfiguration, KeysCellMark ) )
        {
            ioChildBuilder.AddCustomRow( LOCTEXT( "flip-detail-customization.keys-cell-mark.custum-row.name", "Keys Cell Mark" ) )
            .Visibility(TAttribute<EVisibility>(this, &FOdysseyAnimationEditorFlipDetailCustomization::GetKeysCellMarkVisibility))
            .NameContent()
            [
                handle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            //.HAlign( HAlign_Fill )
            [
                SNew( SComboButton )
                .OnGetMenuContent(this, &FOdysseyAnimationEditorFlipDetailCustomization::GetKeysCellMarkMenuContent)
                .ButtonContent()
                [
                    CreateKeysCellMarkButtonWidget()
                ]
            ];
        }
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FOdysseyAnimationFlipConfiguration, LeftLimit ) )
        {
            ioChildBuilder.AddProperty( handle.ToSharedRef() ).Visibility(TAttribute<EVisibility>(this, &FOdysseyAnimationEditorFlipDetailCustomization::GetLimitsCustomVisibility));
        }
        else if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FOdysseyAnimationFlipConfiguration, RightLimit ) )
        {
            ioChildBuilder.AddProperty( handle.ToSharedRef() ).Visibility(TAttribute<EVisibility>(this, &FOdysseyAnimationEditorFlipDetailCustomization::GetLimitsCustomVisibility));
        }
        else
        {
            ioChildBuilder.AddProperty( handle.ToSharedRef() );
        }
    }
}

EVisibility
FOdysseyAnimationEditorFlipDetailCustomization::GetLimitsCustomVisibility() const
{
    return mFlipConfiguration->Limits == EOdysseyAnimationFlipLimits::Custom ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
FOdysseyAnimationEditorFlipDetailCustomization::GetLimitsCellMarkVisibility() const
{
    return mFlipConfiguration->Limits == EOdysseyAnimationFlipLimits::CellMarks ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
FOdysseyAnimationEditorFlipDetailCustomization::GetKeysCellMarkVisibility() const
{
    return mFlipConfiguration->Keys == EOdysseyAnimationFlipKeys::CellMarks ? EVisibility::Visible : EVisibility::Collapsed;
}

FOdysseyAnimationFlipConfiguration*
FOdysseyAnimationEditorFlipDetailCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FOdysseyAnimationFlipConfiguration*> editStruct;

    if( iStructPropertyHandle->IsValidHandle() )
        iStructPropertyHandle->AccessRawData( reinterpret_cast<TArray<void*>&>( editStruct ) );

    if( editStruct.Num() == 1 )
        return editStruct[0];

    return nullptr;
}

TSharedRef<SWidget>
FOdysseyAnimationEditorFlipDetailCustomization::GetLimitsCellMarkMenuContent()
{
    // create zoom menu
    FMenuBuilder menuBuilder( true, nullptr );
    {
        UOdysseyAnimationEditorProjectSettings* settings = UOdysseyAnimationEditorProjectSettings::Get();

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
FOdysseyAnimationEditorFlipDetailCustomization::GetKeysCellMarkMenuContent()
{
    // create zoom menu
    FMenuBuilder menuBuilder( true, nullptr );
    {
        UOdysseyAnimationEditorProjectSettings* settings = UOdysseyAnimationEditorProjectSettings::Get();

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
FOdysseyAnimationEditorFlipDetailCustomization::GetCellMarkName(int iMarkId) const
{
    if (iMarkId == ALL_CELLMARKS_INDEX)
        return LOCTEXT( "flip-detail-customization.all-cell-marks", "All Cell Marks" );

    UOdysseyAnimationEditorProjectSettings* settings = UOdysseyAnimationEditorProjectSettings::Get();
    if (iMarkId < 0 || iMarkId >= settings->AnimationCellsMarks.Num())
        return LOCTEXT( "flip-detail-customization.invalid-cell-mark", "Invalid Value" );

    return FText::FromName(settings->AnimationCellsMarks[iMarkId].Name);
}

const FSlateBrush*
FOdysseyAnimationEditorFlipDetailCustomization::GetCellMarkSymbolIcon(int iMarkId) const
{
    if (iMarkId == ALL_CELLMARKS_INDEX)
        return nullptr;

    UOdysseyAnimationEditorProjectSettings* settings = UOdysseyAnimationEditorProjectSettings::Get();
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
FOdysseyAnimationEditorFlipDetailCustomization::GetCellMarkColor(int iMarkId) const
{
    if (iMarkId == ALL_CELLMARKS_INDEX)
        return FSlateColor();

    UOdysseyAnimationEditorProjectSettings* settings = UOdysseyAnimationEditorProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[iMarkId];
    return markSettings.Color;
}

TSharedRef<SWidget>
FOdysseyAnimationEditorFlipDetailCustomization::CreateCellMarkMenuWidget(int iMarkId)
{
    return SNew(SHorizontalBox)
    + SHorizontalBox::Slot()
    .Padding(FMargin(0, 0, 4, 0))
    .AutoWidth()
    [
        SNew(SImage)
        .Image(this, &FOdysseyAnimationEditorFlipDetailCustomization::GetCellMarkSymbolIcon, iMarkId)
        .ColorAndOpacity(this, &FOdysseyAnimationEditorFlipDetailCustomization::GetCellMarkColor, iMarkId)
    ]
    + SHorizontalBox::Slot()
    .AutoWidth()
    [
        SNew(STextBlock)
        .Text(this, &FOdysseyAnimationEditorFlipDetailCustomization::GetCellMarkName, iMarkId)
    ];
}

TSharedRef<SWidget>
FOdysseyAnimationEditorFlipDetailCustomization::CreateLimitsCellMarkButtonWidget()
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
FOdysseyAnimationEditorFlipDetailCustomization::CreateKeysCellMarkButtonWidget()
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
FOdysseyAnimationEditorFlipDetailCustomization::GetSelectedLimitsCellMarkName() const
{
    return GetCellMarkName(mFlipConfiguration->LimitsCellMark);
}

FText
FOdysseyAnimationEditorFlipDetailCustomization::GetSelectedKeysCellMarkName() const
{
    return GetCellMarkName(mFlipConfiguration->KeysCellMark);
}

//==========================================================

void
FOdysseyAnimationEditorFlipSystem::RegisterDetailCustomization()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomPropertyTypeLayout( 
        FOdysseyAnimationFlipConfiguration::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateLambda( 
            []()
            {
                return MakeShareable( new FOdysseyAnimationEditorFlipDetailCustomization() );
            }
        )
    );
}

void
FOdysseyAnimationEditorFlipSystem::UnregisterDetailCustomization()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.UnregisterCustomPropertyTypeLayout( FOdysseyAnimationFlipConfiguration::StaticStruct()->GetFName() );
}

FOdysseyAnimationEditorFlipSystem::~FOdysseyAnimationEditorFlipSystem()
{

}

FOdysseyAnimationEditorFlipSystem::FOdysseyAnimationEditorFlipSystem(FOdysseyAnimationEditorExtension* iExtension)
    : mExtension(iExtension)
{
    
}

void
FOdysseyAnimationEditorFlipSystem::StartFlipping(const FOdysseyAnimationFlipConfiguration& iFlipConfiguration)
{
    mAnimation = mExtension->Animation();
    if (!mAnimation)
        return;

    if (mIsFlipping)
        return;

    mFlipConfiguration = iFlipConfiguration;
    mIsFlipping = true;
    mStartFrame = mAnimation->CurrentFrame;

    mExtension->Player()->Stop();
    mInitialRenderType = mExtension->Player()->GetRenderType();
    mExtension->Player()->SeekToFrame(mStartFrame);
}

void
FOdysseyAnimationEditorFlipSystem::EndFlipping()
{
    if (!mIsFlipping)
        return;

    mIsFlipping = false;

    if (mFlipConfiguration.Rollback)
    {
        mExtension->Player()->Stop();
        mExtension->Player()->SetRenderType(mInitialRenderType);
        mExtension->Player()->SeekToFrame(mStartFrame);
    }
    else
    {
        mExtension->Player()->SetRenderType(mInitialRenderType);
        int frame = mAnimation->GetFrameIndexAtTime(mExtension->Player()->GetCurrentTime());
        FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, GET_MEMBER_NAME_CHECKED( UOdysseyAnimation, CurrentFrame), frame);
    }
}

void
FOdysseyAnimationEditorFlipSystem::FlipTo(int iDelta)
{
    int frame = INDEX_NONE;
    GetKeyFrame(iDelta, frame);

    if (frame == INDEX_NONE)
        return;
        
    if (frame == mStartFrame)
    {
        mExtension->Player()->SetRenderType(IOdysseyImageRenderer::eRenderType::Render);
    }
    else
    {
        mExtension->Player()->SetRenderType(mFlipConfiguration.OutOfPegs ? IOdysseyImageRenderer::eRenderType::RenderOutOfPegs : IOdysseyImageRenderer::eRenderType::Render);
    }
    mExtension->Player()->SeekToFrame(frame);
}

bool
FOdysseyAnimationEditorFlipSystem::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    if (!InKeyEvent.GetKey().IsModifierKey())
        mLastKey = InKeyEvent.GetKey();

    if (mLastKey != FKey())
    {
        FModifierKeysState modifierKeysState = FSlateApplication::Get().GetModifierKeys();
        const FInputChord activeChord(mLastKey,
            EModifierKey::FromBools(
                modifierKeysState.IsControlDown(),
                modifierKeysState.IsAltDown(),
                modifierKeysState.IsShiftDown(),
                modifierKeysState.IsCommandDown()
            )
        );


        for (int i = 0; i < FOdysseyAnimationEditorCommands::Get().Flip.Num(); i++)
        {
            if (FOdysseyAnimationEditorCommands::Get().Flip[i]->HasActiveChord(activeChord))
            {
                const UOdysseyAnimationEditorUserSettings* settings = UOdysseyAnimationEditorUserSettings::Get();
                StartFlipping(settings->FlipConfigurations[i]);
                return false;
            }
        }
    }

    EndFlipping();

    return false; //false means Unreal will continue as if we did nothing
}

bool
FOdysseyAnimationEditorFlipSystem::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == mLastKey)
        mLastKey = FKey();

    if (mLastKey != FKey())
    {
        FModifierKeysState modifierKeysState = FSlateApplication::Get().GetModifierKeys();
        
        const FInputChord activeChord(mLastKey,
            EModifierKey::FromBools(
                modifierKeysState.IsControlDown(),
                modifierKeysState.IsAltDown(),
                modifierKeysState.IsShiftDown(),
                modifierKeysState.IsCommandDown()
            )
        );

        for (int i = 0; i < FOdysseyAnimationEditorCommands::Get().Flip.Num(); i++)
        {
            if (FOdysseyAnimationEditorCommands::Get().Flip[i]->HasActiveChord(activeChord))
            {
                const UOdysseyAnimationEditorUserSettings* settings = UOdysseyAnimationEditorUserSettings::Get();
                StartFlipping(settings->FlipConfigurations[i]);
                return false;
            }
        }
    }

    EndFlipping();

    return false; //false means Unreal will continue as if we did nothing
}

bool
FOdysseyAnimationEditorFlipSystem::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
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
FOdysseyAnimationEditorFlipSystem::GetKeyFrame(int iDelta, int& oFrame)
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
            UOdysseyAnimationLayerStack* layerStack = mAnimation->GetLayerStack();
            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
            bool useLayerLeftLimit = layer->GetCells().Num() < 2;
            bool useLayerRightLimit = useLayerLeftLimit;

            useLayerLeftLimit |= layer->PreBehaviour == EOdysseyAnimationLayerImagePostBehaviour::None || layer->PreBehaviour == EOdysseyAnimationLayerImagePostBehaviour::Hold;
            useLayerRightLimit |= layer->PostBehaviour == EOdysseyAnimationLayerImagePostBehaviour::None || layer->PostBehaviour == EOdysseyAnimationLayerImagePostBehaviour::Hold;

            int layerLeftLimit;
            int layerRightLimit;
            GetLimits(EOdysseyAnimationFlipLimits::Layer, layerLeftLimit, layerRightLimit);

            if (useLayerLeftLimit && leftLimit == 0) //left limit is never equal to INDEX_NONE, but it also works with 0 in this case
                leftLimit = layerLeftLimit;

            if (useLayerRightLimit && rightLimit == INDEX_NONE)
                rightLimit = layerRightLimit;

            TArray<FGuid> lastFrameComposition = layer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Editor, mStartFrame);
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

                    frameComposition = layer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Editor, oFrame);
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

                    frameComposition = layer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Editor, frame + 1);
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
                    frameComposition = layer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Editor, oFrame);
                }
                while(oFrame != initialFrame && frameComposition == lastFrameComposition);

				//Detect if we cross the start frame or not before the next cell
                //And navigate to the start of the cell
                lastFrameComposition = frameComposition;
                while(oFrame > leftLimit)
                {
                    if (oFrame == mStartFrame)
                        break;

                    frameComposition = layer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Editor, oFrame - 1);
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
            UOdysseyAnimationLayerStack* layerStack = mAnimation->GetLayerStack();
            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
            const TArray<UOdysseyAnimationCell*>& cells = layer->GetCells();
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
            UOdysseyAnimationCell* cell = layer->GetCellAtFrame(mStartFrame);
			if (cell)
				startCellIndex = cell->IndexInLayer;

            TArray<int> keyFrames;
            int currentKeyFrame = INDEX_NONE;
            if (mStartFrame < frameRange.GetLowerBoundValue())
            {
                keyFrames.Add(mStartFrame);
                currentKeyFrame = 0;
            }

            for (int i = 0; i < cells.Num(); i++)
            {
                FInt32Range cellRange = cells[i]->GetFrameRange();
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

                int markId = cells[i]->Mark;
                if (markId != INDEX_NONE && (markId == mFlipConfiguration.KeysCellMark || mFlipConfiguration.KeysCellMark == ALL_CELLMARKS_INDEX) )
                {
                    keyFrames.Add(FMath::Clamp(cellRange.GetLowerBoundValue(), leftLimit, rightLimit));
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
FOdysseyAnimationEditorFlipSystem::GetLimits(EOdysseyAnimationFlipLimits iLimits, int& oLeftLimit, int& oRightLimit)
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
            UOdysseyAnimationLayerStack* layerStack = mAnimation->GetLayerStack();
            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
            FInt32Range frameRange = layer->GetFrameRange();
            oLeftLimit = frameRange.GetLowerBoundValue();
            oRightLimit = frameRange.GetUpperBoundValue();
        }
        break;

        case EOdysseyAnimationFlipLimits::CellMarks: 
        {
            UOdysseyAnimationLayerStack* layerStack = mAnimation->GetLayerStack();
            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());

            int startCellIndex = INDEX_NONE;
			UOdysseyAnimationCell* startCell = layer->GetCellAtFrame(mStartFrame);
			if (startCell)
				startCellIndex = startCell->IndexInLayer;

            if (startCellIndex == INDEX_NONE)
            {
                FInt32Range frameRange = layer->GetFrameRange();
                if (mStartFrame < frameRange.GetLowerBoundValue() )
                    startCellIndex = -1;

                if (mStartFrame > frameRange.GetUpperBoundValue() )
                    startCellIndex = layer->GetCells().Num();
            }

            //Search RightLimit
            const TArray<UOdysseyAnimationCell*>& cells = layer->GetCells();
            for (int i = startCellIndex + 1; i < cells.Num(); i++)
            {
                UOdysseyAnimationCell* cell = cells[i];
                if (cell->Mark == mFlipConfiguration.LimitsCellMark || ( mFlipConfiguration.LimitsCellMark == ALL_CELLMARKS_INDEX && cell->Mark != INDEX_NONE) )
                {
                    oRightLimit = cell->GetFrameRange().GetLowerBoundValue();
                    break;
                }
            }

            for (int i = startCellIndex - 1; i >= 0; i--)
            {
                UOdysseyAnimationCell* cell = cells[i];
                if (cell->Mark == mFlipConfiguration.LimitsCellMark || ( mFlipConfiguration.LimitsCellMark == ALL_CELLMARKS_INDEX && cell->Mark != INDEX_NONE) )
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
FOdysseyAnimationEditorFlipSystem::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
    
}