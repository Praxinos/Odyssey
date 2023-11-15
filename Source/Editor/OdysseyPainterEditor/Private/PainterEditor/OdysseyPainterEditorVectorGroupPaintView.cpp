#include "OdysseyPainterEditorVectorGroupPaintView.h"

UOdysseyPainterEditorVectorGroupPaintView::~UOdysseyPainterEditorVectorGroupPaintView()
{
}

UOdysseyPainterEditorVectorGroupPaintView::UOdysseyPainterEditorVectorGroupPaintView()
    : UOdysseyPainterEditorVectorObjectView()
{
}

void
UOdysseyPainterEditorVectorGroupPaintView::ImportParam()
{
    UOdysseyPainterEditorVectorObjectView::ImportParam();

    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            // Category "PaintGroup"
            Painted         = selectedPaintGroup->IsPainted();
            Monochrome      = selectedPaintGroup->IsMonochrome();
            MonochromeColor = selectedPaintGroup->GetMonochromeColor();
            Realtime        = selectedPaintGroup->IsRealtime();
            GapTolerance    = selectedPaintGroup->GetGapTolerance();
            Wireframe       = selectedPaintGroup->IsWireframe();
            WireframeColor  = selectedPaintGroup->GetWireframeColor();

            break; // only one for now
        }
    }
}

uint64
UOdysseyPainterEditorVectorGroupPaintView::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    uint64 signalFlags = UOdysseyPainterEditorVectorObjectView::PropertyChanged( iPropertyName, iCategory );

    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            if( iPropertyName == "Painted" )
                selectedPaintGroup->SetPainted( Painted );

            if( iPropertyName == "Monochrome" )
                selectedPaintGroup->SetMonochrome( Monochrome );

            if( iPropertyName == "MonochromeColor" )
                selectedPaintGroup->SetMonochromeColor( MonochromeColor );

            if( iPropertyName == "Realtime" )
                selectedPaintGroup->SetRealtime( Realtime );

            if( iPropertyName == "GapTolerance" )
                selectedPaintGroup->SetGapTolerance( GapTolerance );

            if( iPropertyName == "Wireframe" )
                selectedPaintGroup->SetWireframe( Wireframe );

            if( iPropertyName == "WireframeColor" )
                selectedPaintGroup->SetWireframeColor( WireframeColor );
        }
    }

    return signalFlags;
}
