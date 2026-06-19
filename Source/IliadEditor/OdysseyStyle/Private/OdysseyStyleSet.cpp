// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyStyle.h"

#include "Styling/SlateStyleRegistry.h"
#include "Styling/CoreStyle.h"
#include "SlateOdysseyStyle.h"

TSharedPtr< ISlateStyle > FOdysseyStyle::smInstance = nullptr;
TArray< FName > FOdysseyStyle::smTrackedStyleSets = TArray< FName >();

/*static*/
void
FOdysseyStyle::ResetToDefault()
{
    SetStyle( FCoreStyle::Create( "OdysseyStyle" ) );
}

//static
const FSlateStyleSet&
FOdysseyStyle::Get()
{
    return FSlateOdysseyStyle::Get();
}

/*static*/
void
FOdysseyStyle::SetStyle( const TSharedRef< ISlateStyle >& iNewStyle )
{
    if( smInstance == iNewStyle )
        return;

    if( smInstance.IsValid() )
    {
        FSlateStyleRegistry::UnRegisterSlateStyle( *smInstance.Get() );
    }

    smInstance = iNewStyle;

    if( smInstance.IsValid() )
    {
        FSlateStyleRegistry::RegisterSlateStyle( *smInstance.Get() );
    }
    else
    {
        ResetToDefault();
    }
}

//---

/*static*/
const FName&
FOdysseyStyle::GetStyleSetName()
{
    if( !smInstance )
    {
        static FName defaultStyleName("");
        return defaultStyleName;
    }
    return smInstance->GetStyleSetName();
}

/*static*/
float
FOdysseyStyle::GetFloat( FName PropertyName, const ANSICHAR* Specifier )
{
    if( !smInstance )
        return 0.0f; // or some default value
    return smInstance->GetFloat( PropertyName, Specifier );
}

/*static*/
FVector2D
FOdysseyStyle::GetVector( FName PropertyName, const ANSICHAR* Specifier )
{
    if( !smInstance )
        return FVector2D::ZeroVector; // or some default value
    return smInstance->GetVector( PropertyName, Specifier );
}

/*static*/
const FLinearColor&
FOdysseyStyle::GetColor( FName PropertyName, const ANSICHAR* Specifier )
{
    if( !smInstance )
        return FLinearColor::Black; // or some default value
    return smInstance->GetColor( PropertyName, Specifier );
}

/*static*/
const FSlateColor
FOdysseyStyle::GetSlateColor( FName PropertyName, const ANSICHAR* Specifier )
{
    if( !smInstance )
        return FSlateColor::UseForeground(); // or some default value
    return smInstance->GetSlateColor( PropertyName, Specifier );
}

/*static*/
const FMargin&
FOdysseyStyle::GetMargin( FName PropertyName, const ANSICHAR* Specifier )
{
    if( !smInstance )
        return FStyleDefaults::GetMargin(); // or some default value
    return smInstance->GetMargin( PropertyName, Specifier );
}

/*static*/
const FSlateBrush*
FOdysseyStyle::GetBrush( FName PropertyName, const ANSICHAR* Specifier )
{
    if( !smInstance )
        return FStyleDefaults::GetNoBrush();
    return smInstance->GetBrush( PropertyName, Specifier );
}

/*static*/
const TSharedPtr< FSlateDynamicImageBrush >
FOdysseyStyle::GetDynamicImageBrush( FName BrushTemplate, FName TextureName, const ANSICHAR* Specifier )
{
    if( !smInstance )
        return nullptr;
    return smInstance->GetDynamicImageBrush( BrushTemplate, TextureName, Specifier );
}

/*static*/
const TSharedPtr< FSlateDynamicImageBrush >
FOdysseyStyle::GetDynamicImageBrush( FName BrushTemplate, const ANSICHAR* Specifier, class UTexture2D* TextureResource, FName TextureName )
{
    if( !smInstance )
        return nullptr;
    return smInstance->GetDynamicImageBrush( BrushTemplate, Specifier, TextureResource, TextureName );
}

/*static*/
const TSharedPtr< FSlateDynamicImageBrush >
FOdysseyStyle::GetDynamicImageBrush( FName BrushTemplate, class UTexture2D* TextureResource, FName TextureName )
{
    if( !smInstance )
        return nullptr;
    return smInstance->GetDynamicImageBrush( BrushTemplate, TextureResource, TextureName );
}

/*static*/
const FSlateSound&
FOdysseyStyle::GetSound( FName PropertyName, const ANSICHAR* Specifier )
{
    if( !smInstance )
        return FStyleDefaults::GetSound(); // or some default value
    return smInstance->GetSound( PropertyName, Specifier );
}

/*static*/
FSlateFontInfo
FOdysseyStyle::GetFontStyle( FName PropertyName, const ANSICHAR* Specifier )
{
    if( !smInstance )
        return FStyleDefaults::GetFontInfo(); // or some default value
    return smInstance->GetFontStyle( PropertyName, Specifier );
}

/*static*/
const FSlateBrush*
FOdysseyStyle::GetDefaultBrush()
{
    if( !smInstance )
        return FStyleDefaults::GetNoBrush();
    return smInstance->GetDefaultBrush();
}

/*static*/
const FSlateBrush*
FOdysseyStyle::GetNoBrush()
{
    return FStyleDefaults::GetNoBrush();
}

/*static*/
const FSlateBrush*
FOdysseyStyle::GetOptionalBrush( FName PropertyName, const ANSICHAR* Specifier, const FSlateBrush* const DefaultBrush )
{
    if( !smInstance )
        return FStyleDefaults::GetNoBrush();
    return smInstance->GetOptionalBrush( PropertyName, Specifier, DefaultBrush );
}

/*static*/
void
FOdysseyStyle::GetResources( TArray< const FSlateBrush* >& OutResources )
{
    if( !smInstance )
        return;
    smInstance->GetResources( OutResources );
}

/*static*/
TArray< FName >&
FOdysseyStyle::GetTrackedStyleSets()
{
    return FOdysseyStyle::smTrackedStyleSets;
}
