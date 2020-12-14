// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Color/SOdysseyColorSelector.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Brushes/SlateColorBrush.h"

#define LOCTEXT_NAMESPACE "OdysseyColorSelector"


/////////////////////////////////////////////////////
// Cosmetic
static FSlateColorBrush selector_header_brush = FSlateColorBrush( FLinearColor( FColor( 70, 70, 70 ) ) );
static FSlateColor hex_box_bg_brush = FSlateColor( FLinearColor( FColor( 50, 50, 50 ) ) );
static FSlateColor hex_box_fg_brush = FSlateColor( FLinearColor( FColor( 127, 127, 127 ) ) );

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
    options.Add(MakeShareable(new FString("Color Wheel")));
    CurrentItem = options[0];

    ChildSlot
    [
        SNew( SVerticalBox )
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SHorizontalBox )
            +SHorizontalBox::Slot()
            [
                SNew( SBorder )
                .BorderImage( &selector_header_brush )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "Selector", "Selector" ) )
                ]
            ]
            +SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew( SComboBox< FComboItemType > )
                .OptionsSource(&options)
                .OnSelectionChanged(this, &SOdysseyColorSelector::OnSelectionChanged)
                .OnGenerateWidget(this, &SOdysseyColorSelector::MakeWidgetForOption)
                .InitiallySelectedItem(CurrentItem)
                [
                    SNew(STextBlock)
                    .Text(this, &SOdysseyColorSelector::GetCurrentItemLabel)
                ]
            ]
        ]
        +SVerticalBox::Slot()
        [
            SNew( SOverlay )
            +SOverlay::Slot()
            [
                SAssignNew( adv_color_wheel, SOdysseyAdvancedColorWheel )
                .MinDesiredWidth(   150 )
                .MinDesiredHeight(  150 )
                .MaxDesiredWidth(   800 )
                .MaxDesiredHeight(  800 )
                .Color(mColor)
                .OnColorChange( mOnColorChangeCallback )
            ]
            +SOverlay::Slot()
            .HAlign( HAlign_Left )
            .VAlign( VAlign_Top )
            .Padding( TAttribute< FMargin >( this, &SOdysseyColorSelector::GetHexBoxPosition ) )
            [
                SNew( SBox )
                .WidthOverride( this, &SOdysseyColorSelector::GetHexBoxWidth )
                .HeightOverride( this, &SOdysseyColorSelector::GetHexBoxHeight )
                .Visibility( this, &SOdysseyColorSelector::GetHexBoxVisibility )
                [
                    SAssignNew( hex_editable_text_box, SEditableTextBox )
                    .Text( this, &SOdysseyColorSelector::GetColorHex )
                    .BackgroundColor( hex_box_bg_brush )
                    .ForegroundColor( hex_box_fg_brush )
                    .Justification( ETextJustify::Center )
                    .Font( this, &SOdysseyColorSelector::GetHexFont )
                    .SelectAllTextWhenFocused( true )
                    .RevertTextOnEscape( true )
                    .OnKeyCharHandler( this, &SOdysseyColorSelector::HexBoxOnKeyChar )
                    .OnTextChanged( this, &SOdysseyColorSelector::HexBoxOnTextChanged )
                    .OnTextCommitted( this, &SOdysseyColorSelector::HexBoxOnTextCommited )
                ]
            ]
        ]
    ];
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks
TSharedRef<SWidget>
SOdysseyColorSelector::MakeWidgetForOption( FComboItemType InOption )
{
    return SNew(STextBlock).Text(FText::FromString(*InOption));
}


void
SOdysseyColorSelector::OnSelectionChanged(FComboItemType NewValue, ESelectInfo::Type)
{
    CurrentItem = NewValue;
}


FText
SOdysseyColorSelector::GetCurrentItemLabel() const
{
    if (CurrentItem.IsValid())
    {
        return FText::FromString(*CurrentItem);
    }

    return LOCTEXT("InvalidComboEntryText", "<<Invalid option>>");
}


FMargin
SOdysseyColorSelector::GetHexBoxPosition() const
{
    FVector2D temp = adv_color_wheel->GetInternalPadding() + adv_color_wheel->GetInternalSize() * FVector2D( 0.898, 0.957 );
    return FMargin( temp.X - GetHexBoxWidth().Get()
                  , temp.Y - GetHexBoxHeight().Get()
                  , 0
                  , 0
    );
}


FOptionalSize
SOdysseyColorSelector::GetHexBoxWidth() const
{
    return  adv_color_wheel->GetInternalSize().X * 0.25;
}


FOptionalSize
SOdysseyColorSelector::GetHexBoxHeight() const
{
    return  adv_color_wheel->GetInternalSize().Y * 0.075;
}


EVisibility
SOdysseyColorSelector::GetHexBoxVisibility() const
{
    return  adv_color_wheel->IsFullyVisible() ? EVisibility::Visible : EVisibility::Hidden;
}

FSlateFontInfo
SOdysseyColorSelector::GetHexFont() const
{
    FSlateFontInfo nfo = FCoreStyle::Get().GetWidgetStyle< FEditableTextBoxStyle >("NormalEditableTextBox").Font;
    nfo.Size *= adv_color_wheel->GetDrawRatio() * 5;
    return  nfo;
}


bool
SOdysseyColorSelector::HexBoxIsValidChar( TCHAR iChar ) const
{
    return  ( ( iChar >= 'A' && iChar <= 'F' ) || ( iChar >= 'a' && iChar <= 'f' ) || ( iChar >= '0' && iChar <= '9' ) || ( iChar == '\b' ) );
}


FReply
SOdysseyColorSelector::HexBoxOnKeyChar( const FGeometry&, const FCharacterEvent& iEvent ) const
{
    TCHAR mchar = iEvent.GetCharacter();
    int len = hex_editable_text_box->GetText().ToString().Len();
    bool forward = HexBoxIsValidChar( mchar ) && len < 6;
    if( forward == false && mchar == '\b' ) forward = true;
    if( forward == false && hex_editable_text_box->AnyTextSelected() ) forward = true;
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
        hex_editable_text_box->SetText( FText::FromString( res ) ); */

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

    // hex_editable_text_box->SetText( FText::FromString( res ) );
    FString str_r = res.Mid( 0, 2 );
    FString str_g = res.Mid( 2, 2 );
    FString str_b = res.Mid( 4, 2 );
    int r = HexStringToDecimal( str_r );
    int g = HexStringToDecimal( str_g );
    int b = HexStringToDecimal( str_b );
    ::ul3::FPixelValue newColor = ::ul3::FPixelValue::FromRGBA8( r, g, b );

    //adv_color_wheel->SetColor( newColor );

    //Set or Abort + start / adjust if needed
}


FText
SOdysseyColorSelector::GetColorHex() const
{
    //Can be null so we have to check
    ::ul3::FPixelValue color = ::ul3::Conv(mColor.Get(), ULIS3_FORMAT_RGBA8);
    uint8 r = color.Red8();
    uint8 g = color.Green8();
    uint8 b = color.Blue8();
    FString str_r = DecimalToHexString( r );
    FString str_g = DecimalToHexString( g );
    FString str_b = DecimalToHexString( b );
    FString cat = str_r + str_g + str_b;

    // hex_editable_text_box->SetText( FText::FromString( cat ) );
    // OnColorChangedCallback.ExecuteIfBound( iColor );

    return FText::FromString( cat );
}


#undef LOCTEXT_NAMESPACE

