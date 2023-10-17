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

            JointType = selectedPath->GetJointType();
            Brush = selectedPath->GetBrush();

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
                selectedPath->SetJointType( JointType );

            if( iPropertyName == "Brush" )
                selectedPath->SetBrush( Brush );
        }
    }

    return signalFlags;
}
