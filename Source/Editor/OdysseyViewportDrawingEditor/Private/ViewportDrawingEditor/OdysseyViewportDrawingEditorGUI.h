// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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

#include "OdysseyPaintEngine3D.h"
#include "OdysseyBrushAssetBase.h"

class FOdysseyViewportDrawingEditorPainter;
class UOdysseyViewportDrawingEditorSettings;
class IDetailsView;
class SErrorText;
class UOdysseyBrush;
class SOdysseyBrushSelector;

enum class EOdysseyViewportSelectedView: uint8
{
    kBrushSettings,
    kStrokeOptions,
    kLayerStack,
    kTools
};

/** Widget representing the state / functionality and settings for PaintModePainter*/
class SOdysseyViewportDrawingEditorGUI : 
    public TSharedFromThis<SOdysseyViewportDrawingEditorGUI>
{
public:
    // Construction / Destruction
    virtual ~SOdysseyViewportDrawingEditorGUI();
    SOdysseyViewportDrawingEditorGUI();
    void Init(FOdysseyViewportDrawingEditorPainter* iPainter);

private:
	/** Creates a widget to select the different UI tabs of the paint in Viewport (Brush Settings, Stroke Options, Layer Stack...)*/
	TSharedPtr<SWidget> CreateTabSelectorWidget(FOdysseyViewportDrawingEditorPainter* iPainter);	
    void CreateMainWidget(FOdysseyViewportDrawingEditorPainter* iPainter);

private:	
	/** Paint settings instance */
	UOdysseyViewportDrawingEditorSettings* mPaintModeSettings;
	
    FString PaintTexturePath() const;

private:
    EVisibility GetBrushSettingsWidgetVisibility() const;
    EVisibility GetLayerStackWidgetVisibility() const;
    EVisibility GetStrokeOptionsWidgetVisibility() const;
    EVisibility GetToolsWidgetVisibility() const;

public:
    void OnSetOdysseyBrushSettingsView();
    void OnSetOdysseyStrokeOptionsView();
    void OnSetOdysseyLayerStackView();
    void OnSetOdysseyToolsView();

public:
    TSharedPtr<SWidget>&                         GetMainWidget();
    TSharedPtr<SOdysseyBrushSelector>&           GetBrushSelector();
    TSharedPtr<SOdysseyColorSelector>&           GetColorSelector();
    TSharedPtr<SOdysseyColorSliders>&            GetColorSliders();
    TSharedPtr<SOdysseyStrokeOptions>&           GetStrokeOptions();
    TSharedPtr<SOdysseyPaintModifiers>&          GetPaintModifiers();
    TSharedPtr<SOdysseyBrushExposedParameters>&  GetBrushExposedParameters();

    EOdysseyViewportSelectedView                 GetSelectedView();

public:
    void RefreshLayerStackView(FOdysseyLayerStack* iLayerStackData);

private:
    TSharedPtr<SWidget>                         mMainWidget;
    TSharedPtr<SWidget>                         mTabSelectorWidget;

    TSharedPtr<SOdysseyBrushSelector>           mBrushSelector;
    TSharedPtr<SOdysseyColorSelector>           mColorSelector;
    TSharedPtr<SOdysseyColorSliders>            mColorSliders;
    TSharedPtr<SOdysseyStrokeOptions>           mStrokeOptions;
    TSharedPtr<SOdysseyPaintModifiers>          mPaintModifiers;
    TSharedPtr<SOdysseyBrushExposedParameters>  mBrushExposedParameters;

    SVerticalBox::FSlot*                        mLayerStackView;

    EOdysseyViewportSelectedView                mSelectedView;
};
