#include "tileseteditorwindow.h"
#include "ui_tileseteditorwindow.h"

#include <QDebug>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QBrush>

TilesetEditorWindow::TilesetEditorWindow(QWidget *parent, Tileset *tileset) :
    QMainWindow(parent),
    ui(new Ui::TilesetEditorWindow)
{
    this->tileset = tileset;
    this->selectedX = -1;
    this->selectedY = -1;

    ui->setupUi(this);
    this->setWindowTitle("Tileset Editor - CoinKiller");

    tilesetPicker = new TilesetPicker(this);

    ui->tilesetPicker->setWidget(tilesetPicker);
    ui->tilesetPicker->setMinimumSize(442, 442);
    ui->tilesetPicker->setMaximumSize(442, 442);

    connect(tilesetPicker, SIGNAL(selectedTileChanged(int,int)), this, SLOT(updateSelectedTile(int,int)));

    tilesetPicker->setTilesetImage(tileset->getImage());

    ui->hexLineEdit->setInputMask("HH HH HH HH HH HH HH HH");
}

TilesetEditorWindow::~TilesetEditorWindow()
{
    delete ui;
}

void TilesetEditorWindow::updateSelectedTile(int x, int y)
{
    selectedX = x;
    selectedY = y;

    QString selTileText("Selected Tile: (%1, %2)");
    ui->selectedTileLabel->setText(selTileText.arg(x).arg(y));

    QString hexValue;
    for (int i = 0; i < 8; i++)
        hexValue.append(QString("%1 ").arg(tileset->getBehaviorByte(x + y*21, i), 2, 16, QChar('0')));

    ui->hexLineEdit->setText(hexValue);
}

void TilesetEditorWindow::on_hexLineEdit_textEdited(const QString &text)
{
    if (selectedX == -1 || selectedY == -1)
        return;

    QStringList bytes = text.split(" ");

    for (int i = 0; i < 8; i++)
        tileset->setBehaviorByte(selectedX + 21*selectedY, i, (quint8)bytes[i].toUInt(0, 16));
}


TilesetPicker::TilesetPicker(QWidget *parent) : QWidget(parent)
{
    this->selectedX = -1;
    this->selectedY = -1;
    tilesetImage = new QImage(420, 420, QImage::Format_RGBA8888);
}

void TilesetPicker::setTilesetImage(QImage *tilesetImage)
{
    this->tilesetImage = tilesetImage;
}

void TilesetPicker::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.fillRect(evt->rect(), QBrush(QColor(255, 255, 255), Qt::SolidPattern));

    for (int x = 0; x < 21; x++)
    {
        for (int y = 0; y < 21; y++)
        {
            painter.drawImage(x*21, y*21, *tilesetImage, x*20 + x*4 + 2, y*20 + y*4 + 2, 20, 20, Qt::AutoColor);
        }
    }

    if (selectedX != -1 || selectedY != -1)
        painter.fillRect(QRect(selectedX*21, selectedY*21, 20, 20), QBrush(QColor(160,222,255,150), Qt::SolidPattern));
}

void TilesetPicker::mousePressEvent(QMouseEvent* evt)
{
    if (evt->button() != Qt::LeftButton)
        return;

    if (evt->x() % 21 != 20 && evt->x() % 21 != 20)
    {
        selectedX = evt->x() / 21;
        selectedY = evt->y() / 21;

        emit selectedTileChanged(selectedX, selectedY);

        update();
    }
}
