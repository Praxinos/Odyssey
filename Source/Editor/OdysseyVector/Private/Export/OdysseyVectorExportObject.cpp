#include "Export/OdysseyVectorExport.h"

void
FOdysseyVectorExport::WriteObjectsDeclareObject( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::EXPORT_OBJECTS_DECLARE_OBJECT
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectType = iObject.GetType();

        Ar << objectType;
    } );
}
