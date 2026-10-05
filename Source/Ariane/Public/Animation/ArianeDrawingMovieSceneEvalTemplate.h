// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

#include "CoreMinimal.h"
#include "Evaluation/MovieSceneEvalTemplate.h"
#include "ArianeDrawingMovieSceneSection.h"

#include "ArianeDrawingMovieSceneEvalTemplate.generated.h"

struct FArianeDrawingExecutionToken : IMovieSceneExecutionToken
{
    const FArianeDrawingKeyData* KeyData;
    const FArianeDrawingKeyData* NextKeyData;
    float T;
    FMovieSceneEvaluationOperand StoredOperand;

    FArianeDrawingExecutionToken( const FArianeDrawingKeyData* InKeyData
                              , const FArianeDrawingKeyData* InNextKeyData
                              , float InT
                              , const FMovieSceneEvaluationOperand& InOperand );
    virtual void Execute( const FMovieSceneContext& Context
                        , const FMovieSceneEvaluationOperand& Operand
                        , FPersistentEvaluationData& PersistentData
                        , IMovieScenePlayer& Player ) override;
};

USTRUCT()
struct ARIANE_API FArianeDrawingMovieSceneEvalTemplate : public FMovieSceneEvalTemplate
{
    GENERATED_BODY()

    FArianeDrawingMovieSceneEvalTemplate();
    FArianeDrawingMovieSceneEvalTemplate( const UArianeDrawingMovieSceneSection* InSection );

    virtual UScriptStruct& GetScriptStructImpl() const override;

    // executes at each frame to  apply the correct ArianeDrawing to the Layer
    virtual void Evaluate( const FMovieSceneEvaluationOperand& Operand
                         , const FMovieSceneContext& Context
                         , const FPersistentEvaluationData& PersistentData
                         , FMovieSceneExecutionTokens& ExecutionTokens ) const override;

protected:
    UPROPERTY()
    const UArianeDrawingMovieSceneSection* Section;
};
