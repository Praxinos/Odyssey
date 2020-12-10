// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    mCurrentBlendingMode = ::ul3::BM_NORMAL;
    mBlendingModes = GetBlendingModesAsText();

    mOnAlphaModeChangedCallback = InArgs._OnAlphaModeChanged;
    mCurrentAlphaMode = ::ul3::AM_NORMAL;
    mAlphaModes = GetAlphaModesAsText();

    if( InArgs._VerticalAspect.Get())
    {
        ChildSlot
        [
            SNew( SBox )
                [
                    SNew( SVerticalBox )

                    +SVerticalBox::Slot()
                    .Padding( FMargin( 2.f, 3.f ) )
                    [
                        SNew( SHorizontalBox )
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Left )
                            [
                                SNew( STextBlock )
                                .Text( LOCTEXT( "Size", "Size:" ) )
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .VAlign( VAlign_Center )
                        .Padding( 6.f, 0.f )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Fill )
                            [
                                SAssignNew( mSizeSpinBox, SSpinBox< int > )
                                //.Style(&FOdysseyStyle::GetWidgetStyle<FSpinBoxStyle>("OdysseySpinBoxStyle.DarkSpinBox"))
                                .MinValue( 1 )
                                .MaxValue( 1000 )
                                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleSizeSpinBoxChanged )
                                .Delta( 1 )
                            ]
                        ]
                    ]

                    +SVerticalBox::Slot()
                    .Padding(FMargin(2.f,3.f))
                    [
                        SNew( SHorizontalBox )
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Left )
                            [
                                SNew( STextBlock )
                                .Text( LOCTEXT( "Opacity", "Opacity:" ) )
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .VAlign( VAlign_Center )
                        .Padding( 6.f, 0.f )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Fill )
                            [
                                SAssignNew( mOpacitySpinBox, SSpinBox< int > )
                                //.Style(&FOdysseyStyle::GetWidgetStyle<FSpinBoxStyle>("OdysseySpinBoxStyle.DarkSpinBox"))
                                .MinValue( 0 )
                                .MaxValue( 100 )
                                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleOpacitySpinBoxChanged )
                            ]
                        ]
                    ]

                    +SVerticalBox::Slot()
                    .Padding(FMargin(2.f,3.f))
                    [
                        SNew( SHorizontalBox )
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Left )
                            [
                                SNew( STextBlock )
                                .Text( LOCTEXT( "Flow", "Flow:" ) )
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .VAlign( VAlign_Center )
                        .Padding( 6.f, 0.f )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Fill )
                            [
                                SAssignNew( mFlowSpinBox, SSpinBox< int > )
                                //.Style(&FOdysseyStyle::GetWidgetStyle<FSpinBoxStyle>("OdysseySpinBoxStyle.DarkSpinBox"))
                                .MinValue( 0 )
                                .MaxValue( 100 )
                                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleFlowSpinBoxChanged )
                            ]
                        ]
                    ]

                    +SVerticalBox::Slot()
                    .Padding(FMargin(2.f,3.f))
                    [
                        SNew( SHorizontalBox )
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Left )
                            [
                                SNew( STextBlock )
                                .Text( LOCTEXT( "Blend", "Blend:" ) )
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .VAlign( VAlign_Center )
                        .Padding( 6.f, 0.f )
                        [
                            SNew( SBox )
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


                    +SVerticalBox::Slot()
                    .Padding(FMargin(2.f,3.f))
                    [
                        SNew( SHorizontalBox )
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Left )
                            [
                                SNew( STextBlock )
                                .Text( LOCTEXT( "Alpha", "Alpha:" ) )
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .VAlign( VAlign_Center )
                        .Padding( 6.f, 0.f )
                        [
                            SNew( SBox )
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
    else
    {
        ChildSlot
        [
            SNew( SBox )
                .HeightOverride( 25 )
                [
                    SNew( SHorizontalBox )

                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Fill )
                    [
                        SNew( SHorizontalBox )
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Left )
                            [
                                SNew( STextBlock )
                                .Text( LOCTEXT( "Size", "Size:" ) )
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .VAlign( VAlign_Center )
                        .Padding( 6.f, 0.f )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Fill )
                            [
                                SAssignNew( mSizeSpinBox, SSpinBox< int > )
                                //.Style(&FOdysseyStyle::GetWidgetStyle<FSpinBoxStyle>("OdysseySpinBoxStyle.DarkSpinBox"))
                                .MinValue( 1 )
                                .MaxValue( 1000 )
                                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleSizeSpinBoxChanged )
                                .Delta( 1 )
                            ]
                        ]
                    ]

                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Fill )
                    [
                        SNew( SHorizontalBox )
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Left )
                            [
                                SNew( STextBlock )
                                .Text( LOCTEXT( "Opacity", "Opacity:" ) )
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .VAlign( VAlign_Center )
                        .Padding( 6.f, 0.f )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Fill )
                            [
                                SAssignNew( mOpacitySpinBox, SSpinBox< int > )
                                //.Style(&FOdysseyStyle::GetWidgetStyle<FSpinBoxStyle>("OdysseySpinBoxStyle.DarkSpinBox"))
                                .MinValue( 0 )
                                .MaxValue( 100 )
                                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleOpacitySpinBoxChanged )
                            ]
                        ]
                    ]

                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Fill )
                    [
                        SNew( SHorizontalBox )
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Left )
                            [
                                SNew( STextBlock )
                                .Text( LOCTEXT( "Flow", "Flow:" ) )
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .VAlign( VAlign_Center )
                        .Padding( 6.f, 0.f )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Fill )
                            [
                                SAssignNew( mFlowSpinBox, SSpinBox< int > )
                                //.Style(&FOdysseyStyle::GetWidgetStyle<FSpinBoxStyle>("OdysseySpinBoxStyle.DarkSpinBox"))
                                .MinValue( 0 )
                                .MaxValue( 100 )
                                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleFlowSpinBoxChanged )
                            ]
                        ]
                    ]

                    +SHorizontalBox::Slot()
                    .HAlign( HAlign_Fill )
                    [
                        SNew( SHorizontalBox )
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Left )
                            [
                                SNew( STextBlock )
                                .Text( LOCTEXT( "Blend", "Blend:" ) )
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .VAlign( VAlign_Center )
                        .Padding( 6.f, 0.f )
                        [
                            SNew( SBox )
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
                    .HAlign( HAlign_Fill )
                    [
                        SNew( SHorizontalBox )
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SNew( SBox )
                            .HAlign( HAlign_Left )
                            [
                                SNew( STextBlock )
                                .Text( LOCTEXT( "Alpha", "Alpha:" ) )
                            ]
                        ]
                        +SHorizontalBox::Slot()
                        .VAlign( VAlign_Center )
                        .Padding( 6.f, 0.f )
                        [
                            SNew( SBox )
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
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Public Callbacks
void
SOdysseyPaintModifiers::SetSize( int32 iValue )
{
    mSizeSpinBox->SetValue( iValue );
    mOnSizeChangedCallback.ExecuteIfBound( iValue );
}


void
SOdysseyPaintModifiers::SetOpacity( int32 iValue )
{
    mOpacitySpinBox->SetValue( iValue );
    mOnOpacityChangedCallback.ExecuteIfBound( iValue );
}


void
SOdysseyPaintModifiers::SetFlow( int32 iValue )
{
    mFlowSpinBox->SetValue( iValue );
    mOnFlowChangedCallback.ExecuteIfBound( iValue );
}


void
SOdysseyPaintModifiers::SetBlendingMode( ::ul3::eBlendingMode iValue )
{
    mCurrentBlendingMode = iValue;
    TSharedPtr< FText > sel = MakeShared< FText >( GetBlendingModeAsText() );
    HandleOnBlendingModeChanged( sel, ESelectInfo::Direct );
}

void
SOdysseyPaintModifiers::SetAlphaMode( ::ul3::eAlphaMode iValue )
{
    mCurrentAlphaMode = iValue;
    TSharedPtr< FText > sel = MakeShared< FText >( GetAlphaModeAsText() );
    HandleOnAlphaModeChanged( sel, ESelectInfo::Direct );
}

int32
SOdysseyPaintModifiers::GetSize()
{
    return mSizeSpinBox->GetValue();
}

int32
SOdysseyPaintModifiers::GetOpacity()
{
    return mOpacitySpinBox->GetValue();
}

int32
SOdysseyPaintModifiers::GetFlow()
{
    return mFlowSpinBox->GetValue();
}

::ul3::eBlendingMode
SOdysseyPaintModifiers::GetBlendingMode()
{
    return mCurrentBlendingMode;
}

::ul3::eAlphaMode
SOdysseyPaintModifiers::GetAlphaMode()
{
    return mCurrentAlphaMode;
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks

void
SOdysseyPaintModifiers::HandleSizeSpinBoxChanged( int32 iValue, ETextCommit::Type iType )
{
    mOnSizeChangedCallback.ExecuteIfBound( iValue );
}


void
SOdysseyPaintModifiers::HandleOpacitySpinBoxChanged( int32 iValue, ETextCommit::Type iType )
{
    mOnOpacityChangedCallback.ExecuteIfBound( iValue );
}


void
SOdysseyPaintModifiers::HandleFlowSpinBoxChanged( int32 iValue, ETextCommit::Type iType )
{
    mOnFlowChangedCallback.ExecuteIfBound( iValue );
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


    for( uint8 i = 0; i < (int)::ul3::NUM_BLENDING_MODES; ++i )
    {
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[ i ] ) );
        if( NewSelection.Get()->EqualTo( entry ) )
        {
            mCurrentBlendingMode = static_cast<::ul3::eBlendingMode>( i );
        }
    }

    mOnBlendingModeChangedCallback.ExecuteIfBound( (int32)mCurrentBlendingMode );
}

TArray< TSharedPtr< FText > >
SOdysseyPaintModifiers::GetBlendingModesAsText()
{
    TArray< TSharedPtr< FText > > array;
    for( int i = 0; i < (int)::ul3::NUM_BLENDING_MODES; ++i )
        array.Add( MakeShared< FText >( FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[i] ) ) ) );
    return array;
}


FText
SOdysseyPaintModifiers::GetBlendingModeAsText() const
{
    return  FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[ static_cast< int >( mCurrentBlendingMode ) ] ) );
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


    for( uint8 i = 0; i < (int)::ul3::NUM_ALPHA_MODES; ++i )
    {
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ul3::kwAlphaMode[ i ] ) );
        if( NewSelection.Get()->EqualTo( entry ) )
        {
            mCurrentAlphaMode = static_cast<::ul3::eAlphaMode>( i );
        }
    }

    mOnAlphaModeChangedCallback.ExecuteIfBound( (int32)mCurrentAlphaMode );
}

TArray< TSharedPtr< FText > >
SOdysseyPaintModifiers::GetAlphaModesAsText()
{
    TArray< TSharedPtr< FText > > array;
    for( int i = 0; i < (int)::ul3::NUM_ALPHA_MODES; ++i )
        array.Add( MakeShared< FText >( FText::FromString( ANSI_TO_TCHAR( ::ul3::kwAlphaMode[i] ) ) ) );
    return array;
}


FText
SOdysseyPaintModifiers::GetAlphaModeAsText() const
{
    return  FText::FromString( ANSI_TO_TCHAR( ::ul3::kwAlphaMode[ static_cast< int >( mCurrentAlphaMode ) ] ) );
}


#undef LOCTEXT_NAMESPACE

