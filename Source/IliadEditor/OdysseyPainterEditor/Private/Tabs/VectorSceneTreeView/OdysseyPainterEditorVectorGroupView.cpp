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
UOdysseyPainterEditorVectorGroupView::ImportParam()
{
    UOdysseyPainterEditorVectorObjectView::ImportParam();

    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
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
UOdysseyPainterEditorVectorGroupView::PropertyChanged( const FName& iPropertyName
                                                     , const FName& iMemberPropertyName
                                                     , const FName& iCategory )
{
    UOdysseyPainterEditorVectorObjectView::PropertyChanged( iPropertyName
                                                          , iMemberPropertyName
                                                          , iCategory );

    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
        {
            FOdysseyVectorGroup* selectedGroup = static_cast<FOdysseyVectorGroup*>(selectedObject);

            if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorGroupView, HUDColor) )
                selectedGroup->SetHUDColor( HUDColor );
        }
    }
}


#undef LOCTEXT_NAMESPACE
