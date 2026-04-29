// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SAboutWindow.h"

#include "EditorStyleSet.h"
#include "Fonts/SlateFontInfo.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
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
#include "OdysseyStyle.h"

#define LOCTEXT_NAMESPACE "AboutWindowOdyssey"

//---

//static
void
SAboutWindow::Open(TSharedPtr<SWindow> iParentWindow)
{
    TSharedRef<SWindow> window =
        SNew( SWindow )
        .Title( LOCTEXT( "title", "About Odyssey" ) )
        .SupportsMaximize( false )
        .SupportsMinimize( false )
        .SizingRule( ESizingRule::Autosized )
        [
            SNew( SAboutWindow )
        ];

    //FSlateApplication::Get().AddWindowAsNativeChild( aboutWindow.ToSharedRef(), iParentWindow.ToSharedRef() ); // For Debugging with SlateReflector
    FSlateApplication::Get().AddModalWindow( window, iParentWindow );
}

//---

FReply
SAboutWindow::OnClose()
{
    TSharedRef<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow( AsShared() ).ToSharedRef();
    FSlateApplication::Get().RequestDestroyWindow( ParentWindow );

    return FReply::Handled();
}

//---

void
SAboutWindow::Construct( const FArguments& iArgs )
{
    TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin( FString( "Odyssey" ) );
    const FPluginDescriptor& pluginDescriptor = plugin->GetDescriptor();
    FString version = pluginDescriptor.VersionName;
    if( pluginDescriptor.bIsBetaVersion )
        version += "-beta";

    FText VersionFull = FText::Format( LOCTEXT( "VersionLabel", "Version: {0}" ), FText::FromString( version ) );

    //---

    mLogos.Add( ELogo::kPraxinos,       { TEXT( "Odyssey.About.Praxinos" ),     LOCTEXT( "tooltip.praxinos", "Go to Praxinos website" ),                    LOCTEXT( "url.praxinos", "https://praxinos.coop/" ) } );
    mLogos.Add( ELogo::kOdyssey,        { TEXT( "Odyssey.About.Odyssey" ),      LOCTEXT( "tooltip.odyssey", "Go to Odyssey webpage" ),                      LOCTEXT( "url.odyssey", "https://praxinos.coop/odyssey.php" ) } );

    mLogos.Add( ELogo::kTwitter,        { TEXT( "Odyssey.About.Twitter" ),      LOCTEXT( "tooltip.social.twitter", "Praxinos on Twitter" ),                 LOCTEXT( "url.social.twitter", "https://twitter.com/praxinos" ) } );
    mLogos.Add( ELogo::kFacebook,       { TEXT( "Odyssey.About.Facebook" ),     LOCTEXT( "tooltip.social.facebook", "Praxinos on Facebook" ),               LOCTEXT( "url.social.facebook", "https://www.facebook.com/Praxinos" ) } );
    mLogos.Add( ELogo::kLinkedin,       { TEXT( "Odyssey.About.LinkedIn" ),     LOCTEXT( "tooltip.social.linkedin", "Praxinos on LinkedIn" ),               LOCTEXT( "url.social.linkedin", "https://www.linkedin.com/company/praxinos" ) } );
    mLogos.Add( ELogo::kInstagram,      { TEXT( "Odyssey.About.Instagram" ),    LOCTEXT( "tooltip.social.instagram", "Praxinos on Instagram" ),             LOCTEXT( "url.social.instagram", "https://www.instagram.com/praxinos/" ) } );
    mLogos.Add( ELogo::kYoutube,        { TEXT( "Odyssey.About.Youtube" ),      LOCTEXT( "tooltip.social.youtube", "Praxinos on Youtube" ),                 LOCTEXT( "url.social.youtube", "https://www.youtube.com/channel/UCdSBI-_VlBRRRjY_tDz73xQ" ) } );
    mLogos.Add( ELogo::kDiscord,        { TEXT( "Odyssey.About.Discord" ),      LOCTEXT( "tooltip.social.discord", "Praxinos on Discord" ),                 LOCTEXT( "url.social.discord", "https://discord.gg/gEd6pj7" ) } );
    mLogos.Add( ELogo::kOdysseyUserDoc, { TEXT( "Odyssey.About.UserDoc" ),      LOCTEXT( "tooltip.social.userDoc", "Go to Odyssey User Documentation" ),    LOCTEXT( "url.social.userDoc", "https://praxinos.coop/odyssey-user-doc" ) } );
    mLogos.Add( ELogo::kOdysseyGit,     { TEXT( "Odyssey.About.Git" ),          LOCTEXT( "tooltip.social.git", "Praxinos on Git" ),                         LOCTEXT( "url.social.git", "https://github.com/Praxinos" ) } );

    ChildSlot
    [
        SNew( SBox )
        .WidthOverride( 600.f )
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
                    NewButtonImage( ELogo::kPraxinos )
                ]
                +SHorizontalBox::Slot()
                .FillWidth( 1.f )
                .HAlign( HAlign_Right )
                .Padding( FMargin( 0.f, 10.f, 7.f, 0.f ) )
                [
                    SNew( SVerticalBox )
                    +SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign( HAlign_Center )
                    [
                        NewButtonImage( ELogo::kOdyssey )
                    ]
                    +SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew( STextBlock )
                        .ColorAndOpacity( FLinearColor( 0.7f, 0.7f, 0.7f ) )
                        .Text( VersionFull )
                        .Justification(ETextJustify::Center)
                    ]
                ]
            ]
            +SVerticalBox::Slot()
            //.AutoHeight() // No AutoHeight to take all the left space (and there is one due to a 'bug' of ue window when it has titlebar, it adds ~34px)
            .VAlign( VAlign_Center )
            .Padding( FMargin( 5.f, 5.f, 5.f, 5.f ) )
            [
                SNew( SVerticalBox )
                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 2.f, 0.f, 12.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.odyssey", "Odyssey" ) )
                    .TextStyle( FOdysseyStyle::Get(), "Odyssey.About.BigText" )
                ]
                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.copyright", "Odyssey is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc" ) )
                    .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.SmallText" )
                ]
                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 2.f, 0.f, 12.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.iddn", "IDDN.FR.001.060015.014.S.X.2019.000.00000" ) )
                    .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.SmallText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 5.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.team-label", "Praxinos Team: " ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.UnderlineText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.team-names-1", "Elodie Moog, Fabrice Debarge, Thomas Schmitt, Naomiki Sato," ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.SmallText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.team-names-2", "Gary Gabriel, Eric Scholl, Michael Schreiner, Yann Montgermont" ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.SmallText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 5.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.team-former-label", "Thanks to our former collaborators: " ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.UnderlineSubduedText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.team-former-names", "Clément Berthaud, Antoine Antin, Cléa Gonay" ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.SmallSubduedText" )
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 5.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.special-thanks-label", "Special Thanks: " ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.UnderlineSubduedText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.special-tranks.inria-bordeaux-manoa-team", "Inria Bordeaux - Manao Team" ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.SmallSubduedText" )
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 5.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.don-perro-tribute", "In loving memory of Don Perro" ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.SmallSubduedText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 5.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.thirdparty-label", "ThirdParty libraries: " ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.UnderlineText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign( HAlign_Center )
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( SHorizontalBox )

                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew( SButton )
                        .ButtonStyle( FAppStyle::Get(), "NoBorder" )
                        .HAlign( HAlign_Center )
                        .Cursor( EMouseCursor::Hand )
                        .OnClicked_Lambda( []()
                                            {
                                                FPlatformProcess::LaunchURL( TEXT( "https://github.com/blend2d/blend2d" ), nullptr, nullptr );
                                                return FReply::Handled();
                                            } )
                        [
                            SNew( STextBlock )
                            .Text( LOCTEXT( "copyright.thirdparty-blend2d", "blend2d" ) )
                            .Justification( ETextJustify::Center )
                            .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.SmallText" )
                        ]
                    ]

                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew( SButton )
                        .ButtonStyle( FAppStyle::Get(), "NoBorder" )
                        .HAlign( HAlign_Center )
                        .Cursor( EMouseCursor::Hand )
                        .OnClicked_Lambda( []()
                                           {
                                               FPlatformProcess::LaunchURL( TEXT( "https://github.com/libharu/libharu" ), nullptr, nullptr );
                                               return FReply::Handled();
                                           } )
                        [
                            SNew( STextBlock )
                            .Text( LOCTEXT( "copyright.thirdparty-libharu", "libharu" ) )
                            .Justification( ETextJustify::Center )
                            .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.SmallText" )
                        ]
                    ]

                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew( SButton )
                        .ButtonStyle( FAppStyle::Get(), "NoBorder" )
                        .HAlign( HAlign_Center )
                        .Cursor( EMouseCursor::Hand )
                        .OnClicked_Lambda( []()
                                           {
                                               FPlatformProcess::LaunchURL( TEXT( "https://gitlab.com/libeigen/eigen" ), nullptr, nullptr );
                                               return FReply::Handled();
                                           } )
                        [
                            SNew( STextBlock )
                            .Text( LOCTEXT( "copyright.thirdparty-libeigen", "libeigen" ) )
                            .Justification( ETextJustify::Center )
                            .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.SmallText" )
                        ]
                    ]

                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew( SButton )
                        .ButtonStyle( FAppStyle::Get(), "NoBorder" )
                        .HAlign( HAlign_Center )
                        .Cursor( EMouseCursor::Hand )
                        .OnClicked_Lambda( []()
                                           {
                                               FPlatformProcess::LaunchURL( TEXT( "https://developer-support.wacom.com" ), nullptr, nullptr );
                                               return FReply::Handled();
                                           } )
                        [
                            SNew( STextBlock )
                            .Text( LOCTEXT( "copyright.thirdparty-wintab", "Wintab" ) )
                            .Justification( ETextJustify::Center )
                            .TextStyle(FOdysseyStyle::Get(), "Odyssey.About.SmallText" )
                        ]
                    ]
                ]
            ]
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                +SHorizontalBox::Slot()
                .HAlign( HAlign_Left )
                .AutoWidth()
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        NewButtonImage( ELogo::kTwitter )
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        NewButtonImage( ELogo::kFacebook )
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        NewButtonImage( ELogo::kLinkedin )
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        NewButtonImage( ELogo::kInstagram )
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        NewButtonImage( ELogo::kYoutube )
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        NewButtonImage( ELogo::kDiscord )
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        NewButtonImage( ELogo::kOdysseyUserDoc )
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        NewButtonImage( ELogo::kOdysseyGit )
                    ]
                ]
                +SHorizontalBox::Slot()
                .HAlign( HAlign_Right )
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
                        .OnClicked( this, &SAboutWindow::OnClose )
                    ]
                ]
            ]
        ]
    ];
}

TSharedRef<SWidget>
SAboutWindow::NewButtonImage( ELogo iLogo )
{
    const FLogo& logo = mLogos.FindChecked( iLogo );

    FText tooltip_with_url = FText::Format( LOCTEXT( "tooltip-url", "{0} | {1}" ), logo.mTooltip, logo.mUrl );

    TSharedRef<SButton> button = SNew( SButton )
        .ButtonStyle( FAppStyle::Get(), "NoBorder" )
        //.IsEnabled( iLogo != ELogo::kOdysseyUserDoc ) // Remove this line once everything's ok
        .Cursor( EMouseCursor::Hand )
        .ToolTipText( tooltip_with_url )
        .OnClicked_Lambda( [&]() { FPlatformProcess::LaunchURL( *logo.mUrl.ToString(), nullptr, nullptr ); return FReply::Handled(); } )
        [
            SNew( SImage )
            .Image(FOdysseyStyle::Get().GetBrush( logo.mBrushName ) )
        ];

    // There should be something more easier ?!

    button->SetOnHovered( FSimpleDelegate::CreateLambda( [button]()
    {
        FChildren* children = button->GetChildren();
        for( int i = 0; i < children->Num(); i++ )
        {
            TSharedRef<SWidget> child = children->GetChildAt( i );
            SImage* image = static_cast<SImage*>( &child.Get() );
            image->SetColorAndOpacity( FLinearColor( .5f, .5f, .5f, .5f ) );
        }
    } ) );

    button->SetOnUnhovered( FSimpleDelegate::CreateLambda( [button]()
    {
        FChildren* children = button->GetChildren();
        for( int i = 0; i < children->Num(); i++ )
        {
            TSharedRef<SWidget> child = children->GetChildAt( i );
            SImage* image = static_cast<SImage*>( &child.Get() );
            image->SetColorAndOpacity( FLinearColor::White );
        }
    } ) );

    return button;
}

#undef LOCTEXT_NAMESPACE
