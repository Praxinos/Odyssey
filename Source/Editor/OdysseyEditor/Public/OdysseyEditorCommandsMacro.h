// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#define UI_CMD( CommandId, Category, FriendlyName, InDescription, CommandType, InDefaultChord )                           \
    CommandId = FUICommandInfoDecl(                                                                                       \
                this->AsShared(),                                                                                         \
                FName( TEXT( #CommandId ) ),                                                                              \
                LOCTEXT( #CommandId "Label", FriendlyName ),                                                              \
                LOCTEXT( #CommandId "ToolTip", InDescription ),                                                           \
                Category)                                                                                                 \
            .UserInterfaceType( CommandType )                                                                             \
            .DefaultChord( InDefaultChord );

