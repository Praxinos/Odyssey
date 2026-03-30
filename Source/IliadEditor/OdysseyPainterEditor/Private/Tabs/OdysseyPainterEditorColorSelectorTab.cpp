// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorColorSelectorTab.h"

#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "Widgets/Color/SOdysseyAdvancedColorWheel.h"
#include "Widgets/Color/SOdysseyColorSliders.h"
#include "Widgets/Color/SOdysseyColorHexadecimal.h"
#include "Widgets/Palette/SOdysseyPainterEditorPaletteSetList.h"
#include "OdysseyStyle.h"
#include "Tools/OdysseyPainterEditorTool.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorColorSelectorTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_ColorSelector"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorColorSelectorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorColorSelectorTab::~FOdysseyPainterEditorColorSelectorTab()
{
}

FOdysseyPainterEditorColorSelectorTab::FOdysseyPainterEditorColorSelectorTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "color-selector-tab.name", "Color Selector" ), FSlateIcon( "OdysseyStyle", "PainterEditor.ColorWheel16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorColorSelectorTab::CreateWidget()
{
    return
        SNew(SScrollBox)
        +SScrollBox::Slot()
        .AutoSize()
        [
            SNew(SSegmentedControl<EOdysseyPainterEditorColorType>)
            .Visibility(this, &FOdysseyPainterEditorColorSelectorTab::GetColorTypeVisibility)
            .Value(this, &FOdysseyPainterEditorColorSelectorTab::GetColorType)
            .OnValueChecked(this, &FOdysseyPainterEditorColorSelectorTab::OnColorTypeChanged)
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
            .Visibility(this, &FOdysseyPainterEditorColorSelectorTab::GetRawColorWidgetsVisibility)
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SAssignNew(mColorWheelExpanderArrow, SButton)
                    .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                    .VAlign(VAlign_Center)
                    .HAlign(HAlign_Center)
                    .ClickMethod( EButtonClickMethod::MouseDown )
                    .OnClicked( this, &FOdysseyPainterEditorColorSelectorTab::OnColorWheelExpanderArrowClicked )
                    .ContentPadding(0.f)
                    .ForegroundColor( FSlateColor::UseForeground() )
                    .IsFocusable( false )
                    [
                        SNew(SImage)
                        .Image( this, &FOdysseyPainterEditorColorSelectorTab::GetColorWheelExpanderArrowImage )
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
                .Visibility(this, &FOdysseyPainterEditorColorSelectorTab::GetColorWheelVisibility)
                .MinDesiredWidth(   150 )
                .MinDesiredHeight(  150 )
                .MaxDesiredWidth(   200 )
                .MaxDesiredHeight(  200 )
                .DesiredWidth(   175 )
                .DesiredHeight(  175 )
                .Color(this, &FOdysseyPainterEditorColorSelectorTab::GetRawColor)
                .OnColorChanged(this, &FOdysseyPainterEditorColorSelectorTab::OnColorChanged)
            ]
        ]

        +SScrollBox::Slot()
        .AutoSize()
        [
            SNew(SVerticalBox)
            .Visibility(this, &FOdysseyPainterEditorColorSelectorTab::GetRawColorWidgetsVisibility)
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SAssignNew(mColorSlidersExpanderArrow, SButton)
                    .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                    .VAlign(VAlign_Center)
                    .HAlign(HAlign_Center)
                    .ClickMethod( EButtonClickMethod::MouseDown )
                    .OnClicked( this, &FOdysseyPainterEditorColorSelectorTab::OnColorSlidersExpanderArrowClicked )
                    .ContentPadding(0.f)
                    .ForegroundColor( FSlateColor::UseForeground() )
                    .IsFocusable( false )
                    [
                        SNew(SImage)
                        .Image( this, &FOdysseyPainterEditorColorSelectorTab::GetColorSlidersExpanderArrowImage )
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
                .Visibility(this, &FOdysseyPainterEditorColorSelectorTab::GetColorSlidersVisibility)
                .Color(this, &FOdysseyPainterEditorColorSelectorTab::GetRawColor)
                .OnColorChanged(this, &FOdysseyPainterEditorColorSelectorTab::OnColorChanged)
            ]
        ]
        +SScrollBox::Slot()
        .AutoSize()
        [
            SNew(SVerticalBox)
            .Visibility(this, &FOdysseyPainterEditorColorSelectorTab::GetRawColorWidgetsVisibility)
            +SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SAssignNew(mHexadecimalExpanderArrow, SButton)
                    .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                    .VAlign(VAlign_Center)
                    .HAlign(HAlign_Center)
                    .ClickMethod( EButtonClickMethod::MouseDown )
                    .OnClicked( this, &FOdysseyPainterEditorColorSelectorTab::OnHexadecimalExpanderArrowClicked )
                    .ContentPadding(0.f)
                    .ForegroundColor( FSlateColor::UseForeground() )
                    .IsFocusable( false )
                    [
                        SNew(SImage)
                        .Image( this, &FOdysseyPainterEditorColorSelectorTab::GetHexadecimalExpanderArrowImage )
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
                .Visibility(this, &FOdysseyPainterEditorColorSelectorTab::GetColorHexadecimalVisibility)
                .Color(this, &FOdysseyPainterEditorColorSelectorTab::GetRawColor)
                .OnColorChanged(this, &FOdysseyPainterEditorColorSelectorTab::OnColorChanged)
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
                    SAssignNew(mColorPaletteExpanderArrow, SButton)
                    .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                    .VAlign(VAlign_Center)
                    .HAlign(HAlign_Center)
                    .ClickMethod( EButtonClickMethod::MouseDown )
                    .OnClicked( this, &FOdysseyPainterEditorColorSelectorTab::OnColorPaletteExpanderArrowClicked )
                    .ContentPadding(0.f)
                    .ForegroundColor( FSlateColor::UseForeground() )
                    .IsFocusable( false )
                    [
                        SNew(SImage)
                        .Image( this, &FOdysseyPainterEditorColorSelectorTab::GetColorPaletteExpanderArrowImage )
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
                    SNew(SOdysseyPainterEditorPaletteSetList)
                    .Visibility(this, &FOdysseyPainterEditorColorSelectorTab::GetColorPaletteVisibility)
                    .PaletteSets(this, &FOdysseyPainterEditorColorSelectorTab::GetPaletteSets)
                    .CurrentColorEntry(this, &FOdysseyPainterEditorColorSelectorTab::GetCurrentPaletteColorEntry)
                    .CurrentSet(this, &FOdysseyPainterEditorColorSelectorTab::GetCurrentPaletteSet)
                    .ColorType( this, &FOdysseyPainterEditorColorSelectorTab::GetColorType )
                    .OnPaletteSetChanged(this, &FOdysseyPainterEditorColorSelectorTab::OnPaletteSetChanged)
                    .OnAddPaletteSet(this, &FOdysseyPainterEditorColorSelectorTab::OnAddPaletteSet)
                    .OnRemovePaletteSet(this, &FOdysseyPainterEditorColorSelectorTab::OnRemovePaletteSet)
                    .OnCurrentColorEntryChanged(this, &FOdysseyPainterEditorColorSelectorTab::OnPaletteCurrentColorEntryChanged)
                ]
            ]
        ];
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

const FName&
FOdysseyPainterEditorColorSelectorTab::GetId() const
{
    return StaticId();
}

::ULIS::FColor
FOdysseyPainterEditorColorSelectorTab::GetRawColor() const
{
    return mEditor->PaintColor().GetValue();
}

const FSlateBrush*
FOdysseyPainterEditorColorSelectorTab::GetExpanderArrowImage(TSharedPtr<SButton> iExpander, bool iIsExpanded) const
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

const FSlateBrush*
FOdysseyPainterEditorColorSelectorTab::GetColorWheelExpanderArrowImage() const
{
    return GetExpanderArrowImage(mColorWheelExpanderArrow, mIsColorWheelExpanded);
}

const FSlateBrush*
FOdysseyPainterEditorColorSelectorTab::GetColorSlidersExpanderArrowImage() const
{
    return GetExpanderArrowImage(mColorSlidersExpanderArrow, mIsColorSlidersExpanded);
}

const FSlateBrush*
FOdysseyPainterEditorColorSelectorTab::GetHexadecimalExpanderArrowImage() const
{
    return GetExpanderArrowImage(mHexadecimalExpanderArrow, mIsHexadecimalExpanded);
}const FSlateBrush*
FOdysseyPainterEditorColorSelectorTab::GetColorPaletteExpanderArrowImage() const
{
    return GetExpanderArrowImage(mColorPaletteExpanderArrow, mIsColorPaletteExpanded);
}

EOdysseyPainterEditorColorType
FOdysseyPainterEditorColorSelectorTab::GetColorType() const
{
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return EOdysseyPainterEditorColorType::Raw; //Only Raw Colors are displayed by default

    if (!tool->SupportsColorType(mEditor->GetColorType()))
        return EOdysseyPainterEditorColorType::Raw;

    return mEditor->GetColorType();
}

EVisibility
FOdysseyPainterEditorColorSelectorTab::GetColorTypeVisibility() const
{
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return EVisibility::Collapsed; //Only Raw Colors are displayed by default

    return tool->SupportsColorType(EOdysseyPainterEditorColorType::Indexed) ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
FOdysseyPainterEditorColorSelectorTab::GetRawColorWidgetsVisibility() const
{
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return EVisibility::Visible; //Only Raw Colors are displayed by default

    if (!tool->SupportsColorType(EOdysseyPainterEditorColorType::Indexed))
        return EVisibility::Visible; //Only Raw Colors are displayed by default

    return mEditor->GetColorType() == EOdysseyPainterEditorColorType::Raw ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
FOdysseyPainterEditorColorSelectorTab::GetColorWheelVisibility() const
{
    return mIsColorWheelExpanded ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
FOdysseyPainterEditorColorSelectorTab::GetColorSlidersVisibility() const
{
    return mIsColorSlidersExpanded ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
FOdysseyPainterEditorColorSelectorTab::GetColorHexadecimalVisibility() const
{
    return mIsHexadecimalExpanded ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
FOdysseyPainterEditorColorSelectorTab::GetColorPaletteVisibility() const
{
    return mIsColorPaletteExpanded ? EVisibility::Visible : EVisibility::Collapsed;
}


TArray<UOdysseyPaletteSet*>
FOdysseyPainterEditorColorSelectorTab::GetPaletteSets() const
{
    return mEditor->GetPaletteSets();
}

UOdysseyPaletteEntryColor*
FOdysseyPainterEditorColorSelectorTab::GetCurrentPaletteColorEntry() const
{
    return mEditor->GetCurrentPaletteColorEntry();
}

FGuid
FOdysseyPainterEditorColorSelectorTab::GetCurrentPaletteSet() const
{
    return mEditor->GetCurrentPaletteSet();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorColorSelectorTab::OnColorChanged( eOdysseyEventState::Type iEventState, const ::ULIS::FColor& iColor )
{
    mEditor->PaintColor( iColor, iEventState == eOdysseyEventState::kSet );
}

FReply
FOdysseyPainterEditorColorSelectorTab::OnHexadecimalExpanderArrowClicked()
{
    mIsHexadecimalExpanded = !mIsHexadecimalExpanded;
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorColorSelectorTab::OnColorSlidersExpanderArrowClicked()
{
    mIsColorSlidersExpanded = !mIsColorSlidersExpanded;
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorColorSelectorTab::OnColorWheelExpanderArrowClicked()
{
    mIsColorWheelExpanded = !mIsColorWheelExpanded;
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorColorSelectorTab::OnColorPaletteExpanderArrowClicked()
{
    mIsColorPaletteExpanded = !mIsColorPaletteExpanded;
    return FReply::Handled();
}

void
FOdysseyPainterEditorColorSelectorTab::OnColorTypeChanged(EOdysseyPainterEditorColorType iType, ECheckBoxState iState)
{
    if (iState == ECheckBoxState::Checked)
        mEditor->SetColorType(iType);
}

void
FOdysseyPainterEditorColorSelectorTab::OnAddPaletteSet(UOdysseyPalette* iPalette)
{
    mEditor->AddPaletteSet(iPalette);
}

void
FOdysseyPainterEditorColorSelectorTab::OnRemovePaletteSet(UOdysseyPaletteSet* iPaletteSet)
{
    mEditor->RemovePaletteSet(iPaletteSet);
}

void
FOdysseyPainterEditorColorSelectorTab::OnPaletteSetChanged(FGuid iIndex, UOdysseyPaletteSet* iPaletteSet)
{
    mEditor->SetPaletteSet(iIndex, iPaletteSet);
}


void
FOdysseyPainterEditorColorSelectorTab::OnPaletteCurrentColorEntryChanged(UOdysseyPaletteEntryColor* iEntry, FGuid iSet)
{
    mEditor->SetCurrentPaletteColorEntry(iEntry, iSet);
}

#undef LOCTEXT_NAMESPACE
