// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorGuideVisualizer.h"

#include "OdysseyPainterEditorEllipseGuideVisualizer.h"
#include "OdysseyPainterEditorLineGuideVisualizer.h"

const FOdysseyPainterEditorGuideVisualizer* FOdysseyPainterEditorGuideVisualizer::Find(EOdysseyPainterEditorGuideType InType)
{
    static const FOdysseyPainterEditorLineGuideVisualizer LineVisualizer;
    static const FOdysseyPainterEditorEllipseGuideVisualizer EllipseVisualizer;

    switch (InType)
    {
    case EOdysseyPainterEditorGuideType::Line:
        return &LineVisualizer;
    case EOdysseyPainterEditorGuideType::Ellipse:
        return &EllipseVisualizer;
    default:
        return nullptr;
    }
}
