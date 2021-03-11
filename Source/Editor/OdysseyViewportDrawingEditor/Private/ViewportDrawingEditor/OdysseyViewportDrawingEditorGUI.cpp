// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorGUI.h"

#include "OdysseyViewportDrawingEditorTextureSelectorTab.h"
#include "Widgets/Layout/SExpandableArea.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorGUI::~FOdysseyViewportDrawingEditorGUI()
{
}

FOdysseyViewportDrawingEditorGUI::FOdysseyViewportDrawingEditorGUI(FOdysseyViewportDrawingEditor* iEditor) :
	FOdysseyTextureEditorGUI(iEditor),
	mEditor( iEditor ),
    mSelectedView( EOdysseyViewportSelectedView::kBrushSettings ),
    mCommandList(MakeShareable(new FUICommandList()))
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyViewportDrawingEditorGUI::CreateTabs()
{
	FOdysseyTextureEditorGUI::CreateTabs();

	//ADD NEW TABS
	ODYSSEY_ADD_TAB(mTextureSelectorTab, FOdysseyViewportDrawingEditorTextureSelectorTab, mEditor);

    //Adjustments
    mTopTab->IsVertical(true);
}

void
FOdysseyViewportDrawingEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyTextureEditorGUI::BindShortcuts(iToolkit);

	//---

    const FOdysseyViewportDrawingEditorCommands& viewportDrawingEditorCommands = FOdysseyViewportDrawingEditorCommands::Get();

	#define MAP_SELECTVIEW_ACTION(action, view ) mCommandList->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyViewportDrawingEditorGUI::SetSelectedView, EOdysseyViewportSelectedView::view ), FCanExecuteAction(),  \
                                    FIsActionChecked::CreateLambda([=]                                                                                                  \
                                    {                                                                                                                                   \
                                        return mSelectedView == EOdysseyViewportSelectedView::view;                                                                     \
                                    }));

	MAP_SELECTVIEW_ACTION(viewportDrawingEditorCommands.SetOdysseyBrushSettingsView, kBrushSettings)
	MAP_SELECTVIEW_ACTION(viewportDrawingEditorCommands.SetOdysseyStrokeOptionsView, kStrokeOptions)
	MAP_SELECTVIEW_ACTION(viewportDrawingEditorCommands.SetOdysseyLayerStackView, kLayerStack)
	MAP_SELECTVIEW_ACTION(viewportDrawingEditorCommands.SetOdysseyToolsView, kTools)

	#undef MAP_ACTION

    iToolkit->GetToolkitCommands()->Append(mCommandList.ToSharedRef());
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FName
FOdysseyViewportDrawingEditorGUI::GetLayoutName()
{
	return "OdysseyViewportDrawingEditor_Layout";
}


TSharedPtr<FOdysseyViewportDrawingEditorTextureSelectorTab>&
FOdysseyViewportDrawingEditorGUI::GetTextureSelectorTab()
{
    return mTextureSelectorTab;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Layout

TSharedPtr<SWidget>
FOdysseyViewportDrawingEditorGUI::CreateWidget()
{
    FMargin StandardPadding(6.f, 3.f);

    return SNew(SScrollBox)
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
				    CreateTabSelectorWidget().ToSharedRef()
			    ]
			    + SHorizontalBox::Slot()
			    [
				    SNew(SBorder)
				    .BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
				    [
                        // Brush Selector Widget -----------

					    SNew(SVerticalBox)	
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
						    SNew(SVerticalBox)
                            .Visibility(this, &FOdysseyViewportDrawingEditorGUI::GetViewVisibility, EOdysseyViewportSelectedView::kBrushSettings )
                            + CreateSection( GetBrushSelectorTab()->Widget(), LOCTEXT("BrushSelector", "Brush Selector"))
                            + CreateSection( GetTopTab()->Widget(), LOCTEXT("PaintModifiers", "Paint Modifiers")) //TODO: Vertical Paint Modifiers Tab
                            + CreateSection( GetBrushExposedParametersTab()->Widget(), LOCTEXT("ExposedParameters", "Exposed Parameters"))
                            + CreateSection( GetColorWheelTab()->Widget(), LOCTEXT("ColorSelector", "Color Selector"))
                            + CreateSection( GetColorSlidersTab()->Widget(), LOCTEXT("ColorSliders", "Color Sliders"))
                        ]

                        // Stroke Options Widget ----------------

                   		+ SVerticalBox::Slot()
                        .Padding(StandardPadding)
			            .AutoHeight()
						[
                            SNew(SVerticalBox)
                            .Visibility(this,&FOdysseyViewportDrawingEditorGUI::GetViewVisibility, EOdysseyViewportSelectedView::kStrokeOptions)
                            + CreateSection( GetStrokeOptionsTab()->Widget(), LOCTEXT("StrokeOptions", "Stroke Options"))
                        ]

                        // Layer Stack Widget ------------------

                   		+ SVerticalBox::Slot()
                        .Padding(StandardPadding)
			            .AutoHeight()
						[
                            SNew(SVerticalBox)
                            .Visibility(this,&FOdysseyViewportDrawingEditorGUI::GetViewVisibility, EOdysseyViewportSelectedView::kLayerStack)
                            + CreateSection( GetTextureSelectorTab()->Widget(), LOCTEXT("TextureToEdit", "Texture to edit"))
                            + CreateSection( GetLayerStackTab()->Widget(), LOCTEXT("LayerStack", "Layer Stack"))
                        ]

                        // Tools Widgets

                        + SVerticalBox::Slot()
                        .Padding(StandardPadding)
                        .AutoHeight()
                        [
                            SNew(SVerticalBox)
                            .Visibility(this,&FOdysseyViewportDrawingEditorGUI::GetViewVisibility, EOdysseyViewportSelectedView::kTools)
                            + CreateSection( GetToolsTab()->Widget(), LOCTEXT("Tools", "Tools"))
                        ]
		            ]
                ]
	        ]
        ];
}

SVerticalBox::FSlot&
FOdysseyViewportDrawingEditorGUI::CreateSection(TSharedPtr<SWidget> iWidget, FText iName)
{
    FMargin StandardPadding(6.f, 3.f);

    return SVerticalBox::Slot()
    .Padding(StandardPadding)
    .AutoHeight()
    [
        SNew(SExpandableArea)
        .HeaderContent()
        [
            SNew(STextBlock)
            .Text(iName)
        ]
        .BodyContent()
        [
            iWidget.ToSharedRef()
        ]
    ];
}

TSharedPtr<SWidget>
FOdysseyViewportDrawingEditorGUI::CreateTabSelectorWidget()
{
    FVerticalToolBarBuilder toolbar( mCommandList, FMultiBoxCustomization::None );
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

EVisibility
FOdysseyViewportDrawingEditorGUI::GetViewVisibility(EOdysseyViewportSelectedView iView) const
{
    return mSelectedView == iView ? EVisibility::Visible : EVisibility::Collapsed;
}

void
FOdysseyViewportDrawingEditorGUI::SetSelectedView(EOdysseyViewportSelectedView iView)
{
    mSelectedView = iView;
}

#undef LOCTEXT_NAMESPACE
