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

       /**
         * @brief Static function to allocate a new group. Note: this is the proper way to allocate a new group as we don't
         * use the constructor to set parameters because UOBJECTs must have empty constructors.
         * @param iName object's name
         */
        static FOdysseyVectorGroup* New( std::string iName );

        /**
         * @brief destructor
         */
        ~FOdysseyVectorGroup(){};

        /**
         * @brief constructor
         */
        FOdysseyVectorGroup(){};

        /**
         * @brief Init a group
         * @param iName
         */
        void Init( std::string iName );

    protected:
        virtual void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags ) override { };
        virtual bool PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags ) override;
        virtual FOdysseyVectorObject* CopyShape() override;
        virtual void UpdateShape( uint32 iUpdateFlags ) override;
};
