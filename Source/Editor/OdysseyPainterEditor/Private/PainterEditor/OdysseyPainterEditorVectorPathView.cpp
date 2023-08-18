#include "OdysseyPainterEditorVectorPathView.h"

UOdysseyPainterEditorVectorPathView::~UOdysseyPainterEditorVectorPathView()
{
}

UOdysseyPainterEditorVectorPathView::UOdysseyPainterEditorVectorPathView()
    : UOdysseyPainterEditorVectorObjectView()
{
}

void
UOdysseyPainterEditorVectorPathView::ImportParam()
{
    std::list<FOdysseyVectorObject*>::iterator it;

    UOdysseyPainterEditorVectorObjectView::ImportParam();

    for( it = mFocusedObjectList.begin(); it != mFocusedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = (*it);

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* selectedPath = static_cast<FOdysseyVectorPath*>(selectedObject);

            PathParam = selectedPath->mPathParam;

            break; // only one
        }
    }
}

uint64
UOdysseyPainterEditorVectorPathView::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    uint64 signalFlags = UOdysseyPainterEditorVectorObjectView::PropertyChanged( iPropertyName, iCategory );

    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* selectedPath = static_cast<FOdysseyVectorPath*>(selectedObject);

            if( iPropertyName == "JointType" )
                selectedPath->mPathParam.JointType = PathParam.JointType;

            if( iPropertyName == "Filled" )
                selectedPath->mPathParam.Filled = PathParam.Filled;
        }
    }

    return signalFlags;
}
