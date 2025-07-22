// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorFlipbookLayout.h"

#include "OdysseyPainterEditorColorSelectorTab.h"
#include "OdysseyPainterEditorFlipbookTimelineTab.h"
#include "OdysseyPainterEditorLayerStackTab.h"
#include "OdysseyPainterEditorTextureDetailsTab.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Layout

TSharedRef<FTabManager::FLayout>
FOdysseyPainterEditorFlipbookLayout::Create()
{
    return FTabManager::NewLayout("OdysseyFlipbookEditor_Layout")
        ->AddArea
        (
            //MainArea
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Horizontal)
            ->Split(
                //Main Vertical Splitter
                FTabManager::NewSplitter()
                ->SetOrientation(Orient_Vertical)
                ->Split(
                    //Main Horizontal Splitter
                    FTabManager::NewSplitter()
                    ->SetOrientation(Orient_Horizontal)
                    ->Split(
                        //Left Splitter
                        FTabManager::NewSplitter()
                        ->SetOrientation(Orient_Vertical)
                        ->SetSizeCoefficient(0.15f)
                        ->Split(
                            //Tools Stack
                            FTabManager::NewStack()
                            ->SetSizeCoefficient(0.33f)
                            ->AddTab(
                                FOdysseyPainterEditorToolsTab::StaticId(),
                                ETabState::OpenedTab
                            )
                        )
                        ->Split(
                            //VectorSceneTreeView Stack
                            FTabManager::NewStack()
                            ->SetSizeCoefficient(0.33f)
                            ->AddTab(
                                FOdysseyPainterEditorVectorSceneTreeViewTab::StaticId(),
                                ETabState::OpenedTab
                            )
                        )
                    )
                    ->Split(
                        //Center Splitter
                        FTabManager::NewSplitter()
                        ->SetOrientation(Orient_Vertical)
                        ->SetSizeCoefficient(0.7f)
                        ->Split(
                            //Tools Stack
                            FTabManager::NewStack()
                            ->SetSizeCoefficient(1.f)
                            ->AddTab(
                                FOdysseyPainterEditorViewportTab::StaticId(),
                                ETabState::OpenedTab
                            )
                        )
                    )
                    ->Split(
                        //Right Splitter
                        FTabManager::NewSplitter()
                        ->SetOrientation(Orient_Vertical)
                        ->SetSizeCoefficient(0.15f)
                        ->Split(
                            //Color Selector Stack
                            FTabManager::NewStack()
                            ->SetSizeCoefficient(0.5f)
                            ->AddTab(
                                FOdysseyPainterEditorColorSelectorTab::StaticId(),
                                ETabState::OpenedTab
                            )
                        )
                        ->Split(
                            //LayerStack Stack
                            FTabManager::NewStack()
                            ->SetSizeCoefficient(0.5f)
                            ->SetForegroundTab(FOdysseyPainterEditorLayerStackTab::StaticId())
                            ->AddTab(
                                FOdysseyPainterEditorLayerStackTab::StaticId(),
                                ETabState::OpenedTab
                            )
                            ->AddTab(
                                FOdysseyPainterEditorTextureDetailsTab::StaticId(),
                                ETabState::OpenedTab
                            )
                        )
                    )
                )
                ->Split(
                    //Flipbook Timeline Stack
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.2f)
                    ->AddTab(
                        FOdysseyPainterEditorFlipbookTimelineTab::StaticId(),
                        ETabState::OpenedTab
                    )
                )
            )
        );
}

#undef LOCTEXT_NAMESPACE
