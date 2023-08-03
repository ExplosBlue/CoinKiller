#include "editorcommands.h"

namespace Commands::EditorCmd {

SetLayerMask::SetLayerMask(quint8 *layerMask, LAYER_MASK layer, bool state) :
    layerMask(layerMask),
    layer(layer),
    state(state)
{
    QString enabledStr = (state) ? "Enabled" : "Disabled";

    switch (layer) {
    case LAYER_MASK::LAYER_ONE:
        this->setText(QObject::tr("%1 Layer 1").arg(enabledStr));
        break;
    case LAYER_MASK::LAYER_TWO:
        this->setText(QObject::tr("%1 Layer 2").arg(enabledStr));
        break;
    default:
        this->setText(QObject::tr("%1 Layer (Unknown)").arg(enabledStr));
        break;
    }
}

void SetLayerMask::undo() {
    if (state) {
        *layerMask &= ~layer;
    }
    else {
        *layerMask |= layer;
    }
}

void SetLayerMask::redo() {
    if (state) {
        *layerMask |= layer;
    }
    else {
        *layerMask &= ~layer;
    }
}


SetSpriteInteraction::SetSpriteInteraction(bool *spriteInteraction, bool state) :
    spriteInteraction(spriteInteraction),
    state(state) {
    QString enabledStr = (state) ? "Enabled" : "Disabled";
    this->setText(QObject::tr("%1 Sprites").arg(enabledStr));
}

void SetSpriteInteraction::undo() {
    *spriteInteraction = !state;
}

void SetSpriteInteraction::redo() {
    *spriteInteraction = state;
}


SetEntranceInteraction::SetEntranceInteraction(bool *entranceInteraction, bool state) :
    entranceInteraction(entranceInteraction),
    state(state) {
    QString enabledStr = (state) ? "Enabled" : "Disabled";
    this->setText(QObject::tr("%1 Entrances").arg(enabledStr));
}

void SetEntranceInteraction::undo() {
    *entranceInteraction = !state;
}

void SetEntranceInteraction::redo() {
    *entranceInteraction = state;
}


SetPathInteraction::SetPathInteraction(bool *pathInteraction, bool state) :
    pathInteraction(pathInteraction),
    state(state) {
    QString enabledStr = (state) ? "Enabled" : "Disabled";
    this->setText(QObject::tr("%1 Paths").arg(enabledStr));
}

void SetPathInteraction::undo() {
    *pathInteraction = !state;
}

void SetPathInteraction::redo() {
    *pathInteraction = state;
}


SetLocationInteraction::SetLocationInteraction(bool *locationInteraction, bool state) :
    locationInteraction(locationInteraction),
    state(state) {
    QString enabledStr = (state) ? "Enabled" : "Disabled";
    this->setText(QObject::tr("%1 Locations").arg(enabledStr));
}

void SetLocationInteraction::undo() {
    *locationInteraction = !state;
}

void SetLocationInteraction::redo() {
    *locationInteraction = state;
}

} // namespace Commands::EditorCmd
