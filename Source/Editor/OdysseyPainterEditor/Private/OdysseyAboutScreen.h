// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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
class SOdysseyAboutScreen
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

private:
    struct FLineDefinition;

    /**
     * Makes the widget for the checkbox items in the list view
     */
    TSharedRef<ITableRow> MakeAboutTextItemWidget( TSharedRef<FLineDefinition> iItem, const TSharedRef<STableViewBase>& iOwnerTable );

private:
    const FSlateBrush* GetIliadButtonBrush() const;
    const FSlateBrush* GetPraxinosButtonBrush() const;
    const FSlateBrush* GetTwitterButtonBrush() const;
    const FSlateBrush* GetFacebookButtonBrush() const;
    const FSlateBrush* GetLinkedInButtonBrush() const;
    const FSlateBrush* GetInstagramButtonBrush() const;
    const FSlateBrush* GetYoutubeButtonBrush() const;

    FReply OnIliadButtonClicked();
    FReply OnPraxinosButtonClicked();
    FReply OnTwitterButtonClicked();
    FReply OnFacebookButtonClicked();
    FReply OnLinkedInButtonClicked();
    FReply OnInstagramButtonClicked();
    FReply OnYoutubeButtonClicked();
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
};
