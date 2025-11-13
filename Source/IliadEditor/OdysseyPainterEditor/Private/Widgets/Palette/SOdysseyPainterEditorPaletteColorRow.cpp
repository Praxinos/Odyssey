// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Palette/SOdysseyPainterEditorPaletteColorRow.h"

#include "Widgets/Colors/SColorBlock.h"
#include "Palette/OdysseyPaletteEntryColor.h"
#include "OdysseyStyle.h"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyPainterEditorPaletteColorRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& iTreeView)
{
    mIsCurrent = InArgs._IsCurrent;
    mSet = InArgs._Set;
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
//
//TSharedRef<SWidget>
//SOdysseyPainterEditorPaletteColorRow::GenerateWidgetForColumn( const FName& InColumnName )
//{
//    if (InColumnName == "Color")
//    {
//        return GenerateColorWidget();
//    }
//
//    return SOdysseyPainterEditorPaletteEntryRow::GenerateWidgetForColumn(InColumnName);
//}
//
//TSharedRef<SWidget>
//SOdysseyPainterEditorPaletteColorRow::GenerateColorWidget()
//{
//    return SNew(SBox)
//        .Padding(FMargin(10, 2, 0, 2))
//        [
//            SNew(SColorBlock )
//            .Color(this, &SOdysseyPainterEditorPaletteColorRow::GetEntryColorAsLinear)
//        ];
//}

FLinearColor SOdysseyPainterEditorPaletteColorRow::GetEntryColorAsLinear() const
{
    return FLinearColor( mEntryColor.Get()->GetColor( mSet.Get() ) );
}

const FSlateBrush*
SOdysseyPainterEditorPaletteColorRow::GetIcon() const
{
    return FOdysseyStyle::Get().GetBrush("OdysseyPalette.EntryColor");
}

TSharedRef<SWidget>
SOdysseyPainterEditorPaletteColorRow::GenerateContentWidget() //override
{
    return SNew( SHorizontalBox )
        //+ SHorizontalBox::Slot()
        //.AutoWidth()
        //.VAlign( VAlign_Center )
        //[
        //    SNew( SImage )
        //    .Image( this, &SOdysseyPainterEditorPaletteColorRow::GetIcon )
        //    .ColorAndOpacity( this, &SOdysseyPainterEditorPaletteColorRow::GetIconColorAndOpacity )
        //]
        //+ SHorizontalBox::Slot()
        //.AutoWidth()
        //.VAlign( VAlign_Center )
        //[
        //    SNew( SColorBlock )
        //    .Color( this, &SOdysseyPainterEditorPaletteColorRow::GetEntryColorAsLinear )
        //]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign( VAlign_Fill )
        [
            SNew( SBox )
            .WidthOverride( 5.f + 60.f + 5.f )
            .Padding( FMargin( 5.f, 0 ) )
            [
                SNew( SBorder )
                .BorderBackgroundColor_Lambda( [this]()
                                               {
                                                   return FColor( 20, 20, 20 );
                                               } )
                .BorderImage( FOdysseyStyle::Get().GetBrush( "OdysseyPalette.BadgeShape" ) )
                .VAlign( VAlign_Fill )
                .HAlign( HAlign_Center )
                [
                    SNew( STextBlock )
                    .Text( FMath::RandBool() ? FText::FromString( TEXT( "Indexed" ) ) : FText::FromString( TEXT( "Raw" ) ) )
                    //.Text( LOCTEXT( "
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
                .BorderBackgroundColor_Lambda( [this]()
                                               {
                                                   return GetEntryColorAsLinear();
                                               } )
                .BorderImage( FOdysseyStyle::Get().GetBrush( "OdysseyPalette.BadgeShape" ) )
                .VAlign( VAlign_Fill )
            ]
        ];
}

bool
SOdysseyPainterEditorPaletteColorRow::IsItemSelected() const
{
    return mIsCurrent.Get();
}
