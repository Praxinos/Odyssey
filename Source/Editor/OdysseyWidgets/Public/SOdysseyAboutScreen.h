// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
 * About screen contents widget
 */
class ODYSSEYWIDGETS_API SOdysseyAboutScreen
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SOdysseyAboutScreen )
    {
    }
    SLATE_END_ARGS()

    /**
     * Constructs the about screen widgets
     */
    void Construct( const FArguments& iArgs );

    SOdysseyAboutScreen();

private:
    struct FLineDefinition;

    /**
     * Makes the widget for the checkbox items in the list view
     */
    TSharedRef<ITableRow> MakeAboutTextItemWidget( TSharedRef<FLineDefinition> iItem, const TSharedRef<STableViewBase>& iOwnerTable );
    //void OnListViewButtonClicked( TSharedRef<FLineDefinition> iItem );

private:
    const FSlateBrush* GetIliadButtonBrush() const;
    const FSlateBrush* GetPraxinosButtonBrush() const;
    const FSlateBrush* GetTwitterButtonBrush() const;
    const FSlateBrush* GetFacebookButtonBrush() const;
    const FSlateBrush* GetLinkedInButtonBrush() const;
    const FSlateBrush* GetInstagramButtonBrush() const;
    const FSlateBrush* GetYoutubeButtonBrush() const;
    const FSlateBrush* GetDiscordButtonBrush() const;
    const FSlateBrush* GetUserDocButtonBrush() const;
    const FSlateBrush* GetGitButtonBrush() const;


    FReply OnIliadButtonClicked();
    FReply OnPraxinosButtonClicked();
    FReply OnTwitterButtonClicked();
    FReply OnFacebookButtonClicked();
    FReply OnLinkedInButtonClicked();
    FReply OnInstagramButtonClicked();
    FReply OnYoutubeButtonClicked();
    FReply OnDiscordButtonClicked();
    FReply OnUserDocButtonClicked();
    FReply OnGitButtonClicked();

    FReply OnClose();

private:
    TArray< TSharedRef< FLineDefinition > > mAboutLines;
    TSharedPtr<SButton> mIliadButton;
    TSharedPtr<SButton> mPraxinosButton;
    TSharedPtr<SButton> mTwitterButton;
    TSharedPtr<SButton> mFacebookButton;
    TSharedPtr<SButton> mLinkedInButton;
    TSharedPtr<SButton> mInstagramButton;
    TSharedPtr<SButton> mYoutubeButton;
    TSharedPtr<SButton> mDiscordButton;
    TSharedPtr<SButton> mUserDocButton;
    TSharedPtr<SButton> mGitButton;


    FText mPraxinosUrl;
    FText mForumUrl;
    FText mIliadUrl;
    FText mContactUsUrl;
    FText mExternalLibsULISUrl;
    FText mExternalLibsLittleCMSUrl;
    FText mExternalLibsGLMUrl;
    FText mExternalLibsBoostPreprocessorUrl;
    FText mTwitterUrl;
    FText mFacebookUrl;
    FText mLinkedInUrl;
    FText mInstagramUrl;
    FText mYoutubeUrl;
    FText mDiscordUrl;
    FText mUserDocUrl;
    FText mGitUrl;
};
