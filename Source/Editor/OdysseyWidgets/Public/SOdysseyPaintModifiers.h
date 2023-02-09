// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
#include <ULIS>

/////////////////////////////////////////////////////
// SOdysseyPaintModifiers
class ODYSSEYWIDGETS_API SOdysseyPaintModifiers : public SCompoundWidget
{
    typedef SCompoundWidget             tSuperClass;
    typedef SOdysseyPaintModifiers      tSelf;
    typedef TSharedPtr< FString >       FComboItemType;

public:
    DECLARE_DELEGATE_RetVal( float, FOnGetIntProperty );
    DECLARE_DELEGATE_RetVal( float, FOnGetFloatProperty );
    DECLARE_DELEGATE_TwoParams( FOnFloatValueChangedWithType, float, EPropertyChangeType::Type );
    DECLARE_DELEGATE_TwoParams( FOnInt32ValueChangedWithType, int32, EPropertyChangeType::Type );
    

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyPaintModifiers )
        {}
        SLATE_ATTRIBUTE( ::ULIS::eBlendMode, BlendingMode )
        SLATE_ATTRIBUTE( ::ULIS::eAlphaMode, AlphaMode )
        SLATE_ATTRIBUTE( bool, IsEraserButtonActive )
        SLATE_ATTRIBUTE( bool, IsPackageEdited )
        SLATE_ATTRIBUTE( float, MeshMaxSize )
        SLATE_EVENT( FOnGetFloatProperty, OnGetSize )
        SLATE_EVENT( FOnGetFloatProperty, OnGetOpacity )
        SLATE_EVENT( FOnGetFloatProperty, OnGetFlow )
        SLATE_EVENT( FOnFloatValueChangedWithType, OnSizeChanged )
        SLATE_EVENT( FOnInt32ValueChangedWithType, OnOpacityChanged )
        SLATE_EVENT( FOnInt32ValueChangedWithType, OnFlowChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnBlendingModeChanged )
        SLATE_EVENT( FOnInt32ValueChanged, OnAlphaModeChanged )
        SLATE_EVENT( FOnClicked, OnSaveButtonClicked )
        SLATE_EVENT( FOnClicked, OnUndoButtonClicked )
        SLATE_EVENT( FOnClicked, OnRedoButtonClicked )
        SLATE_EVENT( FOnClicked, OnEraserButtonClicked )
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

private:
    // Widget overrides
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

public:
    // Public Callbacks
    void  SetSize( float iValue );
    void  SetOpacity( int iValue );
    void  SetFlow( int iValue );
    void  SetBlendingMode( ::ULIS::eBlendMode iValue );
    void  SetAlphaMode( ::ULIS::eAlphaMode iValue );
    // Public setter for mesh size. If iValue < 0, then we work in pure size, and not in percentage
    void  SetMeshMaxSize( float iValue = -1 );

public:
    // Public Getters
    ::ULIS::eBlendMode GetBlendingMode();
    ::ULIS::eAlphaMode GetAlphaMode();

private:
    // Private Callbacks
    float OnGetSize() const;
    int OnGetOpacity() const;
    int OnGetFlow() const;
    void HandleSizeSpinBoxChanged( float iValue, ETextCommit::Type iType );
    void HandleOpacitySpinBoxChanged( int iValue, ETextCommit::Type iType );
    void HandleFlowSpinBoxChanged( int iValue, ETextCommit::Type iType );

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
    void RefreshSizeWidget();

private:

    // If it is set, then the spin box for the size will be in percentage (1 - 100) 
    // this value here will be the multiplied by the percentage to get the true size of the brush
    TAttribute< float > mMeshMaxSize;

    FOnGetFloatProperty mOnGetSize;
    FOnGetFloatProperty mOnGetOpacity;
    FOnGetFloatProperty mOnGetFlow;

    TAttribute<::ULIS::eBlendMode> mBlendingMode;
    TAttribute<::ULIS::eAlphaMode> mAlphaMode;
    TAttribute<bool> mIsEraserButtonActive;
    TAttribute<bool> mIsPackageEdited;

    ::ULIS::eBlendMode    mCurrentBlendingMode;   //cache value
    ::ULIS::eAlphaMode       mCurrentAlphaMode;      //cache value

    SHorizontalBox::FSlot*          mSizeSpinBoxSlot;
    TSharedPtr< SSpinBox< int > >   mOpacitySpinBox;
    TSharedPtr< SSpinBox< int > >   mFlowSpinBox;

    TSharedPtr<SComboBox<TSharedPtr<FText>>>    mBlendingBox;
    TArray< TSharedPtr<FText> >                 mBlendingModes;
    TSharedPtr<SComboBox<TSharedPtr<FText> > >  mBlendingModeComboBox;

    TSharedPtr<SComboBox<TSharedPtr<FText>>>    mAlphaBox;
    TArray< TSharedPtr<FText> >                 mAlphaModes;
    TSharedPtr<SComboBox<TSharedPtr<FText> > >  mAlphaModeComboBox;

    FOnFloatValueChangedWithType    mOnSizeChangedCallback;
    FOnInt32ValueChangedWithType    mOnOpacityChangedCallback;
    FOnInt32ValueChangedWithType    mOnFlowChangedCallback;
    FOnInt32ValueChanged            mOnBlendingModeChangedCallback;
    FOnInt32ValueChanged            mOnAlphaModeChangedCallback;

};

