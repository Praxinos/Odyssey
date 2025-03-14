// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorVectorPathView.h"

UOdysseyPainterEditorVectorPathView::~UOdysseyPainterEditorVectorPathView()
{
}

UOdysseyPainterEditorVectorPathView::UOdysseyPainterEditorVectorPathView()
    : UOdysseyPainterEditorVectorObjectView()
    , Brush ( nullptr )
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
