#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

class FOdysseyVectorObject;
class FOdysseyVectorTag;

class ODYSSEYVECTOR_API FOdysseyVectorSharedEnv
{
    public:
        virtual ~FOdysseyVectorSharedEnv();
        FOdysseyVectorSharedEnv();

        void AddTag( FOdysseyVectorTag* iVectorTag );
        void RemoveTag( FOdysseyVectorTag* iVectorTag );
        void AddObject( FOdysseyVectorObject* iVectorObject );
        void RemoveObject( FOdysseyVectorObject* iVectorObject );
        bool HasTag( FOdysseyVectorTag* iTag );
        FOdysseyVectorTag* GetSelectedTagByClassType( uint32 iClassType );

        std::list<FOdysseyVectorTag*>& GetTagList();


    private:
        std::list<FOdysseyVectorObject*> mObjectList;
        std::list<FOdysseyVectorTag*> mTagList;
};
