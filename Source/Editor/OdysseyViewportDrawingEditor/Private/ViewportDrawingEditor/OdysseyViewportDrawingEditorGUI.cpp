// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OdysseyViewportDrawingEditorGUI.h"

#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SHeader.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "OdysseyViewportDrawingEditorPainter.h"
#include "OdysseyViewportDrawingEditorSettings.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "OdysseyTextureAssetUserData.h"



#include "Modules/ModuleManager.h"
#include "OdysseyViewportDrawingEditorCommands.h"
#include "DetailLayoutBuilder.h"

#include "EditorModeManager.h"
#include "EditorModes.h"
#include "OdysseyViewportDrawingEditorEdMode.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorGUI"


SOdysseyViewportDrawingEditorGUI::~SOdysseyViewportDrawingEditorGUI()
{
}


SOdysseyViewportDrawingEditorGUI::SOdysseyViewportDrawingEditorGUI()
    : mSelectedView (EOdysseyViewportSelectedView::kBrushSettings)
{
}


void SOdysseyViewportDrawingEditorGUI::Init(FOdysseyViewportDrawingEditorPainter* iPainter)
{
    if( !iPainter )
        return;

    mPaintModeSettings = Cast<UOdysseyViewportDrawingEditorSettings>(iPainter->GetPainterSettings());

    mBrushSelector = SNew(SOdysseyBrushSelector)
        .OnBrushChanged_Raw(iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::OnBrushSelected);

    mPaintModifiers = SNew(SOdysseyPaintModifiers)
        .OnSizeChanged_Raw          (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleSizeModifierChanged)
        .OnOpacityChanged_Raw       (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleOpacityModifierChanged)
        .OnFlowChanged_Raw          (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleFlowModifierChanged)
        .OnBlendingModeChanged_Raw  (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleBlendingModeModifierChanged)
        .OnAlphaModeChanged_Raw     (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleAlphaModeModifierChanged);

    mBrushExposedParameters = SNew(SOdysseyBrushExposedParameters)
        .OnParameterChanged_Raw(iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleBrushParameterChanged);

    mColorSelector = SNew(SOdysseyColorSelector)
        .OnColorChanged_Raw(iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleSelectorColorChanged);

    mColorSliders = SNew(SOdysseyColorSliders)
        .OnColorChanged_Raw(iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleSlidersColorChanged);

    mStrokeOptions = SNew(SOdysseyStrokeOptions)
        .OnStrokeStepChanged_Raw        (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleStrokeStepChanged)
        .OnStrokeAdaptativeChanged_Raw  (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleStrokeAdaptativeChanged)
        .OnStrokePaintOnTickChanged_Raw (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleStrokePaintOnTickChanged)
        .OnInterpolationTypeChanged_Raw (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleInterpolationTypeChanged)
        .OnSmoothingMethodChanged_Raw   (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleSmoothingMethodChanged)
        .OnSmoothingStrengthChanged_Raw (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleSmoothingStrengthChanged)
        .OnSmoothingEnabledChanged_Raw  (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleSmoothingEnabledChanged)
        .OnSmoothingRealTimeChanged_Raw (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleSmoothingRealTimeChanged)
        .OnSmoothingCatchUpChanged_Raw  (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleSmoothingCatchUpChanged);

    CreateMainWidget(iPainter);
}

TSharedPtr<SWidget> SOdysseyViewportDrawingEditorGUI::CreateTabSelectorWidget(FOdysseyViewportDrawingEditorPainter* iPainter)
{
	FToolBarBuilder toolbar(iPainter->GetUICommandList(), FMultiBoxCustomization::None, nullptr, Orient_Vertical);
	toolbar.SetLabelVisibility(EVisibility::Collapsed);
	toolbar.SetStyle(&FEditorStyle::Get(), "FoliageEditToolbar");
	{
		toolbar.AddToolBarButton(FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyBrushSettingsView);
		toolbar.AddToolBarButton(FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyStrokeOptions);
		toolbar.AddToolBarButton(FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyLayerStackView);
	}

	return
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SBorder)
				.HAlign(HAlign_Center)
				.Padding(0)
				.BorderImage(FEditorStyle::GetBrush("NoBorder"))
				.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
				[
					toolbar.MakeWidget()
				]
			]
		];
}

void SOdysseyViewportDrawingEditorGUI::CreateMainWidget(FOdysseyViewportDrawingEditorPainter* iPainter)
{
    FMargin StandardPadding(6.f,3.f);

    mMainWidget = 
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SNew(SVerticalBox)
		    + SVerticalBox::Slot()
		    .AutoHeight()
		    [
			    SNew(SHorizontalBox)
			    + SHorizontalBox::Slot()
			    .AutoWidth()
			    .Padding(1.f, 5.f, 0.f, 5.f)
			    [
				    CreateTabSelectorWidget(iPainter)->AsShared()
			    ]
			    + SHorizontalBox::Slot()
			    [
				    SNew(SBorder)
				    .BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
				    [
					    SNew(SVerticalBox)	
			            /** Texture paint action buttons widget */
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
						    SNew(SVerticalBox)
                            .Visibility(this, &SOdysseyViewportDrawingEditorGUI::GetBrushSettingsWidgetVisibility )
                   		    + SVerticalBox::Slot()
                            .Padding(StandardPadding)
			                .AutoHeight()
						    [
                                SNew(SHeader)
						        [
							        SNew(STextBlock)
							        .Text(LOCTEXT("BrushSelector", "Brush Selector"))
						        ]
                            ]
                   		    + SVerticalBox::Slot()
			                .AutoHeight()
						    [
                                mBrushSelector.ToSharedRef()
                            ]
                   		    + SVerticalBox::Slot()
                            .Padding(StandardPadding)
			                .AutoHeight()
						    [
                                SNew(SHeader)
						        [
							        SNew(STextBlock)
							        .Text(LOCTEXT("PaintModifiers", "Paint Modifiers"))
						        ]
                            ]
                   		    + SVerticalBox::Slot()
			                .AutoHeight()
						    [
                                mPaintModifiers.ToSharedRef()
                            ]
                   		    + SVerticalBox::Slot()
                            .Padding(StandardPadding)
			                .AutoHeight()
						    [
                                SNew(SHeader)
						        [
							        SNew(STextBlock)
							        .Text(LOCTEXT("ExposedParameters", "Exposed Parameters"))
						        ]
                            ]
                   		    + SVerticalBox::Slot()
			                .AutoHeight()
						    [
                                mBrushExposedParameters.ToSharedRef()
                            ]
                   		    + SVerticalBox::Slot()
                            .Padding(StandardPadding)
			                .AutoHeight()
						    [
                                SNew(SHeader)
						        [
							        SNew(STextBlock)
							        .Text(LOCTEXT("ColorSelector", "Color Selector"))
						        ]
                            ]
                   		    + SVerticalBox::Slot()
			                .AutoHeight()
						    [
                                SNew(SBox)
                                .HeightOverride(256)
                                [
                                    mColorSelector.ToSharedRef()
                                ]
                            ]
                   		    + SVerticalBox::Slot()
			                .AutoHeight()
						    [
                                mColorSliders.ToSharedRef()
                            ]
                        ]
                   		+ SVerticalBox::Slot()
                        .Padding(StandardPadding)
			            .AutoHeight()
						[
                            SNew(SHeader)
                            .Visibility(this, &SOdysseyViewportDrawingEditorGUI::GetStrokeOptionsWidgetVisibility )
						    [
							    SNew(STextBlock)
							    .Text(LOCTEXT("StrokeOptions", "Stroke Options"))
						    ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
						    SNew(SHorizontalBox)
                            .Visibility(this, &SOdysseyViewportDrawingEditorGUI::GetStrokeOptionsWidgetVisibility )
                   		    + SHorizontalBox::Slot()
						    [
                                mStrokeOptions.ToSharedRef()
                            ]
                        ]
                   		+ SVerticalBox::Slot()
                        .Padding(StandardPadding)
			            .AutoHeight()
						[
                            SNew(SHeader)
                            .Visibility( this, &SOdysseyViewportDrawingEditorGUI::GetLayerStackWidgetVisibility )
						    [
							    SNew(STextBlock)
							    .Text(LOCTEXT("LayerStack", "Layer Stack"))
						    ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Expose( mLayerStackView )
                        [
                            SNullWidget::NullWidget
                        ]
                        + SVerticalBox::Slot()
                        .Padding(StandardPadding)
                        .AutoHeight()
                        [
                            SNew(SHeader)
                            .Visibility(this,&SOdysseyViewportDrawingEditorGUI::GetLayerStackWidgetVisibility)
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("Texture to edit","Texture to edit"))
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SHorizontalBox)
                            .Visibility(this,&SOdysseyViewportDrawingEditorGUI::GetLayerStackWidgetVisibility)
                            + SHorizontalBox::Slot()
                            [
                                SNew(SObjectPropertyEntryBox)
                                .ObjectPath(this, &SOdysseyViewportDrawingEditorGUI::PaintTexturePath )
                                .AllowedClass(UTexture2D::StaticClass())
                                .OnShouldFilterAsset(FOnShouldFilterAsset::CreateRaw(iPainter,&FOdysseyViewportDrawingEditorPainter::ShouldFilterTextureAsset))
                                .OnObjectChanged(FOnSetObject::CreateRaw(iPainter,&FOdysseyViewportDrawingEditorPainter::PaintTextureChanged))
                                .DisplayUseSelected(false)
                                //.ThumbnailPool(iCustomizationUtils.GetThumbnailPool())
                            ]
                        ]
		            ]
                ]
	        ]
        ];
}

FString
SOdysseyViewportDrawingEditorGUI::PaintTexturePath() const
{
    if(!mPaintModeSettings->mTexturePaintSettings.mPaintTexture)
        return FString();

    return mPaintModeSettings->mTexturePaintSettings.mPaintTexture->GetPathName();
}


//---

EVisibility
SOdysseyViewportDrawingEditorGUI::GetBrushSettingsWidgetVisibility() const
{
    if( mSelectedView == EOdysseyViewportSelectedView::kBrushSettings )
        return EVisibility::Visible;
    else
	    return EVisibility::Collapsed;
}

EVisibility
SOdysseyViewportDrawingEditorGUI::GetLayerStackWidgetVisibility() const
{
    if( mSelectedView == EOdysseyViewportSelectedView::kLayerStack )
        return EVisibility::Visible;
    else
	    return EVisibility::Collapsed;
}

EVisibility
SOdysseyViewportDrawingEditorGUI::GetStrokeOptionsWidgetVisibility() const
{
    if( mSelectedView == EOdysseyViewportSelectedView::kStrokeOptions )
        return EVisibility::Visible;
    else
	    return EVisibility::Collapsed;
}

void
SOdysseyViewportDrawingEditorGUI::OnSetOdysseyBrushSettingsView()
{
    mSelectedView = EOdysseyViewportSelectedView::kBrushSettings;
}

void
SOdysseyViewportDrawingEditorGUI::OnSetOdysseyStrokeOptionsView()
{
    mSelectedView = EOdysseyViewportSelectedView::kStrokeOptions;
}

void
SOdysseyViewportDrawingEditorGUI::OnSetOdysseyLayerStackView()
{
    mSelectedView = EOdysseyViewportSelectedView::kLayerStack;
}

void
SOdysseyViewportDrawingEditorGUI::OnSetOdysseyPerformanceOptionsView()
{
    mSelectedView = EOdysseyViewportSelectedView::kPerformanceOptions;
}

TSharedPtr<SWidget>&
SOdysseyViewportDrawingEditorGUI::GetMainWidget()
{
    return mMainWidget;
}

TSharedPtr<SOdysseyBrushSelector>&           
SOdysseyViewportDrawingEditorGUI::GetBrushSelector()
{
    return mBrushSelector;
}

TSharedPtr<SOdysseyColorSelector>&           
SOdysseyViewportDrawingEditorGUI::GetColorSelector()
{
    return mColorSelector;
}

TSharedPtr<SOdysseyColorSliders>&            
SOdysseyViewportDrawingEditorGUI::GetColorSliders()
{
    return mColorSliders;
}

TSharedPtr<SOdysseyStrokeOptions>&           
SOdysseyViewportDrawingEditorGUI::GetStrokeOptions()
{
    return mStrokeOptions;
}

TSharedPtr<SOdysseyPaintModifiers>&          
SOdysseyViewportDrawingEditorGUI::GetPaintModifiers()
{
    return mPaintModifiers;
}

TSharedPtr<SOdysseyBrushExposedParameters>&  
SOdysseyViewportDrawingEditorGUI::GetBrushExposedParameters()
{
    return mBrushExposedParameters;
}

EOdysseyViewportSelectedView                 
SOdysseyViewportDrawingEditorGUI::GetSelectedView()
{
    return mSelectedView;
}


void
SOdysseyViewportDrawingEditorGUI::RefreshLayerStackView(FOdysseyLayerStack* iLayerStackData)
{
    mLayerStackView->DetachWidget();

    if( iLayerStackData )
    {
        mLayerStackView->AttachWidget( 
			SNew(SHorizontalBox)
            .Visibility( this, &SOdysseyViewportDrawingEditorGUI::GetLayerStackWidgetVisibility )
            + SHorizontalBox::Slot()
		    [
                SNew( SOdysseyLayerStackView )
                .LayerStackData( iLayerStackData )
            ]
        );
    }
}


#undef LOCTEXT_NAMESPACE // "PaintModePainter"
