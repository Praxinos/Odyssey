// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Palette/SOdysseyPainterEditorPaletteColorRow.h"

#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Text/STextBlock.h"

#include "Palette/OdysseyPaletteEntryColor.h"
#include "OdysseyStyle.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyPainterEditorPaletteColorRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& iTreeView)
{
    mIsCurrent = InArgs._IsCurrent;
    mSet = InArgs._Set;
    mColorType = InArgs._ColorType;
    mEntryColor = InArgs._Entry;

    SOdysseyPainterEditorPaletteEntryRow::Construct(
        SOdysseyPainterEditorPaletteEntryRow::FArguments()
            .Entry_Lambda(
                [this]() -> UOdysseyPaletteEntry*
                {
                    return mEntryColor.Get();
                }
            ),
        iTreeView
    );
}

//PRIVATE API-----------------------------------------------------------

FLinearColor SOdysseyPainterEditorPaletteColorRow::GetEntryColorAsLinear() const
{
    return FLinearColor( mEntryColor.Get()->GetColor( mSet.Get() ) );
}

const FSlateBrush*
SOdysseyPainterEditorPaletteColorRow::GetIcon() const
{
    static FSlateNoResource no_brush;
    return &no_brush;
    //return FOdysseyStyle::Get().GetBrush("OdysseyPalette.EntryColor");
}

TSharedRef<SWidget>
SOdysseyPainterEditorPaletteColorRow::GenerateContentWidget() //override
{
    return SNew( SHorizontalBox )

        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign( VAlign_Fill )
        [
            SNew( SBox )
            .WidthOverride( 5.f + 60.f + 5.f )
            .Padding( FMargin( 5.f, 0 ) )
            [
                SNew( SBorder )
                .BorderImage( FOdysseyStyle::Get().GetBrush( "OdysseyPalette.BadgeShape" ) )
                .BorderBackgroundColor_Lambda( [this]()
                                               {
                                                   return FColor( 20, 20, 20 );
                                               } )
                .VAlign( VAlign_Fill )
                .HAlign( HAlign_Center )
                [
                    SNew( STextBlock )
                    .Text_Lambda( [this]()
                                  {
                                      return ( mColorType.Get() == EOdysseyPainterEditorColorType::Indexed ) ? LOCTEXT( "PainterEditor.Palette.ColorTypeBadge-Indexed", "Indexed" ) : LOCTEXT( "PainterEditor.Palette.ColorTypeBadge-Raw", "Raw" );
                                  } )
                ]
            ]
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign( VAlign_Fill )
        [
            SNew( SBox )
            .WidthOverride( 5.f + 50.f + 5.f )
            .Padding( FMargin( 5.f, 0 ) )
            [
                SNew( SBorder )
                .BorderImage( FOdysseyStyle::Get().GetBrush( "OdysseyPalette.BadgeShape" ) )
                .BorderBackgroundColor_Lambda( [this]()
                                               {
                                                   return GetEntryColorAsLinear();
                                               } )
                .VAlign( VAlign_Fill )
            ]
        ];
}

bool
SOdysseyPainterEditorPaletteColorRow::IsItemSelected() const
{
    return mIsCurrent.Get();
}

#undef LOCTEXT_NAMESPACE
