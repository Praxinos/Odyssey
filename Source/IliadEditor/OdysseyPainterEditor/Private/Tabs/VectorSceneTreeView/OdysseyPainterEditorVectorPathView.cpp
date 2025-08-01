// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorVectorPathView.h"

UOdysseyPainterEditorVectorPathView::~UOdysseyPainterEditorVectorPathView()
{
}

UOdysseyPainterEditorVectorPathView::UOdysseyPainterEditorVectorPathView()
    : UOdysseyPainterEditorVectorObjectView()
    , mPathPropertyBits( { 0 } )
    , bDisplayWideningOptions( false )
    , WideningMode ( EPathViewWideningMode::Percent )
    , PathWidthInPercent ( 100.0f )
    , PathWidthInUnits ( 2.0f )
    , Brush ( nullptr )
{
    bDisplayBackgroundProperties = false;
    bDisplayForegroundProperties = true;
}

void
UOdysseyPainterEditorVectorPathView::SetDisplayWideningOptions( bool iValue )
{
    bDisplayWideningOptions = iValue;
}

void
UOdysseyPainterEditorVectorPathView::ImportParam( const std::list<FOdysseyVectorObject*>& iFocusedObjectList )
{
    UOdysseyPainterEditorVectorObjectView::ImportParam( iFocusedObjectList );

    for( FOdysseyVectorObject* selectedObject : iFocusedObjectList )
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
UOdysseyPainterEditorVectorPathView::HasAnyPropertyBit()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( mPathPropertyBits ); i++  )
    {
        if( mPathPropertyBits.raw[i] )
        {
            return true;
        }
    }

    return UOdysseyPainterEditorVectorObjectView::HasAnyPropertyBit();
}

bool
UOdysseyPainterEditorVectorPathView::GetPropertyBit( const FName& iPropertyName )
{
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, WideningMode) )
        return true;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, PathWidthInPercent) )
        return mPathPropertyBits.PathWidthInPercent;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, PathWidthInUnits) )
        return mPathPropertyBits.PathWidthInUnits;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, JointType) )
        return mPathPropertyBits.JointType;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, Brush) )
        return mPathPropertyBits.Brush;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, MiterLimit) )
        return mPathPropertyBits.MiterLimit;

    return UOdysseyPainterEditorVectorObjectView::GetPropertyBit( iPropertyName );
}

void
UOdysseyPainterEditorVectorPathView::ApplyPropertyBits( FOdysseyVectorObject* iObject )
{
    UOdysseyPainterEditorVectorObjectView::ApplyPropertyBits( iObject );

    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);

        if( WideningMode == EPathViewWideningMode::Percent )
        {
            if( mPathPropertyBits.PathWidthInPercent )
            {
                for( FOdysseyVectorVertex* vertex : path->GetVertexList() )
                {
                    vertex->SetRadius( vertex->GetRadius() * PathWidthInPercent * 0.01f );
                }
            }
        }

        if( WideningMode == EPathViewWideningMode::Units )
        {
            if( mPathPropertyBits.PathWidthInUnits )
            {
                for( FOdysseyVectorVertex* vertex : path->GetVertexList() )
                {
                    vertex->SetRadius( PathWidthInUnits );
                }
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
UOdysseyPainterEditorVectorPathView::SetPropertyBit( const FName& iPropertyName
                                                   , const FName& iMemberPropertyName
                                                   , const FName& iCategory
                                                   , bool iState )
{
    UOdysseyPainterEditorVectorObjectView::SetPropertyBit( iPropertyName
                                                         , iMemberPropertyName
                                                         , iCategory
                                                         , iState );
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, PathWidthInPercent) )
        mPathPropertyBits.PathWidthInPercent = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, PathWidthInUnits) )
        mPathPropertyBits.PathWidthInUnits = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, JointType) )
        mPathPropertyBits.JointType = iState;

    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, Brush) )
     || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, Brush) ) )
        mPathPropertyBits.Brush = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, MiterLimit) )
        mPathPropertyBits.MiterLimit = iState;
}
