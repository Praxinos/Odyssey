// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SAboutWindow.h"

#include "EditorStyleSet.h"
#include "Fonts/SlateFontInfo.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"
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

#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "AboutWindow"

//---

//static
void
SAboutWindow::Open(TSharedPtr<SWindow> iParentWindow)
{
    TSharedRef<SWindow> window =
        SNew( SWindow )
        .Title( LOCTEXT( "title", "About Epos" ) )
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
    TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin( FString( "Epos" ) );
    const FPluginDescriptor& pluginDescriptor = plugin->GetDescriptor();
    FString version = pluginDescriptor.VersionName;
    if( pluginDescriptor.bIsBetaVersion )
        version += "-beta";

    FText VersionFull = FText::Format( LOCTEXT( "VersionLabel", "Version: {0}" ), FText::FromString( version ) );

    //---

    mLogos.Add( ELogo::kPraxinos,    { TEXT( "About.Praxinos" ),    LOCTEXT( "tooltip.praxinos", "Go to Praxinos website" ),                LOCTEXT( "url.praxinos", "https://praxinos.coop/" ) } );
    mLogos.Add( ELogo::kEpos,        { TEXT( "About.Epos" ),        LOCTEXT( "tooltip.epos", "Go to Epos webpage" ),                        LOCTEXT( "url.epos", "https://praxinos.coop/epos.php" ) } );
    //mLogos.Add( ELogo::kForum,       { TEXT( "About.Forum" ),       LOCTEXT( "tooltip.forum", "Go to Praxinos forum" ),                     LOCTEXT( "url.forum", "https://praxinos.coop/forum/" ) } );
    //mLogos.Add( ELogo::kContactUs,   { TEXT( "About.ContactUs" ),   LOCTEXT( "tooltip.contact-us", "Go to contact-us form" ),               LOCTEXT( "url.contact-us", "https://praxinos.coop/contact.php" ) } );

    mLogos.Add( ELogo::kTwitter,     { TEXT( "About.Twitter" ),     LOCTEXT( "tooltip.social.twitter", "Praxinos on Twitter" ),             LOCTEXT( "url.social.twitter", "https://twitter.com/praxinos" ) } );
    mLogos.Add( ELogo::kFacebook,    { TEXT( "About.Facebook" ),    LOCTEXT( "tooltip.social.facebook", "Praxinos on Facebook" ),           LOCTEXT( "url.social.facebook", "https://www.facebook.com/Praxinos" ) } );
    mLogos.Add( ELogo::kLinkedin,    { TEXT( "About.LinkedIn" ),    LOCTEXT( "tooltip.social.linkedin", "Praxinos on LinkedIn" ),           LOCTEXT( "url.social.linkedin", "https://www.linkedin.com/company/praxinos" ) } );
    mLogos.Add( ELogo::kInstagram,   { TEXT( "About.Instagram" ),   LOCTEXT( "tooltip.social.instagram", "Praxinos on Instagram" ),         LOCTEXT( "url.social.instagram", "https://www.instagram.com/praxinos/" ) } );
    mLogos.Add( ELogo::kYoutube,     { TEXT( "About.Youtube" ),     LOCTEXT( "tooltip.social.youtube", "Praxinos on Youtube" ),             LOCTEXT( "url.social.youtube", "https://www.youtube.com/channel/UCdSBI-_VlBRRRjY_tDz73xQ" ) } );
    mLogos.Add( ELogo::kDiscord,     { TEXT( "About.Discord" ),     LOCTEXT( "tooltip.social.discord", "Praxinos on Discord" ),             LOCTEXT( "url.social.discord", "https://discord.gg/gEd6pj7" ) } );
    mLogos.Add( ELogo::kEposUserDoc, { TEXT( "About.UserDoc" ),     LOCTEXT( "tooltip.social.userDoc", "Go to Epos User Documentation" ),   LOCTEXT( "url.social.userDoc", "https://praxinos.coop/Documentation/Epos/User/html/" ) } );
    mLogos.Add( ELogo::kEposGit,     { TEXT( "About.Git" ),         LOCTEXT( "tooltip.social.git", "Praxinos on Git" ),                     LOCTEXT( "url.social.git", "https://github.com/Praxinos/Epos" ) } );

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
                    [
                        NewButtonImage( ELogo::kEpos )
                    ]
                    +SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew( STextBlock )
                        .ColorAndOpacity( FLinearColor( 0.7f, 0.7f, 0.7f ) )
                        .Text( VersionFull )
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
                    .Text( LOCTEXT( "copyright.epos", "EPOS" ) )
                    .TextStyle( FEposSequenceEditorStyle::Get(), "About.BigText" )
                ]
                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.copyright", "EPOS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc" ) )
                    .TextStyle( FEposSequenceEditorStyle::Get(), "About.SmallText" )
                ]
                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 2.f, 0.f, 12.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.iddn", "IDDN.FR.001.220036.001.S.P.2021.000.00000" ) )
                    .TextStyle( FEposSequenceEditorStyle::Get(), "About.SmallText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 5.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.team-label", "Praxinos Team: " ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FEposSequenceEditorStyle::Get(), "About.UnderlineText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.team-names-1", "Elodie Moog, Fabrice Debarge, Thomas Schmitt, Clément Berthaud, " ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FEposSequenceEditorStyle::Get(), "About.SmallText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "copyright.team-names-2", "Naomiki Sato, Antoine Antin, Eric Scholl, Michael Schreiner" ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FEposSequenceEditorStyle::Get(), "About.SmallText" )
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
                        NewButtonImage( ELogo::kEposUserDoc )
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        NewButtonImage( ELogo::kEposGit )
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
        .IsEnabled( iLogo != ELogo::kEposUserDoc ) // Remove this line once everything's ok
        .Cursor( EMouseCursor::Hand )
        .ToolTipText( tooltip_with_url )
        .OnClicked_Lambda( [&]() { FPlatformProcess::LaunchURL( *logo.mUrl.ToString(), nullptr, nullptr ); return FReply::Handled(); } )
        [
            SNew( SImage )
            .Image( FEposSequenceEditorStyle::Get().GetBrush( logo.mBrushName ) )
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
