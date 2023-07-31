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
    std::list<FOdysseyVectorObject*>::iterator it;

    UOdysseyPainterEditorVectorObjectView::ImportParam();

    for( it = mFocusedObjectList.begin(); it != mFocusedObjectList.end(); ++it )
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
    std::list<FOdysseyVectorObject*>::iterator it;

    UOdysseyPainterEditorVectorObjectView::PropertyChanged( iPropertyName, iCategory );

    for( it = mFocusedObjectList.begin(); it != mFocusedObjectList.end(); ++it )
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

    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
}
