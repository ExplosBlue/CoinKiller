#include "setlayermask.h"

namespace EditorCommand {

SetLayerMask::SetLayerMask(quint8 *layerMask, LAYER_MASK layer, bool state) :
    layerMask(layerMask), layer(layer), state(state)
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

void SetLayerMask::undo()
{
    if (state) {
        *layerMask &= ~layer;
    }
    else {
        *layerMask |= layer;
    }
}

void SetLayerMask::redo()
{
    if (state) {
        *layerMask |= layer;
    }
    else {
        *layerMask &= ~layer;
    }
}

} // namespace EditorCommand
