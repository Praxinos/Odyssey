// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "SOdysseyTextureConfigureWindow.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SSeparator.h"

/////////////////////////////////////////////////////
// Defines
#define LOCTEXT_NAMESPACE "OdysseyTextureFactory"

#define MAX_CANVAS_SIZE 8192
#define MIN_CANVAS_SIZE 1
#define DEFAULT_CANVAS_SIZE 1024

//---

void
SOdysseyTextureConfigureWindow::Construct( const FArguments& iArgs )
{
    mWidth = DEFAULT_CANVAS_SIZE;
    mHeight = DEFAULT_CANVAS_SIZE;
    mName = LOCTEXT( "default-name", "T_Drawing" );

    mFormat = TSF_BGRA8;
    mAllFormats.Add( MakeShared< ETextureSourceFormat >( TSF_G8 ) );
    mAllFormats.Add( MakeShared< ETextureSourceFormat >( TSF_BGRA8 ) );
    mAllFormats.Add( MakeShared< ETextureSourceFormat >( TSF_BGRE8 ) );
    mAllFormats.Add( MakeShared< ETextureSourceFormat >( TSF_RGBA16 ) );
    mAllFormats.Add( MakeShared< ETextureSourceFormat >( TSF_RGBA16F ) );
    //mAllFormats.Add( MakeShared< TPair<ETextureSourceFormat, FText> >( TSF_G16, FText::FromString( "Grey 16 (unsued?)" ) ) );

    mBackgroundColor = kTransparent;
        
    mWindowAnswer = false;

    //---

    SWindow::Construct( SWindow::FArguments()
        .Title( LOCTEXT( "title", "Create Texture Asset" ) )
        // Remove the empty space at the bottom but doesn't scale anymore
        //.SizingRule( ESizingRule::FixedSize )
        //.ClientSize( FVector2D( 300, 180 ) )
        .SizingRule( ESizingRule::Autosized )
        .SupportsMinimize( false )
        .SupportsMaximize( false )
        [
            SNew( SBorder )
            .BorderImage( FEditorStyle::GetBrush( "Menu.Background" ) )
            [
                SNew( SVerticalBox )

                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( 2, 2 )
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    [
                        SNew( STextBlock )
                        .Text( LOCTEXT( "name-label", "Name" ) )
                    ]
                    +SHorizontalBox::Slot()
                    .FillWidth( 2 )
                    [
                        SNew( SEditableTextBox )
                        .Text( this, &SOdysseyTextureConfigureWindow::GetDefaultName )
                        .OnTextCommitted( this, &SOdysseyTextureConfigureWindow::OnSetName )
                        .OnTextChanged( this, &SOdysseyTextureConfigureWindow::OnChangeName )
                    ]
                ]

                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( 2, 2 )
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    [
                        SNew( STextBlock )
                        .Text( LOCTEXT( "width-label", "Width" ) )
                    ]
                    +SHorizontalBox::Slot()
                    .FillWidth( 2 )
                    .HAlign( EHorizontalAlignment::HAlign_Left )
                    [
                        SNew( SSpinBox<int32> )
                        .MinDesiredWidth( 50.f )
                        .Value( this, &SOdysseyTextureConfigureWindow::GetWidth )
                        .MinValue( MIN_CANVAS_SIZE )
                        .MaxValue( MAX_CANVAS_SIZE )
                        .OnValueCommitted( this, &SOdysseyTextureConfigureWindow::OnSetWidth )
                        .OnValueChanged( this, &SOdysseyTextureConfigureWindow::OnChangeWidth )
                    ]
                ]

                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( 2, 2 )
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    [
                        SNew( STextBlock )
                        .Text( LOCTEXT( "height-label", "Height" ) )
                    ]
                    +SHorizontalBox::Slot()
                    .FillWidth( 2 )
                    .HAlign( EHorizontalAlignment::HAlign_Left )
                    [
                        SNew( SSpinBox<int32> )
                        .MinDesiredWidth( 50.f )
                        .Value( this, &SOdysseyTextureConfigureWindow::GetHeight )
                        .MinValue( MIN_CANVAS_SIZE )
                        .MaxValue( MAX_CANVAS_SIZE )
                        .OnValueCommitted( this, &SOdysseyTextureConfigureWindow::OnSetHeight )
                        .OnValueChanged( this, &SOdysseyTextureConfigureWindow::OnChangeHeight )
                    ]
                ]

                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( 2, 2 )
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    [
                        SNew( STextBlock )
                        .Text( LOCTEXT( "format-label", "Format" ) )
                    ]
                    +SHorizontalBox::Slot()
                    .FillWidth( 2 )
                    [
                        SAssignNew( mFormatComboBox, SComboBox<TSharedPtr<ETextureSourceFormat>> )
                        .OptionsSource( &mAllFormats )
                        .OnGenerateWidget( this, &SOdysseyTextureConfigureWindow::GenerateFormatComboBoxItem )
                        .OnSelectionChanged( this, &SOdysseyTextureConfigureWindow::HandleOnFormatChanged )
                        [
                            SNew( STextBlock )
                            .Text( this, &SOdysseyTextureConfigureWindow::GetFormatText )
                        ]
                    ]
                ]

                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( 2, 2 )
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    [
                        SNew( STextBlock )
                        .Text( LOCTEXT( "background-label", "Color" ) )
                    ]
                    +SHorizontalBox::Slot()
                    .FillWidth( 2 )
                    [
                        SNew( SVerticalBox )

                        +SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew( SCheckBox )
                            .Style( FEditorStyle::Get(), "RadioButton" )
                            .IsChecked( this, &SOdysseyTextureConfigureWindow::IsBackgroundColorRadioChecked, EBackgroundColor::kTransparent )
                            .OnCheckStateChanged( this, &SOdysseyTextureConfigureWindow::OnBackgroundColorRadioChanged, EBackgroundColor::kTransparent )
                            [
                                SNew(STextBlock)
                                .Text( GetBackgroundColorText( EBackgroundColor::kTransparent ) )
                            ]
                        ]

                        +SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew( SCheckBox )
                            .Style( FEditorStyle::Get(), "RadioButton" )
                            .IsChecked( this, &SOdysseyTextureConfigureWindow::IsBackgroundColorRadioChecked, EBackgroundColor::kWhite )
                            .OnCheckStateChanged( this, &SOdysseyTextureConfigureWindow::OnBackgroundColorRadioChanged, EBackgroundColor::kWhite )
                            [
                                SNew(STextBlock)
                                .Text( GetBackgroundColorText( EBackgroundColor::kWhite ) )
                            ]
                        ]

                        +SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew( SCheckBox )
                            .Style( FEditorStyle::Get(), "RadioButton" )
                            .IsChecked( this, &SOdysseyTextureConfigureWindow::IsBackgroundColorRadioChecked, EBackgroundColor::kNormal )
                            .OnCheckStateChanged( this, &SOdysseyTextureConfigureWindow::OnBackgroundColorRadioChanged, EBackgroundColor::kNormal )
                            [
                                SNew(STextBlock)
                                .Text( GetBackgroundColorText( EBackgroundColor::kNormal ) )
                            ]
                        ]
                    ]

                    // Maybe add it to have a custom background color through a 'custom' entry in the previous popup

                    //+SHorizontalBox::Slot()
                    //[
                    //    SNew( SColorBlock )
                    //    .Color( mColor )
                    //    .OnMouseButtonDown( this, &SOdysseyTextureConfigureWindow::OnColorClick )
                    //]
                ]

                +SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    [
                        SNew( SSeparator )
                    ]
                ]

                // From SColorPicker.cpp [OK] [Cancel]
                +SVerticalBox::Slot()
                .AutoHeight()
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                [
                    SNew( SUniformGridPanel )
                    .MinDesiredSlotHeight( FCoreStyle::Get().GetFloat( "StandardDialog.MinDesiredSlotHeight" ) )
                    .MinDesiredSlotWidth( FCoreStyle::Get().GetFloat( "StandardDialog.MinDesiredSlotWidth" ) )
                    .SlotPadding( FCoreStyle::Get().GetMargin( "StandardDialog.SlotPadding" ) )

                    +SUniformGridPanel::Slot( 0, 0 )
                    [
                        SNew( SButton )
                        .ContentPadding( FCoreStyle::Get().GetMargin( "StandardDialog.ContentPadding" ) )
                        .HAlign( HAlign_Center )
                        .Text( LOCTEXT( "create-asset", "Create Asset" ) )
                        .OnClicked_Raw( this, &SOdysseyTextureConfigureWindow::OnAccept )
                    ]

                    +SUniformGridPanel::Slot( 1, 0 )
                    [
                        SNew( SButton )
                        .ContentPadding( FCoreStyle::Get().GetMargin( "StandardDialog.ContentPadding" ) )
                        .HAlign( HAlign_Center )
                        .Text( LOCTEXT( "cancel", "Cancel" ) )
                        .OnClicked_Raw( this, &SOdysseyTextureConfigureWindow::OnCancel )
                    ]
                ]
            ]
        ]
    );

    mFormatComboBox->SetSelectedItem( *mAllFormats.FindByPredicate( [this]( const TSharedPtr<ETextureSourceFormat> iFormat )
    {
        return mFormat == *iFormat;
    } ) );
}

//---

bool
SOdysseyTextureConfigureWindow::GetWindowAnswer()
{
    return mWindowAnswer;
}

int32
SOdysseyTextureConfigureWindow::GetWidth() const
{
    return mWidth;
}

int32
SOdysseyTextureConfigureWindow::GetHeight() const
{
    return mHeight;
}

ETextureSourceFormat
SOdysseyTextureConfigureWindow::GetFormat() const
{
    return mFormat;
}

FText
SOdysseyTextureConfigureWindow::GetDefaultName() const
{
    return mName;
}

FLinearColor
SOdysseyTextureConfigureWindow::GetBackgroundColor() const
{
    switch( mBackgroundColor )
    {
        default:
        case kTransparent:  return FLinearColor( 0.f, 0.f, 0.f, 0.f );
        case kWhite:        return FLinearColor( 1.f, 1.f, 1.f );
        case kNormal:       return FLinearColor( .5f, .5f, 1.f );
    }
}

//---

void
SOdysseyTextureConfigureWindow::OnSetWidth( int32 iNewWidthValue, ETextCommit::Type iCommitInfo )
{
    mWidth = iNewWidthValue;

    if( mWidth > MAX_CANVAS_SIZE )
        mWidth = MAX_CANVAS_SIZE;
    if( mWidth < MIN_CANVAS_SIZE )
        mWidth = MIN_CANVAS_SIZE;
}
void
SOdysseyTextureConfigureWindow::OnChangeWidth( int32 iNewWidthValue )
{
    mWidth = iNewWidthValue;

    if( mWidth > MAX_CANVAS_SIZE )
        mWidth = MAX_CANVAS_SIZE;
    if( mWidth < MIN_CANVAS_SIZE )
        mWidth = MIN_CANVAS_SIZE;
}

void
SOdysseyTextureConfigureWindow::OnSetHeight( int32 iNewHeightValue, ETextCommit::Type iCommitInfo)
{
    mHeight = iNewHeightValue;

    if( mHeight > MAX_CANVAS_SIZE )
        mHeight = MAX_CANVAS_SIZE;
    if( mHeight < MIN_CANVAS_SIZE )
        mHeight = MIN_CANVAS_SIZE;
}
void
SOdysseyTextureConfigureWindow::OnChangeHeight( int32 iNewHeightValue )
{
    mHeight = iNewHeightValue;

    if( mHeight > MAX_CANVAS_SIZE )
        mHeight = MAX_CANVAS_SIZE;
    if( mHeight < MIN_CANVAS_SIZE )
        mHeight = MIN_CANVAS_SIZE;
}

void
SOdysseyTextureConfigureWindow::OnSetName( const FText& iNewNameValue, ETextCommit::Type iCommitInfo )
{
    mName = iNewNameValue;
}
void
SOdysseyTextureConfigureWindow::OnChangeName( const FText& iNewNameValue )
{
    mName = iNewNameValue;
}

//---

FText
SOdysseyTextureConfigureWindow::GetFormatText() const
{
    return GetFormatText( mFormatComboBox->GetSelectedItem() );
}

FText
SOdysseyTextureConfigureWindow::GetFormatText( TSharedPtr<ETextureSourceFormat> iFormat ) const
{
    switch( *iFormat )
    {
        case TSF_G8:        return LOCTEXT( "g8", "Grey 8" );
        default:
        case TSF_BGRA8:     return LOCTEXT( "bgra8", "BGRA 8" );
        case TSF_BGRE8:     return LOCTEXT( "bgre8", "BGRE 8" );
        case TSF_RGBA16:    return LOCTEXT( "rgba16", "RGBA 16" );
        case TSF_RGBA16F:   return LOCTEXT( "rgba16f", "RGBA 16 F" );
    }
}

TSharedRef<SWidget>
SOdysseyTextureConfigureWindow::GenerateFormatComboBoxItem( TSharedPtr<ETextureSourceFormat> InItem )
{
	return  SNew(STextBlock)
            .Text( GetFormatText( InItem ) );
}

void
SOdysseyTextureConfigureWindow::HandleOnFormatChanged( TSharedPtr<ETextureSourceFormat> NewSelection, ESelectInfo::Type SelectInfo )
{
    mFormat = *NewSelection;
}

//---

FText
SOdysseyTextureConfigureWindow::GetBackgroundColorText( EBackgroundColor iBackgroundColor ) const
{
    switch( iBackgroundColor )
    {
        default:
        case kTransparent:  return LOCTEXT( "background-color-transparent", "Transparent" );
        case kWhite:        return LOCTEXT( "background-color-white", "White" );
        case kNormal:       return LOCTEXT( "background-color-normal", "Purple (127, 127, 255)" );
    }
}

//FReply
//SOdysseyTextureConfigureWindow::OnColorClick( const FGeometry& iGeometry, const FPointerEvent& iEvent )
//{
//    FColorPickerArgs args;
//    args.bIsModal = true;
//    args.OnColorCommitted.BindLambda( []( const FLinearColor& iColor ) {} );
//
//    OpenColorPicker( args );
//
//    return FReply::Handled();
//}

ECheckBoxState
SOdysseyTextureConfigureWindow::IsBackgroundColorRadioChecked( EBackgroundColor iBackgroundColor ) const
{
    return ( mBackgroundColor == iBackgroundColor ) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyTextureConfigureWindow::OnBackgroundColorRadioChanged( ECheckBoxState iCheckType, EBackgroundColor iBackgroundColor )
{
    mBackgroundColor = iBackgroundColor;
}

//---

FReply
SOdysseyTextureConfigureWindow::OnAccept()
{
    mWindowAnswer = true;
    RequestDestroyWindow();

    return FReply::Handled();
}

FReply
SOdysseyTextureConfigureWindow::OnCancel()
{
    mWindowAnswer = false;
    RequestDestroyWindow();

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
