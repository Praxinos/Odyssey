// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "SequenceNameElements.generated.h"

USTRUCT()
struct EPOSSEQUENCE_API FSequenceNameElements
{
    GENERATED_BODY()

public:
    /** The studio name. */
    UPROPERTY( EditAnywhere, Category="Name Elements" )
    FString StudioName;
    /** The studio acronym. */
    UPROPERTY( EditAnywhere, Category="Name Elements" )
    FString StudioAcronym;

    /** The license name. */
    UPROPERTY( EditAnywhere, Category="Name Elements" )
    FString LicenseName;
    /** The license acronym. */
    UPROPERTY( EditAnywhere, Category="Name Elements" )
    FString LicenseAcronym;

    /** The title of the production. */
    UPROPERTY( EditAnywhere, Category="Name Elements" )
    FString ProductionName;
    /** The acronym of the production. */
    UPROPERTY( EditAnywhere, Category="Name Elements" )
    FString ProductionAcronym;

    /** Is it a serie?. */
    UPROPERTY( EditAnywhere, Category="Name Elements", meta=(InlineEditConditionToggle) )
    bool IsSerie { false };

    /** The season number. */
    UPROPERTY( EditAnywhere, Category="Name Elements", meta=(EditCondition="IsSerie") )
    int32 Season { INDEX_NONE };

    /** The episode number. */
    UPROPERTY( EditAnywhere, Category="Name Elements", meta=(EditCondition="IsSerie") )
    int32 Episode { INDEX_NONE };

    /** The part of the production. */
    UPROPERTY( EditAnywhere, Category="Name Elements" )
    FString Part;

    ///** The department name. */
    //UPROPERTY( EditAnywhere, Category="Name Elements" )
    //FString DepartmentName;
    ///** The department acronym. */
    //UPROPERTY( EditAnywhere, Category="Name Elements" )
    //FString DepartmentAcronym;

    /** The initials of the user. */
    UPROPERTY( EditAnywhere, Category="Name Elements" )
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
    UPROPERTY( EditAnywhere, Category="Name Elements" )
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
    UPROPERTY( EditAnywhere, Category="Name Elements" )
    int32 Index { INDEX_NONE };

    /** The current take index. */
    UPROPERTY( EditAnywhere, Category="Name Elements" )
    int32 TakeIndex { INDEX_NONE };
 };
