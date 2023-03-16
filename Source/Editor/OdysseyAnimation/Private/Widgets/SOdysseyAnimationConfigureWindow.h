// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationConfiguration.h"

#include "CoreMinimal.h"
#include "Widgets/SWindow.h"
#include "SEnumCombo.h"

#include <ULIS>

class SOdysseyAnimationConfigureWindow
    : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationConfigureWindow)
	{

	}
	SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs);
    void Construct( const FArguments& iArgs, const FOdysseyAnimationConfiguration& iProperties);

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
    TSharedPtr< SEnumComboBox >     mFormatComboBox;
};
