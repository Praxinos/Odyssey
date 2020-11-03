// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateColor.h"
#include "Styling/StyleDefaults.h"
#include "Styling/ISlateStyle.h"

struct FSlateDynamicImageBrush;

/**
 * A collection of named properties that guide the appearance of Slate.
 */
class ODYSSEYSTYLE_API FOdysseyStyle
{
public:
    template< class T >
    static const T&             GetWidgetStyle( FName PropertyName, const ANSICHAR* Specifier = NULL );

    static float                GetFloat( FName PropertyName, const ANSICHAR* Specifier = NULL );
    static FVector2D            GetVector( FName PropertyName, const ANSICHAR* Specifier = NULL );
    static const FLinearColor&  GetColor( FName PropertyName, const ANSICHAR* Specifier = NULL );
    static const FSlateColor    GetSlateColor( FName PropertyName, const ANSICHAR* Specifier = NULL );
    static const FMargin&       GetMargin( FName PropertyName, const ANSICHAR* Specifier = NULL );
    static const FSlateBrush*   GetBrush( FName PropertyName, const ANSICHAR* Specifier = NULL );
    static const TSharedPtr< FSlateDynamicImageBrush > GetDynamicImageBrush( FName BrushTemplate, FName TextureName, const ANSICHAR* Specifier = NULL );
    static const TSharedPtr< FSlateDynamicImageBrush > GetDynamicImageBrush( FName BrushTemplate, const ANSICHAR* Specifier, class UTexture2D* TextureResource, FName TextureName );
    static const TSharedPtr< FSlateDynamicImageBrush > GetDynamicImageBrush( FName BrushTemplate, class UTexture2D* TextureResource, FName TextureName );
    static const FSlateSound&   GetSound( FName PropertyName, const ANSICHAR* Specifier = NULL );
    static FSlateFontInfo       GetFontStyle( FName PropertyName, const ANSICHAR* Specifier = NULL );
    static const FSlateBrush*   GetDefaultBrush();
    static const FSlateBrush*   GetNoBrush();
    static const FSlateBrush*   GetOptionalBrush( FName PropertyName, const ANSICHAR* Specifier = NULL, const FSlateBrush* const DefaultBrush = FStyleDefaults::GetNoBrush() );
    static void                 GetResources( TArray< const FSlateBrush* >& OutResources );

    static const FName&         GetStyleSetName();

public:
    static void ResetToDefault();

protected:
    static void SetStyle( const TSharedRef< ISlateStyle >& iNewStyle );

private:
    /** Singleton instance of the slate style */
    static TSharedPtr< ISlateStyle > smInstance;
};

//---

template< class T >
/*static*/
const T& 
FOdysseyStyle::GetWidgetStyle( FName PropertyName, const ANSICHAR* Specifier )
{
    return smInstance->GetWidgetStyle< T >( PropertyName, Specifier );
}
