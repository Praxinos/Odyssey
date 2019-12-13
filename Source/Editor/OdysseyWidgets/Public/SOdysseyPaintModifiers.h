// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
#include <ULIS_BLENDINGMODES>

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
        SLATE_EVENT( FOnInt32ValueChanged, OnSizeChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnOpacityChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnFlowChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnBlendingModeChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnAlphaModeChanged )
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

public:
    // Public Callbacks
    void  SetSize( int32 iValue );
    void  SetOpacity( int32 iValue );
    void  SetFlow( int32 iValue );
    void  SetBlendingMode( ::ULIS::eBlendingMode iValue );
    void  SetAlphaMode( ::ULIS::eAlphaMode iValue );

private:
    // Private Callbacks
    void HandleSizeSliderChanged( float iValue );
    void HandleOpacitySliderChanged( float iValue );
    void HandleFlowSliderChanged( float iValue );
    void HandleSizeSpinBoxChanged( int32 iValue );
    void HandleOpacitySpinBoxChanged( int32 iValue );
    void HandleFlowSpinBoxChanged( int32 iValue );

private:
    // Blending mode Callbacks
    TSharedRef<SWidget> GenerateBlendingComboBoxItem( TSharedPtr<FText> InItem );
    TSharedRef<SWidget> CreateBlendingModeTextWidget( TSharedPtr<FText> InItem );
    void HandleOnBlendingModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo );
    TArray< TSharedPtr< FText > > GetBlendingModesAsText();
    FText GetBlendingModeAsText() const;

private:
    // Alpha mode Callbacks
    TSharedRef<SWidget> GenerateAlphaComboBoxItem( TSharedPtr<FText> InItem );
    TSharedRef<SWidget> CreateAlphaModeTextWidget( TSharedPtr<FText> InItem );
    void HandleOnAlphaModeChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo );
    TArray< TSharedPtr< FText > > GetAlphaModesAsText();
    FText GetAlphaModeAsText() const;

private:
    // Private Data Members
    TSharedPtr< SSlider >           mSizeSlider;
    TSharedPtr< SSpinBox< int > >   mSizeSpinBox;
    TSharedPtr< SSlider >           mOpacitySlider;
    TSharedPtr< SSpinBox< int > >   mOpacitySpinBox;
    TSharedPtr< SSlider >           mFlowSlider;
    TSharedPtr< SSpinBox< int > >   mFlowSpinBox;

    TSharedPtr<SComboBox<TSharedPtr<FText>>>    mBlendingBox;
    ::ULIS::eBlendingMode                       mCurrentBlendingMode;
    TArray< TSharedPtr<FText> >                 mBlendingModes;
    TSharedPtr<SComboBox<TSharedPtr<FText> > >  mBlendingModeComboBox;

    TSharedPtr<SComboBox<TSharedPtr<FText>>>    mAlphaBox;
    ::ULIS::eAlphaMode                          mCurrentAlphaMode;
    TArray< TSharedPtr<FText> >                 mAlphaModes;
    TSharedPtr<SComboBox<TSharedPtr<FText> > >  mAlphaModeComboBox;

    FOnInt32ValueChanged            mOnSizeChangedCallback;
    FOnInt32ValueChanged            mOnOpacityChangedCallback;
    FOnInt32ValueChanged            mOnFlowChangedCallback;
    FOnInt32ValueChanged            mOnBlendingModeChangedCallback;
    FOnInt32ValueChanged            mOnAlphaModeChangedCallback;
};

