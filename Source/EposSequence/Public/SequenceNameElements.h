// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "SequenceNameElements.generated.h"

USTRUCT()
struct EPOSSEQUENCE_API FSequenceNameElements
{
    GENERATED_BODY()

public:
    /** The studio name. */
    UPROPERTY( EditAnywhere )
    FString StudioName;
    /** The studio acronym. */
    UPROPERTY( EditAnywhere )
    FString StudioAcronym;

    /** The license name. */
    UPROPERTY( EditAnywhere )
    FString LicenseName;
    /** The license acronym. */
    UPROPERTY( EditAnywhere )
    FString LicenseAcronym;

    /** The title of the production. */
    UPROPERTY( EditAnywhere )
    FString ProductionName;
    /** The acronym of the production. */
    UPROPERTY( EditAnywhere )
    FString ProductionAcronym;

    /** Is it a serie?. */
    UPROPERTY( EditAnywhere, meta=(InlineEditConditionToggle) )
    bool IsSerie { false };

    /** The season number. */
    UPROPERTY( EditAnywhere, meta=(EditCondition="IsSerie") )
    int32 Season { INDEX_NONE };

    /** The episode number. */
    UPROPERTY( EditAnywhere, meta=(EditCondition="IsSerie") )
    int32 Episode { INDEX_NONE };

    /** The part of the production. */
    UPROPERTY( EditAnywhere )
    FString Part;

    ///** The department name. */
    //UPROPERTY( EditAnywhere )
    //FString DepartmentName;
    ///** The department acronym. */
    //UPROPERTY( EditAnywhere )
    //FString DepartmentAcronym;

    /** The initials of the user. */
    UPROPERTY( EditAnywhere )
    FString Initials;
 };

//---

USTRUCT()
struct EPOSSEQUENCE_API FBoardNameElements
    : public FSequenceNameElements
{
    GENERATED_BODY()

 public:
     bool IsValid() const;

public:
    /** The current index. */
    UPROPERTY( EditAnywhere )
    int32 Index { INDEX_NONE };
 };

USTRUCT()
struct EPOSSEQUENCE_API FShotNameElements
    : public FSequenceNameElements
{
    GENERATED_BODY()

 public:
     bool IsValid() const;

public:
    /** The current index. */
    UPROPERTY( EditAnywhere )
    int32 Index { INDEX_NONE };

    /** The current take index. */
    UPROPERTY( EditAnywhere )
    int32 TakeIndex { INDEX_NONE };
 };
