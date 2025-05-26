// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Color/SOdysseyColorHexadecimal.h"

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
// SOdysseyColorHexadecimal
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyColorHexadecimal::Construct( const FArguments& InArgs )
{
    mColor = InArgs._Color;
    mOnColorChanged = InArgs._OnColorChanged;

    ChildSlot
    [
        SNew(SEditableTextBox)
        .MinDesiredWidth(90.0f)
        .Text(this, &SOdysseyColorHexadecimal::GetHexText)
        .OnTextChanged( this, &SOdysseyColorHexadecimal::OnHexTextChanged )
        .OnTextCommitted( this, &SOdysseyColorHexadecimal::OnHexTextCommitted )
    ];
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks

bool
SOdysseyColorHexadecimal::HexBoxIsValidChar( TCHAR iChar ) const
{
    return  ( ( iChar >= 'A' && iChar <= 'F' ) || ( iChar >= 'a' && iChar <= 'f' ) || ( iChar >= '0' && iChar <= '9' ) || ( iChar == '\b' ) );
}


FReply
SOdysseyColorHexadecimal::HexBoxOnKeyChar( const FGeometry&, const FCharacterEvent& iEvent ) const
{
    TCHAR mchar = iEvent.GetCharacter();
    int len = mHexTextBox->GetText().ToString().Len();
    bool forward = HexBoxIsValidChar( mchar ) && len < 6;
    if( forward == false && mchar == '\b' ) forward = true;
    if( forward == false && mHexTextBox->AnyTextSelected() ) forward = true;
    return forward ? FReply::Unhandled() : FReply::Handled();
}

void
SOdysseyColorHexadecimal::HexBoxOnTextChanged( const FText& iText )
{
    FString str = iText.ToString();
    FString res;
    for( int i = 0; i < FMath::Min( 6, str.Len() ); ++i )
    {
        TCHAR cchar = str[i];
        if( HexBoxIsValidChar( cchar ) )
            res.Append( &cchar, 1 );
    }
}


void
SOdysseyColorHexadecimal::HexBoxOnTextCommited( const FText& iText, ETextCommit::Type )
{
    FString str = iText.ToString();
    FString res = str.ToUpper();
    for( int i = str.Len(); i < 6; ++i )
    {
        TCHAR filler = '0';
        res.Append( &filler, 1 );
    }

    FString str_r = res.Mid( 0, 2 );
    FString str_g = res.Mid( 2, 2 );
    FString str_b = res.Mid( 4, 2 );
    int r = HexStringToDecimal( str_r );
    int g = HexStringToDecimal( str_g );
    int b = HexStringToDecimal( str_b );
    ::ULIS::FColor newColor = ::ULIS::FColor::FromRGBA8( r, g, b );

    //Set or Abort + start / adjust if needed
}


FText
SOdysseyColorHexadecimal::GetColorHex() const
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

    return FText::FromString( cat );
}


FText SOdysseyColorHexadecimal::GetHexText() const
{
    ::ULIS::FColor ulisColor = mColor.Get().ToFormat( ::ULIS::Format_RGBA8 );
    uint8 r = ulisColor.Red8();
    uint8 g = ulisColor.Green8();
    uint8 b = ulisColor.Blue8();
    FColor color(r, g, b, 255);
    return FText::FromString(color.ToHex());
}

void
SOdysseyColorHexadecimal::OnHexTextChanged( const FText& iText )
{
    FColor color = FColor::FromHex(iText.ToString());
    ::ULIS::FColor ulisColor = ::ULIS::FColor::FromRGBA8(color.R, color.G, color.B, color.A);
    mOnColorChanged.ExecuteIfBound(eOdysseyEventState::kAdjust, ulisColor );
}

void SOdysseyColorHexadecimal::OnHexTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
    if (!Text.IsEmpty() && ((CommitType == ETextCommit::OnEnter) || (CommitType == ETextCommit::OnUserMovedFocus)))
    {
        FColor color = FColor::FromHex(Text.ToString());
        ::ULIS::FColor ulisColor = ::ULIS::FColor::FromRGBA8(color.R, color.G, color.B, color.A);
        mOnColorChanged.ExecuteIfBound(eOdysseyEventState::kSet, ulisColor );
    }
}

#undef LOCTEXT_NAMESPACE
