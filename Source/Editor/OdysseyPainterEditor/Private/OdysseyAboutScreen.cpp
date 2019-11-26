// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyAboutScreen.h"
#include "Fonts/SlateFontInfo.h"
#include "Misc/Paths.h"
#include "Misc/EngineVersion.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/SListView.h"
#include "Styling/CoreStyle.h"
#include "EditorStyleSet.h"
#include "UnrealEdMisc.h"
#include "OdysseyStyleSet.h"


#define LOCTEXT_NAMESPACE "AboutScreen"

void
SOdysseyAboutScreen::Construct(const FArguments& InArgs)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4428)    // universal-character-name encountered in source
#endif
    AboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "Copyright1", "Copyright 2018-2019 Praxinos SCOP arl. All rights reserved" ), 11, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.f ) ) ) );
    AboutLines.Add(MakeShareable( new FLineDefinition( LOCTEXT( "Copyright2", "Praxinos Team : Elodie Moog, Fabrice Debarge, Thomas Schmitt, Clément Berthaud, Naomiki Sato, Antoine Antin, Eric Scholl, Michael Schreiner"), 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.0f, 2.0f ) ) ) );
    AboutLines.Add(MakeShareable( new FLineDefinition( LOCTEXT( "Copyright3", "ILIAD : Intelligent Layered Imaging Architecture for Drawing"), 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.0f, 2.0f ) ) ) );
    AboutLines.Add(MakeShareable( new FLineDefinition( LOCTEXT( "Copyright4", "IDDN FR.001.250001.002.S.P.2019.000.00000"), 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.0f, 2.0f ) ) ) );
    
#ifdef _MSC_VER
#pragma warning(pop)
#endif

    FText Version = LOCTEXT("VersionLabel", "Version: Alpha 0.1");

    ChildSlot
        [
            SNew(SOverlay)
            +SOverlay::Slot()
            [
                SNew(SVerticalBox)
                +SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Top)
                    .Padding(FMargin(10.f, 10.f, 0.f, 0.f))
                    [
                        SAssignNew( PraxinosButton, SButton )
                        .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                        .OnClicked(this, &SOdysseyAboutScreen::OnPraxinosButtonClicked)
                        [
                            SNew(SImage).Image(this, &SOdysseyAboutScreen::GetPraxinosButtonBrush)
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .FillWidth(1.f)
                    .HAlign(HAlign_Right)
                    .Padding(FMargin(0.f, 10.f, 7.f, 0.f))
                    [
                        SNew(SVerticalBox)
                        +SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SAssignNew( IliadButton, SButton )
                            .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                            .OnClicked(this, &SOdysseyAboutScreen::OnIliadButtonClicked)
                            [
                                SNew(SImage).Image(this, &SOdysseyAboutScreen::GetIliadButtonBrush)
                            ]
                        ]
                        +SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SEditableText)
                            .ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))
                            .IsReadOnly(true)
                            .Text( Version )
                        ]
                    ]
                ]
                +SVerticalBox::Slot()
                .Padding(FMargin(5.f, 5.f, 5.f, 5.f))
                .VAlign(VAlign_Top)
                [
                    SNew(SListView<TSharedRef<FLineDefinition>>)
                    .ListItemsSource(&AboutLines)
                    .OnGenerateRow(this, &SOdysseyAboutScreen::MakeAboutTextItemWidget)
                    .SelectionMode( ESelectionMode::None )
                ]
                +SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                    +SHorizontalBox::Slot()
                    .HAlign(HAlign_Left)
                    .FillWidth( 0.4f )
                    [
                        SNew(SHorizontalBox)
                        +SHorizontalBox::Slot()
                        .HAlign(HAlign_Left)
                        .Padding(FMargin(5.f, 0.f, 5.f, 5.f))
                        [
                            SAssignNew( TwitterButton, SButton )
                            .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                            .ToolTipText(LOCTEXT("TwitterToolTip", "Praxinos on Twitter"))
                            .OnClicked( this, &SOdysseyAboutScreen::OnTwitterButtonClicked )
                            [
                                SNew(SImage).Image(this, &SOdysseyAboutScreen::GetTwitterButtonBrush)
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .HAlign(HAlign_Left)
                        .Padding(FMargin(5.f, 0.f, 5.f, 5.f))
                        [
                            SAssignNew(FacebookButton, SButton)
                            .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                            .ToolTipText(LOCTEXT("FacebookToolTip", "Praxinos on Facebook"))
                            .OnClicked(this, &SOdysseyAboutScreen::OnFacebookButtonClicked)
                            [
                                SNew(SImage).Image(this, &SOdysseyAboutScreen::GetFacebookButtonBrush)
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .HAlign(HAlign_Left)
                        .Padding(FMargin(5.f, 0.f, 5.f, 5.f))
                        [
                            SAssignNew(LinkedInButton, SButton)
                            .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                            .ToolTipText(LOCTEXT("LinkedInToolTip", "Praxinos on LinkedIn"))
                            .OnClicked(this, &SOdysseyAboutScreen::OnLinkedInButtonClicked)
                            [
                                SNew(SImage).Image(this, &SOdysseyAboutScreen::GetLinkedInButtonBrush)
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .HAlign(HAlign_Left)
                        .Padding(FMargin(5.f, 0.f, 5.f, 5.f))
                        [
                            SAssignNew(LinkedInButton, SButton)
                            .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                            .ToolTipText(LOCTEXT("InstagramToolTip", "Praxinos on Instagram"))
                            .OnClicked(this, &SOdysseyAboutScreen::OnInstagramButtonClicked)
                            [
                                SNew(SImage).Image(this, &SOdysseyAboutScreen::GetInstagramButtonBrush)
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .HAlign(HAlign_Left)
                        .Padding(FMargin(5.f, 0.f, 5.f, 5.f))
                        [
                            SAssignNew(YoutubeButton, SButton)
                            .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                            .ToolTipText(LOCTEXT("YoutubeToolTip", "Praxinos on Youtube"))
                            .OnClicked(this, &SOdysseyAboutScreen::OnYoutubeButtonClicked)
                            [
                                SNew(SImage).Image(this, &SOdysseyAboutScreen::GetYoutubeButtonBrush)
                            ]
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign(HAlign_Right)
                    .FillWidth( 0.6f )
                    [
                        SNew(SHorizontalBox)
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .HAlign(HAlign_Right)
                        .VAlign(VAlign_Bottom)
                        .Padding(FMargin(5.f, 0.f, 5.f,5.f))
                        [
                            SNew(SButton)
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Center)
                            .Text(LOCTEXT("Close", "Close"))
                            .ButtonColorAndOpacity(FLinearColor(0.6f, 0.6f, 0.6f))
                            .OnClicked(this, &SOdysseyAboutScreen::OnClose)
                        ]
                    ]
                ]
            ]
        ];
}

TSharedRef<ITableRow>
SOdysseyAboutScreen::MakeAboutTextItemWidget(TSharedRef<FLineDefinition> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    if( Item->Text.IsEmpty() )
    {
        return
            SNew(STableRow< TSharedPtr<FString> >, OwnerTable)
            .Padding(6.0f)
            [
                SNew(SSpacer)
            ];
    }
    else
    {
        return
            SNew(STableRow< TSharedPtr<FString> >, OwnerTable)
            .Padding( Item->Margin )
            [
                SNew(STextBlock)
                .ColorAndOpacity( Item->TextColor )
                .Font( FCoreStyle::GetDefaultFontStyle("Regular", Item->FontSize) )
                .Text( Item->Text )
            ];
    }
}

const FSlateBrush*
SOdysseyAboutScreen::GetIliadButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT("About.Iliad") );
}

const FSlateBrush*
SOdysseyAboutScreen::GetPraxinosButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT("About.Praxinos") );
}

const FSlateBrush*
SOdysseyAboutScreen::GetTwitterButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT("About.Twitter") );
}

const FSlateBrush*
SOdysseyAboutScreen::GetFacebookButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT("About.Facebook") );
}

const FSlateBrush*
SOdysseyAboutScreen::GetLinkedInButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT("About.LinkedIn") );
}

const FSlateBrush*
SOdysseyAboutScreen::GetInstagramButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT("About.Instagram") );
}

const FSlateBrush*
SOdysseyAboutScreen::GetYoutubeButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT("About.Youtube") );
}

FReply
SOdysseyAboutScreen::OnIliadButtonClicked()
{
    FString iliadURL = "https://praxinos.coop/";
    FPlatformProcess::LaunchURL( *iliadURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnPraxinosButtonClicked()
{
    FString praxinosURL = "https://praxinos.coop/";
    FPlatformProcess::LaunchURL( *praxinosURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnTwitterButtonClicked()
{
    FString twitterURL = "https://twitter.com/praxinos";
    FPlatformProcess::LaunchURL( *twitterURL, NULL, NULL );

    return FReply::Handled();
}


FReply
SOdysseyAboutScreen::OnFacebookButtonClicked()
{
    FString FacebookURL = "https://www.facebook.com/Praxinos";
    FPlatformProcess::LaunchURL( *FacebookURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnLinkedInButtonClicked()
{
    FString linkedInURL = "https://www.linkedin.com/company/praxinos";
    FPlatformProcess::LaunchURL( *linkedInURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnInstagramButtonClicked()
{
    FString instagramURL = "https://www.instagram.com/praxinos/";
    FPlatformProcess::LaunchURL( *instagramURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnYoutubeButtonClicked()
{
    FString youtubeURL = "https://www.youtube.com/channel/UCdSBI-_VlBRRRjY_tDz73xQ";
    FPlatformProcess::LaunchURL( *youtubeURL, NULL, NULL );

    return FReply::Handled();
}


FReply
SOdysseyAboutScreen::OnClose()
{
    TSharedRef<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow( AsShared() ).ToSharedRef();
    FSlateApplication::Get().RequestDestroyWindow( ParentWindow );
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
