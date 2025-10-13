// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Color/SOdysseyColorHexadecimal.h"

#define LOCTEXT_NAMESPACE "Widgets"

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

FText SOdysseyColorHexadecimal::GetHexText() const
{
    ::ULIS::FColor ulisColor = mColor.Get().ToFormat( ::ULIS::Format_RGBA8 );
    uint8 r = ulisColor.Red8();
    uint8 g = ulisColor.Green8();
    uint8 b = ulisColor.Blue8();
    FColor color(r, g, b, 255);

    FString hex_RRGGBBAA = color.ToHex();
    FString hex_RRGGBB = hex_RRGGBBAA.LeftChop( 2 ); // Remove the last 2 characters as they are always "FF"

    return FText::FromString( hex_RRGGBB );
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
