// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "Shapes/OdysseyPainterEditorLineGuide.h"

#define LOCTEXT_NAMESPACE "OdysseyGuides"

UOdysseyPainterEditorLineGuide::UOdysseyPainterEditorLineGuide()
{
    Name = LOCTEXT("line-guide-name", "Line");
}

EOdysseyPainterEditorGuideType UOdysseyPainterEditorLineGuide::GetGuideType() const
{
    return EOdysseyPainterEditorGuideType::Line;
}

#undef LOCTEXT_NAMESPACE
