#include "levelmanager.h"
#include "leveleditorwindow.h"
#include "game.h"

LevelManager::LevelManager(WindowBase *parentWidget, Game* game, QString lvlPath)
{
    this->parentWidget = parentWidget;
    this->lvlPath = lvlPath;
    this->game = game;

    archive = new SarcFilesystem(game->fs->openFile(lvlPath));
}

LevelManager::~LevelManager()
{
    delete archive;
}

Level* LevelManager::openArea(int id)
{
    if (!hasArea(id))
        throw std::runtime_error("Opening Level Area failed: Area does not exist.");

    Level* openArea = new Level(game, archive, id, lvlPath);
    openedAreas.append(openArea);
    return openArea;
}

void LevelManager::closeArea(Level *area)
{
    openedAreas.removeOne(area);
    delete area;
}

void LevelManager::openAreaEditor(int id)
{
    if (!hasArea(id))
        throw std::runtime_error("Opening Level Editor failed: Area does not exist.");

    LevelEditorWindow* lvlEditor = new LevelEditorWindow(this, id);
    connect(this, SIGNAL(updateLevelEditors()), lvlEditor, SLOT(handleMgrUpdate()));
    lvlEditor->show();
}

int LevelManager::getAreaCount()
{
    int areaCount = 0;
    for (;;)
    {
        if (!archive->fileExists(QString("course/course%1.bin").arg(areaCount+1))) break;
        areaCount++;
    }
    return areaCount;
}

bool LevelManager::hasArea(int id)
{
    return archive->fileExists(QString("course/course%1.bin").arg(id));
}

int LevelManager::addArea(int id)
{
    if (getAreaCount() >= 4)
            throw std::runtime_error("Adding area to level failed: Exceeded maximum area count (4).");

    QFile new_course(QCoreApplication::applicationDirPath() + "/coinkiller_data/blank_course.bin");
    if(!new_course.open(QIODevice::ReadOnly))
        throw std::runtime_error("Blank course file is missing or failed to open (/coinkiller_data/blank_course.bin)");

    int newAreaId = id+1;

    for (int i = getAreaCount(); i >= newAreaId; i--)
    {
        archive->renameFile(QString("course/course%1.bin").arg(i), QString("course%1.bin").arg(i+1));
        archive->renameFile(QString("course/course%1_bgdatL1.bin").arg(i), QString("course%1_bgdatL1.bin").arg(i+1));
        archive->renameFile(QString("course/course%1_bgdatL2.bin").arg(i), QString("course%1_bgdatL2.bin").arg(i+1));

        foreach (Level* area, openedAreas)
        {
            if (area->getAreaID() == i)
                area->setAreaID(i+1);
        }
    }

    new_course.seek(0);
    char* data = new char[new_course.size()];
    new_course.read(data, new_course.size());
    new_course.close();


    FileBase* newCourseFile = new MemoryFile(archive, new_course.size());
    newCourseFile->setIdPath(QString("course/course%1.bin").arg(newAreaId));
    newCourseFile->open();
    newCourseFile->seek(0);
    newCourseFile->writeData((quint8*)data, new_course.size());
    delete[] data;
    newCourseFile->save();
    newCourseFile->close();

    emit updateLevelEditors();

    return newAreaId;
}

int LevelManager::removeArea(Level *level)
{
    int areaId = level->getAreaID();

    if (!hasArea(areaId))
        throw std::runtime_error("Failed to remove level area: Area doesn't exist");

    closeArea(level);

    int oldAreaCount = getAreaCount();

    archive->deleteFile(QString("course/course%1.bin").arg(areaId));
    archive->deleteFile(QString("course/course%1_bgdatL1.bin").arg(areaId));
    archive->deleteFile(QString("course/course%1_bgdatL2.bin").arg(areaId));

    for (int i = areaId+1; i <= oldAreaCount; i++)
    {
        archive->renameFile(QString("course/course%1.bin").arg(i), QString("course%1.bin").arg(i-1));
        archive->renameFile(QString("course/course%1_bgdatL1.bin").arg(i), QString("course%1_bgdatL1.bin").arg(i-1));
        archive->renameFile(QString("course/course%1_bgdatL2.bin").arg(i), QString("course%1_bgdatL2.bin").arg(i-1));

        foreach (Level* area, openedAreas)
        {
            if (area->getAreaID() == i)
                area->setAreaID(i-1);
        }
    }

    emit updateLevelEditors();

    int seekArea = -1;

    if (areaId <= getAreaCount() && !areaIsOpen(areaId))
        return areaId;

    else if (!areaIsOpen(areaId-1) && (areaId-1) >= 1)
        seekArea = areaId-1;

    else for (int i = 1; i <= getAreaCount(); i++)
    {
        if (!areaIsOpen(i))
        {
            seekArea = i;
            break;
        }
    }

    return seekArea;
}

WindowBase* LevelManager::getParent()
{
    return parentWidget;
}

Game* LevelManager::getGame()
{
    return game;
}

int LevelManager::getOpenedAreaCount()
{
    return openedAreas.count();
}

bool LevelManager::areaIsOpen(int id)
{
    foreach (Level* area, openedAreas)
    {
        if (area->getAreaID() == id)
            return true;
    }

    return false;
}
