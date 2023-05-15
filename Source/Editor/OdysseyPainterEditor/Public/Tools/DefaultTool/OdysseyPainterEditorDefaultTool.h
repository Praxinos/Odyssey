// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorDefaultTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorDefaultTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorDefaultTool();

    //Constructor
    UOdysseyPainterEditorDefaultTool();
 
protected:
    void Copy( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void Paste( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );

public:
    virtual bool OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                , FOdysseyVectorScene* iScene
                                , const FKey& iKey );
    virtual bool OnKeyUpVector( FOdysseyVectorEngine* iEngine
                              , FOdysseyVectorScene* iScene
                              , const FKey& iKey );

    protected:
        static std::list<FOdysseyVectorObject*>& GetCopiedObjectList();
};
