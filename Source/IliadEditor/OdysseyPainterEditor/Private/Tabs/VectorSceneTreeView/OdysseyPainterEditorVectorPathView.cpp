// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorVectorPathView.h"

UOdysseyPainterEditorVectorPathView::~UOdysseyPainterEditorVectorPathView()
{
}

UOdysseyPainterEditorVectorPathView::UOdysseyPainterEditorVectorPathView()
    : UOdysseyPainterEditorVectorObjectView()
    , PathWidth ( 100.0f )
    , Brush ( nullptr )
    , mPathPropertyBits( { 0 } )
{
    bDisplayBackgroundProperties = false;
    bDisplayForegroundProperties = true;
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
UOdysseyPainterEditorVectorPathView::ClearPropertyBits()
{
    UOdysseyPainterEditorVectorObjectView::ClearPropertyBits();

    memset( &mPathPropertyBits, 0, sizeof( mPathPropertyBits ) );
}

bool
UOdysseyPainterEditorVectorPathView::HasPropertyBits()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( mPathPropertyBits ); i++  )
    {
        if( mPathPropertyBits.raw[i] )
        {
            return true;
        }
    }

    return UOdysseyPainterEditorVectorObjectView::HasPropertyBits();
}

void
UOdysseyPainterEditorVectorPathView::ApplyPropertyBits( FOdysseyVectorObject* iObject )
{
    UOdysseyPainterEditorVectorObjectView::ApplyPropertyBits( iObject );

    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);

        if( mPathPropertyBits.PathWidth )
        {
            for( FOdysseyVectorVertex* vertex : path->GetVertexList() )
            {
                vertex->SetRadius( vertex->GetRadius() * PathWidth * 0.01f );
            }
        }

        if( mPathPropertyBits.JointType )
            path->SetJointType( JointType, true );

        if( mPathPropertyBits.Brush )
            path->SetBrush( Brush );

        if( mPathPropertyBits.MiterLimit )
            path->SetMiterLimit( MiterLimit, true );
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

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, PathWidth) )
        mPathPropertyBits.PathWidth = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, JointType) )
        mPathPropertyBits.JointType = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, Brush) )
        mPathPropertyBits.Brush = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, MiterLimit) )
        mPathPropertyBits.MiterLimit = 1;
}
