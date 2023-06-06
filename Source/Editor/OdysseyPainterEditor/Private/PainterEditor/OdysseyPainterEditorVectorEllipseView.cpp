#include "OdysseyPainterEditorVectorEllipseView.h"

UOdysseyPainterEditorVectorEllipseView::~UOdysseyPainterEditorVectorEllipseView()
{
}

UOdysseyPainterEditorVectorEllipseView::UOdysseyPainterEditorVectorEllipseView()
    : UOdysseyPainterEditorVectorPathView()
{
}

void
UOdysseyPainterEditorVectorEllipseView::ImportParam( FOdysseyVectorObject* iObject )
{
    UOdysseyPainterEditorVectorPathView::ImportParam( iObject );

     EllipseParam = static_cast<FOdysseyVectorEllipse*>(iObject)->mEllipseParam;
}

void
UOdysseyPainterEditorVectorEllipseView::ExportParam( FOdysseyVectorObject* iObject )
{
    UOdysseyPainterEditorVectorPathView::ExportParam( iObject );

    static_cast<FOdysseyVectorEllipse*>(iObject)->mEllipseParam = EllipseParam;
}

void
UOdysseyPainterEditorVectorEllipseView::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    FOdysseyVectorScene* scene = mObject->GetScene();

    UOdysseyPainterEditorVectorPathView::PropertyChanged( iPropertyName, iCategory );

    if( iCategory == "Geometry" )
    {
        mObject->Update( 0 );
    }

    mObject->Invalidate( FOdysseyVectorObject::INVALIDATE_ALL );

    scene->Update(0);
}
