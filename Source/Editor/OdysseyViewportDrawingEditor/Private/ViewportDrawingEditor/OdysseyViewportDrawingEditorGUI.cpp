// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
#include "Widgets/Layout/SExpandableArea.h"
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
        .OnAlphaModeChanged_Raw     (iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleAlphaModeModifierChanged)
        .VerticalAspect( true );

    mBrushExposedParameters = SNew(SOdysseyBrushExposedParameters)
        .OnParameterChanged_Raw(iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandleBrushParameterChanged);

    mColorSelector = SNew(SOdysseyColorSelector)
        .Color(iPainter->GetController()->GetData().ToSharedRef(),&FOdysseyViewportDrawingEditorData::PaintColor)
        .OnColorChange_Raw(iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandlePaintColorChange);

    mColorSliders = SNew(SOdysseyColorSliders)
        .Color(iPainter->GetController()->GetData().ToSharedRef(),&FOdysseyViewportDrawingEditorData::PaintColor)
        .OnColorChange_Raw(iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::HandlePaintColorChange);

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
    FVerticalToolBarBuilder toolbar( iPainter->GetUICommandList(), FMultiBoxCustomization::None );
	toolbar.SetLabelVisibility(EVisibility::Collapsed);
	toolbar.SetStyle(&FEditorStyle::Get(), "FoliageEditToolbar");
	{
		toolbar.AddToolBarButton(FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyBrushSettingsView);
		toolbar.AddToolBarButton(FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyStrokeOptionsView);
		toolbar.AddToolBarButton(FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyLayerStackView);
        toolbar.AddToolBarButton(FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyToolsView);
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
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [

                            // Brush Selector Widget -----------

						    SNew(SVerticalBox)
                            .Visibility(this, &SOdysseyViewportDrawingEditorGUI::GetBrushSettingsWidgetVisibility )
                   		    + SVerticalBox::Slot()
                            .Padding(StandardPadding)
			                .AutoHeight()
						    [
                                SNew(SExpandableArea)
                                .HeaderContent()
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("BrushSelector","Brush Selector"))
                                ]
                                .BodyContent()
                                [
                                    mBrushSelector.ToSharedRef()
                                ]
                            ]
                   		    + SVerticalBox::Slot()
                            .Padding(StandardPadding)
			                .AutoHeight()
						    [
                                SNew(SExpandableArea)
                                .HeaderContent()
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("PaintModifiers","Paint Modifiers"))
                                ]
                                .BodyContent()
                                [
                                    mPaintModifiers.ToSharedRef()
                                ]
                            ]
                   		    + SVerticalBox::Slot()
                            .Padding(StandardPadding)
			                .AutoHeight()
						    [
                                SNew(SExpandableArea)
                                .HeaderContent()
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("ExposedParameters","Exposed Parameters"))
                                ]
                                .BodyContent()
                                [
                                    mBrushExposedParameters.ToSharedRef()
                                ]
                            ]
                   		    + SVerticalBox::Slot()
                            .Padding(StandardPadding)
			                .AutoHeight()
						    [
                                SNew(SExpandableArea)
                                .HeaderContent()
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("ColorSelector","Color Selector"))
                                ]
                                .BodyContent()
                                [
                                    SNew(SBox)
                                    .HeightOverride(256)
                                    [
                                        mColorSelector.ToSharedRef()
                                    ]
                                ]
                            ]
                            + SVerticalBox::Slot()
                            .Padding(StandardPadding)
                            .AutoHeight()
                            [
                                SNew(SExpandableArea)
                                .HeaderContent()
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("ColorSliders","Color Sliders"))
                                ]
                                .BodyContent()
                                [
                                    mColorSliders.ToSharedRef()
                                ]
                            ]
                        ]

                        // Stroke Options Widget ----------------

                   		+ SVerticalBox::Slot()
                        .Padding(StandardPadding)
			            .AutoHeight()
						[
                            SNew(SVerticalBox)
                            .Visibility(this,&SOdysseyViewportDrawingEditorGUI::GetStrokeOptionsWidgetVisibility)
                            + SVerticalBox::Slot()
                            .Padding(StandardPadding)
                            .AutoHeight()
                            [
                                SNew(SExpandableArea)
                                .HeaderContent()
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("StrokeOptions","Stroke Options"))
                                ]
                                .BodyContent()
                                [
                                    mStrokeOptions.ToSharedRef()
                                ]
                            ]
                        ]

                        // Layer Stack Widget ------------------

                   		+ SVerticalBox::Slot()
                        .Padding(StandardPadding)
			            .AutoHeight()
						[
                            SNew(SVerticalBox)
                            .Visibility(this,&SOdysseyViewportDrawingEditorGUI::GetLayerStackWidgetVisibility)
                            + SVerticalBox::Slot()
                            .Padding(StandardPadding)
                            .AutoHeight()
                            [
                                SNew(SExpandableArea)
                                .HeaderContent()
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("Texture to edit","Texture to edit"))
                                ]
                                .BodyContent()
                                [
                                    SNew(SObjectPropertyEntryBox)
                                    .ObjectPath(this, &SOdysseyViewportDrawingEditorGUI::PaintTexturePath)
                                    .AllowedClass(UTexture2D::StaticClass())
                                    .OnShouldFilterAsset(FOnShouldFilterAsset::CreateRaw(iPainter, &FOdysseyViewportDrawingEditorPainter::ShouldFilterTextureAsset))
                                    .OnObjectChanged(FOnSetObject::CreateRaw(iPainter, &FOdysseyViewportDrawingEditorPainter::PaintTextureChanged))
                                    .DisplayUseSelected(false)
                                    //.ThumbnailPool(iCustomizationUtils.GetThumbnailPool())
                                ]
                            ]
                            + SVerticalBox::Slot()
                            .Padding(StandardPadding)
                            .AutoHeight()
                            [
                                SNew(SExpandableArea)
                                .HeaderContent()
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("LayerStack","Layer Stack"))
                                ]
                                .BodyContent()
                                [
                                    SNew(SVerticalBox)
                                    + SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Expose(mLayerStackView)
                                    [
                                        SNullWidget::NullWidget
                                    ]
                                ]
                            ]
                        ]

                        // Tools Widgets

                        + SVerticalBox::Slot()
                        .Padding(StandardPadding)
                        .AutoHeight()
                        [
                            SNew(SVerticalBox)
                            .Visibility(this,&SOdysseyViewportDrawingEditorGUI::GetToolsWidgetVisibility)
                            + SVerticalBox::Slot()
                            .Padding(StandardPadding)
                            .AutoHeight()
                            [
                                SNew(SExpandableArea)
                                .HeaderContent()
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("Tools","Tools"))
                                ]
                                .BodyContent()
                                [
                                    SNew(SScrollBox)
                                    .Orientation(Orient_Vertical)
                                    .ScrollBarAlwaysVisible(false)
                                    +SScrollBox::Slot()
                                    [
                                        SNew(SExpandableArea)
                                        .HeaderContent()
                                        [
                                            SNew(STextBlock)
                                            .Text(LOCTEXT("Utils","Utils"))
                                            .Font(FEditorStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
                                            .ShadowOffset(FVector2D(1.0f,1.0f))
                                        ]
                                        .BodyContent()
                                        [
                                            SNew(SWrapBox)
                                            .UseAllottedWidth(true)
                                            +SWrapBox::Slot()
                                            [
                                                SNew(SButton)
                                                .ButtonStyle(FCoreStyle::Get(),"NoBorder")
                                                .OnClicked_Raw(iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::OnClear)
                                                [
                                                    SNew(SImage)
                                                    .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Shredder32"))
                                                ]
                                            ]
                                            +SWrapBox::Slot()
                                            [
                                                SNew(SButton)
                                                .ButtonStyle(FCoreStyle::Get(),"NoBorder")
                                                .OnClicked_Raw(iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::OnFill)
                                                [
                                                    SNew(SImage)
                                                    .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.PaintBucket32"))
                                                ]
                                            ]
                                        ]
                                    ]
                                    +SScrollBox::Slot()
                                    [
                                        SNew(SExpandableArea)
                                        .HeaderContent()
                                        [
                                            SNew(STextBlock)
                                            .Text(LOCTEXT("UndoRedo  (Experimental)","UndoRedo  (Experimental)"))
                                            .Font(FEditorStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
                                            .ShadowOffset(FVector2D(1.0f,1.0f))
                                        ]
                                        .BodyContent()
                                        [
                                            SNew(SWrapBox)
                                            .UseAllottedWidth(true)
                                            +SWrapBox::Slot()
                                            [
                                                SNew(SButton)
                                                .ButtonStyle(FCoreStyle::Get(),"NoBorder")
                                                .OnClicked_Raw(iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::OnUndoIliad)
                                                [
                                                    SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Undo32"))
                                                ]
                                            ]
                                            +SWrapBox::Slot()
                                            [
                                                SNew(SButton)
                                                .ButtonStyle(FCoreStyle::Get(),"NoBorder")
                                                .OnClicked_Raw(iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::OnRedoIliad)
                                                [
                                                    SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Redo32"))
                                                ]
                                            ]
                                            +SWrapBox::Slot()
                                            [
                                                SNew(SButton)
                                                .Text(LOCTEXT("Clear Undo History","Clear Undo History"))
                                                .ToolTipText(LOCTEXT("Clear Undos tooltip","If the undo/redo is slow, clear the cache by clicking this button"))
                                                .VAlign(EVerticalAlignment::VAlign_Center)
                                                .OnClicked_Raw(iPainter->GetController().Get(),&FOdysseyViewportDrawingEditorController::OnClearUndo)
                                            ]
                                        ]
                                    ]
                                ]
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

EVisibility
SOdysseyViewportDrawingEditorGUI::GetToolsWidgetVisibility() const
{
    if( mSelectedView == EOdysseyViewportSelectedView::kTools )
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
SOdysseyViewportDrawingEditorGUI::OnSetOdysseyToolsView()
{
    mSelectedView = EOdysseyViewportSelectedView::kTools;
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
