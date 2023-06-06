#include "OdysseyPainterEditorVectorPathView.h"

UOdysseyPainterEditorVectorPathView::~UOdysseyPainterEditorVectorPathView()
{
}

UOdysseyPainterEditorVectorPathView::UOdysseyPainterEditorVectorPathView()
    : UOdysseyPainterEditorVectorObjectView()
{
}

void
UOdysseyPainterEditorVectorPathView::ImportParam( FOdysseyVectorObject* iObject )
{
    UOdysseyPainterEditorVectorObjectView::ImportParam( iObject );

     PathParam = static_cast<FOdysseyVectorPath*>(iObject)->mPathParam;
}

void
UOdysseyPainterEditorVectorPathView::ExportParam( FOdysseyVectorObject* iObject )
{
    UOdysseyPainterEditorVectorObjectView::ExportParam( iObject );

    static_cast<FOdysseyVectorPath*>(iObject)->mPathParam = PathParam;
}

void
UOdysseyPainterEditorVectorPathView::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    UOdysseyPainterEditorVectorObjectView::PropertyChanged( iPropertyName, iCategory );

    /*if( iCategory == "Transform" )
    {
        mObject->UpdateMatrix();
    }*/
}
