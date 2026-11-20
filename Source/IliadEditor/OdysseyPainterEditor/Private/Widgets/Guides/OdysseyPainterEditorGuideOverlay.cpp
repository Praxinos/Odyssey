// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorGuideOverlay.h"

#include "OdysseyAnimation.h"
#include "OdysseyHUDElement.h"
#include "OdysseyHUDEllipse.h"
#include "OdysseyHUDLine.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorGuideCollection.h"
#include "Shapes/OdysseyPainterEditorEllipseGuide.h"
#include "Shapes/OdysseyPainterEditorLineGuide.h"
#include "Sources/OdysseyPainterEditorTextureSource.h"
#include "Engine/Texture2D.h"

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

    UOdysseyPainterEditorGuideCollection* Collection = nullptr;
    if (UOdysseyAnimation* Animation = EditorPtr->GetAnimation())
    {
        Collection = Animation->GetGuideCollection();
    }
    else if (TSharedPtr<FOdysseyPainterEditorTextureSource> TextureSource = EditorPtr->GetSourceTyped<FOdysseyPainterEditorTextureSource>())
    {
        if (UTexture2D* Texture = TextureSource->GetTexture())
            Collection = Cast<UOdysseyPainterEditorGuideCollection>(Texture->GetAssetUserDataOfClass(UOdysseyPainterEditorGuideCollection::StaticClass()));
    }
    if (!Collection)
    {
        GuideShapes.Empty();
        return;
    }

    TArray<TWeakObjectPtr<UOdysseyPainterEditorGuide>> StaleGuideKeys;
    for (const TPair<TWeakObjectPtr<UOdysseyPainterEditorGuide>, FGuideHUDShape>& ShapePair : GuideShapes)
    {
        UOdysseyPainterEditorGuide* Guide = ShapePair.Key.Get();
        if (!Guide || !Collection->Guides.Contains(Guide))
            StaleGuideKeys.Add(ShapePair.Key);
    }
    for (const TWeakObjectPtr<UOdysseyPainterEditorGuide>& StaleGuideKey : StaleGuideKeys)
        GuideShapes.Remove(StaleGuideKey);

    for (const TObjectPtr<UOdysseyPainterEditorGuide>& GuideObject : Collection->Guides)
    {
        UOdysseyPainterEditorGuide* Guide = GuideObject.Get();
        if (!Guide)
            continue;

        FGuideHUDShape* Shape = GuideShapes.Find(TWeakObjectPtr<UOdysseyPainterEditorGuide>(Guide));
        if (!Shape)
            Shape = &GuideShapes.Add(TWeakObjectPtr<UOdysseyPainterEditorGuide>(Guide), FGuideHUDShape());

        if (UOdysseyPainterEditorLineGuide* Line = Cast<UOdysseyPainterEditorLineGuide>(Guide))
        {
            if (!Shape->Line)
                Shape->Line = MakeShared<FOdysseyHUDLine>(FVector2D::ZeroVector, FVector2D::ZeroVector);
            Shape->Line->SetStartPoint(FVector2D(Line->Start.X * Params.mTextureWidth, Line->Start.Y * Params.mTextureHeight));
            Shape->Line->SetEndPoint(FVector2D(Line->End.X * Params.mTextureWidth, Line->End.Y * Params.mTextureHeight));
            Shape->Line->SetColor(Guide->Color);
            Shape->Line->SetOpacity(Guide->Opacity);
            Shape->Line->SetThickness(Guide->Thickness);
            Shape->Line->SetIsVisible(TAttribute<bool>(Guide->bVisible));
            AddElement(Shape->Line);
        }
        else if (UOdysseyPainterEditorEllipseGuide* Ellipse = Cast<UOdysseyPainterEditorEllipseGuide>(Guide))
        {
            if (!Shape->Ellipse)
                Shape->Ellipse = MakeShared<FOdysseyHUDEllipse>(FVector2D::ZeroVector, 0, 0);
            Shape->Ellipse->SetCenter(FVector2D(Ellipse->Center.X * Params.mTextureWidth, Ellipse->Center.Y * Params.mTextureHeight));
            Shape->Ellipse->SetXRadius(FMath::RoundToInt(Ellipse->Radius.X * Params.mTextureWidth));
            Shape->Ellipse->SetYRadius(FMath::RoundToInt(Ellipse->Radius.Y * Params.mTextureHeight));
            Shape->Ellipse->SetRotation(Ellipse->Angle);
            Shape->Ellipse->SetColor(Guide->Color);
            Shape->Ellipse->SetOpacity(Guide->Opacity);
            Shape->Ellipse->SetThickness(Guide->Thickness);
            Shape->Ellipse->SetIsVisible(TAttribute<bool>(Guide->bVisible));
            AddElement(Shape->Ellipse);
        }
    }
}
