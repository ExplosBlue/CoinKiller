#ifndef LEVELVIEW_H
#define LEVELVIEW_H

#include <QWidget>
#include <QLayout>
#include <QList>

#include "filesystem.h"

class LevelView : public QWidget
{
    Q_OBJECT
public:
    explicit LevelView(QWidget *parent, SarcFilesystem* level);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    SarcFilesystem* levelArchive;
    QImage* imgtest; // REMOVE ME!!!


    // MOVE THIS SHIT ELSEWHERE!!!!! TEST!!!
    struct BgdatObject
    {
        quint16 id;
        quint16 x, y; // [RoadrunnerWMC] signed position??? needs investigation
        quint16 width, height;

        // extra crap, but it's all zeroes
    };

    QList<BgdatObject> objects;
};

#endif // LEVELVIEW_H
