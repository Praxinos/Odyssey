// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorGuideOverlay.h"

#include "OdysseyHUDElement.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorGuideVisualizer.h"
#include "OdysseyPainterEditorGuideCollection.h"

FOdysseyPainterEditorGuideOverlay::FOdysseyPainterEditorGuideOverlay(FOdysseyPainterEditor* InEditor)
    : Editor(InEditor)
{
}

void FOdysseyPainterEditorGuideOverlay::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& Params)
{
    EmptyElements();
    FOdysseyPainterEditor* EditorPtr = Editor;
    if (!EditorPtr || !Params.mCanvas || Params.mTextureWidth <= 0 || Params.mTextureHeight <= 0)
        return;

    UOdysseyPainterEditorGuideCollection* Collection = EditorPtr->GetGuideCollection();
    if (!Collection)
    {
        GuideShapes.Empty();
        return;
    }

    TArray<TWeakObjectPtr<UOdysseyPainterEditorGuide>> StaleGuideKeys;
    for (const TPair<TWeakObjectPtr<UOdysseyPainterEditorGuide>, TSharedPtr<FOdysseyHUDElement>>& ShapePair : GuideShapes)
    {
        UOdysseyPainterEditorGuide* Guide = ShapePair.Key.Get();
        if (!Guide || !Collection->Guides.Contains(Guide))
            StaleGuideKeys.Add(ShapePair.Key);
    }
    for (const TWeakObjectPtr<UOdysseyPainterEditorGuide>& StaleGuideKey : StaleGuideKeys)
        GuideShapes.Remove(StaleGuideKey);

    const FVector2D TextureSize(Params.mTextureWidth, Params.mTextureHeight);
    for (const TObjectPtr<UOdysseyPainterEditorGuide>& GuideObject : Collection->Guides)
    {
        UOdysseyPainterEditorGuide* Guide = GuideObject.Get();
        if (!Guide)
            continue;

        const FOdysseyPainterEditorGuideVisualizer* Visualizer = FOdysseyPainterEditorGuideVisualizer::Find(Guide->GetGuideType());
        if (!Visualizer)
            continue;

        TSharedPtr<FOdysseyHUDElement>* Shape = GuideShapes.Find(TWeakObjectPtr<UOdysseyPainterEditorGuide>(Guide));
        if (!Shape)
            Shape = &GuideShapes.Add(TWeakObjectPtr<UOdysseyPainterEditorGuide>(Guide), Visualizer->CreateShape());

        Visualizer->UpdateShape(**Shape, *Guide, TextureSize);
        (*Shape)->SetIsVisible(TAttribute<bool>(Guide->bVisible));
        AddElement(*Shape);
    }
}
