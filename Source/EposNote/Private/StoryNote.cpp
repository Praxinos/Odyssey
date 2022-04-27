// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "StoryNote.h"

#include "Components/Widget.h"
#include "Engine/Font.h"
#include "UObject/ConstructorHelpers.h"

//---

UStoryNote::UStoryNote( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj( *UWidget::GetDefaultFontName() );
    Font = FSlateFontInfo( RobotoFontObj.Object, 12, FName( "Regular" ) );
}
