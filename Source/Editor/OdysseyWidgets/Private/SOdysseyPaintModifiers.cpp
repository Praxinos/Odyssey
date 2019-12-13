// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyPaintModifiers.h"

#define LOCTEXT_NAMESPACE "OdysseyPaintModifiers"


/////////////////////////////////////////////////////
// SOdysseyPaintModifiers
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyPaintModifiers::Construct( const FArguments& InArgs )
{
    mOnSizeChangedCallback      = InArgs._OnSizeChanged;
    mOnOpacityChangedCallback   = InArgs._OnOpacityChanged;
    mOnFlowChangedCallback      = InArgs._OnFlowChanged;
    mOnBlendingModeChangedCallback = InArgs._OnBlendingModeChanged;
    mCurrentBlendingMode = ::ULIS::eBlendingMode::kNormal;
    mBlendingModes = GetBlendingModesAsText();

    mOnAlphaModeChangedCallback = InArgs._OnAlphaModeChanged;
    mCurrentAlphaMode = ::ULIS::eAlphaMode::kNormal;
    mAlphaModes = GetAlphaModesAsText();

    ChildSlot
    [
        SNew( SBox )
            .HeightOverride( 25 )
            [
                SNew( SHorizontalBox )

                +SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign( VAlign_Center )
                    [
                        SNew( SBox )
                        .WidthOverride( 50 )
                        .HAlign( HAlign_Center )
                        [
                            SNew( STextBlock )
                            .Text( LOCTEXT( "Size", "Size:" ) )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew( SBox )
                        .WidthOverride( 100 )
                        [
                            SAssignNew( mSizeSlider, SSlider )
                            .OnValueChanged( this, &SOdysseyPaintModifiers::HandleSizeSliderChanged )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding( 0.f, 2.f )
                    [
                        SNew( SBox )
                        .WidthOverride( 50 )
                        [
                            SAssignNew( mSizeSpinBox, SSpinBox< int > )
                            .MinValue( 1 )
                            .MaxValue( 2000 )
                            .OnValueChanged( this, &SOdysseyPaintModifiers::HandleSizeSpinBoxChanged )
                            .Delta( 1 )
                        ]
                    ]
                ]

                +SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign( VAlign_Center )
                    [
                        SNew( SBox )
                        .WidthOverride( 50 )
                        .HAlign( HAlign_Center )
                        [
                            SNew( STextBlock )
                            .Text( LOCTEXT( "Opacity", "Opacity:" ) )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew( SBox )
                        .WidthOverride( 100 )
                        [
                            SAssignNew( mOpacitySlider, SSlider )
                            .OnValueChanged( this, &SOdysseyPaintModifiers::HandleOpacitySliderChanged )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding( 0.f, 2.f )
                    [
                        SNew( SBox )
                        .WidthOverride( 50 )
                        [
                            SAssignNew( mOpacitySpinBox, SSpinBox< int > )
                            .MinValue( 0 )
                            .MaxValue( 100 )
                            .OnValueChanged( this, &SOdysseyPaintModifiers::HandleOpacitySpinBoxChanged )
                        ]
                    ]
                ]

                +SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign( VAlign_Center )
                    [
                        SNew( SBox )
                        .WidthOverride( 50 )
                        .HAlign( HAlign_Center )
                        [
                            SNew( STextBlock )
                            .Text( LOCTEXT( "Flow", "Flow:" ) )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew( SBox )
                        .WidthOverride( 100 )
                        [
                            SAssignNew( mFlowSlider, SSlider )
                            .OnValueChanged( this, &SOdysseyPaintModifiers::HandleFlowSliderChanged )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding( 0.f, 2.f )
                    [
                        SNew( SBox )
                        .WidthOverride( 50 )
                        [
                            SAssignNew( mFlowSpinBox, SSpinBox< int > )
                            .MinValue( 0 )
                            .MaxValue( 100 )
                            .OnValueChanged( this, &SOdysseyPaintModifiers::HandleFlowSpinBoxChanged )
                        ]
                    ]
                ]

                +SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding( 0.f, 2.f )
                    .VAlign( VAlign_Center )
                    [
                        SNew( SBox )
                        .WidthOverride( 50 )
                        .HAlign( HAlign_Center )
                        [
                            SNew( STextBlock )
                            .Text( LOCTEXT( "Blend", "Blend:" ) )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding( 0.f, 2.f )
                    .VAlign( VAlign_Center )
                    [
                        SNew( SBox )
                        .WidthOverride( 100 )
                        .HAlign( HAlign_Fill )
                        [
                            SAssignNew( mBlendingModeComboBox, SComboBox<TSharedPtr<FText>>)
                            .OptionsSource(&mBlendingModes)
                            .OnGenerateWidget(this, &SOdysseyPaintModifiers::GenerateBlendingComboBoxItem)
                            .OnSelectionChanged(this, &SOdysseyPaintModifiers::HandleOnBlendingModeChanged )
                            .Content()
                            [
                                CreateBlendingModeTextWidget( mBlendingModes[0] )
                            ]
                        ]
                    ]
                ]


                +SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding( 0.f, 2.f )
                    .VAlign( VAlign_Center )
                    [
                        SNew( SBox )
                        .WidthOverride( 50 )
                        .HAlign( HAlign_Center )
                        [
                            SNew( STextBlock )
                            .Text( LOCTEXT( "Alpha", "Alpha:" ) )
                        ]
                    ]
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding( 0.f, 2.f )
                    .VAlign( VAlign_Center )
                    [
                        SNew( SBox )
                        .WidthOverride( 100 )
                        .HAlign( HAlign_Fill )
                        [
                            SAssignNew( mAlphaModeComboBox, SComboBox<TSharedPtr<FText>>)
                            .IsFocusable( false )
                            .OptionsSource(&mAlphaModes)
                            .OnGenerateWidget(this, &SOdysseyPaintModifiers::GenerateAlphaComboBoxItem)
                            .OnSelectionChanged(this, &SOdysseyPaintModifiers::HandleOnAlphaModeChanged )
                            .Content()
                            [
                                CreateAlphaModeTextWidget( mAlphaModes[0] )
                            ]
                        ]
                    ]
                ]

            ]
    ];
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Public Callbacks
void
SOdysseyPaintModifiers::SetSize( int32 iValue )
{
    mSizeSpinBox->SetValue( iValue );
}


void
SOdysseyPaintModifiers::SetOpacity( int32 iValue )
{
    mOpacitySpinBox->SetValue( iValue );
}


void
SOdysseyPaintModifiers::SetFlow( int32 iValue )
{
    mFlowSpinBox->SetValue( iValue );
}


void
SOdysseyPaintModifiers::SetBlendingMode( ::ULIS::eBlendingMode iValue )
{
    mCurrentBlendingMode = iValue;
    TSharedPtr< FText > sel = MakeShared< FText >( GetBlendingModeAsText() );
    HandleOnBlendingModeChanged( sel, ESelectInfo::Direct );
}

void
SOdysseyPaintModifiers::SetAlphaMode( ::ULIS::eAlphaMode iValue )
{
    mCurrentAlphaMode = iValue;
    TSharedPtr< FText > sel = MakeShared< FText >( GetAlphaModeAsText() );
    HandleOnAlphaModeChanged( sel, ESelectInfo::Direct );
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks
void
SOdysseyPaintModifiers::HandleSizeSliderChanged( float iValue )
{
    int32 value = FMath::Pow( iValue, 3.f ) * mSizeSpinBox->GetMaxValue() + 1;
    int32 spinvalue = mSizeSpinBox->GetValue();
    if( value != spinvalue )
    {
        mSizeSpinBox->SetValue( value );
        mOnSizeChangedCallback.ExecuteIfBound( value );
    }
}


void
SOdysseyPaintModifiers::HandleOpacitySliderChanged( float iValue )
{
    int32 value = iValue * mOpacitySpinBox->GetMaxValue();
    int32 spinvalue = mOpacitySpinBox->GetValue();
    if( value != spinvalue )
    {
        mOpacitySpinBox->SetValue( value );
        mOnOpacityChangedCallback.ExecuteIfBound( value );
    }
}


void
SOdysseyPaintModifiers::HandleFlowSliderChanged( float iValue )
{
    int32 value = iValue * mFlowSpinBox->GetMaxValue();
    int32 spinvalue = mFlowSpinBox->GetValue();
    if( value != spinvalue )
    {
        mFlowSpinBox->SetValue( value );
        mOnFlowChangedCallback.ExecuteIfBound( value );
    }
}


void
SOdysseyPaintModifiers::HandleSizeSpinBoxChanged( int32 iValue )
{
    int32 value = iValue - 1;
    auto  slider_value      = mSizeSlider->GetValue();          // Actual slider value
    auto  corrected_value   = FMath::Pow( slider_value, 3.f );  // Corrected slider value
    int32 display_value     = corrected_value * mSizeSpinBox->GetMaxValue();

    if( value != display_value )
    {
        mSizeSlider->SetValue( FMath::Pow( float( value ) / mSizeSpinBox->GetMaxValue(), 1.f / 3.f ) );
        mOnSizeChangedCallback.ExecuteIfBound( iValue );
    }
}


void
SOdysseyPaintModifiers::HandleOpacitySpinBoxChanged( int32 iValue )
{
    int32 value = iValue;
    int32 slideValue = mOpacitySlider->GetValue() * mOpacitySpinBox->GetMaxValue();
    if( value != slideValue )
    {
        mOpacitySlider->SetValue( float( value ) / mOpacitySpinBox->GetMaxValue() );
        mOnOpacityChangedCallback.ExecuteIfBound( value );
    }
}


void
SOdysseyPaintModifiers::HandleFlowSpinBoxChanged( int32 iValue )
{
    int32 value = iValue;
    int32 slideValue = mFlowSlider->GetValue() * mFlowSpinBox->GetMaxValue();
    if( value != slideValue )
    {
        mFlowSlider->SetValue( float( value ) / mFlowSpinBox->GetMaxValue() );
        mOnFlowChangedCallback.ExecuteIfBound( value );
    }
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------- Blending mode Callbacks
TSharedRef<SWidget>
SOdysseyPaintModifiers::GenerateBlendingComboBoxItem( TSharedPtr<FText> InItem )
{
    return SNew(STextBlock)
           .Text(*(InItem.Get()));
}


TSharedRef<SWidget>
SOdysseyPaintModifiers::CreateBlendingModeTextWidget( TSharedPtr<FText> InItem )
{
    return SNew(STextBlock)
           .Text(*(InItem.Get()));
}


void
SOdysseyPaintModifiers::HandleOnBlendingModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo )
{
    mBlendingModeComboBox->SetContent(
        SAssignNew( mBlendingBox, SComboBox<TSharedPtr<FText>> )
                .OptionsSource(&mBlendingModes)
                .OnGenerateWidget(this, &SOdysseyPaintModifiers::GenerateBlendingComboBoxItem)
                .OnSelectionChanged(this, &SOdysseyPaintModifiers::HandleOnBlendingModeChanged )
                .Content()
                [
                    CreateBlendingModeTextWidget( NewSelection )
                ]
    );


    for( uint8 i = 0; i < (int)::ULIS::eBlendingMode::kNumBlendingModes; ++i )
    {
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendingMode[ i ] ) );
        if( NewSelection.Get()->EqualTo( entry ) )
        {
            mCurrentBlendingMode = static_cast<::ULIS::eBlendingMode>( i );
        }
    }

    mOnBlendingModeChangedCallback.ExecuteIfBound( (int32)mCurrentBlendingMode );
}

TArray< TSharedPtr< FText > >
SOdysseyPaintModifiers::GetBlendingModesAsText()
{
    TArray< TSharedPtr< FText > > array;
    for( int i = 0; i < (int)::ULIS::eBlendingMode::kNumBlendingModes; ++i )
        array.Add( MakeShared< FText >( FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendingMode[i] ) ) ) );
    return array;
}


FText
SOdysseyPaintModifiers::GetBlendingModeAsText() const
{
    return  FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendingMode[ static_cast< int >( mCurrentBlendingMode ) ] ) );
}



//--------------------------------------------------------------------------------------
//-------------------------------------------------------------- Alpha mode Callbacks
TSharedRef<SWidget>
SOdysseyPaintModifiers::GenerateAlphaComboBoxItem( TSharedPtr<FText> InItem )
{
    return SNew(STextBlock)
           .Text(*(InItem.Get()));
}


TSharedRef<SWidget>
SOdysseyPaintModifiers::CreateAlphaModeTextWidget( TSharedPtr<FText> InItem )
{
    return SNew(STextBlock)
           .Text(*(InItem.Get()));
}


void
SOdysseyPaintModifiers::HandleOnAlphaModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo )
{
    mAlphaModeComboBox->SetContent(
        SAssignNew( mAlphaBox, SComboBox<TSharedPtr<FText>> )
                .OptionsSource(&mAlphaModes)
                .OnGenerateWidget(this, &SOdysseyPaintModifiers::GenerateAlphaComboBoxItem)
                .OnSelectionChanged(this, &SOdysseyPaintModifiers::HandleOnAlphaModeChanged )
                .Content()
                [
                    CreateAlphaModeTextWidget( NewSelection )
                ]
    );


    for( uint8 i = 0; i < (int)::ULIS::eAlphaMode::kNumAlphaModes; ++i )
    {
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwAlphaMode[ i ] ) );
        if( NewSelection.Get()->EqualTo( entry ) )
        {
            mCurrentAlphaMode = static_cast<::ULIS::eAlphaMode>( i );
        }
    }

    mOnAlphaModeChangedCallback.ExecuteIfBound( (int32)mCurrentAlphaMode );
}

TArray< TSharedPtr< FText > >
SOdysseyPaintModifiers::GetAlphaModesAsText()
{
    TArray< TSharedPtr< FText > > array;
    for( int i = 0; i < (int)::ULIS::eAlphaMode::kNumAlphaModes; ++i )
        array.Add( MakeShared< FText >( FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwAlphaMode[i] ) ) ) );
    return array;
}


FText
SOdysseyPaintModifiers::GetAlphaModeAsText() const
{
    return  FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwAlphaMode[ static_cast< int >( mCurrentAlphaMode ) ] ) );
}


#undef LOCTEXT_NAMESPACE

