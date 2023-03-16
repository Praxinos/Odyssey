// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPaintModifiers.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "OdysseyPaintModifiers"

#define MinSize 1
#define MaxSize 2000

/////////////////////////////////////////////////////
// SOdysseyPaintModifiers
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyPaintModifiers::Construct( const FArguments& InArgs )
{
    mOnGetSize = InArgs._OnGetSize;
    mOnGetOpacity = InArgs._OnGetOpacity;
    mOnGetFlow = InArgs._OnGetFlow;
    mBlendingMode = InArgs._BlendingMode;
    mAlphaMode = InArgs._AlphaMode;
    if (InArgs._MeshMaxSize.IsSet())
        mMeshMaxSize = InArgs._MeshMaxSize.Get();
    else
        mMeshMaxSize = -1;
    mCurrentBlendingMode = mBlendingMode.Get();
    mCurrentAlphaMode = mAlphaMode.Get();
    mOnSizeChangedCallback      = InArgs._OnSizeChanged;
    mOnOpacityChangedCallback   = InArgs._OnOpacityChanged;
    mOnFlowChangedCallback      = InArgs._OnFlowChanged;
    mOnBlendingModeChangedCallback = InArgs._OnBlendingModeChanged;
    mOnAlphaModeChangedCallback = InArgs._OnAlphaModeChanged;
    mAlphaModes = GetAlphaModesAsText();
    mBlendingModes = GetBlendingModesAsText();
    mIsEraserButtonActive = InArgs._IsEraserButtonActive;

    ChildSlot
    [
        SNew( SWrapBox )
        .UseAllottedWidth( true ) // if true put all slot horizontally   if false put all slot vertically
        +SWrapBox::Slot()
        [
            SNew( SHorizontalBox )

            +SHorizontalBox::Slot()
            .Padding( 6.f, 3.f, 3.f, 3.f )
            [
             SNew( SButton )
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .ToolTipText( LOCTEXT("SaveCurrentImageButton", "Save the current image within the current project.") )
            .VAlign( VAlign_Center )
            .ContentPadding( FMargin( 0.0, 0.0 ) )
            .OnClicked( InArgs._OnSaveButtonClicked )
            .IsEnabled( InArgs._IsPackageEdited )
                [
                    SNew( SImage )
                    .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Save32" ) )
                ]
            ]
            +SHorizontalBox::Slot()
            .Padding( 3.f, 3.f, 3.f, 3.f )
            [
            SNew( SButton )
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .ToolTipText( LOCTEXT("UndoActionButton", "Undo the previous action.") )
            .VAlign( VAlign_Center )
            .ContentPadding( FMargin( 0.0, 0.0 ) )
            .OnClicked( InArgs._OnUndoButtonClicked )
                [
                    SNew( SImage )
                    .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Undo32" ) )
                ]
            ]
            +SHorizontalBox::Slot()
            .Padding( 3.f, 3.f, 3.f, 3.f )
            [
            SNew( SButton )
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .ToolTipText( LOCTEXT("RedoActionButton", "Redo the next action.") )
            .VAlign( VAlign_Center )
            .ContentPadding( FMargin( 0.0, 0.0 ) )
            .OnClicked( InArgs._OnRedoButtonClicked )
                [
                    SNew( SImage )
                    .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Redo32" ) )
                ]
            ]
            +SHorizontalBox::Slot()
            .Padding( 3.f, 3.f, 33.f, 3.f )
            [
            SNew( SButton )
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .ToolTipText( LOCTEXT("EraserButton", "Switch the current tool to Eraser mode.") )
            .VAlign( VAlign_Center )
            .ContentPadding( FMargin( 0.0, 0.0 ) )
            .OnClicked( InArgs._OnEraserButtonClicked )
                [
                    SNew( SImage )
                    .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Eraser32" ) )
                    .ColorAndOpacity_Lambda( [this](){ return ( mIsEraserButtonActive.Get() == true ? FSlateColor( FLinearColor( 1.0f, 0.5f, 0.0f, 1.0f ) ) : FSlateColor( FLinearColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) ); } )
                ]
            ]
        ]
        +SWrapBox::Slot()
        [
            SNew( SHorizontalBox )

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 3.f, 3.f )
            .MaxWidth( 45.f )
            [
                SNew( STextBlock )
                .Text( LOCTEXT( "Size", "Size :" ) )
            ]

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 13.f, 3.f )
            .Expose(mSizeSpinBoxSlot)
            [
                SNullWidget::NullWidget
            ]
        ]
        +SWrapBox::Slot()
        [
            SNew( SHorizontalBox )

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 3.f, 3.f )
            .MaxWidth( 45.f )
            [
                SNew( STextBlock )
                .Text( LOCTEXT( "Opacity", "Opacity:" ) )
            ]

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 13.f, 3.f )
            [
                SAssignNew( mOpacitySpinBox, SSpinBox< int > )
                .Value( this, &SOdysseyPaintModifiers::OnGetOpacity )
                .LinearDeltaSensitivity( 15 ) // Doesn't work with MinValue, MinSliderValue ...
                .Delta( 1 )
                .TypeInterface( MakeShared<TNumericUnitTypeInterface<int>>( EUnit::Percentage ) )
                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleOpacitySpinBoxChanged )
                .OnValueChanged( this, &SOdysseyPaintModifiers::SetOpacity )
                .MinDesiredWidth( 83.0f ) // Depends on the size of the text in the previous slot
            ]
        ]
        +SWrapBox::Slot()
        [
            SNew( SHorizontalBox )

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 3.f, 3.f )
            .MaxWidth( 45.f )
            [
                SNew( STextBlock )
                .Text( LOCTEXT( "Flow", "Flow :" ) )
            ]

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 13.f, 3.f )
            [
                SAssignNew( mFlowSpinBox, SSpinBox< int > )
                .Value( this, &SOdysseyPaintModifiers::OnGetFlow )
                .LinearDeltaSensitivity( 15 ) // Doesn't work with MinValue, MinSliderValue ...
                .Delta( 1 )
                .TypeInterface( MakeShared<TNumericUnitTypeInterface<int>>( EUnit::Percentage ) )
                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleFlowSpinBoxChanged )
                .OnValueChanged( this, &SOdysseyPaintModifiers::SetFlow )
                .MinDesiredWidth( 96.0f ) // Depends on the size of the text in the previous slot
            ]
        ]
        +SWrapBox::Slot()
        [
            SNew( SHorizontalBox )

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 10.f, 3.f )
            .MaxWidth( 45.f )
            [
                SNew( STextBlock )
                .Text( LOCTEXT( "Blend", "Blend :" ) )
            ]

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 33.f, 3.f )
            .AutoWidth() // The size of the SComboBox will automatically adapt to the text it contains, according to the user's choice
            [
                SAssignNew( mBlendingModeComboBox, SComboBox<TSharedPtr<FText>> )
                .IsFocusable( false )
                .OptionsSource( &mBlendingModes )
                .InitiallySelectedItem( mBlendingModes[mBlendingMode.Get()] )
                .OnGenerateWidget( this, &SOdysseyPaintModifiers::GenerateBlendingComboBoxItem )
                .OnSelectionChanged( this, &SOdysseyPaintModifiers::HandleOnBlendingModeChanged )
                .IsEnabled_Lambda( [this](){ return ( mIsEraserButtonActive.Get() == true ? false : true ); } )
                .Content()
                [
                    CreateBlendingModeTextWidget()
                ]
            ]
        ]
        +SWrapBox::Slot()
        [
            SNew( SHorizontalBox )

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 9.f, 3.f )
            .MaxWidth( 45.f )
            [
                SNew( STextBlock )
                .Text( LOCTEXT( "Alpha", "Alpha :" ) )
            ]

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 13.f, 3.f )
            .AutoWidth() // The size of the SComboBox will automatically adapt to the text it contains, according to the user's choice
            [
                SAssignNew( mAlphaModeComboBox, SComboBox<TSharedPtr<FText>> )
                .IsFocusable( false )
                .OptionsSource( &mAlphaModes )
                .InitiallySelectedItem( mAlphaModes[mAlphaMode.Get()] )
                .OnGenerateWidget( this, &SOdysseyPaintModifiers::GenerateAlphaComboBoxItem )
                .OnSelectionChanged( this, &SOdysseyPaintModifiers::HandleOnAlphaModeChanged )
                .IsEnabled_Lambda( [this](){ return ( mIsEraserButtonActive.Get() == true ? false : true ); } )
                .Content()
                [
                    CreateAlphaModeTextWidget()
                ]
            ]
        ]
        +SWrapBox::Slot() // just to add a line when it's docked on the sides.
        [
            SNew( SHorizontalBox )

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 3.f, 3.f )
            .MaxWidth( 45.f )
            [
                SNew( STextBlock )
                .Text( LOCTEXT( "    ", "    " ) )
            ]
        ]
    ];

    RefreshSizeWidget();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Public Callbacks

void
SOdysseyPaintModifiers::SetSize( float iValue )
{
    int value = FMath::Clamp( mMeshMaxSize.Get() > 0 ? (iValue * mMeshMaxSize.Get()) / 100 : iValue, MinSize, mMeshMaxSize.Get() > 0 ? mMeshMaxSize.Get() : MaxSize);
    if (value == OnGetSize())
        return;

    mOnSizeChangedCallback.ExecuteIfBound( value, EPropertyChangeType::Interactive );
}

void
SOdysseyPaintModifiers::SetOpacity( int iValue )
{
    int value = FMath::Clamp(iValue, 0, 100);
    if (value == OnGetOpacity())
        return;

    mOnOpacityChangedCallback.ExecuteIfBound( value, EPropertyChangeType::Interactive );
}

void
SOdysseyPaintModifiers::SetFlow( int iValue )
{
    int value = FMath::Clamp(iValue, 0, 100);
    if (value == OnGetFlow())
        return;

    mOnFlowChangedCallback.ExecuteIfBound( value, EPropertyChangeType::Interactive );
}

void
SOdysseyPaintModifiers::SetBlendingMode( ::ULIS::eBlendMode iValue )
{
    mBlendingMode = iValue;
    TSharedPtr< FText > sel = MakeShared< FText >( GetBlendingModeAsText() );
    HandleOnBlendingModeChanged( sel, ESelectInfo::Direct );
}

void
SOdysseyPaintModifiers::SetAlphaMode( ::ULIS::eAlphaMode iValue )
{
    mAlphaMode = iValue;
    TSharedPtr< FText > sel = MakeShared< FText >( GetAlphaModeAsText() );
    HandleOnAlphaModeChanged( sel, ESelectInfo::Direct );
}

void SOdysseyPaintModifiers::SetMeshMaxSize(float iValue /*= -1 */)
{
    mMeshMaxSize.Set( iValue );
    RefreshSizeWidget();
}

::ULIS::eBlendMode
SOdysseyPaintModifiers::GetBlendingMode()
{
    return mBlendingMode.Get();
}

::ULIS::eAlphaMode
SOdysseyPaintModifiers::GetAlphaMode()
{
    return mAlphaMode.Get();
}

float
SOdysseyPaintModifiers::OnGetSize() const
{
    if( mOnGetSize.IsBound() )
    {
        if (mMeshMaxSize.Get() > 0)
        {
            return FMath::Clamp( (mOnGetSize.Execute() * 100) / mMeshMaxSize.Get(), MinSize, 100 );
        }
        else
        {
            return mOnGetSize.Execute();
        }
    }
    else 
    {
        return -1;
    }
}

int
SOdysseyPaintModifiers::OnGetOpacity() const
{
    if( mOnGetOpacity.IsBound() )
    {
        return mOnGetOpacity.Execute();
    }
    else 
    {
        return -1;
    }
}

int
SOdysseyPaintModifiers::OnGetFlow() const
{
    if( mOnGetFlow.IsBound() )
    {
        return mOnGetFlow.Execute();
    }
    else 
    {
        return -1;
    }
}

void
SOdysseyPaintModifiers::HandleSizeSpinBoxChanged( float iValue, ETextCommit::Type iType )
{
    mOnSizeChangedCallback.ExecuteIfBound( FMath::Clamp( mMeshMaxSize.Get() > 0 ? (iValue * mMeshMaxSize.Get()) / 100 : iValue, MinSize, mMeshMaxSize.Get() > 0 ? mMeshMaxSize.Get() : MaxSize), EPropertyChangeType::ValueSet );
}

void
SOdysseyPaintModifiers::HandleOpacitySpinBoxChanged(int iValue, ETextCommit::Type iType )
{
    mOnOpacityChangedCallback.ExecuteIfBound( FMath::Clamp( iValue, 0, 100 ), EPropertyChangeType::ValueSet );
}

void
SOdysseyPaintModifiers::HandleFlowSpinBoxChanged(int iValue, ETextCommit::Type iType )
{

    mOnFlowChangedCallback.ExecuteIfBound( FMath::Clamp( iValue, 0, 100 ), EPropertyChangeType::ValueSet );
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
    ::ULIS::eBlendMode blendingMode = ::ULIS::Blend_Normal;
    for( uint8 i = 0; i < (int)::ULIS::NumBlendModes; ++i )
    {
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendMode[ i ] ) );
        if( NewSelection.Get()->EqualTo( entry ) )
        {
            blendingMode = static_cast<::ULIS::eBlendMode>( i );
        }
    }

    mOnBlendingModeChangedCallback.ExecuteIfBound( (int32)blendingMode );
}

TArray< TSharedPtr< FText > >
SOdysseyPaintModifiers::GetBlendingModesAsText()
{
    TArray< TSharedPtr< FText > > array;
    for( int i = 0; i < (int)::ULIS::NumBlendModes; ++i )
        array.Add( MakeShared< FText >( FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendMode[i] ) ) ) );
    return array;
}

FText
SOdysseyPaintModifiers::GetBlendingModeAsText() const
{
    return  FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendMode[ static_cast< int >( mBlendingMode.Get() ) ] ) );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides

void
SOdysseyPaintModifiers::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    ::ULIS::eBlendMode blendingMode = mBlendingMode.Get();
    if (blendingMode != mCurrentBlendingMode)
    {
        mCurrentBlendingMode = blendingMode;
        mBlendingModeComboBox->SetSelectedItem(mBlendingModes[blendingMode]);
    }

    ::ULIS::eAlphaMode alphaMode = mAlphaMode.Get();
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
    ::ULIS::eAlphaMode alphaMode = ::ULIS::Alpha_Normal;
    for( uint8 i = 0; i < (int)::ULIS::NumAlphaModes; ++i )
    {
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwAlphaMode[ i ] ) );
        if( NewSelection.Get()->EqualTo( entry ) )
        {
            alphaMode = static_cast<::ULIS::eAlphaMode>( i );
        }
    }

    mOnAlphaModeChangedCallback.ExecuteIfBound( (int32)alphaMode );
}

TArray< TSharedPtr< FText > >
SOdysseyPaintModifiers::GetAlphaModesAsText()
{
    TArray< TSharedPtr< FText > > array;
    for( int i = 0; i < (int)::ULIS::NumAlphaModes; ++i )
        array.Add( MakeShared< FText >( FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwAlphaMode[i] ) ) ) );
    return array;
}

FText
SOdysseyPaintModifiers::GetAlphaModeAsText() const
{
    return  FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwAlphaMode[ static_cast< int >( mAlphaMode.Get() ) ] ) );
}

void SOdysseyPaintModifiers::RefreshSizeWidget()
{
    mSizeSpinBoxSlot->DetachWidget();

    if (mMeshMaxSize.Get() > 0)
    {
        mSizeSpinBoxSlot->AttachWidget( SNew(SSpinBox< float >)
            .Value(this, &SOdysseyPaintModifiers::OnGetSize)
            .OnValueCommitted(this, &SOdysseyPaintModifiers::HandleSizeSpinBoxChanged)
            .OnValueChanged(this, &SOdysseyPaintModifiers::SetSize)
            .TypeInterface(MakeShared<TNumericUnitTypeInterface<float>>(EUnit::Percentage))
            .MaxFractionalDigits(1)
            .ShiftMouseMovePixelPerDelta(15)
            .Delta(0.1f)
            .SliderExponent(0.8f) // Can't work properly if the following options are in use :  LinearDeltaSensitivity MinValue MaxValue
            .SliderExponentNeutralValue(100)
            .MinDesiredWidth(100.0f) // Depends on the size of the text in the previous slot
            );

        SetSize(OnGetSize());
    }
    else
    {
        mSizeSpinBoxSlot->AttachWidget( SNew(SSpinBox< float >)
            .Value(this, &SOdysseyPaintModifiers::OnGetSize)
            .OnValueCommitted(this, &SOdysseyPaintModifiers::HandleSizeSpinBoxChanged)
            .OnValueChanged(this, &SOdysseyPaintModifiers::SetSize)
            .MaxFractionalDigits(1)
            .ShiftMouseMovePixelPerDelta(15)
            .Delta(1)
            .SliderExponent(0.8f) // Can't work properly if the following options are in use :  LinearDeltaSensitivity MinValue MaxValue
            .SliderExponentNeutralValue(100)
            .MinDesiredWidth(100.0f) // Depends on the size of the text in the previous slot
            );

        SetSize(OnGetSize());
    }
}

#undef LOCTEXT_NAMESPACE

