// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorLayoutBuilder.h"

class FOdysseyPainterEditor;

/**
 * Base class for an Editor Extension
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorExtension
    : public FGCObject //Allows us to register External UObject in Garbage Collector
{   
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorExtension();
    FOdysseyPainterEditorExtension( FOdysseyPainterEditor* iEditor );

public:
    virtual void Initialize();
    virtual void Finalize();
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender );
    virtual void BindShortcuts(FBaseToolkit* iToolkit);
	virtual void BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder);

    FOdysseyPainterEditor* GetEditor() const;
    
public:
    virtual void AddReferencedObjects(FReferenceCollector& Collector);
    virtual FString GetReferencerName() const;

public:
    FOdysseyPainterEditor* mEditor;
};
