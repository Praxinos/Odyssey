// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyAboutScreen.h"

#include "EditorStyleSet.h"
#include "Fonts/SlateFontInfo.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/Paths.h"
#include "Misc/EngineVersion.h"
#include "Styling/CoreStyle.h"
#include "UnrealEdMisc.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/SListView.h"

#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "AboutScreen"

//---

struct SOdysseyAboutScreen::FLineDefinition
{
public:
    FLineDefinition( const FText& iText )
        : mText( iText )
        , mFontSize( 9 )
        , mTextColor( FLinearColor( 0.5f, 0.5f, 0.5f ) )
        , mMargin( FMargin( 6.f, 0.f, 0.f, 0.f ) )
    {
    }

    FLineDefinition( const FText& iText, int32 iFontSize, const FLinearColor& iTextColor, const FMargin& iMargin )
        : mText( iText )
        , mFontSize( iFontSize )
        , mTextColor( iTextColor )
        , mMargin( iMargin )
    {
    }

public:
    FText mText;
    int32 mFontSize;
    FLinearColor mTextColor;
    FMargin mMargin;
};

//---

void
SOdysseyAboutScreen::Construct( const FArguments& iArgs )
{
    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "Copyright1", "Copyright 2018-2019 Praxinos SCOP arl. All rights reserved" ), 11, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.f ) ) ) );
    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "Copyright2", "Praxinos Team : Elodie Moog, Fabrice Debarge, Thomas Schmitt, Clément Berthaud," ), 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.0f, 2.0f ) ) ) );
                                                                                // This spaces are here to match the start of the first person name in the first line
    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "Copyright3", "                             Naomiki Sato, Antoine Antin, Eric Scholl, Michael Schreiner" ), 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.0f, 2.0f ) ) ) );
    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "Copyright4", "ILIAD : Intelligent Layered Imaging Architecture for Drawing" ), 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.0f, 2.0f ) ) ) );
    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "Copyright5", "IDDN FR.001.250001.002.S.P.2019.000.00000" ), 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.0f, 2.0f ) ) ) );

    FText Version = LOCTEXT( "VersionLabel", "Version: Alpha 0.1" );

    ChildSlot
    [
        SNew( SOverlay )
        +SOverlay::Slot()
        [
            SNew( SVerticalBox )
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                +SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign( VAlign_Top )
                .Padding( FMargin( 10.f, 10.f, 0.f, 0.f ) )
                [
                    SAssignNew( mPraxinosButton, SButton )
                    .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                    .OnClicked( this, &SOdysseyAboutScreen::OnPraxinosButtonClicked )
                    [
                        SNew( SImage ).Image( this, &SOdysseyAboutScreen::GetPraxinosButtonBrush )
                    ]
                ]
                +SHorizontalBox::Slot()
                .FillWidth( 1.f )
                .HAlign( HAlign_Right )
                .Padding( FMargin( 0.f, 10.f, 7.f, 0.f ) )
                [
                    SNew( SVerticalBox )
                    +SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SAssignNew( mIliadButton, SButton )
                        .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                        .OnClicked( this, &SOdysseyAboutScreen::OnIliadButtonClicked )
                        [
                            SNew( SImage ).Image( this, &SOdysseyAboutScreen::GetIliadButtonBrush )
                        ]
                    ]
                    +SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew( SEditableText )
                        .ColorAndOpacity( FLinearColor( 0.7f, 0.7f, 0.7f ) )
                        .IsReadOnly( true )
                        .Text( Version )
                    ]
                ]
            ]
            +SVerticalBox::Slot()
            .Padding( FMargin( 5.f, 5.f, 5.f, 5.f ) )
            .VAlign( VAlign_Top )
            [
                SNew( SListView<TSharedRef<FLineDefinition>> )
                .ListItemsSource( &mAboutLines )
                .OnGenerateRow( this, &SOdysseyAboutScreen::MakeAboutTextItemWidget )
                .SelectionMode( ESelectionMode::None )
            ]
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                +SHorizontalBox::Slot()
                .HAlign( HAlign_Left )
                .FillWidth( 0.4f )
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        SAssignNew( mTwitterButton, SButton )
                        .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                        .ToolTipText( LOCTEXT( "TwitterToolTip", "Praxinos on Twitter" ) )
                        .OnClicked( this, &SOdysseyAboutScreen::OnTwitterButtonClicked )
                        [
                            SNew( SImage ).Image( this, &SOdysseyAboutScreen::GetTwitterButtonBrush )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        SAssignNew( mFacebookButton, SButton )
                        .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                        .ToolTipText( LOCTEXT( "FacebookToolTip", "Praxinos on Facebook" ) )
                        .OnClicked( this, &SOdysseyAboutScreen::OnFacebookButtonClicked )
                        [
                            SNew( SImage ).Image( this, &SOdysseyAboutScreen::GetFacebookButtonBrush )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        SAssignNew( mLinkedInButton, SButton )
                        .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                        .ToolTipText( LOCTEXT( "LinkedInToolTip", "Praxinos on LinkedIn" ) )
                        .OnClicked( this, &SOdysseyAboutScreen::OnLinkedInButtonClicked )
                        [
                            SNew( SImage ).Image( this, &SOdysseyAboutScreen::GetLinkedInButtonBrush )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        SAssignNew( mInstagramButton, SButton )
                        .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                        .ToolTipText( LOCTEXT( "InstagramToolTip", "Praxinos on Instagram" ) )
                        .OnClicked( this, &SOdysseyAboutScreen::OnInstagramButtonClicked )
                        [
                            SNew( SImage ).Image( this, &SOdysseyAboutScreen::GetInstagramButtonBrush )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        SAssignNew( mYoutubeButton, SButton )
                        .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                        .ToolTipText( LOCTEXT( "YoutubeToolTip", "Praxinos on Youtube" ) )
                        .OnClicked( this, &SOdysseyAboutScreen::OnYoutubeButtonClicked )
                        [
                            SNew( SImage ).Image( this, &SOdysseyAboutScreen::GetYoutubeButtonBrush )
                        ]
                    ]
                ]
                +SHorizontalBox::Slot()
                .HAlign( HAlign_Right )
                .FillWidth( 0.6f )
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .HAlign( HAlign_Right )
                    .VAlign( VAlign_Bottom )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        SNew( SButton )
                        .HAlign( HAlign_Center )
                        .VAlign( VAlign_Center )
                        .Text( LOCTEXT( "Close", "Close" ) )
                        .ButtonColorAndOpacity( FLinearColor( 0.6f, 0.6f, 0.6f ) )
                        .OnClicked( this, &SOdysseyAboutScreen::OnClose )
                    ]
                ]
            ]
        ]
    ];
}

TSharedRef<ITableRow>
SOdysseyAboutScreen::MakeAboutTextItemWidget( TSharedRef<FLineDefinition> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    if( iItem->mText.IsEmpty() )
    {
        return
            SNew( STableRow< TSharedPtr<FString> >, iOwnerTable )
            .Padding( 6.0f )
            [
                SNew( SSpacer )
            ];
    }
    else
    {
        return
            SNew( STableRow< TSharedPtr<FString> >, iOwnerTable )
            .Padding( iItem->mMargin )
            [
                SNew( STextBlock )
                .ColorAndOpacity( iItem->mTextColor )
                .Font( FCoreStyle::GetDefaultFontStyle( "Regular", iItem->mFontSize ) )
                .Text( iItem->mText )
            ];
    }
}

const FSlateBrush*
SOdysseyAboutScreen::GetIliadButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT( "About.Iliad" ) );
}

const FSlateBrush*
SOdysseyAboutScreen::GetPraxinosButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT( "About.Praxinos" ) );
}

const FSlateBrush*
SOdysseyAboutScreen::GetTwitterButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT( "About.Twitter" ) );
}

const FSlateBrush*
SOdysseyAboutScreen::GetFacebookButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT( "About.Facebook" ) );
}

const FSlateBrush*
SOdysseyAboutScreen::GetLinkedInButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT( "About.LinkedIn" ) );
}

const FSlateBrush*
SOdysseyAboutScreen::GetInstagramButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT( "About.Instagram" ) );
}

const FSlateBrush*
SOdysseyAboutScreen::GetYoutubeButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT( "About.Youtube" ) );
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
