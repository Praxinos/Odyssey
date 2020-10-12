// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Misc/NotifyHook.h"
#include "Widgets/Notifications/SErrorText.h"

#include "Brush/SOdysseyBrushExposedParameters.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Color/SOdysseyColorSelector.h"
#include "Color/SOdysseyColorSliders.h"
#include "SOdysseyPaintModifiers.h"
#include "SOdysseyStrokeOptions.h"
#include "OdysseyLayerStack.h"

#include "OdysseyPaintEngine.h"
#include "OdysseyBrushAssetBase.h"

class FOdysseyViewportDrawingEditorPainter;
class UOdysseyViewportDrawingEditorSettings;
class IDetailsView;
class SErrorText;
class UOdysseyBrush;
class SOdysseyBrushSelector;

/** Widget representing the state / functionality and settings for PaintModePainter*/
class SOdysseyViewportDrawingEditorWidget : public SCompoundWidget, public FNotifyHook
{
public:
	SLATE_BEGIN_ARGS(SOdysseyViewportDrawingEditorWidget) {}
	SLATE_END_ARGS()

	/** Slate widget construction */
	void Construct(const FArguments& iArgs, FOdysseyViewportDrawingEditorPainter* iPainter);

protected:
	/** Creates and sets up details view */
	void CreateDetailsView();

	/** Returns a widget comprising UI elements for texture painting */
	TSharedPtr<SWidget> CreateTexturePaintWidget();	

	/** Returns a widget comprising UI elements for texture painting */
	TSharedPtr<SWidget> CreateTabSelectorWidget();	


protected:	
	/** Objects displayed in the details view */
	TArray<UObject*> mSettingsObjects;
	/** Details view for brush and paint settings */
	TSharedPtr<IDetailsView> mSettingsDetailsView;
	/** Ptr to painter for which this widget is the ui representation */
	FOdysseyViewportDrawingEditorPainter* mMeshPainter;
	/** Paint settings instance */
	UOdysseyViewportDrawingEditorSettings* mPaintModeSettings;

	TSharedPtr<SErrorText> mErrorTextWidget;
		
private:
	virtual void NotifyPostChange(const FPropertyChangedEvent& iPropertyChangedEvent, FProperty* iPropertyThatChanged) override;
	
	bool GetMeshPaintEditorIsEnabled() const;
    void OnBrushSelected( UOdysseyBrush* iBrush );
    void OnBrushCompiled( UBlueprint* iBrush );
	void OnPaintEngineStrokeChanged(const TArray<::ul3::FRect>& iChangedTiles);
	void OnPaintEngineStrokeWillEnd(const TArray<::ul3::FRect>& iChangedTiles);
	void OnPaintEngineStrokeEnd(const TArray<::ul3::FRect>& iChangedTiles);
	void OnPaintEngineStrokeAbort();

private:
    // Brush Handlers
    void HandleBrushParameterChanged();

    // Color Handlers
    void HandleSelectorColorChanged( const ::ul3::FPixelValue& iColor );
    void HandleSlidersColorChanged( const ::ul3::FPixelValue& iColor );

    // Modifiers Handlers
    void HandleSizeModifierChanged( int32 iValue );
    void HandleOpacityModifierChanged( int32 iValue );
    void HandleFlowModifierChanged( int32 iValue );
    void HandleBlendingModeModifierChanged( int32 iValue );
    void HandleAlphaModeModifierChanged( int32 iValue );

    // Stroke Options Handlers
    void HandleStrokeStepChanged( int32 iValue );
    void HandleStrokeAdaptativeChanged( bool iValue );
    void HandleStrokePaintOnTickChanged( bool iValue );
    void HandleInterpolationTypeChanged( int32 iValue );
    void HandleSmoothingMethodChanged( int32 iValue );
    void HandleSmoothingStrengthChanged( int32 iValue );
    void HandleSmoothingEnabledChanged( bool iValue );
    void HandleSmoothingRealTimeChanged( bool iValue );
    void HandleSmoothingCatchUpChanged( bool iValue );

    //PaintTexturePath
    FString PaintTexturePath() const;

private:
    EVisibility GetBrushSettingsWidgetVisibility() const;
    EVisibility GetLayerStackWidgetVisibility() const;
    EVisibility GetStrokeOptionsWidgetVisibility() const;

public:
    FOdysseyPaintEngine* GetPaintEngine();
    FOdysseySurface* GetSurface();

public:
    void RefreshLayerStackView();

private:
    TSharedPtr<SOdysseyBrushSelector>           mBrushSelector;
    TSharedPtr<SOdysseyColorSelector>           mColorSelector;
    TSharedPtr<SOdysseyColorSliders>            mColorSliders;
    TSharedPtr<SOdysseyStrokeOptions>           mStrokeOptions;
    TSharedPtr<SOdysseyPaintModifiers>          mPaintModifiers;
    TSharedPtr<SOdysseyBrushExposedParameters>  mBrushExposedParameters;

    SVerticalBox::FSlot*                        mLayerStackView;

    FOdysseyLayerStack*         mLayerStack;
    FOdysseyPaintEngine         mPaintEngine;
    UOdysseyBrush*              mBrush;
    UOdysseyBrushAssetBase*     mBrushInstance;

};
