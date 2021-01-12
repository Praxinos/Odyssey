// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
#include "OdysseySurfaceEditable.h"
#include "OdysseyEventState.h"
#include <ULIS3>

DECLARE_DELEGATE_TwoParams( FOnColorChange, eOdysseyEventState::Type, const ::ul3::FPixelValue& );
/* DECLARE_DELEGATE_TwoParams( FOnChannelChanged, EOdysseyEventState, float ); */
/* DECLARE_DELEGATE_TwoParams( FOnValueChanged, float ); */

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
    // void  SetColor( const ::ul3::FPixelValue& iColor );
    // void  SetPosition( float iPos );
    // float GetPosition() const;
    float  GetProportionForColor(const ::ul3::FPixelValue& iColor) const;
    ::ul3::FPixelValue GetColorForProportion(float t) const;

public:
    // Event
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    // Private IOdysseyChannelSlider API
    // virtual void SetColor_Imp( const ::ul3::FPixelValue& iColor ) = 0
    float GetProportionForMousePosition( FVector2D iPos ) const;
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& iColor, float t ) const = 0;
    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const = 0;
    //void  SetPosition( float iPos );

protected:
    // Protected data members
    TAttribute< ::ul3::FPixelValue > mColor;
    uint32 mFormat;
    FOnColorChange OnColorChangeCallback;

private:
    // Private data members
    mutable TUniquePtr< FOdysseySurfaceEditable > cursor_surface;
    mutable TUniquePtr< FSlateBrush > cursor_brush;
    mutable FVector2D cursor_size;
    float cursor_t;
    mutable FVector2D cursor_pos;
    mutable ::ul3::FPixelValue mDisplayedColor;
    mutable bool bMarkedAsInvalid;
};

/////////////////////////////////////////////////////
// Inheritance Utility Define



/////////////////////////////////////////////////////
// Inheritance Utility Define
#define ODYSSEY_CHANNEL_SLIDER( iClass, iFormat )                                                   \
public:                                                                                             \
    SLATE_BEGIN_ARGS( iClass )                                                                      \
        ODYSSEY_LEAF_WIDGET_CONSTRUCT_ARGS                                                          \
        {}                                                                                          \
        ODYSSEY_LEAF_WIDGET_CONSTRUCT_ATTRIBUTES                                                    \
        SLATE_ATTRIBUTE( ::ul3::FPixelValue, Color )                                                \
        SLATE_EVENT( FOnColorChange, OnColorChange )                                                \
    SLATE_END_ARGS()                                                                                \
                                                                                                    \
public:                                                                                             \
    void Construct(const FArguments& InArgs)                                                        \
    {                                                                                               \
        ODYSSEY_LEAF_WIDGET_FORWARD_CONSTRUCT_ARGS                                                  \
        mColor = InArgs._Color;                                                                     \
        OnColorChangeCallback = InArgs._OnColorChange;                                              \
        Init();                                                                                     \
    }                                                                                               \
                                                                                                    \
    iClass () : IOdysseyChannelSlider( iFormat ) {}                                                 \
                                                                                                    \
    static int GetMinValue()                                                                        \
    {                                                                                               \
        return iClass::MinValue;                                                                    \
    }                                                                                               \
                                                                                                    \
    static int GetMaxValue()                                                                        \
    {                                                                                               \
        return iClass::MaxValue;                                                                    \
    }                                                                                               

#define ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP( Setter, Getter )                                          \
private:                                                                                            \
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& iColor, float t ) const override {  \
        iColor.Setter( t );                                                                         \
    }                                                                                               \
                                                                                                    \
    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {    \
        return  iColor.Getter();                                                                    \
    }                                                                                               

#define ODYSSEY_CHANNEL_SLIDER_DATA( Min, Max, Pre, U )                                             \
public:                                                                                             \
    static const int MinValue = Min;                                                                \
    static const int MaxValue = Max;                                                                \
    static const TCHAR Prefix = Pre;                                                                \
    static const WIDECHAR Unit = U;                                                                 


/////////////////////////////////////////////////////
// RGB
// R
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_R : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_R, ULIS3_FORMAT_RGBAF)
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetRedF, RedF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 255, 'R', ' ')
};

// G
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_G : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_G, ULIS3_FORMAT_RGBAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetGreenF, GreenF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 255, 'G', ' ')
};

// B
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_B : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_B, ULIS3_FORMAT_RGBAF)
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetBlueF, BlueF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 255, 'B', ' ')
};

// A
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_A : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_A, ULIS3_FORMAT_RGBAF)
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetAlphaF, AlphaF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 255, 'A', ' ')
};


/////////////////////////////////////////////////////
// HSV
// H
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSV_H : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSV_H, ULIS3_FORMAT_HSVAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& iColor, float t ) const override {
        iColor.SetHueF( t );
        // iColor.SetSaturationF( 1.f );
        // iColor.SetValueF( 1.f );
        // iColor.SetAlphaF( 1.f );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  iColor.HueF();
    }

    ODYSSEY_CHANNEL_SLIDER_DATA(0, 360, 'H', L'°')
};

// S
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSV_S : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSV_S, ULIS3_FORMAT_HSVAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetSaturationF, SaturationF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 100, 'S', L'%')
};

// V
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSV_V : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSV_V, ULIS3_FORMAT_HSVAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetValueF, ValueF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 100, 'V', L'%')
};


/////////////////////////////////////////////////////
// HSL
// H
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSL_H : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSL_H, ULIS3_FORMAT_HSLAF )

private:
    virtual void SetColorForProportion_Imp( ::ul3::FPixelValue& iColor, float t ) const override {
        iColor.SetHueF( t );
        // iColor.SetSaturationF( 1.f );
        // iColor.SetLightnessF( 0.5f );
        // iColor.SetAlphaF( 1.f );
    }

    virtual float GetProportionForColor_Imp( const ::ul3::FPixelValue& iColor ) const override {
        return  iColor.HueF();
    }
    
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 360, 'H', L'°')
};

// S
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSL_S : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSL_S, ULIS3_FORMAT_HSLAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetSaturationF, SaturationF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 100, 'S', L'%')
};

// L
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_HSL_L : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_HSL_L, ULIS3_FORMAT_HSLAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetLightnessF, LightnessF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 100, 'L', L'%')
};



/////////////////////////////////////////////////////
// CMYK
// C
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_C : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_C, ULIS3_FORMAT_CMYKAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetCyanF, CyanF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 100, 'C', L'%')
};

// M
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_M : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_M, ULIS3_FORMAT_CMYKAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetMagentaF, MagentaF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 100, 'M', L'%')
};

// Y
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_Y : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_Y, ULIS3_FORMAT_CMYKAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetYellowF, YellowF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 100, 'Y', L'%')
};

// K
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_K : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_K, ULIS3_FORMAT_CMYKAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetKeyF, KeyF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 100, 'K', L'%')
};

/////////////////////////////////////////////////////
// YUV
// Y
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_YUV_Y : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_YUV_Y, ULIS3_FORMAT_YUVAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetLumaF, LumaF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 100, 'Y', L' ')
};

// S
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_YUV_U : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_YUV_U, ULIS3_FORMAT_YUVAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetUF, UF)
    ODYSSEY_CHANNEL_SLIDER_DATA(-127, 128, 'U', L' ')
};

// L
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_YUV_V : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_YUV_V, ULIS3_FORMAT_YUVAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetVF, VF)
    ODYSSEY_CHANNEL_SLIDER_DATA(-127, 128, 'V', L' ')
};

/////////////////////////////////////////////////////
// Lab
// L
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_Lab_L : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_Lab_L, ULIS3_FORMAT_LabAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetLF, LF)
    ODYSSEY_CHANNEL_SLIDER_DATA(0, 100, 'L', L' ')
};

// a
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_Lab_a : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_Lab_a, ULIS3_FORMAT_LabAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetaF, aF)
    ODYSSEY_CHANNEL_SLIDER_DATA(-127, 128, 'a', L' ')
};

// b
class ODYSSEYWIDGETS_API SOdysseyChannelSlider_Lab_b : public IOdysseyChannelSlider
{
    ODYSSEY_CHANNEL_SLIDER( SOdysseyChannelSlider_Lab_b, ULIS3_FORMAT_LabAF )
    ODYSSEY_CHANNEL_SLIDER_SIMPLEIMP(SetbF, bF)
    ODYSSEY_CHANNEL_SLIDER_DATA(-127, 128, 'b', L' ')
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
    // virtual float GetValue() = 0;
    // virtual void SetValue( float ) = 0;
    // virtual void SetColor( const ::ul3::FPixelValue& iColor ) = 0;
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
    SLATE_ATTRIBUTE( ::ul3::FPixelValue, Color )
    SLATE_EVENT( FOnColorChange, OnColorChange )
    SLATE_END_ARGS()

private:
    // Private data members
    TAttribute<::ul3::FPixelValue> mColor;
    TSharedPtr< SSpinBox< int > > mSpinbox;
    TSharedPtr< TSliderType > mSlider;
    FOnColorChange mOnColorChangeCallback;
    eOdysseyEventState::Type mEventState;
    // bool DisableCallbackPropagation;
    // bool DisableNextCallback;

public:
    // Construction / Destruction
    virtual ~TOdysseyPrettyChannelSlider() {}
    void Construct(const FArguments& InArgs)
    {
        // DisableCallbackPropagation = false;
        // DisableNextCallback = false;
        TCHAR PrefixChar = TSliderType::Prefix;
        WIDECHAR UnitChar = TSliderType::Unit;
        int HeightOverride = InArgs._HeightOverride;
        FString PrefixString;
        PrefixString.AppendChar( PrefixChar );
        FString UnitString;
        UnitString.AppendChar( UnitChar );

        mColor = InArgs._Color;
        mOnColorChangeCallback = InArgs._OnColorChange;
        mEventState = eOdysseyEventState::kNone;

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
                SAssignNew( mSlider, TSliderType )
                .DesiredHeight( HeightOverride )
                .Color(mColor)
                .OnColorChange( mOnColorChangeCallback )
            ]
            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .AutoWidth()
            [
                SNew( SBox )
                .VAlign( VAlign_Center )
                .WidthOverride( 50 )
                [
                    SAssignNew( mSpinbox, SSpinBox< int > )
                    .Value(this, &tSelf::GetSpinboxValue )
                    .MinValue( TSliderType::GetMinValue() )
                    .MaxValue( TSliderType::GetMaxValue() )
                    .OnValueChanged( this, &tSelf::OnSpinboxValueChanged )
                    .OnValueCommitted( this, &tSelf::OnSpinboxValueCommited )
                    .OnBeginSliderMovement( this, &tSelf::OnSpinboxBeginSliderMovement )
                    .OnEndSliderMovement( this, &tSelf::OnSpinboxEndSliderMovement )
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
    void SendColorChangeEvent( eOdysseyEventState::Type iState, const ::ul3::FPixelValue& iValue )
    {
        if (mEventState == iState && mEventState == eOdysseyEventState::kAdjust)
        {
            mOnColorChangeCallback.ExecuteIfBound(eOdysseyEventState::kAdjust, iValue);
            return;
        }

        while (mEventState != iState)
        {
            switch(mEventState)
            {
                case eOdysseyEventState::kNone:
                case eOdysseyEventState::kSet:
                case eOdysseyEventState::kAbort:
                {
                    mOnColorChangeCallback.ExecuteIfBound( eOdysseyEventState::kStart, iValue );
                    mEventState = eOdysseyEventState::kStart;
                }
                break;

                case eOdysseyEventState::kStart:
                {
                    mOnColorChangeCallback.ExecuteIfBound( eOdysseyEventState::kAdjust, iValue );
                    mEventState = eOdysseyEventState::kAdjust;
                }
                break;

                case eOdysseyEventState::kAdjust:
                {
                    if (iState == eOdysseyEventState::kAbort)
                    {
                        mOnColorChangeCallback.ExecuteIfBound( eOdysseyEventState::kAbort, iValue );
                        mEventState = eOdysseyEventState::kAbort;
                    }
                    else
                    {
                        mOnColorChangeCallback.ExecuteIfBound( eOdysseyEventState::kSet, iValue );
                        mEventState = eOdysseyEventState::kSet;
                    }
                }
                break;
            }
        }
    }

    void OnSpinboxValueChanged( int iValue )
    {
        float prop = ToNormalized( iValue );
        ::ul3::FPixelValue value = mSlider->GetColorForProportion(prop);
        if (mEventState == eOdysseyEventState::kStart || mEventState == eOdysseyEventState::kAdjust)
        {
            SendColorChangeEvent(eOdysseyEventState::kAdjust, value);
        }
    }

    // Callbacks
    void OnSpinboxValueCommited( int iValue, ETextCommit::Type iType )
    {
        float prop = ToNormalized( iValue );
        ::ul3::FPixelValue value = mSlider->GetColorForProportion(prop);
        if (iType == ETextCommit::OnCleared)
        {
            SendColorChangeEvent(eOdysseyEventState::kAbort, value);
        }
        else
        {
            SendColorChangeEvent(eOdysseyEventState::kSet, value);
        }
    }

    void OnSpinboxBeginSliderMovement()
    {
        SendColorChangeEvent(eOdysseyEventState::kStart, mColor.Get());
    }

    void OnSpinboxEndSliderMovement( int iValue )
    {
        float prop = ToNormalized( iValue );
        ::ul3::FPixelValue value = mSlider->GetColorForProportion(prop);
        SendColorChangeEvent(eOdysseyEventState::kSet, value);
    }

private:
    float ToNormalized( int iValue )
    {
        int delta = TSliderType::MaxValue - TSliderType::MinValue;
        float result = ( iValue - TSliderType::MinValue ) / float( delta );
        return  result;
    }

    int GetSpinboxValue() const
    {
        if (!mSlider)
            return 0;

        int delta = TSliderType::MaxValue - TSliderType::MinValue;
        return mSlider->GetProportionForColor(mColor.Get()) * delta + TSliderType::MinValue;
    }

public:
    // API
    /* virtual float GetValue() override
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
    } */
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
    TAttribute<::ul3::FPixelValue> mColor;
    TArray< TSharedPtr< IOdysseyPrettyChannelSlider > > mSliders;
    FOnColorChange mOnColorChangeCallback;
    int mHeightOverride;
    // ::ul3::FPixelValue mColor;
    // bool DisableCallbackPropagation;

public:
    // Construction / Destruction
    void Construct(const FArguments& InArgs)
    {}

    IOdysseyGroupChannelSlider()
    {
    }

    void BuildContents()
    {
        TSharedPtr< SVerticalBox > vbox = SNew( SVerticalBox );

        for( int i = 0; i < mSliders.Num(); ++i )
        {
            vbox->AddSlot()
            .Padding( 2, 2, 2, 2 )
            [
                mSliders[i].ToSharedRef()
            ];
        }

        ChildSlot
        [
            vbox.ToSharedRef()
        ];
    }

    /* void HandleValueChanged( float )
    {
        ComputeColorOnChanged();
        for( int i = 0; i < mSliders.Num(); ++i )
        {
            sliders[i]->SetColor( mColor );
        }
        if( !DisableCallbackPropagation )
            OnColorChangedCallback.ExecuteIfBound( mColor );
    } */

    /* void SetColor( const ::ul3::FPixelValue& iColor )
    {
        ComputeColorOnSet( iColor );
        DisableCallbackPropagation = true;
        for( int i = 0; i < mSliders.Num(); ++i )
        {
            sliders[i]->SetColor( iColor );
        }
        DisableCallbackPropagation = false;
        ::ul3::Conv( iColor, mColor );
    } */

    /* virtual void ComputeColorOnChanged() = 0;
    virtual void ComputeColorOnSet( const ::ul3::FPixelValue& iColor ) {
        ::ul3::Conv( iColor, mColor );
    } */

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
        SLATE_ATTRIBUTE( ::ul3::FPixelValue, Color )        \
        SLATE_EVENT( FOnColorChange, OnColorChange )        \
        SLATE_ARGUMENT( int, HeightOverride )               \
    SLATE_END_ARGS()                                        \
public:                                                     \
    void Construct(const FArguments& InArgs)                \
    {                                                       \
        mColor = InArgs._Color;                             \
        mOnColorChangeCallback = InArgs._OnColorChange;     \
        mHeightOverride = InArgs._HeightOverride;           \
        BuildWidgets();                                     \
        BuildContents();                                    \
    }                                                       \
                                                            \
    iClass() : IOdysseyGroupChannelSlider() {}

/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_RGB
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_RGB : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_RGB, ULIS3_FORMAT_RGB8 )
    void BuildWidgets()
    {
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_R > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_G > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_B > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_RGBA
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_RGBA : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_RGBA, ULIS3_FORMAT_RGBA8 )
    void BuildWidgets()
    {
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_R > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_G > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_B > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_A > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_HA (Hue, Alpha)
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_HA : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_HA, ULIS3_FORMAT_HSVA8 )
    void BuildWidgets()
    {
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSV_H > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_A > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_HSV
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_HSV : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_HSV, ULIS3_FORMAT_HSV8 )
    void BuildWidgets()
    {
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSV_H > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSV_S > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSV_V > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_HSL
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_HSL : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_HSL, ULIS3_FORMAT_HSL8 )
    void BuildWidgets()
    {
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSL_H > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSL_S > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_HSL_L > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
    }

};



/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_CMYK
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_CMYK : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_CMYK, ULIS3_FORMAT_CMYK8 )
    void BuildWidgets()
    {
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_C > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_M > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_Y > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_K > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
    }
};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_YUV
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_YUV : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_YUV, ULIS3_FORMAT_YUV8 )
    void BuildWidgets()
    {
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_YUV_Y > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_YUV_U > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_YUV_V > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
    }

};


/////////////////////////////////////////////////////
// FOdysseyGroupChannelSlider_Lab
class ODYSSEYWIDGETS_API FOdysseyGroupChannelSlider_Lab : public IOdysseyGroupChannelSlider
{
    ODYSSEY_GROUP_SLIDER( FOdysseyGroupChannelSlider_Lab, ULIS3_FORMAT_Lab8 )
    void BuildWidgets()
    {
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_Lab_L > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_Lab_a > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
        mSliders.Add( SNew( TOdysseyPrettyChannelSlider< SOdysseyChannelSlider_Lab_b > )
                .Color(mColor)
                .HeightOverride( mHeightOverride )
                .OnColorChange( mOnColorChangeCallback ) );
    }

};



