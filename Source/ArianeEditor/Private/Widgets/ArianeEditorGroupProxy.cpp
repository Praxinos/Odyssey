// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor Headers
#include "ArianeEditorGroupProxy.h"
#include "ArianeEditor.h"
#include "ArianeEditorSettings.h"
// Ariane Headers
#include "ArianeGroup.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorGroupProxy::~UArianeEditorGroupProxy()
{
}

UArianeEditorGroupProxy::UArianeEditorGroupProxy()
    : HUDForegroundColor ( GetDefault<UArianeEditorSettings>() ? GetDefault<UArianeEditorSettings>()->GetHUDForegroundColor()
                                                               : FColor::Black )
{
    const UArianeEditorSettings* Settings = GetDefault<UArianeEditorSettings>();

    //bDisplayBackgroundProperties = false;
    //bDisplayForegroundProperties = false;
}

void
UArianeEditorGroupProxy::ImportParamFromOtherProxy( UArianeEditorObjectProxy* OtherProxy )
{
    UArianeEditorGroupProxy* OtherGroupProxy = Cast<UArianeEditorGroupProxy>(OtherProxy);

    if( OtherGroupProxy )
    {
        GroupPropertyBits = OtherGroupProxy->GroupPropertyBits;

        HUDForegroundColor = OtherGroupProxy->HUDForegroundColor;
    }

    Super::ImportParamFromOtherProxy( OtherProxy );
}

void
UArianeEditorGroupProxy::ImportParam( const TArray<FArianeObject*>& EditedObjects )
{
    Super::ImportParam( EditedObjects );

    for( FArianeObject* EditedObject : EditedObjects )
    {
        if( EditedObject->HasBaseClass( FArianeGroup::StaticClass() ) )
        {
            FArianeGroup* ModifiedGroup = static_cast<FArianeGroup*>(EditedObject);

            // Category "Appearance"
            HUDForegroundColor = ModifiedGroup->GetHUDForegroundColor();

            break; // only one for now
        }
    }
}

void
UArianeEditorGroupProxy::ClearPropertyBits()
{
    Super::ClearPropertyBits();

    memset( &GroupPropertyBits, 0, sizeof( GroupPropertyBits ) );
}

bool
UArianeEditorGroupProxy::HasAnyPropertyBit()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( GroupPropertyBits ); i++  )
    {
        if( GroupPropertyBits.raw[i] )
        {
            return true;
        }
    }

    return Super::HasAnyPropertyBit();
}

bool
UArianeEditorGroupProxy::GetPropertyBit( const FName& iPropertyName )
{
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorGroupProxy, HUDForegroundColor) )
        return GroupPropertyBits.HUDForegroundColor;

    return Super::GetPropertyBit( iPropertyName  );
}

void
UArianeEditorGroupProxy::ApplyPropertyBits( FArianeObject* Object )
{
    Super::ApplyPropertyBits( Object );

    if( Object->HasBaseClass( FArianeGroup::StaticClass() ) )
    {
        FArianeGroup* Group = static_cast<FArianeGroup*>(Object);

        if( GroupPropertyBits.HUDForegroundColor )
        {
            Group->SetHUDForegroundColor( HUDForegroundColor );
        }
    }
}

void
UArianeEditorGroupProxy::SetPropertyBit( const FName& PropertyName
                                       , const FName& MemberPropertyName
                                       , const FName& Category
                                       , bool bState )
{
    Super::SetPropertyBit( PropertyName
                         , MemberPropertyName
                         , Category
                         , bState );

    if( PropertyName == GET_MEMBER_NAME_CHECKED( UArianeEditorGroupProxy, HUDForegroundColor ) )
        GroupPropertyBits.HUDForegroundColor = bState;
}

#undef LOCTEXT_NAMESPACE
