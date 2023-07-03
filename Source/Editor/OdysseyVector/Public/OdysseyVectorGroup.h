#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"

//#include "OdysseyVectorGroup.generated.h"

class ODYSSEYVECTOR_API FOdysseyVectorGroup : public FOdysseyVectorObject
{
    private:
        static const uint32 mStaticClass = 0xd4c8357b; // value is crc32 FOdysseyVectorGroup

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

        bool HasBaseClass( uint32 iBaseClassID );

        /**
         * @brief destructor
         */
        virtual ~FOdysseyVectorGroup(){};

        /**
         * @brief constructor
         */
        FOdysseyVectorGroup( const FString& iName );

        /**
         * @brief Init a group
         * @param iName
         */
        void Init( FString& iName );

    protected:
        virtual void DrawShape( uint64 iFlags ) override { };
        virtual bool PickShape( const ::ULIS::FRectD& iRoi, uint32 iSelectionFlags ) override;
        virtual FOdysseyVectorObject* CopyShape() override;
        virtual void UpdateShape( uint32 iUpdateFlags ) override;
};
