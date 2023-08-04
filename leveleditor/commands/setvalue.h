#ifndef EDITORCOMMAND_SETVALUE_H
#define EDITORCOMMAND_SETVALUE_H

#include <QUndoCommand>
#include <QString>

#include "commandids.h"

using namespace Commands;

namespace EditorCommand {

template<typename T>
class SetValue : public QUndoCommand {
public:
    SetValue(T *value, T newValue, QString message);
    ~SetValue() {}

    void undo() override;
    void redo() override;
    int id() const override;
    bool mergeWith(const QUndoCommand *otherCommand) override;

private:
    T *value;
    T newValue;
    T oldValue;
};

template<typename T>
SetValue<T>::SetValue(T *value, T newValue, QString message) :
    value(value), newValue(newValue) {
    oldValue = *value;
    this->setText(message);
}

template<typename T>
void SetValue<T>::undo() {
    *value = oldValue;
}

template<typename T>
void SetValue<T>::redo() {
    *value = newValue;
}

template<typename T>
int SetValue<T>::id() const {
    return CommandID::Cmd_SetValue;
}

template<typename T>
bool SetValue<T>::mergeWith(const QUndoCommand *otherCommand) {
    if (otherCommand->id() != this->id()) {
        return false;
    }

    const SetValue *otherSetValue = static_cast<const SetValue*>(otherCommand);

    if (otherSetValue->value != this->value) {
        return false;
    }

    newValue = otherSetValue->newValue;
    return true;
}

} // namespace EditorCommand

#endif // EDITORCOMMAND_SETVALUE_H
