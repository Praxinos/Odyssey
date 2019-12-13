// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Types/SlateStructs.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Layout/Visibility.h"
#include "Widgets/SWidget.h"
#include "Layout/Margin.h"
#include "Widgets/Layout/SBox.h"
#include "Layout/Children.h"
#include "Widgets/SPanel.h"
#include "SOdysseyLeafWidget.h"
#include "Widgets/Input/SSpinBox.h"
#include "OdysseySurface.h"
#include <ULIS_CCOLOR>

DECLARE_DELEGATE_OneParam( FOnColorChanged, const ::ULIS::CColor& );
DECLARE_DELEGATE_OneParam( FOnChannelChanged, float );
DECLARE_DELEGATE_OneParam( FOnValueChanged, float );


/////////////////////////////////////////////////////
// IOdysseyChannelSlider
class ODYSSEYWIDGETS_API IOdysseyChannelSlider : public SOdysseyLeafWidget
{
    typedef SOdysseyLeafWidget tSuperClass;

public:
    SLATE_BEGIN_ARGS( IOdysseyChannelSlider )
        ODYSSEY_LEAF_WIDGET_CONSTRUCT_ARGS
        {}
        ODYSSEY_LEAF_WIDGET_CONSTRUCT_ATTRIBUTES
        SLATE_EVENT( FOnChannelChanged, OnChannelChanged )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& InArgs);

protected:
    virtual void Init();

private:
    // Public SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args
                         , const FGeometry& AllottedGeometry
                         , const FSlateRect& MyCullingRect
                         , FSlateWindowElementList& OutDrawElements
                         , int32 LayerId
                         , const FWidgetStyle& InWidgetStyle
                         , bool bParentEnabled ) const override;

protected:
    // SOdysseyLeafWidget Buffer Utilities overrides
    virtual void InitInternalBuffers() const override;
    virtual void PaintInternalBuffer( int iReason = 0 ) const override;

public:
    // Public IOdysseyChannelSlider API
    void  SetColor( const ::ULIS::CColor& iColor );
    void  SetPosition( float iPos );
    float GetPosition() const;

public:
    // Event
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    // Private Event Handling
    void  ProcessMouseAction( FVector2D iPos );

private:
    // Private IOdysseyChannelSlider API
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor );
    ::ULIS::CColor GetColorForProportion( float t ) const;
    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const;
    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const = 0;

protected:
    // Protected data members
    ::ULIS::CColor mColor;
    FOnChannelChanged OnChannelChangedCallback;

private:
    // Private data members
    mutable TUniquePtr< FOdysseySurface > cursor_surface;
    mutable TUniquePtr< FSlateBrush > cursor_brush;
    mutable FVector2D cursor_size;
    float cursor_t;
    mutable FVector2D cursor_pos;
    mutable bool bMarkedAsInvalid;
};

/////////////////////////////////////////////////////
// Inheritance Utility Define
#define ODYSSEY_CHANNEL_SLIDER( iClass )                    \
public:                                                     \
    SLATE_BEGIN_ARGS( iClass )                              \
        ODYSSEY_LEAF_WIDGET_CONSTRUCT_ARGS                  \
        {}                                                  \
        ODYSSEY_LEAF_WIDGET_CONSTRUCT_ATTRIBUTES            \
        SLATE_EVENT( FOnChannelChanged, OnChannelChanged )  \
    SLATE_END_ARGS()                                        \
                                                            \
public:                                                     \
    void Construct(const FArguments& InArgs)                \
    {                                                       \
        ODYSSEY_LEAF_WIDGET_FORWARD_CONSTRUCT_ARGS          \
        OnChannelChangedCallback = InArgs._OnChannelChanged;\
        Init();                                             \
    }                                                       \
                                                            \
    static int GetMinValue()                                \
    {                                                       \
        return iClass::MinValue;                            \
    }                                                       \
                                                            \
    static int GetMaxValue()                                \
    {                                                       \
        return iClass::MaxValue;                            \
    }


/////////////////////////////////////////////////////
// RGB
// R
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_R : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_R )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToRGB();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetRedF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.RedF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 255;
    static const TCHAR Prefix = 'R';
    static const TCHAR Unit = ' ';
};

// G
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_G : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_G )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToRGB();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetGreenF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.GreenF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 255;
    static const TCHAR Prefix = 'G';
    static const TCHAR Unit = ' ';
};

// B
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_B : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_B )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToRGB();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetBlueF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.BlueF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 255;
    static const TCHAR Prefix = 'B';
    static const TCHAR Unit = ' ';
};

// A
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_A : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_A )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToRGB();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetAlphaF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.AlphaF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 255;
    static const TCHAR Prefix = 'A';
    static const TCHAR Unit = ' ';
};


/////////////////////////////////////////////////////
// HSV
// H
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSV_H : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSV_H )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToHSV();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetHSVF( t, 1, 1 );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.HSVHueF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 360;
    static const TCHAR Prefix = 'H';
    static const TCHAR Unit = '\'';
};

// S
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSV_S : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSV_S )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToHSV();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetHSVF( color.HSVHueF(), t, color.ValueF() );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.HSVSaturationF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'S';
    static const TCHAR Unit = '%';
};

// V
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSV_V : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSV_V )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToHSV();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetHSVF( color.HSVHueF(), color.HSVSaturationF(), t );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.ValueF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'V';
    static const TCHAR Unit = '%';
};


/////////////////////////////////////////////////////
// HSL
// H
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSL_H : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSL_H )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToHSL();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetHSLF( t, 1, 0.5 );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.HSLHueF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 360;
    static const TCHAR Prefix = 'H';
    static const TCHAR Unit = '\'';
};

// S
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSL_S : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSL_S )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToHSL();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetHSLF( color.HSLHueF(), t, color.LightnessF() );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.HSLSaturationF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'S';
    static const TCHAR Unit = '%';
};

// L
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSL_L : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSL_L )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToHSL();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetHSLF( color.HSLHueF(), color.HSLSaturationF(), t );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.LightnessF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'L';
    static const TCHAR Unit = '%';
};



/////////////////////////////////////////////////////
// CMYK
// C
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_C : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_C )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToCMYK();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetCMYKF( t, color.MagentaF(), color.YellowF(), color.KeyF() );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.CyanF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'C';
    static const TCHAR Unit = '%';
};

// M
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_M : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_M )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToCMYK();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetCMYKF( color.CyanF(), t, color.YellowF(), color.KeyF() );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.MagentaF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'M';
    static const TCHAR Unit = '%';
};

// Y
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_Y : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_Y )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToCMYK();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetCMYKF( color.CyanF(), color.MagentaF(), t, color.KeyF() );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.YellowF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'Y';
    static const TCHAR Unit = '%';
};

// K
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_K : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_K )

private:
    virtual void SetColor_Imp( const ::ULIS::CColor& iColor ) override {
        mColor = iColor.ToCMYK();
    }

    virtual void SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const override {
        color.SetCMYKF( color.CyanF(), color.MagentaF(), color.YellowF(), t );
    }

    virtual float GetProportionForColor_Imp( const ::ULIS::CColor& iColor ) const override {
        return  mColor.KeyF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'K';
    static const TCHAR Unit = '%';
};


/////////////////////////////////////////////////////
// IOdysseyPrettyChannelSlider
class ODYSSEYWIDGETS_API IOdysseyPrettyChannelSlider : public SCompoundWidget
{
    typedef SCompoundWidget tSuperClass;
    typedef IOdysseyPrettyChannelSlider tSelf;

public:
    SLATE_BEGIN_ARGS( IOdysseyPrettyChannelSlider )
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    virtual ~IOdysseyPrettyChannelSlider() {}
    void Construct(const FArguments& InArgs)
    {}

public:
    // API
    virtual float GetValue() = 0;
    virtual void SetValue( float ) = 0;
    virtual void SetColor( const ::ULIS::CColor& iColor ) = 0;
};

/////////////////////////////////////////////////////
// TOdysseyPrettyChannelSlider
template< class T >
class ODYSSEYWIDGETS_API TOdysseyPrettyChannelSlider : public IOdysseyPrettyChannelSlider
{
    typedef IOdysseyPrettyChannelSlider tSuperClass;
    typedef T TSliderType;
    typedef TOdysseyPrettyChannelSlider< TSliderType > tSelf;

public:
    SLATE_BEGIN_ARGS( TOdysseyPrettyChannelSlider )
        {}
    SLATE_ARGUMENT( int, HeightOverride )
    SLATE_EVENT( FOnValueChanged, OnValueChanged )
    SLATE_END_ARGS()

private:
    // Private data members
    TSharedPtr< SSpinBox< int > > spin_box;
    TSharedPtr< TSliderType > slider;
    FOnValueChanged OnValueChangedCallback;
    bool DisableCallbackPropagation;
    bool DisableNextCallback;

public:
    // Construction / Destruction
    virtual ~TOdysseyPrettyChannelSlider() {}
    void Construct(const FArguments& InArgs)
    {
        DisableCallbackPropagation = false;
        DisableNextCallback = false;
        TCHAR PrefixChar = TSliderType::Prefix;
        TCHAR UnitChar = TSliderType::Unit;
        int HeightOverride = InArgs._HeightOverride;
        FString PrefixString;
        PrefixString.AppendChar( PrefixChar );
        FString UnitString;
        UnitString.AppendChar( UnitChar );
        OnValueChangedCallback = InArgs._OnValueChanged;

        ChildSlot
        [
            SNew( SHorizontalBox )
            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .AutoWidth()
            [
                SNew( SBox )
                .VAlign( VAlign_Center )
                .WidthOverride( 15 )
                [
                    SNew( STextBlock )
                    .Text( FText::FromString( PrefixString ) )
                    .Justification( ETextJustify::Center )
                ]
            ]
            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .FillWidth( 1.0 )
            [
                SAssignNew( slider, TSliderType )
                .DesiredHeight( HeightOverride )
                .OnChannelChanged( this, &tSelf::HandleOnChannelChangedCallback )
            ]
            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .AutoWidth()
            [
                SNew( SBox )
                .VAlign( VAlign_Center )
                .WidthOverride( 50 )
                [
                    SAssignNew( spin_box, SSpinBox< int > )
                    .MinValue( TSliderType::GetMinValue() )
                    .MaxValue( TSliderType::GetMaxValue() )
                    .OnValueChanged( this, &tSelf::HandleOnSpinboxChangedCallback )
                ]
            ]
            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .AutoWidth()
            [
                SNew( SBox )
                .VAlign( VAlign_Center )
                .WidthOverride( 15 )
                [
                    SNew( STextBlock )
                    .Text( FText::FromString( UnitString ) )
                    .Justification( ETextJustify::Center )
                ]
            ]
        ];
    }

private:
    // Callbacks
    void HandleOnChannelChangedCallback( float iValue )
    {
        if( DisableNextCallback )
        {
            DisableNextCallback = false;
        }
        else
        {
            DisableNextCallback = true;
            int intvalue = EncodeToIntegerRange( iValue );

            spin_box->SetValue( intvalue );
            if( !DisableCallbackPropagation )
                OnValueChangedCallback.ExecuteIfBound( iValue );
        }
    }

    // Callbacks
    void HandleOnSpinboxChangedCallback( int iValue )
    {
        if( DisableNextCallback )
        {
            DisableNextCallback = false;
        }
        else
        {
            DisableNextCallback = true;
            float floatvalue = DecodeToFloatRange( iValue );

            slider->SetPosition( floatvalue );
            if( !DisableCallbackPropagation )
                OnValueChangedCallback.ExecuteIfBound( floatvalue );
        }
    }

private:
    // Internal conversions
    int EncodeToIntegerRange( float iValue )
    {
        int delta = TSliderType::MaxValue - TSliderType::MinValue;
        int result = iValue * delta + TSliderType::MinValue;
        return  result;
    }

    float DecodeToFloatRange( int iValue )
    {
        int delta = TSliderType::MaxValue - TSliderType::MinValue;
        float result = ( iValue - TSliderType::MinValue ) / float( delta );
        return  result;
    }

public:
    // API
    virtual float GetValue() override
    {
        return  slider->GetPosition();
    }

    virtual void SetValue( float iValue ) override
    {
        slider->SetPosition( iValue );
    }

    virtual void SetColor( const ::ULIS::CColor& iColor ) override
    {
        DisableCallbackPropagation = true;
        slider->SetColor( iColor );
        DisableCallbackPropagation = false;
    }
};


/////////////////////////////////////////////////////
// IOdysseyGroupChannelSlider
class ODYSSEYWIDGETS_API IOdysseyGroupChannelSlider : public SCompoundWidget
{
    typedef SCompoundWidget tSuperClass;
    typedef IOdysseyGroupChannelSlider tSelf;

public:
    SLATE_BEGIN_ARGS( IOdysseyGroupChannelSlider )
        {}
    SLATE_END_ARGS()

protected:
    // Protected data members
    TArray< TSharedPtr< IOdysseyPrettyChannelSlider > > sliders;
    FOnColorChanged OnColorChangedCallback;
    int HeightOverride;
    ::ULIS::CColor mColor;
    bool DisableCallbackPropagation;

public:
    // Construction / Destruction
    void Construct(const FArguments& InArgs)
    {}

    void BuildContents()
    {
        TSharedPtr< SVerticalBox > vbox = SNew( SVerticalBox );

        for( int i = 0; i < sliders.Num(); ++i )
        {
            vbox->AddSlot()
            .Padding( 2, 2, 2, 2 )
            [
                sliders[i].ToSharedRef()
            ];
        }

        ChildSlot
        [
            vbox.ToSharedRef()
        ];
    }

    void HandleValueChanged( float )
    {
        ComputeColorOnChanged();
        for( int i = 0; i < sliders.Num(); ++i )
        {
            sliders[i]->SetColor( mColor );
        }
        if( !DisableCallbackPropagation )
            OnColorChangedCallback.ExecuteIfBound( mColor );
    }

    void SetColor( const ::ULIS::CColor& iColor )
    {
        ComputeColorOnSet( iColor );
        DisableCallbackPropagation = true;
        for( int i = 0; i < sliders.Num(); ++i )
        {
            sliders[i]->SetColor( iColor );
        }
        DisableCallbackPropagation = false;
        mColor = iColor;
    }

    virtual void ComputeColorOnChanged() = 0;
    virtual void ComputeColorOnSet( const ::ULIS::CColor& iColor ) = 0;
};



/////////////////////////////////////////////////////
// Inheritance Utility Define
#define ODYSSEY_GROUP_SLIDER( iClass )                      \
    typedef IOdysseyGroupChannelSlider tSuperClass;         \
    typedef iClass tSelf;                                   \
public:                                                     \
    SLATE_BEGIN_ARGS( iClass )                              \
        ODYSSEY_LEAF_WIDGET_CONSTRUCT_ARGS                  \
        {}                                                  \
        ODYSSEY_LEAF_WIDGET_CONSTRUCT_ATTRIBUTES            \
        SLATE_EVENT( FOnColorChanged, OnColorChanged )      \
        SLATE_ARGUMENT( int, HeightOverride )               \
    SLATE_END_ARGS()                                        \
public:                                                     \
    void Construct(const FArguments& InArgs)                \
    {                                                       \
        OnColorChangedCallback = InArgs._OnColorChanged;    \
        HeightOverride = InArgs._HeightOverride;            \
        DisableCallbackPropagation = false;                 \
        BuildWidgets();                                     \
        BuildContents();                                    \
    }

/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_RGB
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_RGB : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_RGB )
    void BuildWidgets()
    {
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_R > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_G > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_B > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
    }

    virtual void ComputeColorOnChanged()
    {
        mColor = ::ULIS::CColor::FromRGBF( sliders[0]->GetValue()
                                        , sliders[1]->GetValue()
                                        , sliders[2]->GetValue() );
    }

    virtual void ComputeColorOnSet( const ::ULIS::CColor& iColor )
    {
        mColor = iColor.ToRGB();
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_RGBA
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_RGBA : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_RGBA )
    void BuildWidgets()
    {
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_R > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_G > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_B > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_A > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
    }

    virtual void ComputeColorOnChanged()
    {
        mColor = ::ULIS::CColor::FromRGBF( sliders[0]->GetValue()
                                        , sliders[1]->GetValue()
                                        , sliders[2]->GetValue()
                                        , sliders[3]->GetValue() );
    }

    virtual void ComputeColorOnSet( const ::ULIS::CColor& iColor )
    {
        mColor = iColor.ToRGB();
        mColor.SetAlphaF( sliders[3]->GetValue() );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_HA (Hue, Alpha)
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_HA : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_HA )
    void BuildWidgets()
    {
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSV_H > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_A > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
    }

    virtual void ComputeColorOnChanged()
    {
        mColor = ::ULIS::CColor::FromHSVF( sliders[0]->GetValue()
                                        , 1.f
                                        , 1.f
                                        , sliders[1]->GetValue() );
    }

    virtual void ComputeColorOnSet( const ::ULIS::CColor& iColor )
    {
        mColor = iColor.ToRGB();
        mColor.SetAlphaF( sliders[1]->GetValue() );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_HSV
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_HSV : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_HSV )
    void BuildWidgets()
    {
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSV_H > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSV_S > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSV_V > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
    }

    virtual void ComputeColorOnChanged()
    {
        mColor = ::ULIS::CColor::FromHSVF( sliders[0]->GetValue()
                                        , sliders[1]->GetValue()
                                        , sliders[2]->GetValue() );
    }

    virtual void ComputeColorOnSet( const ::ULIS::CColor& iColor )
    {
        mColor = iColor.ToHSV();
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_HSL
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_HSL : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_HSL )
    void BuildWidgets()
    {
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSL_H > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSL_S > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSL_L > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
    }


    virtual void ComputeColorOnChanged()
    {
        mColor = ::ULIS::CColor::FromHSLF( sliders[0]->GetValue()
                                        , sliders[1]->GetValue()
                                        , sliders[2]->GetValue() );
    }

    virtual void ComputeColorOnSet( const ::ULIS::CColor& iColor )
    {
        mColor = iColor.ToHSL();
    }
};



/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_CMYK
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_CMYK : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_CMYK )
    void BuildWidgets()
    {
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_C > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_M > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_Y > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_K > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
    }

    virtual void ComputeColorOnChanged()
    {
        mColor = ::ULIS::CColor::FromCMYKF( sliders[0]->GetValue()
                                         , sliders[1]->GetValue()
                                         , sliders[2]->GetValue()
                                         , sliders[3]->GetValue() );
    }

    virtual void ComputeColorOnSet( const ::ULIS::CColor& iColor )
    {
        mColor = iColor.ToCMYK();
    }
};

