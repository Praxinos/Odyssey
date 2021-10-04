// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

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
    UPROPERTY( BlueprintReadWrite, EditAnywhere, Category = "StoryNote", meta=(MultiLine))
    FString Text;

    UPROPERTY( BlueprintReadWrite, EditAnywhere, Category = "StoryNote" )
    int TmpSize;

    UPROPERTY( BlueprintReadWrite, EditAnywhere, Category = "StoryNote" )
    FLinearColor TmpColor;
};
