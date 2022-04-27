// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Fonts/SlateFontInfo.h"

#include "StoryNote.generated.h"

/*
 * A note.
 */
UCLASS( BlueprintType )
class EPOSNOTE_API UStoryNote
    : public UObject
{
public:
    GENERATED_BODY()

    UStoryNote( const FObjectInitializer& ObjectInitializer );

public:
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "StoryNote", meta=(MultiLine="true") )
    FString Text;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "StoryNote" )
    FSlateFontInfo Font;
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "StoryNote" )
    FLinearColor ColorAndOpacity { FLinearColor::White };
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "StoryNote" )
    FVector2D ShadowOffset { FVector2D::UnitVector };
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "StoryNote" )
    FLinearColor ShadowColorAndOpacity { FLinearColor::Black };
};
