#ifndef COMMANDIDS_H
#define COMMANDIDS_H

namespace Commands {

enum CommandID {
    // Level View
    Cmd_Move,
    Cmd_Resize,
    Cmd_IncreasePosition,
    Cmd_SetValue,

    // SpriteCmd
    SpriteCmd_SetId,
    SpriteCmd_SetLayer,
    SpriteCmd_SetBits,
    SpriteCmd_SetByte,
    SpriteCmd_SetNybbleData
};

} // namespace Commands

#endif // COMMANDIDS_H
