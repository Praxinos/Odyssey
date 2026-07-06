// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeEditorSettings.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "EngineUtils.h"

UArianeEditorSettings::UArianeEditorSettings( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
    , DistanceToNewActor( 5000 )
    , DefaultPathDrawingMaterial( Cast<UMaterial>( StaticLoadObject( UMaterial::StaticClass()
                                                                   , nullptr
                                                                   , TEXT("/Odyssey/Materials/ArianeDefaultMaterial.ArianeDefaultMaterial") ) ) )
    , GridSize( 4000 )
    , GridOpacity( 0.250f )
    , GridColor( FLinearColor::Gray )
    , GridXAxisColor( FLinearColor::Red )
    , GridYAxisColor( FLinearColor::Green )
    , HUDForegroundColor ( FColor( 0, 169, 157, 255 ) ) // Odyssey's teal

{
}

//Static
UArianeEditorSettings*
UArianeEditorSettings::Get()
{
    return CastChecked<UArianeEditorSettings>(UArianeEditorSettings::StaticClass()->GetDefaultObject());
}

#if WITH_EDITOR
void
UArianeEditorSettings::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if( PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED( UArianeEditorSettings, GridSize ) )
    {
        //OnGridSizeChanged.Broadcast();
    }

    Super::PostEditChangeProperty( PropertyChangedEvent );
}
#endif

double
UArianeEditorSettings::GetGridSize() const
{
    return GridSize;
}

void
UArianeEditorSettings::SetGridSize( double InGridSize )
{
    if( GridSize == InGridSize )
        return;

    GridSize = InGridSize;

/** Gary
    if( !bIsInteractiveMode )
        SaveConfig();
*/

    //OnGridSizeChanged.Broadcast();
}

/*
UArianeEditorSettings::FOnGridSizeChanged&
UArianeEditorSettings::GetOnGridSizeChanged()
{
    return OnGridSizeChanged;
}
*/


double
UArianeEditorSettings::GetGridOpacity() const
{
    return GridOpacity;
}

void
UArianeEditorSettings::SetGridOpacity( double InGridOpacity )
{
    GridOpacity = InGridOpacity;
}

FLinearColor
UArianeEditorSettings::GetGridColor() const
{
    return GridColor;
}

void
UArianeEditorSettings::SetGridColor( FLinearColor InGridColor )
{
    GridColor = InGridColor;
}

FLinearColor
UArianeEditorSettings::GetGridXAxisColor() const
{
    return GridXAxisColor;
}

void
UArianeEditorSettings::SetGridXAxisColor( FLinearColor InGridXAxisColor )
{
    GridXAxisColor = InGridXAxisColor;
}

FLinearColor
UArianeEditorSettings::GetGridYAxisColor() const
{
    return GridYAxisColor;
}

void
UArianeEditorSettings::SetGridYAxisColor( FLinearColor InGridYAxisColor )
{
    GridYAxisColor = InGridYAxisColor;
}

double
UArianeEditorSettings::GetDistanceToNewActor() const
{
    return DistanceToNewActor;
}

void
UArianeEditorSettings::SetDistanceToNewActor( double InDistanceToNewActor )
{
    DistanceToNewActor = InDistanceToNewActor;
}


FColor
UArianeEditorSettings::GetHUDForegroundColor() const
{
    return HUDForegroundColor;
}

void
UArianeEditorSettings::SetDefaultPathDrawingMaterial( UMaterial* InDefaultPathDrawingMaterial )
{
    DefaultPathDrawingMaterial = InDefaultPathDrawingMaterial;
}

UMaterial*
UArianeEditorSettings::GetDefaultPathDrawingMaterial() const
{
    return DefaultPathDrawingMaterial;
}
