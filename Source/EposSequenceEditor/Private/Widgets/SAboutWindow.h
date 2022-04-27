// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Layout/Margin.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class ITableRow;
class SButton;
class STableViewBase;
struct FSlateBrush;

/**
 * About window contents widget
 */
class SAboutWindow
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SAboutWindow )
    {
    }
    SLATE_END_ARGS()

    /**
     * Constructs the about window widgets
     */
    void Construct( const FArguments& iArgs );

    static void Open(TSharedPtr<SWindow> iParentWindow);

private:
    FReply OnClose();

private:
    enum class ELogo
    {
        kEpos,
        kPraxinos,
        kTwitter,
        kFacebook,
        kLinkedin,
        kInstagram,
        kYoutube,
        kDiscord,
        kEposUserDoc,
        kEposGit,
    };

    struct FLogo
    {
        FName mBrushName;
        FText mTooltip;
        FText mUrl;
    };

    TMap<ELogo, FLogo> mLogos;

    TSharedRef<SWidget> NewButtonImage( ELogo iLogo );
};
