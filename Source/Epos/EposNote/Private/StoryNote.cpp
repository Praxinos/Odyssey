// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
