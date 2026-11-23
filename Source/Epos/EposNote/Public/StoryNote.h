// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
    /**
     * Text of the note.
     *
     * Can be used for dialogues, actions, notes for the staff,
     * explanations, questions, etc.
     */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "StoryNote", meta=(MultiLine="true") )
    FString Text;

    /** Font used to display the note text. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "StoryNote" )
    FSlateFontInfo Font;
    /** Color and opacity of the note text. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "StoryNote" )
    FLinearColor ColorAndOpacity { FLinearColor::White };
    /** Offset of the shadow drawn behind the note text. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "StoryNote" )
    FVector2D ShadowOffset { FVector2D::UnitVector };
    /** Color and opacity of the shadow drawn behind the note text. */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "StoryNote" )
    FLinearColor ShadowColorAndOpacity { FLinearColor::Black };
};
