// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWindow.h"
#include "OdysseyAnimationSettings.h"

#include "SOdysseyAnimationConfigureWindow.generated.h"

USTRUCT()
struct FOdysseyAnimationConfiguration
{
    GENERATED_BODY()

public:
    /** The default name of the asset. */
    UPROPERTY( EditAnywhere, Category = "Animation Asset" )
    FString Name = TEXT( "Animation" );

    /** The other settings of the asset. */
    UPROPERTY( EditAnywhere, Category = "Animation Asset", meta=(ShowOnlyInnerProperties) )
    FOdysseyAnimationSettings Settings;
};

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationConfigureWindow
    : public SWindow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationConfigureWindow)
    {}
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs);

    bool GetWindowAnswer();

public:
    //Getters
    const FOdysseyAnimationConfiguration& GetConfiguration() const;

private:
    //Internal

    FReply OnAccept();
    FReply OnCancel();

private:
    FOdysseyAnimationConfiguration  mConfiguration;
    bool                            mWindowAnswer;
    TSharedPtr<class SEnumComboBox> mFormatComboBox;
};
