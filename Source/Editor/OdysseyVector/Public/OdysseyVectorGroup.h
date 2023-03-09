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
       /**
         * @brief Static function to allocate a new group. Note: this is the proper way to allocate a new group as we don't
         * use the constructor to set parameters because UOBJECTs must have empty constructors.
         * @param iName object's name
         */
        static UOdysseyVectorGroup* New( std::string iName );

        /**
         * @brief destructor
         */
        ~UOdysseyVectorGroup(){};

        /**
         * @brief constructor
         */
        UOdysseyVectorGroup(){};

        /**
         * @brief Init a group
         * @param iName
         */
        void Init( std::string iName );

    protected:
        virtual void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags ) override { };
        virtual bool PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags ) override;
        virtual UOdysseyVectorObject* CopyShape() override;
        virtual void UpdateShape( uint32 iUpdateFlags ) override;
};
