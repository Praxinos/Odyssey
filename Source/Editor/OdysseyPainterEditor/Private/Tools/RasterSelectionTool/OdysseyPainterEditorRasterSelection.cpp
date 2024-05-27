// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterSelection.h"

FOdysseyPainterEditorRasterSelection::FOdysseyPainterEditorRasterSelection( TArray<FVector2D>& iSelectionArea ):
    mSelectionArea( iSelectionArea )
{
}

FOdysseyPainterEditorRasterSelection::~FOdysseyPainterEditorRasterSelection()
{
}

bool FOdysseyPainterEditorRasterSelection::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

EMouseCursor::Type
FOdysseyPainterEditorRasterSelection::GetMouseCursor() const
{
    return EMouseCursor::Default;
}

TArray<FVector2D>& FOdysseyPainterEditorRasterSelection::GetSelectionArea()
{
    return mSelectionArea;
}

void FOdysseyPainterEditorRasterSelection::SetSelectionArea( TArray<FVector2D>& iSelectionArea )
{
    mSelectionArea = iSelectionArea;
}

void FOdysseyPainterEditorRasterSelection::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
}

bool FOdysseyPainterEditorRasterSelection::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

bool FOdysseyPainterEditorRasterSelection::OnKeyUp(const FKey& iKey)
{
    return false;
}