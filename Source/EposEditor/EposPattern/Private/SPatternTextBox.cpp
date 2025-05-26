// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SPatternTextBox.h"

#include "Brushes/SlatecolorBrush.h"
#include "HAL/PlatformApplicationMisc.h"
#include "PropertyHandle.h"
#include "Styling/StyleColors.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/STextBlock.h"


#define LOCTEXT_NAMESPACE "SPatternTextBox"

//---

void
SPatternTextBox::Construct( const FArguments& iArgs, TSharedPtr<IPropertyHandle> iPatternHandle )
{
    mPatternHandle = iPatternHandle;

    mOnVerifyPattern = iArgs._OnVerifyPattern;

    TSharedRef<SVerticalBox> keyword_labels_widget = SNew( SVerticalBox );
    TSharedRef<SVerticalBox> keyword_helps_widget = SNew( SVerticalBox );

    check( iArgs._Keywords.Num() == iArgs._Keywords.Num() && iArgs._Keywords.Num() == iArgs._KeywordHelps.Num() );

    for( int i = 0; i < iArgs._Keywords.Num(); i++ )
    {
        mKeywords.Add( iArgs._Keywords[i] );

        FText label = iArgs._KeywordLabels[i];

        keyword_labels_widget->AddSlot()
            [
                SNew( SButton )
                .Text( label )
                .IsEnabled_Lambda( [this, iKeywordIndex = i]() -> bool { return !SPatternTextBox::IsKeywordUsed( iKeywordIndex ); } )
                .OnClicked( this, &SPatternTextBox::OnClickKeyword, i )
            ];

        FText help = FText::Format( LOCTEXT( "keywords-explanation-separator", " : {0}" ), iArgs._KeywordHelps[i] );

        keyword_helps_widget->AddSlot()
            [
                SNew( STextBlock )
                .Text( help )
            ];
    }

    TSharedRef<SVerticalBox> main =
        SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            [
                SAssignNew( mTextBoxWidget, SEditableTextBox )
                .Text( this, &SPatternTextBox::GetPatternText )
                .Font( FAppStyle::Get().GetFontStyle( TEXT( "PropertyWindow.NormalFont" ) ) )
                .SelectAllTextWhenFocused( false )
                .ClearKeyboardFocusOnCommit( false )
                .OnTextCommitted( this, &SPatternTextBox::OnPatternTextCommited )
                .OnTextChanged( this, &SPatternTextBox::OnPatternTextChanged )
                .SelectAllTextOnCommit( true )
                //.OnContextMenuOpening_Lambda( [this]() -> TSharedPtr<SWidget>
                //                       {
                //                           return SNew( STextBlock )
                //                               .Text( LOCTEXT( "rrr", "context menu" ) );
                //                       } )
                //.ContextMenuExtender( this, &SPatternTextBox::ContextMenuExtender )
            ]

            + SHorizontalBox::Slot()
            .FillWidth( .1f )
            [
                SNew( SSpacer )
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 0 )
        [
            SNew( SButton )
            .ButtonStyle( FAppStyle::Get(), "NoBorder" )
            .HAlign( HAlign_Center )
            .OnClicked( this, &SPatternTextBox::OnClickExpanderButton )
            [
                SNew( SImage )
                .Image( this, &SPatternTextBox::GetExpanderIcon )
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 0, 4, 0, 0 )
        [
            SNew( SHorizontalBox )
            .Visibility( this, &SPatternTextBox::GetKeywordsVisibility )

            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                keyword_labels_widget
            ]

            + SHorizontalBox::Slot()
            [
                keyword_helps_widget
            ]
        ];

    if( iArgs._MoreExplanation.IsSet() )
    {
        main->AddSlot()
        .AutoHeight()
        .Padding( 0, 4, 0, 0 )
        [
            SNew( STextBlock )
            .Text( iArgs._MoreExplanation )
        ];
    }

    ChildSlot
    [
        main
    ];
}

FText
SPatternTextBox::GetPatternText() const
{
    FText pattern;
    mPatternHandle->GetValueAsFormattedText( pattern );

    return pattern;
}

void
SPatternTextBox::OnPatternTextCommited( const FText& iNewText, ETextCommit::Type iCommitInfo )
{
    FString new_pattern = iNewText.ToString();

    FText current_pattern;
    mPatternHandle->GetValueAsFormattedText( current_pattern );
    if( new_pattern.Equals( current_pattern.ToString() ) )
        return;

    mPatternHandle->SetValueFromFormattedString( new_pattern );
}

void
SPatternTextBox::OnPatternTextChanged( const FText& iNewText )
{
    if( !mOnVerifyPattern.IsBound() || mOnVerifyPattern.Execute( iNewText.ToString() ) )
    {
        mTextBoxWidget->SetError( FText::GetEmpty() );
    }
    else
    {
        mTextBoxWidget->SetError( LOCTEXT( "wrong-pattern", "Syntax error in the pattern" ) );
    }
}

FReply
SPatternTextBox::OnClickExpanderButton()
{
    mIsExpanded = !mIsExpanded;

    return FReply::Handled();
}

const FSlateBrush*
SPatternTextBox::GetExpanderIcon() const
{
    return mIsExpanded ? &FAppStyle::Get().GetWidgetStyle<FExpandableAreaStyle>( "ExpandableArea" ).ExpandedImage : &FAppStyle::Get().GetWidgetStyle<FExpandableAreaStyle>( "ExpandableArea" ).CollapsedImage;
}

EVisibility
SPatternTextBox::GetKeywordsVisibility() const
{
    return mIsExpanded ? EVisibility::Visible : EVisibility::Collapsed;
}

bool
SPatternTextBox::IsKeywordUsed( int iKeywordIndex ) const
{
    check( mKeywords.IsValidIndex( iKeywordIndex ) );

    FString keyword = mKeywords[iKeywordIndex];

    FString pattern;
    mPatternHandle->GetValueAsFormattedString( pattern );

    return pattern.Contains( keyword );
}

FReply
SPatternTextBox::OnClickKeyword( int iKeywordIndex )
{
    check( mKeywords.IsValidIndex( iKeywordIndex ) );

    FString keyword_clicked = mKeywords[iKeywordIndex];

    FPlatformApplicationMisc::ClipboardCopy( *keyword_clicked );

    return FReply::Handled();
}

//void
//SPatternTextBox::ContextMenuExtender( FMenuBuilder& iMenuBuilder )
//{
//    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "keyword-list", "Keywords" ) );
//
//    for( auto keyword : mKeywords )
//    {
//        FMenuEntryParams params;
//        params.LabelOverride = FText::FromString( keyword );
//        params.DirectActions = FUIAction( FExecuteAction::CreateSP( this, &SPatternTextBox::AddKeywordAtCursor, keyword )
//                                        );
//
//        iMenuBuilder.AddMenuEntry( params );
//    }
//
//    iMenuBuilder.EndSection();
//}
//
//void
//SPatternTextBox::AddKeywordAtCursor( FString iKeyword )
//{
//    // Can't get the cursor position ...
//}

#undef LOCTEXT_NAMESPACE
