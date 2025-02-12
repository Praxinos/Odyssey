// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "SOdysseyAboutWindow.h"

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

#include "OdysseyStyle.h"

#define LOCTEXT_NAMESPACE "Widgets"

//---

//static
void
SOdysseyAboutWindow::Open(TSharedPtr<SWindow> iParentWindow)
{
    TSharedRef<SWindow> window =
        SNew( SWindow )
        .Title( LOCTEXT( "about-window.title", "About Odyssey" ) )
        .SupportsMaximize( false )
        .SupportsMinimize( false )
        .SizingRule( ESizingRule::Autosized )
        [
            SNew( SOdysseyAboutWindow )
        ];

    //FSlateApplication::Get().AddWindowAsNativeChild( aboutWindow.ToSharedRef(), iParentWindow.ToSharedRef() ); // For Debugging with SlateReflector
    FSlateApplication::Get().AddModalWindow( window, iParentWindow );
}

//---

FReply
SOdysseyAboutWindow::OnClose()
{
    TSharedRef<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow( AsShared() ).ToSharedRef();
    FSlateApplication::Get().RequestDestroyWindow( ParentWindow );

    return FReply::Handled();
}

//---

void
SOdysseyAboutWindow::Construct( const FArguments& iArgs )
{
    TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin( FString( "Odyssey" ) );
    const FPluginDescriptor& pluginDescriptor = plugin->GetDescriptor();
    FString version = pluginDescriptor.VersionName;
    if( pluginDescriptor.bIsBetaVersion )
        version += "-beta";

    FText VersionFull = FText::Format( LOCTEXT( "about-window.version-label", "Version: {0}" ), FText::FromString( version ) );

    //---

    mLogos.Add( ELogo::kPraxinos,    { TEXT( "About.Praxinos" ),    LOCTEXT( "about-window.praxinos.tooltip", "Go to Praxinos website" ),                LOCTEXT( "about-window.praxinos.url", "https://praxinos.coop/" ) } );
    mLogos.Add( ELogo::kOdyssey,        { TEXT( "About.Odyssey" ),        LOCTEXT( "about-window.odyssey.tooltip", "Go to Odyssey webpage" ),                        LOCTEXT( "about-window.odyssey.url", "https://praxinos.coop/odyssey.php" ) } );

    mLogos.Add( ELogo::kTwitter,     { TEXT( "About.Twitter" ),     LOCTEXT( "about-window.twitter.tooltip", "Praxinos on Twitter" ),             LOCTEXT( "about-window.twitter.url", "https://twitter.com/praxinos" ) } );
    mLogos.Add( ELogo::kFacebook,    { TEXT( "About.Facebook" ),    LOCTEXT( "about-window.facebook.tooltip", "Praxinos on Facebook" ),           LOCTEXT( "about-window.facebook.url", "https://www.facebook.com/Praxinos" ) } );
    mLogos.Add( ELogo::kLinkedin,    { TEXT( "About.LinkedIn" ),    LOCTEXT( "about-window.linkedin.tooltip", "Praxinos on LinkedIn" ),           LOCTEXT( "about-window.linkedin.url", "https://www.linkedin.com/company/praxinos" ) } );
    mLogos.Add( ELogo::kInstagram,   { TEXT( "About.Instagram" ),   LOCTEXT( "about-window.instagram.tooltip", "Praxinos on Instagram" ),         LOCTEXT( "about-window.instagram.url", "https://www.instagram.com/praxinos/" ) } );
    mLogos.Add( ELogo::kYoutube,     { TEXT( "About.Youtube" ),     LOCTEXT( "about-window.youtube.tooltip", "Praxinos on Youtube" ),             LOCTEXT( "about-window.youtube.url", "https://www.youtube.com/channel/UCdSBI-_VlBRRRjY_tDz73xQ" ) } );
    mLogos.Add( ELogo::kDiscord,     { TEXT( "About.Discord" ),     LOCTEXT( "about-window.discord.tooltip", "Praxinos on Discord" ),             LOCTEXT( "about-window.discord.url", "https://discord.gg/gEd6pj7" ) } );
    mLogos.Add( ELogo::kOdysseyUserDoc, { TEXT( "About.UserDoc" ),     LOCTEXT( "about-window.userDoc.tooltip", "Go to Odyssey User Documentation" ),   LOCTEXT( "about-window.userDoc.url", "https://praxinos.coop/Documentation/Odyssey/User/html/" ) } );
    mLogos.Add( ELogo::kOdysseyGithub,     { TEXT( "About.Github" ),         LOCTEXT( "about-window.github.tooltip", "Praxinos on Github" ),                     LOCTEXT( "about-window.git.url", "https://github.com/Praxinos/Odyssey" ) } );

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
                        NewButtonImage( ELogo::kOdyssey )
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
                    .Text( LOCTEXT( "about-window.copyright.Odyssey", "Odyssey" ) )
                    .TextStyle( &FOdysseyStyle::Get(), "About.BigText" )
                ]
                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "about-window.copyright.copyright", "Odyssey is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc" ) )
                    .TextStyle( FOdysseyStyle::Get(), "About.SmallText" )
                ]
                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 2.f, 0.f, 12.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "about-window.copyright.iddn", "IDDN.FR.001.250001.006.S.P.2019.000.00000" ) )
                    .TextStyle( FOdysseyStyle::Get(), "About.SmallText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 5.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "about-window.copyright.team-label", "Praxinos Team: " ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FOdysseyStyle::Get(), "About.UnderlineText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "about-window.copyright.team-names-1", "Elodie Moog, Fabrice Debarge, Thomas Schmitt, Cléa Gonay, " ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FOdysseyStyle::Get(), "About.SmallText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "about-window.copyright.team-names-2", "Naomiki Sato, Gary Gabriel, Eric Scholl, Michael Schreiner" ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FOdysseyStyle::Get(), "About.SmallText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 5.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "about-window.copyright.team-former-label", "Former Collaborators: " ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FOdysseyStyle::Get(), "About.UnderlineSubduedText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "about-window.copyright.team-former-names", "Clément Berthaud, Antoine Antin" ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FOdysseyStyle::Get(), "About.SmallSubduedText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 5.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "about-window.copyright.external-libs", "External libraries:" ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FOdysseyStyle::Get(), "About.UnderlineText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "about-window.copyright.external-libs.ULIS", "- ULIS : https://github.com/Praxinos/ULIS" ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FOdysseyStyle::Get(), "About.SmallText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "about-window.copyright.external-libs.littlecms", "- littleCMS : http://www.littlecms.com/" ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FOdysseyStyle::Get(), "About.SmallText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "about-window.copyright.external-libs.glm", "- glm : https://www.opengl.org/sdk/libs/GLM/" ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FOdysseyStyle::Get(), "About.SmallText" )
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding( FMargin( 0.f, 1.f ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "about-window.copyright.external-libs.boost-preprocessor", "- boost.preprocessor : https://www.boost.org/doc/libs/1_71_0/libs/preprocessor/doc/index.html" ) )
                    .Justification( ETextJustify::Center )
                    .TextStyle( FOdysseyStyle::Get(), "About.SmallText" )
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
                        NewButtonImage( ELogo::kOdysseyGithub )
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
                        .Text( LOCTEXT( "about-window.close", "Close" ) )
                        .ButtonColorAndOpacity( FLinearColor( 0.6f, 0.6f, 0.6f ) )
                        .OnClicked( this, &SOdysseyAboutWindow::OnClose )
                    ]
                ]
            ]
        ]
    ];
}

TSharedRef<SWidget>
SOdysseyAboutWindow::NewButtonImage( ELogo iLogo )
{
    const FLogo& logo = mLogos.FindChecked( iLogo );

    FText tooltip_with_url = FText::Format( LOCTEXT( "about-window.tooltip-and-url", "{0} | {1}" ), logo.mTooltip, logo.mUrl );

    TSharedRef<SButton> button = SNew( SButton )
        .ButtonStyle( FAppStyle::Get(), "NoBorder" )
        .IsEnabled( iLogo != ELogo::kOdysseyUserDoc ) // Remove this line once everything's ok
        .Cursor( EMouseCursor::Hand )
        .ToolTipText( tooltip_with_url )
        .OnClicked_Lambda( [&]() { FPlatformProcess::LaunchURL( *logo.mUrl.ToString(), nullptr, nullptr ); return FReply::Handled(); } )
        [
            SNew( SImage )
            .Image( FOdysseyStyle::Get().GetBrush( logo.mBrushName ) )
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
