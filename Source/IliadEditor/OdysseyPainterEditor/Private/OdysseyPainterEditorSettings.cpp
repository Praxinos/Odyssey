// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorSettings.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "EngineUtils.h"

FBrushDefaults::FBrushDefaults()
    : DefaultBrush(FSoftObjectPath(TEXT("/Odyssey/Brushes/Bitmap_Tools/OB_Penbrush1.OB_Penbrush1")))
{
}

UOdysseyPainterEditorSettings::UOdysseyPainterEditorSettings( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
    , Background( kOdysseyPainterEditorBackground_Checkered )
    , BackgroundColor( FColor( 127, 127, 127 ) )
    , CheckerColorOne( FColor( 255, 255, 255 ) )
    , CheckerColorTwo( FColor( 247, 247, 247 ) )
    , CheckerSize( 16 )
    , FitToViewport( true )
    , TextureBorderColor( FColor::White )
    , TextureBorderEnabled( false )
    , BrushDefaults()
{
    CheckerPresets.Add( FCheckboardPreset( TEXT( "Light" ), FColor( 255, 255, 255 ), FColor( 247, 247, 247 ), 16 ) );
    CheckerPresets.Add( FCheckboardPreset( TEXT( "Medium" ), FColor( 230, 230, 230 ), FColor( 220, 220, 220 ), 16 ) );
    CheckerPresets.Add( FCheckboardPreset( TEXT( "Dark" ), FColor( 205, 205, 205 ), FColor( 190, 190, 190 ), 16 ) );
}

//Static
UOdysseyPainterEditorSettings*
UOdysseyPainterEditorSettings::Get()
{
    return CastChecked<UOdysseyPainterEditorSettings>(UOdysseyPainterEditorSettings::StaticClass()->GetDefaultObject());
}

void
UOdysseyPainterEditorSettings::SetInteractiveMode()
{
    bIsInteractiveMode = true;
}
void
UOdysseyPainterEditorSettings::RemoveInteractiveMode()
{
    bIsInteractiveMode = false;
}

#if WITH_EDITOR
void
UOdysseyPainterEditorSettings::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if( PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorSettings, BackgroundColor ) )
    {
        OnBackgroundColorChangedEvent.Broadcast();
    }
    else if( PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorSettings, CheckerColorOne ) )
    {
        OnCheckerColorChangedEvent.Broadcast();
    }
    else if( PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorSettings, CheckerColorTwo ) )
    {
        OnCheckerColorChangedEvent.Broadcast();
    }
    else if( PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorSettings, CheckerSize ) )
    {
        CheckerSize = FMath::Max( 2u, FMath::RoundUpToPowerOfTwo( CheckerSize ) );
        OnCheckerSizeChangedEvent.Broadcast();
    }
    else if( PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorSettings, CheckerPresets ) )
    {
        if( PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED( FCheckboardPreset, Size ) )
        {
            for( FCheckboardPreset& preset : CheckerPresets )
                preset.Size = FMath::Max( 2u, FMath::RoundUpToPowerOfTwo( preset.Size ) );
        }
    }

    Super::PostEditChangeProperty( PropertyChangedEvent );
}
#endif

//---

FColor
UOdysseyPainterEditorSettings::GetBackgroundColor() const
{
    return BackgroundColor;
}
void
UOdysseyPainterEditorSettings::SetBackgroundColor( FColor iBackgroundColor )
{
    if( BackgroundColor == iBackgroundColor )
        return;

    BackgroundColor = iBackgroundColor;
    if( !bIsInteractiveMode )
        SaveConfig();
    OnBackgroundColorChangedEvent.Broadcast();
}
UOdysseyPainterEditorSettings::FOnBackgroundColorChanged&
UOdysseyPainterEditorSettings::GetOnBackgroundColorChanged()
{
    return OnBackgroundColorChangedEvent;
}

FColor
UOdysseyPainterEditorSettings::GetCheckerColorOne() const
{
    return CheckerColorOne;
}
FColor
UOdysseyPainterEditorSettings::GetCheckerColorTwo() const
{
    return CheckerColorTwo;
}
void
UOdysseyPainterEditorSettings::SetCheckerColorOne( FColor iCheckerColorOne )
{
    if( CheckerColorOne == iCheckerColorOne )
        return;

    CheckerColorOne = iCheckerColorOne;
    if( !bIsInteractiveMode )
        SaveConfig();
    OnCheckerColorChangedEvent.Broadcast();
}
void
UOdysseyPainterEditorSettings::SetCheckerColorTwo( FColor iCheckerColorTwo )
{
    if( CheckerColorTwo == iCheckerColorTwo )
        return;

    CheckerColorTwo = iCheckerColorTwo;
    if( !bIsInteractiveMode )
        SaveConfig();
    OnCheckerColorChangedEvent.Broadcast();
}
void
UOdysseyPainterEditorSettings::SetCheckerColor( FColor iCheckerColorOne, FColor iCheckerColorTwo )
{
    if( CheckerColorOne == iCheckerColorOne && CheckerColorTwo == iCheckerColorTwo )
        return;

    CheckerColorOne = iCheckerColorOne;
    CheckerColorTwo = iCheckerColorTwo;
    if( !bIsInteractiveMode )
        SaveConfig();
    OnCheckerColorChangedEvent.Broadcast();
}
UOdysseyPainterEditorSettings::FOnBackgroundColorChanged&
UOdysseyPainterEditorSettings::GetOnCheckerColorChanged()
{
    return OnCheckerColorChangedEvent;
}

int32
UOdysseyPainterEditorSettings::GetCheckerSize() const
{
    return CheckerSize;
}
void
UOdysseyPainterEditorSettings::SetCheckerSize( int32 iCheckerSize )
{
    if( CheckerSize == iCheckerSize )
        return;

    CheckerSize = FMath::Max( 2u, FMath::RoundUpToPowerOfTwo( iCheckerSize ) );
    if( !bIsInteractiveMode )
        SaveConfig();
    OnCheckerSizeChangedEvent.Broadcast();
}
UOdysseyPainterEditorSettings::FOnCheckerSizeChanged&
UOdysseyPainterEditorSettings::GetOnCheckerSizeChanged()
{
    return OnCheckerSizeChangedEvent;
}

TArray<FCheckboardPreset>
UOdysseyPainterEditorSettings::GetCheckerPresets() const
{
    return CheckerPresets;
}
