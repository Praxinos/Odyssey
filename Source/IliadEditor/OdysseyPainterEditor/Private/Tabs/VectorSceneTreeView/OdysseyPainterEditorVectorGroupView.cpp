// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorVectorGroupView.h"
//#include "Undo/OdysseyVectorUndoPropertyChanged.h"
#include "Undo/OdysseyVectorUndoObjectParam.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "HUD/OdysseyVectorHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

UOdysseyPainterEditorVectorGroupView::~UOdysseyPainterEditorVectorGroupView()
{
}

UOdysseyPainterEditorVectorGroupView::UOdysseyPainterEditorVectorGroupView()
    : HUDColor ( FOdysseyVectorHUD::GetForegroundColor() )
{
    bDisplayBackgroundProperties = false;
    bDisplayForegroundProperties = false;
}

void
UOdysseyPainterEditorVectorGroupView::ImportParamFromOtherView( UOdysseyPainterEditorVectorObjectView* iOtherView )
{
    UOdysseyPainterEditorVectorGroupView* otherGroupView = Cast<UOdysseyPainterEditorVectorGroupView>(iOtherView);

    if( otherGroupView )
    {
        mGroupPropertyBits = otherGroupView->mGroupPropertyBits;

        HUDColor = otherGroupView->HUDColor;
    }

    UOdysseyPainterEditorVectorObjectView::ImportParamFromOtherView( iOtherView );
}

void
UOdysseyPainterEditorVectorGroupView::ImportParam( const std::list<FOdysseyVectorObject*>& iFocusedObjectList )
{
    UOdysseyPainterEditorVectorObjectView::ImportParam( iFocusedObjectList );

    for( FOdysseyVectorObject* selectedObject : iFocusedObjectList )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
        {
            FOdysseyVectorGroup* selectedGroup = static_cast<FOdysseyVectorGroup*>(selectedObject);

            // Category "Appearance"
            HUDColor = selectedGroup->GetHUDColor();

            break; // only one for now
        }
    }
}

void
UOdysseyPainterEditorVectorGroupView::ClearPropertyBits()
{
    UOdysseyPainterEditorVectorObjectView::ClearPropertyBits();

    memset( &mGroupPropertyBits, 0, sizeof( mGroupPropertyBits ) );
}

bool
UOdysseyPainterEditorVectorGroupView::HasAnyPropertyBit()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( mGroupPropertyBits ); i++  )
    {
        if( mGroupPropertyBits.raw[i] )
        {
            return true;
        }
    }

    return UOdysseyPainterEditorVectorObjectView::HasAnyPropertyBit();
}

bool
UOdysseyPainterEditorVectorGroupView::GetPropertyBit( const FName& iPropertyName )
{
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupView, HUDColor) )
        return mGroupPropertyBits.HUDColor;

    return UOdysseyPainterEditorVectorObjectView::GetPropertyBit( iPropertyName  );
}

void
UOdysseyPainterEditorVectorGroupView::ApplyPropertyBits( FOdysseyVectorObject* iObject )
{
    UOdysseyPainterEditorVectorObjectView::ApplyPropertyBits( iObject );

    if( iObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
    {
        FOdysseyVectorGroup* group = static_cast<FOdysseyVectorGroup*>(iObject);

        if( mGroupPropertyBits.HUDColor )
            group->SetHUDColor( HUDColor );
    }
}

void
UOdysseyPainterEditorVectorGroupView::SetPropertyBit( const FName& iPropertyName
                                                    , const FName& iMemberPropertyName
                                                    , const FName& iCategory
                                                    , bool iState )
{
    UOdysseyPainterEditorVectorObjectView::SetPropertyBit( iPropertyName
                                                        , iMemberPropertyName
                                                        , iCategory
                                                        , iState );

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupView, HUDColor) )
        mGroupPropertyBits.HUDColor = iState;
}

#undef LOCTEXT_NAMESPACE
