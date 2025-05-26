// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "FastXml.h"

class FOdysseyVectorVertex;
class FOdysseyVectorObject;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorSegment;

class ODYSSEYVECTOR_API FOdysseyVectorImportSVG : public IFastXmlCallback
{
    public:
        virtual ~FOdysseyVectorImportSVG( );
        FOdysseyVectorImportSVG( );
        FOdysseyVectorImportSVG( FOdysseyVectorGroupPaint* iScene
                               , const FString& iPath );

        bool PathProcessAttribute ( const TCHAR* AttributeName
                                  , const TCHAR* AttributeValue );
        bool PathProcessAttributeD ( const TCHAR* AttributeValue );
        TCHAR* PathProcessAttributeDCommand ( const TCHAR* iCommandName
                                            , const TCHAR* iCommandValue );

        virtual bool ProcessAttribute ( const TCHAR* AttributeName
                                      , const TCHAR* AttributeValue ) override;
        virtual bool ProcessClose ( const TCHAR* Element ) override;
        virtual bool ProcessComment ( const TCHAR* Comment ) override  { return true; };
        virtual bool ProcessElement ( const TCHAR* ElementName
                                    , const TCHAR* ElementData
                                    , int32 XmlFileLineNumber ) override;
        virtual bool ProcessXmlDeclaration( const TCHAR* ElementData
                                          , int32 XmlFileLineNumber ) override;

    private:
        TCHAR* PathProcessAttributeDCommandM( const TCHAR* iCommandValue, bool iAbsolute );
        TCHAR* PathProcessAttributeDCommandC( const TCHAR* iCommandValue, bool iAbsolute );
        TCHAR* PathProcessAttributeDCommandS( const TCHAR* iCommandValue, bool iAbsolute );
        TCHAR* PathProcessAttributeDCommandZ( const TCHAR* iCommandValue, bool iAbsolute );

    protected:
        bool (FOdysseyVectorImportSVG::*ProcessAttributeCallback) ( const TCHAR* AttributeName
                                                                  , const TCHAR* AttributeValue );
        TCHAR* (FOdysseyVectorImportSVG::*ProcessCommandCallback) ( const TCHAR* iCommandName
                                                                  , const TCHAR* iCommandValue );

        FOdysseyVectorGroupPaint* mScene;
        FOdysseyVectorObject* mProcessedObject;
        FOdysseyVectorVertex* mLastVertex;
        FOdysseyVectorSegment* mLastSegment;
        TCHAR mLastCommand;
};
