#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorObject.h"

class FOdysseyVectorTag;

class ODYSSEYVECTOR_API FOdysseyVectorSharedEnv : public FOdysseyVectorObject
{
    private:
        static const uint32 mStaticClass = 0x442744a7; // value is crc32 FOdysseyVectorSharedEnv

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID );

    public:
        virtual ~FOdysseyVectorSharedEnv();
        FOdysseyVectorSharedEnv();

        void AddSharedTag( FOdysseyVectorTag* iVectorTag );
        void RemoveSharedTag( FOdysseyVectorTag* iVectorTag );
        void AddSharedObject( FOdysseyVectorObject* iVectorObject );
        void RemoveSharedObject( FOdysseyVectorObject* iVectorObject );
        bool HasSharedTag( FOdysseyVectorTag* iTag );
        FOdysseyVectorTag* GetSelectedTagByClassType( uint32 iClassType );
        void GetSelectedTagByClassType( uint32 iClassType
                                      , std::list<FOdysseyVectorTag*>& oSelectedTagList );

        std::list<FOdysseyVectorTag*>& GetSharedTagList();


    private:
        std::list<FOdysseyVectorObject*> mSharedObjectList;
        std::list<FOdysseyVectorTag*> mSharedTagList;
};
