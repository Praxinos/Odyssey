// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyStyleSet.h"

#include "Styling/SlateStyleRegistry.h"
#include "Styling/CoreStyle.h"

TSharedPtr< ISlateStyle > FOdysseyStyle::smInstance = nullptr;

/*static*/
void
FOdysseyStyle::ResetToDefault()
{
    SetStyle( FCoreStyle::Create( "OdysseyStyle" ) );
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
    return smInstance->GetStyleSetName();
}

/*static*/
float
FOdysseyStyle::GetFloat( FName PropertyName, const ANSICHAR* Specifier )
{
    return smInstance->GetFloat( PropertyName, Specifier );
}

/*static*/
FVector2D
FOdysseyStyle::GetVector( FName PropertyName, const ANSICHAR* Specifier )
{
    return smInstance->GetVector( PropertyName, Specifier );
}

/*static*/
const FLinearColor&
FOdysseyStyle::GetColor( FName PropertyName, const ANSICHAR* Specifier )
{
    return smInstance->GetColor( PropertyName, Specifier );
}

/*static*/
const FSlateColor
FOdysseyStyle::GetSlateColor( FName PropertyName, const ANSICHAR* Specifier )
{
    return smInstance->GetSlateColor( PropertyName, Specifier );
}

/*static*/
const FMargin&
FOdysseyStyle::GetMargin( FName PropertyName, const ANSICHAR* Specifier )
{
    return smInstance->GetMargin( PropertyName, Specifier );
}

/*static*/
const FSlateBrush*
FOdysseyStyle::GetBrush( FName PropertyName, const ANSICHAR* Specifier )
{
    return smInstance->GetBrush( PropertyName, Specifier );
}

/*static*/
const TSharedPtr< FSlateDynamicImageBrush >
FOdysseyStyle::GetDynamicImageBrush( FName BrushTemplate, FName TextureName, const ANSICHAR* Specifier )
{
    return smInstance->GetDynamicImageBrush( BrushTemplate, TextureName, Specifier );
}

/*static*/
const TSharedPtr< FSlateDynamicImageBrush >
FOdysseyStyle::GetDynamicImageBrush( FName BrushTemplate, const ANSICHAR* Specifier, class UTexture2D* TextureResource, FName TextureName )
{
    return smInstance->GetDynamicImageBrush( BrushTemplate, Specifier, TextureResource, TextureName );
}

/*static*/
const TSharedPtr< FSlateDynamicImageBrush >
FOdysseyStyle::GetDynamicImageBrush( FName BrushTemplate, class UTexture2D* TextureResource, FName TextureName )
{
    return smInstance->GetDynamicImageBrush( BrushTemplate, TextureResource, TextureName );
}

/*static*/
const FSlateSound&
FOdysseyStyle::GetSound( FName PropertyName, const ANSICHAR* Specifier )
{
    return smInstance->GetSound( PropertyName, Specifier );
}

/*static*/
FSlateFontInfo
FOdysseyStyle::GetFontStyle( FName PropertyName, const ANSICHAR* Specifier )
{
    return smInstance->GetFontStyle( PropertyName, Specifier );
}

/*static*/
const FSlateBrush*
FOdysseyStyle::GetDefaultBrush()
{
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
    return smInstance->GetOptionalBrush( PropertyName, Specifier, DefaultBrush );
}

/*static*/
void
FOdysseyStyle::GetResources( TArray< const FSlateBrush* >& OutResources )
{
    return smInstance->GetResources( OutResources );
}
