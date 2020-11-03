// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "SOdysseyAboutScreen.h"

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

#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "AboutScreen"

//---

struct SOdysseyAboutScreen::FLineDefinition
{
public:
    FLineDefinition( const FText& iText )
        : mText( iText )
        , mUrl()
        , mFontSize( 9 )
        , mTextColor( FLinearColor( 0.5f, 0.5f, 0.5f ) )
        , mMargin( FMargin( 6.f, 0.f, 0.f, 0.f ) )
    {
    }

    FLineDefinition( const FText& iText, int32 iFontSize, const FLinearColor& iTextColor, const FMargin& iMargin )
        : mText( iText )
        , mUrl()
        , mFontSize( iFontSize )
        , mTextColor( iTextColor )
        , mMargin( iMargin )
    {
    }

    FLineDefinition( const FText& iText, const FText& iUrl, int32 iFontSize, const FLinearColor& iTextColor, const FMargin& iMargin )
        : mText( iText )
        , mUrl( iUrl )
        , mFontSize( iFontSize )
        , mTextColor( iTextColor )
        , mMargin( iMargin )
    {
    }

public:
    FText mText;
    FText mUrl;
    int32 mFontSize;
    FLinearColor mTextColor;
    FMargin mMargin;
};

//---

SOdysseyAboutScreen::SOdysseyAboutScreen()
    : mAboutLines()
    , mIliadButton()
    , mPraxinosButton()
    , mTwitterButton()
    , mFacebookButton()
    , mLinkedInButton()
    , mInstagramButton()
    , mYoutubeButton()
    , mPraxinosUrl( LOCTEXT( "url.praxinos", "https://praxinos.coop/" ) )
    , mForumUrl( LOCTEXT( "url.forum", "https://praxinos.coop/forum/" ) )
    , mIliadUrl( LOCTEXT( "url.iliad", "https://praxinos.coop/iliad.php" ) )
    , mContactUsUrl( LOCTEXT( "url.contact-us", "https://praxinos.coop/contact.php" ) )
    , mExternalLibsULISUrl( LOCTEXT( "url.external-libs.ULIS", "https://github.com/Praxinos/ULIS" ) )
    , mExternalLibsLittleCMSUrl( LOCTEXT( "url.external-libs.littlecms", "http://www.littlecms.com/" ) )
    , mExternalLibsGLMUrl( LOCTEXT( "url.external-libs.glm", "https://www.opengl.org/sdk/libs/GLM/" ) )
    , mExternalLibsBoostPreprocessorUrl( LOCTEXT( "url.external-libs.boost-preprocessor", "https://www.boost.org/doc/libs/1_71_0/libs/preprocessor/doc/index.html" ) )
    , mTwitterUrl( LOCTEXT( "url.social.twitter", "https://twitter.com/praxinos" ) )
    , mFacebookUrl( LOCTEXT( "url.social.facebook", "https://www.facebook.com/Praxinos" ) )
    , mLinkedInUrl( LOCTEXT( "url.social.linkedin", "https://www.linkedin.com/company/praxinos" ) )
    , mInstagramUrl( LOCTEXT( "url.social.instagram", "https://www.instagram.com/praxinos/" ) )
    , mYoutubeUrl( LOCTEXT( "url.social.youtube", "https://www.youtube.com/channel/UCdSBI-_VlBRRRjY_tDz73xQ" ) )
    , mDiscordUrl( LOCTEXT( "url.social.discord", "https://discord.gg/gEd6pj7" ) )
    , mUserDocUrl( LOCTEXT( "url.social.userDoc", "https://praxinos.coop/Documentation/Iliad/User/html/" ))
    , mGitUrl(LOCTEXT( "url.social.git", "https://github.com/Praxinos/Iliad" ))
{
}

void
SOdysseyAboutScreen::Construct( const FArguments& iArgs )
{
    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "copyright.iliad", "ILIAD : Intelligent Layered Imaging Architecture for Drawing" ), 15, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.f, 2.f, 0.f, 12.f ) ) ) );

    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "copyright.copyright", "ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc" ), 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.f ) ) ) );
    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "copyright.iddn", "IDDN FR.001.250001.004.S.X.2019.000.00000" ), 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.f, 2.f, 0.f, 12.f ) ) ) );

    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "copyright.team-1", "Praxinos Team : Elodie Moog, Fabrice Debarge, Thomas Schmitt, Clément Berthaud," ), 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.f, 2.f ) ) ) );
                                                                                // This spaces are here to match the start of the first person name in the first line
    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "copyright.team-2", "                             Naomiki Sato, Antoine Antin, Eric Scholl, Michael Schreiner" ), 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.f, 2.f, 0.f, 12.f ) ) ) );

    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "copyright.external-libs", "External libraries:" ), 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.f, 2.f ) ) ) );
    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "copyright.external-libs.ULIS", "- ULIS" ), mExternalLibsULISUrl, 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.f, 2.f ) ) ) );
    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "copyright.external-libs.little-cms", "- littleCMS" ), mExternalLibsLittleCMSUrl, 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.f, 2.f ) ) ) );
    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "copyright.external-libs.glm", "- glm" ), mExternalLibsGLMUrl, 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.f, 2.f ) ) ) );
    mAboutLines.Add( MakeShareable( new FLineDefinition( LOCTEXT( "copyright.external-libs.boost-preprocessor", "- boost.preprocessor" ), mExternalLibsBoostPreprocessorUrl, 8, FLinearColor( 1.f, 1.f, 1.f ), FMargin( 0.f, 2.f ) ) ) );

    TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin( FString( "Iliad" ) );
    const FPluginDescriptor& pluginDescriptor = plugin->GetDescriptor();
    FString version = pluginDescriptor.VersionName;
    if( pluginDescriptor.bIsBetaVersion )
        version += "-beta";

    FText VersionFull = FText::Format( LOCTEXT( "VersionLabel", "Version: {0}" ), FText::FromString( version ) );

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
                    .Cursor( EMouseCursor::Hand )
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
                        .Cursor( EMouseCursor::Hand )
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
                        .Text( VersionFull )
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
                //.OnMouseButtonClick( this, &SOdysseyAboutScreen::OnListViewButtonClicked )
                .SelectionMode( ESelectionMode::None )
            ]
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                +SHorizontalBox::Slot()
                .HAlign( HAlign_Left )
                .FillWidth( 0.64f )
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        SAssignNew( mTwitterButton, SButton )
                        .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                        .Cursor( EMouseCursor::Hand )
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
                        .Cursor( EMouseCursor::Hand )
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
                        .Cursor( EMouseCursor::Hand )
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
                        .Cursor( EMouseCursor::Hand )
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
                        .Cursor( EMouseCursor::Hand )
                        .ToolTipText( LOCTEXT( "YoutubeToolTip", "Praxinos on Youtube" ) )
                        .OnClicked( this, &SOdysseyAboutScreen::OnYoutubeButtonClicked )
                        [
                            SNew( SImage ).Image( this, &SOdysseyAboutScreen::GetYoutubeButtonBrush )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        SAssignNew( mDiscordButton, SButton )
                        .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                        .Cursor( EMouseCursor::Hand )
                        .ToolTipText( LOCTEXT( "DiscordToolTip", "Praxinos on Discord" ) )
                        .OnClicked( this, &SOdysseyAboutScreen::OnDiscordButtonClicked )
                        [
                            SNew( SImage ).Image( this, &SOdysseyAboutScreen::GetDiscordButtonBrush )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        SAssignNew( mUserDocButton, SButton )
                        .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                        .Cursor( EMouseCursor::Hand )
                        .ToolTipText( LOCTEXT( "UserDocToolTip", "Iliad User Documentation" ) )
                        .OnClicked( this, &SOdysseyAboutScreen::OnUserDocButtonClicked )
                        [
                            SNew( SImage ).Image( this, &SOdysseyAboutScreen::GetUserDocButtonBrush )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Left )
                    .Padding( FMargin( 5.f, 0.f, 5.f, 5.f ) )
                    [
                        SAssignNew( mGitButton, SButton )
                        .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                        .Cursor( EMouseCursor::Hand )
                        .ToolTipText( LOCTEXT( "GitToolTip", "Praxinos on Git" ) )
                        .OnClicked( this, &SOdysseyAboutScreen::OnGitButtonClicked )
                        [
                            SNew( SImage ).Image( this, &SOdysseyAboutScreen::GetGitButtonBrush )
                        ]
                    ]
                ]
                +SHorizontalBox::Slot()
                .HAlign( HAlign_Right )
                .FillWidth( 0.36f )
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

    FText text = iItem->mText;
    if( !iItem->mUrl.IsEmpty() )
        text = FText::Format( LOCTEXT( "about.list", "{0}: {1}" ), iItem->mText, iItem->mUrl );

    return 
        SNew( STableRow< TSharedPtr<FString> >, iOwnerTable )
        .Padding( iItem->mMargin )
        [
            //SNew( SButton )
            //.ButtonStyle( FEditorStyle::Get(), "NoBorder" )
            //.ContentPadding( FMargin( 0.0 ) )
            //.Cursor( !iItem->mUrl.IsEmpty() ? EMouseCursor::Hand : EMouseCursor::Default )
            //.ButtonColorAndOpacity( iItem->mTextColor )
            //.TextStyle( FCoreStyle::GetDefaultFontStyle( "Regular", iItem->mFontSize ) )
            //.Text( text )
            //[
                SNew( STextBlock )
                .ColorAndOpacity( iItem->mTextColor )
                .Font( FCoreStyle::GetDefaultFontStyle( "Regular", iItem->mFontSize ) )
                .Text( text )
            //]
        ];
}

//void
//SOdysseyAboutScreen::OnListViewButtonClicked( TSharedRef<FLineDefinition> iItem )
//{
//    // This CB is never called -_-
//    if( iItem->mUrl.IsEmpty() )
//        return;
//
//    FString iliadURL = iItem->mUrl.ToString();
//    FPlatformProcess::LaunchURL( *iliadURL, NULL, NULL );
//}

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

const FSlateBrush*
SOdysseyAboutScreen::GetDiscordButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT( "About.Discord" ) );
}

const FSlateBrush*
SOdysseyAboutScreen::GetUserDocButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT( "About.UserDoc" ) );
}

const FSlateBrush*
SOdysseyAboutScreen::GetGitButtonBrush() const
{
    return FOdysseyStyle::GetBrush( TEXT( "About.Git" ) );
}

FReply
SOdysseyAboutScreen::OnIliadButtonClicked()
{
    FString iliadURL = mIliadUrl.ToString();
    FPlatformProcess::LaunchURL( *iliadURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnPraxinosButtonClicked()
{
    FString praxinosURL = mPraxinosUrl.ToString();
    FPlatformProcess::LaunchURL( *praxinosURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnTwitterButtonClicked()
{
    FString twitterURL = mTwitterUrl.ToString();
    FPlatformProcess::LaunchURL( *twitterURL, NULL, NULL );

    return FReply::Handled();
}


FReply
SOdysseyAboutScreen::OnFacebookButtonClicked()
{
    FString FacebookURL = mFacebookUrl.ToString();
    FPlatformProcess::LaunchURL( *FacebookURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnLinkedInButtonClicked()
{
    FString linkedInURL = mLinkedInUrl.ToString();
    FPlatformProcess::LaunchURL( *linkedInURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnInstagramButtonClicked()
{
    FString instagramURL = mInstagramUrl.ToString();
    FPlatformProcess::LaunchURL( *instagramURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnYoutubeButtonClicked()
{
    FString youtubeURL = mYoutubeUrl.ToString();
    FPlatformProcess::LaunchURL( *youtubeURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnDiscordButtonClicked()
{
    FString discordURL = mDiscordUrl.ToString();
    FPlatformProcess::LaunchURL( *discordURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnUserDocButtonClicked()
{
    FString userDocURL = mUserDocUrl.ToString();
    FPlatformProcess::LaunchURL( *userDocURL, NULL, NULL );

    return FReply::Handled();
}

FReply
SOdysseyAboutScreen::OnGitButtonClicked()
{
    FString gitURL = mGitUrl.ToString();
    FPlatformProcess::LaunchURL( *gitURL, NULL, NULL );

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
