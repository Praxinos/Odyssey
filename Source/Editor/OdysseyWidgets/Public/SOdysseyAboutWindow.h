// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
class ODYSSEYWIDGETS_API SOdysseyAboutWindow
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SOdysseyAboutWindow )
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
        kIliad,
        kPraxinos,
        kTwitter,
        kFacebook,
        kLinkedin,
        kInstagram,
        kYoutube,
        kDiscord,
        kIliadUserDoc,
        kIliadGit,
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
