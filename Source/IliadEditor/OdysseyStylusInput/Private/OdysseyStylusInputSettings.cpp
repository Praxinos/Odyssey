// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyStylusInputSettings.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"

#include "IOdysseyStylusInputModule.h"
#include "StylusInputInterface.h"

#define LOCTEXT_NAMESPACE "StylusInput"

void
FOdysseyStylusInputSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& iDetailBuilder)
{
    mStylusAPIProperty = iDetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UOdysseyStylusInputSettings, StylusInputDriver));

    for (const FName& interfaceName : UE::StylusInput::GetAvailableInterfaces())
    {
        mStylusAPIs.Add(MakeShared<FName>(interfaceName));

        mStylusAPIsLabels.Add(interfaceName, UOdysseyStylusInputSettings::GetFormatText(interfaceName));
    }

    iDetailBuilder.EditDefaultProperty(mStylusAPIProperty)->CustomWidget()
        .NameContent()
        [
            mStylusAPIProperty->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(250.f)
        [
            SNew(SComboBox<TSharedPtr<FName>>)
                .OptionsSource(&mStylusAPIs)
                .OnGenerateWidget_Lambda([this](TSharedPtr<FName> item)
                    {
                        return SNew(STextBlock)
                            .Text(mStylusAPIsLabels[*item]);
                    })
                .OnSelectionChanged_Lambda([this](TSharedPtr<FName> newValue, ESelectInfo::Type)
                    {
                        if (newValue.IsValid())
                        {
                            mStylusAPIProperty->SetValue(*newValue);
                        }
                    })
                .Content()
                [
                    SNew(STextBlock)
                        .Text_Lambda([this]()
                            {
                                FName currentValue;
                                mStylusAPIProperty->GetValue(currentValue);

                                return mStylusAPIsLabels.Contains(currentValue)
                                    ? mStylusAPIsLabels[currentValue]
                                    : FText::FromName(currentValue);
                            })
                ]
        ];
}

FOnStylusInputDriverChanged UOdysseyStylusInputSettings::OnStylusInputDriverChanged;

UOdysseyStylusInputSettings::UOdysseyStylusInputSettings( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
    , StylusInputDriver("None")
{
}

void
UOdysseyStylusInputSettings::PostEditChangeProperty( struct FPropertyChangedEvent& iPropertyChangedEvent )
{
    Super::PostEditChangeProperty( iPropertyChangedEvent );
}

FName
UOdysseyStylusInputSettings::GetStylusDriver() const
{
    if (StylusInputDriver != "None")
    {
        return StylusInputDriver;
    }

    TArray<FName> Interfaces = UE::StylusInput::GetAvailableInterfaces();

    return Interfaces.Num() > 0 ? Interfaces[0] : "None";
}

//static
FText
UOdysseyStylusInputSettings::GetFormatText(FName InStylusInputDriver)
{
    static const TMap<FName, FText> DriverLabels =
    {
        { "WinTab",      FText::FromString("Wintab") },
        { "RealTimeStylus",  FText::FromString("Windows Ink - RealTimeStylus") },
        { "NSEvent", FText::FromString("NSEvent") }
    };

    if (const FText* Found = DriverLabels.Find(InStylusInputDriver))
    {
        return *Found;
    }

    return FText::FromName(InStylusInputDriver);
}

TArray<FName>
UOdysseyStylusInputSettings::GetAvailableStylusDrivers()
{
    return UE::StylusInput::GetAvailableInterfaces();
}

FName UOdysseyStylusInputSettings::GetContainerName() const
{
    return TEXT("Editor");
}

FName UOdysseyStylusInputSettings::GetCategoryName() const
{
    return TEXT("Plugins");
}

#undef LOCTEXT_NAMESPACE
