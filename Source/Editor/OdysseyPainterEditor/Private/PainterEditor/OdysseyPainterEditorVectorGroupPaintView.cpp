#include "OdysseyPainterEditorVectorGroupPaintView.h"

UOdysseyPainterEditorVectorGroupPaintView::~UOdysseyPainterEditorVectorGroupPaintView()
{
}

UOdysseyPainterEditorVectorGroupPaintView::UOdysseyPainterEditorVectorGroupPaintView()
    : UOdysseyPainterEditorVectorObjectView()
{
}

void
UOdysseyPainterEditorVectorGroupPaintView::ImportParam( FOdysseyVectorObject* iObject )
{
    UOdysseyPainterEditorVectorObjectView::ImportParam( iObject );

    GroupPaintParam = static_cast<FOdysseyVectorGroupPaint*>(iObject)->mGroupPaintParam;
}

void
UOdysseyPainterEditorVectorGroupPaintView::ExportParam( FOdysseyVectorObject* iObject )
{
    UOdysseyPainterEditorVectorObjectView::ExportParam( iObject );

    static_cast<FOdysseyVectorGroupPaint*>(iObject)->mGroupPaintParam = GroupPaintParam;
}

void
UOdysseyPainterEditorVectorGroupPaintView::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    FOdysseyVectorScene* scene = mObject->GetScene();

    UOdysseyPainterEditorVectorObjectView::PropertyChanged( iPropertyName, iCategory );

    /*if( iCategory == "Transform" )
    {
        mObject->UpdateMatrix();
    }*/

    mObject->Invalidate( FOdysseyVectorObject::INVALIDATE_ALL );

    scene->Update( 0 );
}
