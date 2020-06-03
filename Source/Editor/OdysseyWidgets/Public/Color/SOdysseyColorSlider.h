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
#include <ULIS3>

DECLARE_DELEGATE_OneParam( FOnColorChanged, const ::ul3::FPixelValue& );
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
    void Construct( const FArguments& InArgs );
    IOdysseyChannelSlider( uint32 iFormat );

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
    void  SetColor( const ::ul3::FPixelValue& iColor );
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
    virtual void SetColor_Imp( const ::ul3::FPixelValue& iColor ) {
        ::ul3::Conv( iColor, mColor );
    }

    ::ul3::FPixelValue GetColorForProportion( float t ) const;
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const;
    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const = 0;

protected:
    // Protected data members
    ::ul3::FPixelValue mColor;
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
#define ODYSSEY_CHANNEL_SLIDER( iClass, iFormat )           \
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
    iClass () : IOdysseyChannelSlider( iFormat ) {}         \
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
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_R, ULIS3_FORMAT_RGBAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetRedF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.RedF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 255;
    static const TCHAR Prefix = 'R';
    static const WIDECHAR Unit = ' ';
};

// G
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_G : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_G, ULIS3_FORMAT_RGBAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetGreenF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.GreenF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 255;
    static const TCHAR Prefix = 'G';
    static const WIDECHAR Unit = ' ';
};

// B
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_B : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_B, ULIS3_FORMAT_RGBAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetBlueF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.BlueF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 255;
    static const TCHAR Prefix = 'B';
    static const WIDECHAR Unit = ' ';
};

// A
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_A : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_A, ULIS3_FORMAT_RGBAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetAlphaF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.AlphaF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 255;
    static const TCHAR Prefix = 'A';
    static const WIDECHAR Unit = ' ';
};


/////////////////////////////////////////////////////
// HSV
// H
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSV_H : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSV_H, ULIS3_FORMAT_HSVAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetHueF( t );
        color.SetSaturationF( 1.f );
        color.SetValueF( 1.f );
        color.SetAlphaF( 1.f );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.HueF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 360;
    static const TCHAR Prefix = 'H';
    static const WIDECHAR Unit = L'°';
};

// S
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSV_S : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSV_S, ULIS3_FORMAT_HSVAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetSaturationF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.SaturationF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'S';
    static const WIDECHAR Unit = '%';
};

// V
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSV_V : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSV_V, ULIS3_FORMAT_HSVAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetValueF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.ValueF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'V';
    static const WIDECHAR Unit = '%';
};


/////////////////////////////////////////////////////
// HSL
// H
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSL_H : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSL_H, ULIS3_FORMAT_HSLAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetHueF( t );
        color.SetSaturationF( 1.f );
        color.SetLightnessF( 0.5f );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.HueF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 360;
    static const TCHAR Prefix = 'H';
    static const WIDECHAR Unit = L'°';
};

// S
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSL_S : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSL_S, ULIS3_FORMAT_HSLAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetSaturationF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.SaturationF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'S';
    static const WIDECHAR Unit = '%';
};

// L
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSL_L : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSL_L, ULIS3_FORMAT_HSLAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetLightnessF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.LightnessF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'L';
    static const WIDECHAR Unit = '%';
};



/////////////////////////////////////////////////////
// CMYK
// C
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_C : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_C, ULIS3_FORMAT_CMYKAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetCyanF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.CyanF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'C';
    static const WIDECHAR Unit = '%';
};

// M
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_M : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_M, ULIS3_FORMAT_CMYKAF )

private:virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetMagentaF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.MagentaF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'M';
    static const WIDECHAR Unit = '%';
};

// Y
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_Y : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_Y, ULIS3_FORMAT_CMYKAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetYellowF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.YellowF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'Y';
    static const WIDECHAR Unit = '%';
};

// K
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_K : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_K, ULIS3_FORMAT_CMYKAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetKeyF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.KeyF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'K';
    static const WIDECHAR Unit = '%';
};

/////////////////////////////////////////////////////
// YUV
// Y
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_YUV_Y : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_YUV_Y, ULIS3_FORMAT_YUVAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetLumaF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.LumaF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'Y';
    static const WIDECHAR Unit = ' ';
};

// S
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_YUV_U : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_YUV_U, ULIS3_FORMAT_YUVAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetUF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.UF();
    }

public:
    static const int MinValue = -127;
    static const int MaxValue = 128;
    static const TCHAR Prefix = 'U';
    static const WIDECHAR Unit = ' ';
};

// L
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_YUV_V : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_YUV_V, ULIS3_FORMAT_YUVAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetVF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.VF();
    }

public:
    static const int MinValue = -127;
    static const int MaxValue = 128;
    static const TCHAR Prefix = 'V';
    static const WIDECHAR Unit = ' ';
};

/////////////////////////////////////////////////////
// Lab
// L
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_Lab_L : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_Lab_L, ULIS3_FORMAT_LabAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetLF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.LF();
    }

public:
    static const int MinValue = 0;
    static const int MaxValue = 100;
    static const TCHAR Prefix = 'L';
    static const WIDECHAR Unit = ' ';
};

// a
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_Lab_a : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_Lab_a, ULIS3_FORMAT_LabAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetaF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.aF();
    }

public:
    static const int MinValue = -127;
    static const int MaxValue = 128;
    static const TCHAR Prefix = 'a';
    static const WIDECHAR Unit = ' ';
};

// b
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_Lab_b : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_Lab_b, ULIS3_FORMAT_LabAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const override {
        color.SetbF( t );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  mColor.bF();
    }

public:
    static const int MinValue = -127;
    static const int MaxValue = 128;
    static const TCHAR Prefix = 'b';
    static const WIDECHAR Unit = ' ';
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
    virtual void SetColor( const ::ul3::FPixelValue& iColor ) = 0;
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
        WIDECHAR UnitChar = TSliderType::Unit;
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

    virtual void SetColor( const ::ul3::FPixelValue& iColor ) override
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
    ::ul3::FPixelValue mColor;
    bool DisableCallbackPropagation;

public:
    // Construction / Destruction
    void Construct(const FArguments& InArgs)
    {}

    IOdysseyGroupChannelSlider( uint32 iFormat )
        : mColor( iFormat )
    {
    }

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

    void SetColor( const ::ul3::FPixelValue& iColor )
    {
        ComputeColorOnSet( iColor );
        DisableCallbackPropagation = true;
        for( int i = 0; i < sliders.Num(); ++i )
        {
            sliders[i]->SetColor( iColor );
        }
        DisableCallbackPropagation = false;
        ::ul3::Conv( iColor, mColor );
    }

    virtual void ComputeColorOnChanged() = 0;
    virtual void ComputeColorOnSet( const ::ul3::FPixelValue& iColor ) {
        ::ul3::Conv( iColor, mColor );
    }

};



/////////////////////////////////////////////////////
// Inheritance Utility Define
#define ODYSSEY_GROUP_SLIDER( iClass , iFormat )            \
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
    }                                                       \
                                                            \
    iClass() : IOdysseyGroupChannelSlider( iFormat ) {}

/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_RGB
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_RGB : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_RGB, ULIS3_FORMAT_RGB8 )
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
        ::ul3::Conv( ::ul3::FPixelValue::FromRGBAF( sliders[0]->GetValue(), sliders[1]->GetValue(), sliders[2]->GetValue() ), mColor );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_RGBA
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_RGBA : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_RGBA, ULIS3_FORMAT_RGBA8 )
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
        ::ul3::Conv( ::ul3::FPixelValue::FromRGBAF( sliders[0]->GetValue(), sliders[1]->GetValue(), sliders[2]->GetValue(), sliders[3]->GetValue() ), mColor );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_HA (Hue, Alpha)
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_HA : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_HA, ULIS3_FORMAT_HSVA8 )
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
        ::ul3::Conv( ::ul3::FPixelValue::FromHSVAF( sliders[0]->GetValue(), 1.f, 1.f, sliders[1]->GetValue() ), mColor );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_HSV
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_HSV : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_HSV, ULIS3_FORMAT_HSV8 )
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
        ::ul3::Conv( ::ul3::FPixelValue::FromHSVAF( sliders[0]->GetValue(), sliders[1]->GetValue(), sliders[2]->GetValue() ), mColor );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_HSL
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_HSL : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_HSL, ULIS3_FORMAT_HSL8 )
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
        ::ul3::Conv( ::ul3::FPixelValue::FromHSLAF( sliders[0]->GetValue(), sliders[1]->GetValue(), sliders[2]->GetValue() ), mColor );
    }
};



/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_CMYK
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_CMYK : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_CMYK, ULIS3_FORMAT_CMYK8 )
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
        ::ul3::Conv( ::ul3::FPixelValue::FromCMYKAF( sliders[0]->GetValue(), sliders[1]->GetValue(), sliders[2]->GetValue(), sliders[3]->GetValue() ), mColor );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_YUV
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_YUV : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_YUV, ULIS3_FORMAT_YUV8 )
    void BuildWidgets()
    {
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_YUV_Y > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_YUV_U > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_YUV_V > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
    }


    virtual void ComputeColorOnChanged()
    {
        ::ul3::Conv( ::ul3::FPixelValue::FromYUVAF( sliders[0]->GetValue(), sliders[1]->GetValue(), sliders[2]->GetValue() ), mColor );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_Lab
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_Lab : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_Lab, ULIS3_FORMAT_Lab8 )
    void BuildWidgets()
    {
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_Lab_L > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_Lab_a > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
        sliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_Lab_b > )
                .HeightOverride( HeightOverride )
                .OnValueChanged( this, &tSelf::HandleValueChanged ) );
    }


    virtual void ComputeColorOnChanged()
    {
        ::ul3::Conv( ::ul3::FPixelValue::FromLabAF( sliders[0]->GetValue(), sliders[1]->GetValue(), sliders[2]->GetValue() ), mColor );
    }
};



