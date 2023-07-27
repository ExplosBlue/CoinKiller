#include "insertzonebackground.h"

namespace EditorCommand {

InsertZoneBackground::InsertZoneBackground(Level *level, ZoneBackground *background) :
    level(level), background(background)
{
    this->setText(QObject::tr("Inserted ZoneBackground"));
}

InsertZoneBackground::~InsertZoneBackground()
{
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::InsertZoneBackground - Deleting ZoneBackground";
        delete background;
    }
}

void InsertZoneBackground::undo()
{
    level->backgrounds.removeOne(background);
    deletable = true;
}

void InsertZoneBackground::redo()
{
    level->backgrounds.append(background);
    deletable = false;
}

} // namespace EditorCommand
