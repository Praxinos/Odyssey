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

void
UOdysseyPainterEditorVectorPathView::PropertyChanged( const FName& iPropertyName
                                                    , const FName& iMemberPropertyName
                                                    , const FName& iCategory)
{
    UOdysseyPainterEditorVectorObjectView::PropertyChanged( iPropertyName
                                                          , iMemberPropertyName
                                                          , iCategory );

    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* selectedPath = static_cast<FOdysseyVectorPath*>(selectedObject);

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, JointType) )
                selectedPath->SetJointType( JointType, true );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, Brush) )
                selectedPath->SetBrush( Brush );

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, MiterLimit) )
                selectedPath->SetMiterLimit( MiterLimit, true );
        }
    }
}
