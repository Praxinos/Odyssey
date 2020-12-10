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
        SLATE_EVENT( FOnInt32ValueChanged, OnSizeChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnOpacityChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnFlowChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnBlendingModeChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnAlphaModeChanged )
        SLATE_ATTRIBUTE(bool, VerticalAspect)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

public:
    // Public Callbacks
    void  SetSize( int32 iValue );
    void  SetOpacity( int32 iValue );
    void  SetFlow( int32 iValue );
    void  SetBlendingMode( ::ul3::eBlendingMode iValue );
    void  SetAlphaMode( ::ul3::eAlphaMode iValue );

public:
    // Public Getters
    int32  GetSize();
    int32  GetOpacity();
    int32  GetFlow();
    ::ul3::eBlendingMode  GetBlendingMode();
    ::ul3::eAlphaMode  GetAlphaMode();

private:
    // Private Callbacks
    void HandleSizeSpinBoxChanged( int32 iValue, ETextCommit::Type iType );
    void HandleOpacitySpinBoxChanged( int32 iValue, ETextCommit::Type iType );
    void HandleFlowSpinBoxChanged( int32 iValue, ETextCommit::Type iType );

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
    TSharedPtr< SSpinBox< int > >   mSizeSpinBox;
    TSharedPtr< SSpinBox< int > >   mOpacitySpinBox;
    TSharedPtr< SSpinBox< int > >   mFlowSpinBox;

    TSharedPtr<SComboBox<TSharedPtr<FText>>>    mBlendingBox;
    ::ul3::eBlendingMode                       mCurrentBlendingMode;
    TArray< TSharedPtr<FText> >                 mBlendingModes;
    TSharedPtr<SComboBox<TSharedPtr<FText> > >  mBlendingModeComboBox;

    TSharedPtr<SComboBox<TSharedPtr<FText>>>    mAlphaBox;
    ::ul3::eAlphaMode                          mCurrentAlphaMode;
    TArray< TSharedPtr<FText> >                 mAlphaModes;
    TSharedPtr<SComboBox<TSharedPtr<FText> > >  mAlphaModeComboBox;

    FOnInt32ValueChanged            mOnSizeChangedCallback;
    FOnInt32ValueChanged            mOnOpacityChangedCallback;
    FOnInt32ValueChanged            mOnFlowChangedCallback;
    FOnInt32ValueChanged            mOnBlendingModeChangedCallback;
    FOnInt32ValueChanged            mOnAlphaModeChangedCallback;
};

