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
    std::list<FOdysseyVectorObject*>& selectedObjectList = mScene->GetSelectedObjectList();

    UOdysseyPainterEditorVectorObjectView::ImportParam();

    for ( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = (*it);

        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            GroupPaintParam = selectedPaintGroup->mGroupPaintParam;

            break; // only one for now
        }
    }
}

void
UOdysseyPainterEditorVectorGroupPaintView::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = mScene->GetSelectedObjectList();

    UOdysseyPainterEditorVectorObjectView::PropertyChanged( iPropertyName, iCategory );

    for ( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = (*it);

        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            if( iPropertyName == "Realtime" )
                selectedPaintGroup->mGroupPaintParam.Realtime =  GroupPaintParam.Realtime;

            if( iPropertyName == "Tolerance" )
            {
                selectedPaintGroup->mGroupPaintParam.Tolerance =  GroupPaintParam.Tolerance;

                selectedPaintGroup->Invalidate( FOdysseyVectorObject::INVALIDATE_ALL );
            }

            if( iPropertyName == "Wireframe" )
                selectedPaintGroup->mGroupPaintParam.Wireframe =  GroupPaintParam.Wireframe;

            if( iPropertyName == "WireframeColor" )
                selectedPaintGroup->mGroupPaintParam.WireframeColor =  GroupPaintParam.WireframeColor;
        }
    }
}
