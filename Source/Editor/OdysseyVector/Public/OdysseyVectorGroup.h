#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"

#include "OdysseyVectorGroup.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorGroup : public UOdysseyVectorObject
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyVectorGroup(){};
        UOdysseyVectorGroup(){};
        void Init( std::string iName );

    private:
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags ) { };
        bool PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags );
        UOdysseyVectorObject* CopyShape();
        void UpdateShape();
};
