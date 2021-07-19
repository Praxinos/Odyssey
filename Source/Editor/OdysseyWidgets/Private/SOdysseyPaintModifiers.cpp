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
    mSize = InArgs._Size;
    mOpacity = InArgs._Opacity;
    mFlow = InArgs._Flow;
    mBlendingMode = InArgs._BlendingMode;
    mAlphaMode = InArgs._AlphaMode;

    mCurrentBlendingMode = mBlendingMode.Get();
    mCurrentAlphaMode = mAlphaMode.Get();

    mOnSizeChangedCallback      = InArgs._OnSizeChanged;
    mOnOpacityChangedCallback   = InArgs._OnOpacityChanged;
    mOnFlowChangedCallback      = InArgs._OnFlowChanged;
    mOnBlendingModeChangedCallback = InArgs._OnBlendingModeChanged;
    mBlendingModes = GetBlendingModesAsText();

    mOnAlphaModeChangedCallback = InArgs._OnAlphaModeChanged;
    mAlphaModes = GetAlphaModesAsText();

    if( InArgs._VerticalAspect)
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
                                SAssignNew( mSizeSpinBox, SSpinBox< float > )
                                //.Style(&FOdysseyStyle::GetWidgetStyle<FSpinBoxStyle>("OdysseySpinBoxStyle.DarkSpinBox"))
                                //.Value(mSize)
                                .Value( this, &SOdysseyPaintModifiers::GetSize )
                                .MaxFractionalDigits(0)
                                .MinValue( 1 )
                                .MaxValue( 1000 )
                                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleSizeSpinBoxChanged )
                                .OnValueChanged( this, &SOdysseyPaintModifiers::SetSize )
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
                                SAssignNew( mOpacitySpinBox, SSpinBox< float > )
                                //.Style(&FOdysseyStyle::GetWidgetStyle<FSpinBoxStyle>("OdysseySpinBoxStyle.DarkSpinBox"))
                                //.Value(mOpacity)
                                .Value(this, &SOdysseyPaintModifiers::GetOpacity )
                                .MaxFractionalDigits(0)
                                .MinValue( 0 )
                                .MaxValue( 100 )
                                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleOpacitySpinBoxChanged )
                                .OnValueChanged( this, &SOdysseyPaintModifiers::SetOpacity )
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
                                SAssignNew( mFlowSpinBox, SSpinBox< float > )
                                //.Style(&FOdysseyStyle::GetWidgetStyle<FSpinBoxStyle>("OdysseySpinBoxStyle.DarkSpinBox"))
                                //.Value(mFlow)
                                .Value(this, &SOdysseyPaintModifiers::GetFlow )
                                .MaxFractionalDigits(0)
                                .MinValue( 0 )
                                .MaxValue( 100 )
                                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleFlowSpinBoxChanged )
                                .OnValueChanged( this, &SOdysseyPaintModifiers::SetFlow )
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
                                .IsFocusable(false)
                                .OptionsSource(&mBlendingModes)
                                .InitiallySelectedItem(mBlendingModes[mBlendingMode.Get()])
                                .OnGenerateWidget(this, &SOdysseyPaintModifiers::GenerateBlendingComboBoxItem)
                                .OnSelectionChanged(this, &SOdysseyPaintModifiers::HandleOnBlendingModeChanged )
                                .Content()
                                [
                                    CreateBlendingModeTextWidget()
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
                                .InitiallySelectedItem(mAlphaModes[mAlphaMode.Get()])
                                .OnGenerateWidget(this, &SOdysseyPaintModifiers::GenerateAlphaComboBoxItem)
                                .OnSelectionChanged(this, &SOdysseyPaintModifiers::HandleOnAlphaModeChanged )
                                .Content()
                                [
                                    CreateAlphaModeTextWidget()
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
                                SAssignNew( mSizeSpinBox, SSpinBox< float > )
                                //.Style(&FOdysseyStyle::GetWidgetStyle<FSpinBoxStyle>("OdysseySpinBoxStyle.DarkSpinBox"))
                                //.Value(mSize)
                                .Value( this, &SOdysseyPaintModifiers::GetSize )
                                .MaxFractionalDigits(0)
                                .MinValue( 1 )
                                .MaxValue( 1000 )
                                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleSizeSpinBoxChanged )
                                .OnValueChanged( this, &SOdysseyPaintModifiers::SetSize )
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
                                SAssignNew( mOpacitySpinBox, SSpinBox< float > )
                                //.Style(&FOdysseyStyle::GetWidgetStyle<FSpinBoxStyle>("OdysseySpinBoxStyle.DarkSpinBox"))
                                //.Value(mOpacity)
                                .Value( this, &SOdysseyPaintModifiers::GetOpacity )
                                .MaxFractionalDigits(0)
                                .MinValue( 0 )
                                .MaxValue( 100 )
                                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleOpacitySpinBoxChanged )
                                .OnValueChanged( this, &SOdysseyPaintModifiers::SetOpacity )
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
                                SAssignNew( mFlowSpinBox, SSpinBox< float > )
                                //.Style(&FOdysseyStyle::GetWidgetStyle<FSpinBoxStyle>("OdysseySpinBoxStyle.DarkSpinBox"))
                                //.Value(mFlow)
                                .Value( this, &SOdysseyPaintModifiers::GetFlow )
                                .MaxFractionalDigits(0)
                                .MinValue( 0 )
                                .MaxValue( 100 )
                                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleFlowSpinBoxChanged )
                                .OnValueChanged( this, &SOdysseyPaintModifiers::SetFlow )
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
                                .IsFocusable(false)
                                .OptionsSource(&mBlendingModes)
                                .InitiallySelectedItem(mBlendingModes[mBlendingMode.Get()])
                                .OnGenerateWidget(this, &SOdysseyPaintModifiers::GenerateBlendingComboBoxItem)
                                .OnSelectionChanged(this, &SOdysseyPaintModifiers::HandleOnBlendingModeChanged )
                                .Content()
                                [
                                    CreateBlendingModeTextWidget()
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
                                .InitiallySelectedItem(mAlphaModes[mAlphaMode.Get()])
                                .OnGenerateWidget(this, &SOdysseyPaintModifiers::GenerateAlphaComboBoxItem)
                                .OnSelectionChanged(this, &SOdysseyPaintModifiers::HandleOnAlphaModeChanged )
                                .Content()
                                [
                                    CreateAlphaModeTextWidget()
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
SOdysseyPaintModifiers::SetSize( float iValue )
{
    mSize = iValue;
    mOnSizeChangedCallback.ExecuteIfBound( iValue );
}


void
SOdysseyPaintModifiers::SetOpacity( float iValue )
{
    mOpacity = iValue;
//    UE_LOG(LogTemp, Display, TEXT("%lf"), mOpacity.Get());
    mOnOpacityChangedCallback.ExecuteIfBound( iValue );
}


void
SOdysseyPaintModifiers::SetFlow( float iValue )
{
    mFlow = iValue;
    mOnFlowChangedCallback.ExecuteIfBound( iValue );
}


void
SOdysseyPaintModifiers::SetBlendingMode( ::ul3::eBlendingMode iValue )
{
    mBlendingMode = iValue;
    TSharedPtr< FText > sel = MakeShared< FText >( GetBlendingModeAsText() );
    HandleOnBlendingModeChanged( sel, ESelectInfo::Direct );
}

void
SOdysseyPaintModifiers::SetAlphaMode( ::ul3::eAlphaMode iValue )
{
    mAlphaMode = iValue;
    TSharedPtr< FText > sel = MakeShared< FText >( GetAlphaModeAsText() );
    HandleOnAlphaModeChanged( sel, ESelectInfo::Direct );
}

float
SOdysseyPaintModifiers::GetSize() const
{
    return mSize.Get();
}

float
SOdysseyPaintModifiers::GetOpacity() const
{
    return mOpacity.Get();
}

float
SOdysseyPaintModifiers::GetFlow() const
{
    return mFlow.Get();
}

::ul3::eBlendingMode
SOdysseyPaintModifiers::GetBlendingMode()
{
    return mBlendingMode.Get();
}

::ul3::eAlphaMode
SOdysseyPaintModifiers::GetAlphaMode()
{
    return mAlphaMode.Get();
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks

void
SOdysseyPaintModifiers::HandleSizeSpinBoxChanged( float iValue, ETextCommit::Type iType )
{
    mOnSizeChangedCallback.ExecuteIfBound( iValue );
}


void
SOdysseyPaintModifiers::HandleOpacitySpinBoxChanged(float iValue, ETextCommit::Type iType )
{
    mOnOpacityChangedCallback.ExecuteIfBound( iValue );
}


void
SOdysseyPaintModifiers::HandleFlowSpinBoxChanged(float iValue, ETextCommit::Type iType )
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
SOdysseyPaintModifiers::CreateBlendingModeTextWidget()
{
    return SNew(STextBlock)
           .Text_Lambda([&](){ return GetBlendingModeAsText(); });
}


void
SOdysseyPaintModifiers::HandleOnBlendingModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo )
{
    ::ul3::eBlendingMode blendingMode = ::ul3::BM_NORMAL;
    for( uint8 i = 0; i < (int)::ul3::NUM_BLENDING_MODES; ++i )
    {
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[ i ] ) );
        if( NewSelection.Get()->EqualTo( entry ) )
        {
            blendingMode = static_cast<::ul3::eBlendingMode>( i );
        }
    }

    mOnBlendingModeChangedCallback.ExecuteIfBound( (int32)blendingMode );
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
    return  FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[ static_cast< int >( mBlendingMode.Get() ) ] ) );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides

void
SOdysseyPaintModifiers::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    ::ul3::eBlendingMode blendingMode = mBlendingMode.Get();
    if (blendingMode != mCurrentBlendingMode)
    {
        mCurrentBlendingMode = blendingMode;
        mBlendingModeComboBox->SetSelectedItem(mBlendingModes[blendingMode]);
    }

    ::ul3::eAlphaMode alphaMode = mAlphaMode.Get();
    if (alphaMode != mCurrentAlphaMode)
    {
        mCurrentAlphaMode = alphaMode;
        mAlphaModeComboBox->SetSelectedItem(mAlphaModes[alphaMode]);
    }
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
SOdysseyPaintModifiers::CreateAlphaModeTextWidget()
{
    return SNew(STextBlock)
           .Text_Lambda([&](){ return GetAlphaModeAsText(); });
}


void
SOdysseyPaintModifiers::HandleOnAlphaModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo )
{
    ::ul3::eAlphaMode alphaMode = ::ul3::AM_NORMAL;
    for( uint8 i = 0; i < (int)::ul3::NUM_ALPHA_MODES; ++i )
    {
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ul3::kwAlphaMode[ i ] ) );
        if( NewSelection.Get()->EqualTo( entry ) )
        {
            alphaMode = static_cast<::ul3::eAlphaMode>( i );
        }
    }

    mOnAlphaModeChangedCallback.ExecuteIfBound( (int32)alphaMode );
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
    return  FText::FromString( ANSI_TO_TCHAR( ::ul3::kwAlphaMode[ static_cast< int >( mAlphaMode.Get() ) ] ) );
}


#undef LOCTEXT_NAMESPACE

