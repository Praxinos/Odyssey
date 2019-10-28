// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Margin.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class ITableRow;
class SButton;
class STableViewBase;
struct FSlateBrush;

/**
 * About screen contents widget
 */
class SOdysseyAboutScreen : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SOdysseyAboutScreen )
        {}
    SLATE_END_ARGS()

    /**
     * Constructs the about screen widgets
     */
    void Construct(const FArguments& InArgs);

private:

    struct FLineDefinition
    {
    public:
        FText Text;
        int32 FontSize;
        FLinearColor TextColor;
        FMargin Margin;

        FLineDefinition( const FText& InText )
            : Text( InText )
            , FontSize( 9 )
            , TextColor( FLinearColor(0.5f, 0.5f, 0.5f) )
            , Margin( FMargin(6.f, 0.f, 0.f, 0.f) )
        {

        }

        FLineDefinition( const FText& InText, int32 InFontSize, const FLinearColor& InTextColor, const FMargin& InMargin )
            : Text( InText )
            , FontSize( InFontSize )
            , TextColor( InTextColor )
            , Margin( InMargin )
        {

        }
    };

    TArray< TSharedRef< FLineDefinition > > AboutLines;
    TSharedPtr<SButton> IliadButton;
    TSharedPtr<SButton> PraxinosButton;
    TSharedPtr<SButton> TwitterButton;
    TSharedPtr<SButton> FacebookButton;
    TSharedPtr<SButton> LinkedInButton;
    TSharedPtr<SButton> InstagramButton;
    TSharedPtr<SButton> YoutubeButton;

    /**
     * Makes the widget for the checkbox items in the list view
     */
    TSharedRef<ITableRow> MakeAboutTextItemWidget(TSharedRef<FLineDefinition> Item, const TSharedRef<STableViewBase>& OwnerTable);

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
};

