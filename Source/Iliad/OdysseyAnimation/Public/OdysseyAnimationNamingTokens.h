// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "NamingTokens.h"
#include "OdysseyAnimation.h"

#include "OdysseyAnimationNamingTokens.generated.h"

class UOdysseyAnimation;
class UOdysseyAnimationPlayer;

/** Context object that lets callers supply an animation asset when evaluating tokens */
UCLASS()
class UOdysseyAnimationNamingTokensContext
    : public UObject
{
    GENERATED_BODY()

public:
    /** The animation asset to use when evaluating tokens */
    TWeakObjectPtr<UOdysseyAnimation> Animation;
};

/** Context object that lets callers supply an animation player when evaluating tokens */
UCLASS()
class UOdysseyAnimationPlayerNamingTokensContext
    : public UObject
{
    GENERATED_BODY()

public:
    /** The animation player to use when evaluating tokens */
    TWeakObjectPtr<UOdysseyAnimationPlayer> AnimationPlayer;
};

//---

/* Naming Tokens related to Odyssey Animation */
UCLASS(MinimalAPI, NotBlueprintable)
class UOdysseyAnimationNamingTokens
    : public UNamingTokens
{
    GENERATED_BODY()

protected:
    // ~Begin UNamingTokens
    virtual void OnCreateDefaultTokens(TArray<FNamingTokenData>& Tokens) override;
    virtual void OnPreEvaluate_Implementation(const FNamingTokensEvaluationData& InEvaluationData) override;
    virtual void OnPostEvaluate_Implementation() override;
    // ~End UNamingTokens

public:

    UOdysseyAnimationNamingTokens();

    static ODYSSEYANIMATION_API FString TokenNamespace;

    /** Utility function that will evaluate the input string with the naming tokens subsystem and return the result evaluated text using the input animation */
    //static ODYSSEYANIMATION_API FText GetResolvedText(const FString& InStringToEvaluate, UOdysseyAnimation* InAnimation);

    /** Adds a metadata token with the same name as the input key which will return the result of value of that metadata if found in the Animation */
    //ODYSSEYANIMATION_API void AddMetadataToken(const FString& InTokenKey);

private:
    /** The current animation context to use when evaluating tokens */
    TObjectPtr<UOdysseyAnimationNamingTokensContext> ContextAnimation;
    /** The current animation player context to use when evaluating tokens */
    TObjectPtr<UOdysseyAnimationPlayerNamingTokensContext> ContextPlayer;
};
