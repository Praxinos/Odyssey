// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

#include "CoreMinimal.h"
#include "Evaluation/MovieSceneEvalTemplate.h"
#include "ArianeImageMovieSceneSection.h"

#include "ArianeImageMovieSceneEvalTemplate.generated.h"

struct FArianeImageExecutionToken : IMovieSceneExecutionToken
{
    const FArianeImageKeyData* KeyData;
    const FArianeImageKeyData* NextKeyData;
    float T;
    FMovieSceneEvaluationOperand StoredOperand;

    FArianeImageExecutionToken( const FArianeImageKeyData* InKeyData
                              , const FArianeImageKeyData* InNextKeyData
                              , float InT
                              , const FMovieSceneEvaluationOperand& InOperand );
    virtual void Execute( const FMovieSceneContext& Context
                        , const FMovieSceneEvaluationOperand& Operand
                        , FPersistentEvaluationData& PersistentData
                        , IMovieScenePlayer& Player ) override;
};

USTRUCT()
struct ARIANE_API FArianeImageMovieSceneEvalTemplate : public FMovieSceneEvalTemplate
{
    GENERATED_BODY()

    FArianeImageMovieSceneEvalTemplate();
    FArianeImageMovieSceneEvalTemplate( const UArianeImageMovieSceneSection* InSection );

    virtual UScriptStruct& GetScriptStructImpl() const override;

    // executes at each frame to  apply the correct ArianeImage to the Layer
    virtual void Evaluate( const FMovieSceneEvaluationOperand& Operand
                         , const FMovieSceneContext& Context
                         , const FPersistentEvaluationData& PersistentData
                         , FMovieSceneExecutionTokens& ExecutionTokens ) const override;

protected:
    UPROPERTY()
    const UArianeImageMovieSceneSection* Section;
};
