// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SPatternTextBox.h"

#include "PropertyHandle.h"
#include "Widgets/Layout/SSpacer.h"

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
        mValidKeywords.Add( iArgs._Keywords[i] );

        FText label = iArgs._KeywordLabels[i];

        keyword_labels_widget->AddSlot()
            [
                SNew( STextBlock )
                .Text( label )
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
                .SelectAllTextWhenFocused( true )
                .ClearKeyboardFocusOnCommit( false )
                .OnTextCommitted( this, &SPatternTextBox::OnPatternTextCommited )
                .OnTextChanged( this, &SPatternTextBox::OnPatternTextChanged )
                .SelectAllTextOnCommit( true )
            ]

            + SHorizontalBox::Slot()
            .FillWidth( .1f )
            [
                SNew( SSpacer )
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 0, 4, 0, 0 )
        [
            SNew( SHorizontalBox )
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

    if( mOnVerifyPattern.IsBound() && !mOnVerifyPattern.Execute( new_pattern ) )
        return;

    //-

    mPatternHandle->SetValueFromFormattedString( new_pattern );
}

void
SPatternTextBox::OnPatternTextChanged( const FText& iNewText )
{
    if( !mOnVerifyPattern.IsBound() || mOnVerifyPattern.Execute( iNewText.ToString() ) )
    {
        TAttribute<FSlateColor> empty;
        mTextBoxWidget->SetTextBoxBackgroundColor( empty ); // Remove the attribute to use the "real" background style of the widget
    }
    else
    {
        mTextBoxWidget->SetTextBoxBackgroundColor( FLinearColor( 1, 0, 0, 0.35f ) );
        //mTextBoxWidget->SetTextBoxBackgroundColor( FAppStyle::Get().GetColor( TEXT( "ErrorReporting.BackgroundColor" ) ) );
    }
}

#undef LOCTEXT_NAMESPACE
