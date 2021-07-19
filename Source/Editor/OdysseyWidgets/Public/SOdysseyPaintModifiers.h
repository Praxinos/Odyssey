// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Layout/Children.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SPanel.h"
#include <ULIS3>

/////////////////////////////////////////////////////
// SOdysseyPaintModifiers
class ODYSSEYWIDGETS_API SOdysseyPaintModifiers : public SCompoundWidget
{
    typedef SCompoundWidget             tSuperClass;
    typedef SOdysseyPaintModifiers      tSelf;
    typedef TSharedPtr< FString >       FComboItemType;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyPaintModifiers )
        {}
        SLATE_ATTRIBUTE( float, Size )
        SLATE_ATTRIBUTE( float, Opacity )
        SLATE_ATTRIBUTE( float, Flow )
        SLATE_ATTRIBUTE( ::ul3::eBlendingMode, BlendingMode )
        SLATE_ATTRIBUTE( ::ul3::eAlphaMode, AlphaMode )
        SLATE_EVENT( FOnInt32ValueChanged, OnSizeChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnOpacityChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnFlowChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnBlendingModeChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnAlphaModeChanged )
        SLATE_ARGUMENT(bool, VerticalAspect)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

private:
    // Widget overrides
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

public:
    // Public Callbacks
    void  SetSize( float iValue );
    void  SetOpacity(float iValue );
    void  SetFlow(float iValue );
    void  SetBlendingMode( ::ul3::eBlendingMode iValue );
    void  SetAlphaMode( ::ul3::eAlphaMode iValue );

public:
    // Public Getters
    float  GetSize() const;
    float  GetOpacity() const;
    float  GetFlow() const;
    ::ul3::eBlendingMode  GetBlendingMode();
    ::ul3::eAlphaMode  GetAlphaMode();

private:
    // Private Callbacks
    void HandleSizeSpinBoxChanged( float iValue, ETextCommit::Type iType );
    void HandleOpacitySpinBoxChanged( float iValue, ETextCommit::Type iType );
    void HandleFlowSpinBoxChanged( float iValue, ETextCommit::Type iType );

private:
    // Blending mode Callbacks
    TSharedRef<SWidget> GenerateBlendingComboBoxItem( TSharedPtr<FText> InItem );
    TSharedRef<SWidget> CreateBlendingModeTextWidget();
    void HandleOnBlendingModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo );
    TArray< TSharedPtr< FText > > GetBlendingModesAsText();
    FText GetBlendingModeAsText() const;

private:
    // Alpha mode Callbacks
    TSharedRef<SWidget> GenerateAlphaComboBoxItem( TSharedPtr<FText> InItem );
    TSharedRef<SWidget> CreateAlphaModeTextWidget();
    void HandleOnAlphaModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo );
    TArray< TSharedPtr< FText > > GetAlphaModesAsText();
    FText GetAlphaModeAsText() const;

private:
    // Private Data Members
    TAttribute<float> mSize;
    TAttribute<float> mOpacity;
    TAttribute<float> mFlow;
    TAttribute<::ul3::eBlendingMode> mBlendingMode;
    TAttribute<::ul3::eAlphaMode> mAlphaMode;

    ::ul3::eBlendingMode                       mCurrentBlendingMode; //cache value
    ::ul3::eAlphaMode                          mCurrentAlphaMode; //cache value

    TSharedPtr< SSpinBox< float > >   mSizeSpinBox;
    TSharedPtr< SSpinBox< float > >   mOpacitySpinBox;
    TSharedPtr< SSpinBox< float > >   mFlowSpinBox;

    TSharedPtr<SComboBox<TSharedPtr<FText>>>    mBlendingBox;
    TArray< TSharedPtr<FText> >                 mBlendingModes;
    TSharedPtr<SComboBox<TSharedPtr<FText> > >  mBlendingModeComboBox;

    TSharedPtr<SComboBox<TSharedPtr<FText>>>    mAlphaBox;
    TArray< TSharedPtr<FText> >                 mAlphaModes;
    TSharedPtr<SComboBox<TSharedPtr<FText> > >  mAlphaModeComboBox;

    FOnInt32ValueChanged            mOnSizeChangedCallback;
    FOnInt32ValueChanged            mOnOpacityChangedCallback;
    FOnInt32ValueChanged            mOnFlowChangedCallback;
    FOnInt32ValueChanged            mOnBlendingModeChangedCallback;
    FOnInt32ValueChanged            mOnAlphaModeChangedCallback;
};

