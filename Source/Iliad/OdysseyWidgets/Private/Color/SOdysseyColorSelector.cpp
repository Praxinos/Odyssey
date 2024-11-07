// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Color/SOdysseyColorSelector.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Brushes/SlateColorBrush.h"

#define LOCTEXT_NAMESPACE "Widgets"

/////////////////////////////////////////////////////
// Utility
FString
DecimalToHexString( int dec )
{
    FString hexchars( "0123456789ABCDEF" );
    FString res;

    int num = dec;
    while( true )
    {
        int quot = num / 16;
        int rem = num % 16;
        TCHAR hexrem = hexchars[ rem ];
        res.AppendChar( hexrem );
        num = quot;
        if( num == 0 )
            break;
    }

    if( res.Len() == 1 )
        res.AppendChar( '0' );

    return res.Reverse();
}

int
HexCharToDecimal( TCHAR iChar )
{
    int ret = 0;
    if( iChar >= 'A' && iChar <= 'F' ) ret = iChar - 'A' + 10;
    if( iChar >= 'a' && iChar <= 'f' ) ret = iChar - 'a' + 10;
    if( iChar >= '0' && iChar <= '9' ) ret = iChar - '0';
    return ret;
}

int
HexStringToDecimal( const FString& iStr )
{
    int res = 0;
    for( int i = 0; i < iStr.Len(); ++i )
    {
        res *= 16;
        res += HexCharToDecimal( iStr[i] );
    }
    return res;
}

/////////////////////////////////////////////////////
// SOdysseyColorSelector
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyColorSelector::Construct( const FArguments& InArgs )
{
    mColor = InArgs._Color;
    mOnColorChangeCallback = InArgs._OnColorChange;

    ChildSlot
    [
        SNew( SOverlay )
        +SOverlay::Slot()
        [
            SAssignNew( mAdvancedColorWheel, SOdysseyAdvancedColorWheel )
            .MinDesiredWidth(   150 )
            .MinDesiredHeight(  150 )
            .MaxDesiredWidth(   800 )
            .MaxDesiredHeight(  800 )
            .Color(mColor)
            .OnColorChange( mOnColorChangeCallback )
        ]
        +SOverlay::Slot()
        .HAlign( HAlign_Right )
        .VAlign( VAlign_Bottom )
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNullWidget::NullWidget //spacer
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .MaxWidth(90.0f)
            [
                SNew(SEditableTextBox)
                .MinDesiredWidth(90.0f)
                .Text(this, &SOdysseyColorSelector::GetHexText)
                .OnTextChanged( this, &SOdysseyColorSelector::OnHexTextChanged )
                .OnTextCommitted( this, &SOdysseyColorSelector::OnHexTextCommitted )
            ]
        ]
    ];
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks

bool
SOdysseyColorSelector::HexBoxIsValidChar( TCHAR iChar ) const
{
    return  ( ( iChar >= 'A' && iChar <= 'F' ) || ( iChar >= 'a' && iChar <= 'f' ) || ( iChar >= '0' && iChar <= '9' ) || ( iChar == '\b' ) );
}


FReply
SOdysseyColorSelector::HexBoxOnKeyChar( const FGeometry&, const FCharacterEvent& iEvent ) const
{
    TCHAR mchar = iEvent.GetCharacter();
    int len = mHexTextBox->GetText().ToString().Len();
    bool forward = HexBoxIsValidChar( mchar ) && len < 6;
    if( forward == false && mchar == '\b' ) forward = true;
    if( forward == false && mHexTextBox->AnyTextSelected() ) forward = true;
    return forward ? FReply::Unhandled() : FReply::Handled();
}

void
SOdysseyColorSelector::HexBoxOnTextChanged( const FText& iText )
{
    FString str = iText.ToString();
    FString res;
    for( int i = 0; i < FMath::Min( 6, str.Len() ); ++i )
    {
        TCHAR cchar = str[i];
        if( HexBoxIsValidChar( cchar ) )
            res.Append( &cchar, 1 );
    }

    /* if( !res.Equals( str ) )
        mHexTextBox->SetText( FText::FromString( res ) ); */

    //Adjust
}


void
SOdysseyColorSelector::HexBoxOnTextCommited( const FText& iText, ETextCommit::Type )
{
    FString str = iText.ToString();
    FString res = str.ToUpper();
    for( int i = str.Len(); i < 6; ++i )
    {
        TCHAR filler = '0';
        res.Append( &filler, 1 );
    }

    // mHexTextBox->SetText( FText::FromString( res ) );
    FString str_r = res.Mid( 0, 2 );
    FString str_g = res.Mid( 2, 2 );
    FString str_b = res.Mid( 4, 2 );
    int r = HexStringToDecimal( str_r );
    int g = HexStringToDecimal( str_g );
    int b = HexStringToDecimal( str_b );
    ::ULIS::FColor newColor = ::ULIS::FColor::FromRGBA8( r, g, b );

    //mAdvancedColorWheel->SetColor( newColor );

    //Set or Abort + start / adjust if needed
}


FText
SOdysseyColorSelector::GetColorHex() const
{
    //Can be null so we have to check
    ::ULIS::FColor color = mColor.Get().ToFormat( ::ULIS::Format_RGBA8 );
    uint8 r = color.Red8();
    uint8 g = color.Green8();
    uint8 b = color.Blue8();
    FString str_r = DecimalToHexString( r );
    FString str_g = DecimalToHexString( g );
    FString str_b = DecimalToHexString( b );
    FString cat = str_r + str_g + str_b;

    // mHexTextBox->SetText( FText::FromString( cat ) );
    // OnColorChangedCallback.ExecuteIfBound( iColor );

    return FText::FromString( cat );
}


FText SOdysseyColorSelector::GetHexText() const
{
    ::ULIS::FColor ulisColor = mColor.Get().ToFormat( ::ULIS::Format_RGBA8 );
    uint8 r = ulisColor.Red8();
    uint8 g = ulisColor.Green8();
    uint8 b = ulisColor.Blue8();
    FColor color(r, g, b, 255);
    return FText::FromString(color.ToHex());
}

void
SOdysseyColorSelector::OnHexTextChanged( const FText& iText )
{
    FColor color = FColor::FromHex(iText.ToString());
    ::ULIS::FColor ulisColor = ::ULIS::FColor::FromRGBA8(color.R, color.G, color.B, color.A);
    mOnColorChangeCallback.ExecuteIfBound(eOdysseyEventState::kAdjust, ulisColor );
}

void SOdysseyColorSelector::OnHexTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
    if (!Text.IsEmpty() && ((CommitType == ETextCommit::OnEnter) || (CommitType == ETextCommit::OnUserMovedFocus)))
    {
        FColor color = FColor::FromHex(Text.ToString());
        ::ULIS::FColor ulisColor = ::ULIS::FColor::FromRGBA8(color.R, color.G, color.B, color.A);
        mOnColorChangeCallback.ExecuteIfBound(eOdysseyEventState::kSet, ulisColor );
    }
}

#undef LOCTEXT_NAMESPACE

