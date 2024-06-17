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
            Painted           = selectedPaintGroup->IsPainted();
            Monochrome        = selectedPaintGroup->IsMonochrome();
            MonochromeColor   = selectedPaintGroup->GetMonochromeColor();
            Realtime          = selectedPaintGroup->IsRealtime();
            GapTolerance      = selectedPaintGroup->GetGapTolerance();
            //Wireframe       = selectedPaintGroup->IsWireframe();
            WireframeColor    = selectedPaintGroup->GetWireframeColor();
            Multithreaded     = selectedPaintGroup->IsMultithreaded();
            IntersectsCanvas = selectedPaintGroup->IntersectsCanvas();

            break; // only one for now
        }
    }
}

uint64
UOdysseyPainterEditorVectorGroupPaintView::PropertyChanged( const FName& iPropertyName
                                                          , const FName& iMemberPropertyName
                                                          , const FName& iCategory)
{
    uint64 signalFlags = UOdysseyPainterEditorVectorObjectView::PropertyChanged( iPropertyName
                                                                               , iMemberPropertyName
                                                                               , iCategory );

    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            if( iPropertyName == "Painted" )
                selectedPaintGroup->SetPainted( Painted );

            if( iPropertyName == "Monochrome" )
                selectedPaintGroup->SetMonochrome( Monochrome );

            // Note: iMemberPropertyName because FColor is a struct 
            // and we can edit individual struct members RGBA
            if( ( iPropertyName == "MonochromeColor" ) || ( iMemberPropertyName == "MonochromeColor" ) )
                selectedPaintGroup->SetMonochromeColor( MonochromeColor );

            if( iPropertyName == "Realtime" )
                selectedPaintGroup->SetRealtime( Realtime );

            if( iPropertyName == "GapTolerance" )
                selectedPaintGroup->SetGapTolerance( GapTolerance );

            if( iPropertyName == "IntersectsCanvas" )
                selectedPaintGroup->SetIntersectsCanvas( IntersectsCanvas );

            //if( iPropertyName == "Wireframe" )
            //    selectedPaintGroup->SetWireframe( Wireframe );

            // Note: iMemberPropertyName because FColor is a struct 
            // and we can edit individual struct members RGBA
            if( ( iPropertyName == "WireframeColor" ) || ( iMemberPropertyName == "WireframeColor" ) )
                selectedPaintGroup->SetWireframeColor( WireframeColor );

            if( iPropertyName == "Multithreaded" )
                selectedPaintGroup->SetMultithreaded( Multithreaded );
        }
    }

    return signalFlags;
}
