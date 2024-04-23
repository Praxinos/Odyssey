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
    UOdysseyPainterEditorVectorObjectView::ImportParam();

    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* selectedPath = static_cast<FOdysseyVectorPath*>(selectedObject);

            JointType  = selectedPath->GetJointType();
            Brush      = selectedPath->GetBrush();
            MiterLimit = selectedPath->GetMiterLimit();

            break; // only one
        }
    }
}

uint64
UOdysseyPainterEditorVectorPathView::PropertyChanged( const FName& iPropertyName
                                                    , const FName& iMemberPropertyName
                                                    , const FName& iCategory)
{
    uint64 signalFlags = UOdysseyPainterEditorVectorObjectView::PropertyChanged( iPropertyName
                                                                               , iMemberPropertyName
                                                                               , iCategory );

    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* selectedPath = static_cast<FOdysseyVectorPath*>(selectedObject);

            if( iPropertyName == "JointType" )
                selectedPath->SetJointType( JointType, true );

            if( iPropertyName == "Brush" )
                selectedPath->SetBrush( Brush );

            if( iPropertyName == "MiterLimit" )
                selectedPath->SetMiterLimit( MiterLimit, true );
        }
    }

    return signalFlags;
}
