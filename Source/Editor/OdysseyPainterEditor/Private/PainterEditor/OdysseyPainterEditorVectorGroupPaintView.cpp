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
            IntersectsCanevas = selectedPaintGroup->IntersectsCanevas();

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

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Painted) )
                selectedPaintGroup->SetPainted( Painted );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Monochrome) )
                selectedPaintGroup->SetMonochrome( Monochrome );

            // Note: iMemberPropertyName because FColor is a struct 
            // and we can edit individual struct members RGBA
            if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, MonochromeColor) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, MonochromeColor) ) )
                selectedPaintGroup->SetMonochromeColor( MonochromeColor );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Realtime) )
                selectedPaintGroup->SetRealtime( Realtime );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, GapTolerance) )
                selectedPaintGroup->SetGapTolerance( GapTolerance );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, IntersectsCanevas) )
                selectedPaintGroup->SetIntersectsCanevas( IntersectsCanevas );

            //if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Wireframe) )
            //    selectedPaintGroup->SetWireframe( Wireframe );

            // Note: iMemberPropertyName because FColor is a struct 
            // and we can edit individual struct members RGBA
            if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, WireframeColor) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, WireframeColor) ) )
                selectedPaintGroup->SetWireframeColor( WireframeColor );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupPaintView, Multithreaded) )
                selectedPaintGroup->SetMultithreaded( Multithreaded );
        }
    }

    return signalFlags;
}
