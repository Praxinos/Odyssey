// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "SArianeEditorColorSelectorPanel.h"
#include "SArianeEditorLayerStack.h"
#include "ArianeEditor.h"
#include "ArianeEditorTool.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerStack.h"
// Odyssey Headers
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/Color/SOdysseyAdvancedColorWheel.h"
#include "Widgets/Color/SOdysseyColorSliders.h"
#include "Widgets/Color/SOdysseyColorHexadecimal.h"
//#include "Widgets/Palette/SOdysseyPainterEditorPaletteSetList.h"
#include "OdysseyStyle.h"
#include "OdysseyPalette.h"
// Unreal headers
#include "Widgets/Input/SSegmentedControl.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

SArianeEditorColorSelectorPanel::~SArianeEditorColorSelectorPanel()
{}

SArianeEditorColorSelectorPanel::SArianeEditorColorSelectorPanel()
{}

void
SArianeEditorColorSelectorPanel::Construct(const FArguments& InArgs, FArianeEditor* InEditor)
{
    Editor = InEditor;

    ChildSlot
    [
        SNew(SScrollBox)
        +SScrollBox::Slot()
        .AutoSize()
        [
            SNew(SSegmentedControl<EOdysseyPainterEditorColorType>)
            //.Visibility(this, &SArianeEditorColorSelectorPanel::GetColorTypeVisibility)
            .Value(this, &SArianeEditorColorSelectorPanel::GetColorType)
            .OnValueChecked(this, &SArianeEditorColorSelectorPanel::OnColorTypeChanged)
            .Style( &FOdysseyStyle::Get(), "OdysseyPalette.Tabs" )

            //Color Type: RAW
            + SSegmentedControl<EOdysseyPainterEditorColorType>::Slot(EOdysseyPainterEditorColorType::Raw)
            .Text(LOCTEXT("color-selector.color-type.raw.name", "Raw"))
            .ToolTip(LOCTEXT("color-selector.color-type.raw.tooltip", "Switch to Raw colors"))
            .Icon(FOdysseyStyle::GetBrush( "PainterEditor.ColorSelector.ColorType.Raw" ))

            //Color Type: INDEXED
            + SSegmentedControl<EOdysseyPainterEditorColorType>::Slot(EOdysseyPainterEditorColorType::Indexed)
            .Text(LOCTEXT("color-selector.color-type.indexed.name", "Indexed"))
            .ToolTip(LOCTEXT("color-selector.color-type.indexed.tooltip", "Switch to Indexed colors"))
            .Icon(FOdysseyStyle::GetBrush( "PainterEditor.ColorSelector.ColorType.Indexed" ))
        ]
        +SScrollBox::Slot()
        .AutoSize()
        [
            SNew(SVerticalBox)
            //.Visibility(this, &SArianeEditorColorSelectorPanel::GetRawColorWidgetsVisibility)
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    //SAssignNew(mColorWheelExpanderArrow, SButton)
                    SNew(SButton)
                    .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                    .VAlign(VAlign_Center)
                    .HAlign(HAlign_Center)
                    .ClickMethod( EButtonClickMethod::MouseDown )
                    //.OnClicked( this, &SArianeEditorColorSelectorPanel::OnColorWheelExpanderArrowClicked )
                    .ContentPadding(0.f)
                    .ForegroundColor( FSlateColor::UseForeground() )
                    .IsFocusable( false )
                    [
                        SNew(SImage)
                        //.Image( this, &SArianeEditorColorSelectorPanel::GetColorWheelExpanderArrowImage )
                        .ColorAndOpacity( FSlateColor::UseSubduedForeground() )
                    ]
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT( "color-selector.color-wheel.name", "Color Wheel" ))
                ]
            ]
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SOdysseyAdvancedColorWheel )
                //.Visibility(this, &SArianeEditorColorSelectorPanel::GetColorWheelVisibility)
                .MinDesiredWidth(   150 )
                .MinDesiredHeight(  150 )
                .MaxDesiredWidth(   200 )
                .MaxDesiredHeight(  200 )
                .DesiredWidth(   175 )
                .DesiredHeight(  175 )
                .Color(this, &SArianeEditorColorSelectorPanel::GetRawColor)
                .OnColorChanged(this, &SArianeEditorColorSelectorPanel::OnColorChanged)
            ]
        ]

        +SScrollBox::Slot()
        .AutoSize()
        [
            SNew(SVerticalBox)
            //.Visibility(this, &SArianeEditorColorSelectorPanel::GetRawColorWidgetsVisibility)
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    //SAssignNew(mColorSlidersExpanderArrow, SButton)
                    SNew(SButton)
                    .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                    .VAlign(VAlign_Center)
                    .HAlign(HAlign_Center)
                    .ClickMethod( EButtonClickMethod::MouseDown )
                    //.OnClicked( this, &SArianeEditorColorSelectorPanel::OnColorSlidersExpanderArrowClicked )
                    .ContentPadding(0.f)
                    .ForegroundColor( FSlateColor::UseForeground() )
                    .IsFocusable( false )
                    [
                        SNew(SImage)
                        //.Image( this, &SArianeEditorColorSelectorPanel::GetColorSlidersExpanderArrowImage )
                        .ColorAndOpacity( FSlateColor::UseSubduedForeground() )
                    ]
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT( "color-selector.color-sliders.name", "Color Sliders" ))
                ]
            ]
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SOdysseyColorSliders )
                //.Visibility(this, &SArianeEditorColorSelectorPanel::GetColorSlidersVisibility)
                .Color(this, &SArianeEditorColorSelectorPanel::GetRawColor)
                .OnColorChanged(this, &SArianeEditorColorSelectorPanel::OnColorChanged)
            ]
        ]
        +SScrollBox::Slot()
        .AutoSize()
        [
            SNew(SVerticalBox)
            //.Visibility(this, &SArianeEditorColorSelectorPanel::GetRawColorWidgetsVisibility)
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    //SAssignNew(mHexadecimalExpanderArrow, SButton)
                    SNew(SButton)
                    .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                    .VAlign(VAlign_Center)
                    .HAlign(HAlign_Center)
                    .ClickMethod( EButtonClickMethod::MouseDown )
                    //.OnClicked( this, &SArianeEditorColorSelectorPanel::OnHexadecimalExpanderArrowClicked )
                    .ContentPadding(0.f)
                    .ForegroundColor( FSlateColor::UseForeground() )
                    .IsFocusable( false )
                    [
                        SNew(SImage)
                        //.Image( this, &SArianeEditorColorSelectorPanel::GetHexadecimalExpanderArrowImage )
                        .ColorAndOpacity( FSlateColor::UseSubduedForeground() )
                    ]
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT( "color-selector.hexadecimal.name", "Hexadecimal" ))
                ]
            ]
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SOdysseyColorHexadecimal)
                //.Visibility(this, &SArianeEditorColorSelectorPanel::GetColorHexadecimalVisibility)
                .Color(this, &SArianeEditorColorSelectorPanel::GetRawColor)
                .OnColorChanged(this, &SArianeEditorColorSelectorPanel::OnColorChanged)
            ]
        ]

        +SScrollBox::Slot()
        .FillSize(1.0f)
        [
            SNew(SVerticalBox)
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    //SAssignNew(mColorPaletteExpanderArrow, SButton)
                    SNew(SButton)
                    .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                    .VAlign(VAlign_Center)
                    .HAlign(HAlign_Center)
                    .ClickMethod( EButtonClickMethod::MouseDown )
                    //.OnClicked( this, &SArianeEditorColorSelectorPanel::OnColorPaletteExpanderArrowClicked )
                    .ContentPadding(0.f)
                    .ForegroundColor( FSlateColor::UseForeground() )
                    .IsFocusable( false )
                    [
                        SNew(SImage)
                        //.Image( this, &SArianeEditorColorSelectorPanel::GetColorPaletteExpanderArrowImage )
                        .ColorAndOpacity( FSlateColor::UseSubduedForeground() )
                    ]
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT( "color-selector.color-palette.name", "Color Palette" ))
                ]
            ]
            +SVerticalBox::Slot()
            [
                SNew(SBox)
                .MaxDesiredHeight(100.f)
                [
                    SNullWidget::NullWidget
/* Gazry
                    SNew(SOdysseyPainterEditorPaletteSetList)
                    .Visibility(this, &SArianeEditorColorSelectorPanel::GetColorPaletteVisibility)
                    .PaletteSets(this, &SArianeEditorColorSelectorPanel::GetPaletteSets)
                    .CurrentColorEntry(this, &SArianeEditorColorSelectorPanel::GetCurrentPaletteColorEntry)
                    .CurrentSet(this, &SArianeEditorColorSelectorPanel::GetCurrentPaletteSet)
                    .ColorType( this, &SArianeEditorColorSelectorPanel::GetColorType )
                    .OnPaletteSetChanged(this, &SArianeEditorColorSelectorPanel::OnPaletteSetChanged)
                    .OnAddPaletteSet(this, &SArianeEditorColorSelectorPanel::OnAddPaletteSet)
                    .OnRemovePaletteSet(this, &SArianeEditorColorSelectorPanel::OnRemovePaletteSet)
                    .OnCurrentColorEntryChanged(this, &SArianeEditorColorSelectorPanel::OnPaletteCurrentColorEntryChanged)
*/
                ]
            ]
        ]
    ];
}

::ULIS::FColor
SArianeEditorColorSelectorPanel::GetRawColor() const
{
    return Editor->GetPaintColor();
}

/*
const FSlateBrush*
SArianeEditorColorSelectorPanel::GetExpanderArrowImage(TSharedPtr<SButton> iExpander, bool iIsExpanded) const
{
    FName resourceName;
    if (iIsExpanded)
    {
        if ( iExpander->IsHovered() )
        {
            static FName expandedHoveredName = "TreeArrow_Expanded_Hovered";
            resourceName = expandedHoveredName;
        }
        else
        {
            static FName expandedName = "TreeArrow_Expanded";
            resourceName = expandedName;
        }
    }
    else
    {
        if ( iExpander->IsHovered() )
        {
            static FName collapsedHoveredName = "TreeArrow_Collapsed_Hovered";
            resourceName = collapsedHoveredName;
        }
        else
        {
            static FName collapsedName = "TreeArrow_Collapsed";
            resourceName = collapsedName;
        }
    }

    return FAppStyle::Get().GetBrush(resourceName);
}
*/

/*
const FSlateBrush*
SArianeEditorColorSelectorPanel::GetColorWheelExpanderArrowImage() const
{
    return GetExpanderArrowImage(mColorWheelExpanderArrow, mIsColorWheelExpanded);
}
*/

/*
const FSlateBrush*
SArianeEditorColorSelectorPanel::GetColorSlidersExpanderArrowImage() const
{
    return GetExpanderArrowImage(mColorSlidersExpanderArrow, mIsColorSlidersExpanded);
}
*/

/*
const FSlateBrush*
SArianeEditorColorSelectorPanel::GetHexadecimalExpanderArrowImage() const
{
    return GetExpanderArrowImage(mHexadecimalExpanderArrow, mIsHexadecimalExpanded);
}
*/

/*
const FSlateBrush*
SArianeEditorColorSelectorPanel::GetColorPaletteExpanderArrowImage() const
{
    return GetExpanderArrowImage(mColorPaletteExpanderArrow, mIsColorPaletteExpanded);
}
*/

EOdysseyPainterEditorColorType
SArianeEditorColorSelectorPanel::GetColorType() const
{
    return Editor->GetColorType();
}

EVisibility
SArianeEditorColorSelectorPanel::GetColorTypeVisibility() const
{
    UArianeEditorTool* Tool = Editor->GetCurrentTool();

    if ( Tool )
    {
        Tool->SupportsColorType( EOdysseyPainterEditorColorType::Indexed ) ? EVisibility::Visible
                                                                           : EVisibility::Collapsed;
    }

    return EVisibility::Collapsed; //Only Raw Colors are displayed by default}
}

EVisibility
SArianeEditorColorSelectorPanel::GetRawColorWidgetsVisibility() const
{
    UArianeEditorTool* tool = Editor->GetCurrentTool();
    if (!tool)
        return EVisibility::Visible; //Only Raw Colors are displayed by default

    if (!tool->SupportsColorType(EOdysseyPainterEditorColorType::Indexed))
        return EVisibility::Visible; //Only Raw Colors are displayed by default

    return Editor->GetColorType() == EOdysseyPainterEditorColorType::Raw ? EVisibility::Visible : EVisibility::Collapsed;
}

const TArray<UOdysseyPaletteSet*>*
SArianeEditorColorSelectorPanel::GetPaletteSets() const
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        &Painting3DComponent->GetPaletteSets();
    }

    return nullptr;
}

UOdysseyPaletteEntryColor*
SArianeEditorColorSelectorPanel::GetCurrentPaletteColorEntry() const
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        return Painting3DComponent->GetCurrentPaletteColorEntry();
    }

    return nullptr;
}

FGuid
SArianeEditorColorSelectorPanel::GetCurrentPaletteSet() const
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        return Painting3DComponent->GetCurrentPaletteSet();
    }

    return FGuid();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
SArianeEditorColorSelectorPanel::OnColorChanged( eOdysseyEventState::Type EventState
                                               , const ::ULIS::FColor& Color )
{
    Editor->SetPaintColor( Color/*, EventState == eOdysseyEventState::kSet*/ );
}

/*
FReply
SArianeEditorColorSelectorPanel::OnHexadecimalExpanderArrowClicked()
{
    mIsHexadecimalExpanded = !mIsHexadecimalExpanded;
    return FReply::Handled();
}
*/

/*
FReply
SArianeEditorColorSelectorPanel::OnColorSlidersExpanderArrowClicked()
{
    mIsColorSlidersExpanded = !mIsColorSlidersExpanded;
    return FReply::Handled();
}
*/

/*
FReply
SArianeEditorColorSelectorPanel::OnColorWheelExpanderArrowClicked()
{
    mIsColorWheelExpanded = !mIsColorWheelExpanded;
    return FReply::Handled();
}
*/

/*
FReply
SArianeEditorColorSelectorPanel::OnColorPaletteExpanderArrowClicked()
{
    mIsColorPaletteExpanded = !mIsColorPaletteExpanded;
    return FReply::Handled();
}
*/

void
SArianeEditorColorSelectorPanel::OnColorTypeChanged( EOdysseyPainterEditorColorType iType, ECheckBoxState iState )
{
    if ( iState == ECheckBoxState::Checked )
        Editor->SetColorType(iType);
}

void
SArianeEditorColorSelectorPanel::OnAddPaletteSet( UOdysseyPalette* Palette )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        Painting3DComponent->AddPaletteSet( Palette );
    }
}

void
SArianeEditorColorSelectorPanel::OnRemovePaletteSet(UOdysseyPaletteSet* PaletteSet)
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        Painting3DComponent->RemovePaletteSet( PaletteSet );
    }
}

void
SArianeEditorColorSelectorPanel::OnPaletteSetChanged(FGuid iIndex, UOdysseyPaletteSet* PaletteSet )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        Painting3DComponent->SetPaletteSet(iIndex, PaletteSet );
    }
}


void
SArianeEditorColorSelectorPanel::OnPaletteCurrentColorEntryChanged(UOdysseyPaletteEntryColor* Entry, FGuid Set )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        Painting3DComponent->SetCurrentPaletteColorEntry( Entry, Set );
    }
}

#undef LOCTEXT_NAMESPACE
