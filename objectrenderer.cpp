#include "objectrenderer.h"
#include "objects.h"

#include <QPainter>

ObjectRenderer::ObjectRenderer() { }

ObjectRenderer::ObjectRenderer(Object *obj)
{

}

ObjectRenderer::~ObjectRenderer() { }

void ObjectRenderer::render(QPainter *painter) { }


SpriteRenderer::SpriteRenderer() { }

SpriteRenderer::SpriteRenderer(const Sprite *spr)
{
    this->spr = spr;

    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    switch (spr->getid()) {
    case 66:
        ret = new NormalImageRenderer(spr, basePath + "pipe_cannon.png");
        break;
    case 135:
        ret = new NormalImageRenderer(spr, basePath + "goomba.png");
        break;
    case 219:
        ret = new NormalImageRenderer(spr, basePath + "star_coin.png");
        break;
    case 267:
        ret = new NormalImageRenderer(spr, basePath + "long_question_block.png");
        break;
    case 269:
        ret = new NormalImageRenderer(spr, basePath + "gold_ship.png");
        break;
    case 274:
        ret = new NormalImageRenderer(spr, basePath + "flying_gold_block.png");
        break;
    case 278:
        ret = new NormalImageRenderer(spr, basePath + "assist_block.png");
        break;
    default:
        ret = new RoundedRectRenderer(spr, QString("%1").arg(spr->getid()), QColor(0,90,150,200));
        break;
    }
}

void SpriteRenderer::render(QPainter *painter)
{
    ret->render(painter);
}


NormalImageRenderer::NormalImageRenderer() { }

NormalImageRenderer::NormalImageRenderer(const Object *obj, QString filename)
{
    this->obj = obj;
    this->filename = filename;
}

void NormalImageRenderer::render(QPainter *painter)
{
    painter->drawPixmap(obj->getx()+obj->getOffsetX(), obj->gety()+obj->getOffsetY(), obj->getwidth(), obj->getheight(), QPixmap(filename));
}


RoundedRectRenderer::RoundedRectRenderer() { }

RoundedRectRenderer::RoundedRectRenderer(const Object *obj, QString text, QColor color)
{
    this->obj = obj;
    this->text = text;
    this->color = color;
}

void RoundedRectRenderer::render(QPainter *painter)
{
    QRect rect(obj->getx()+obj->getOffsetX(), obj->gety()+obj->getOffsetY(), obj->getwidth(), obj->getheight());

    painter->setPen(QColor(0,0,0));

    QPainterPath path;
    path.addRoundedRect(rect, 2.0, 2.0);
    painter->fillPath(path, color);
    painter->drawPath(path);

    painter->setFont(QFont("Arial", 7, QFont::Normal));
    painter->drawText(rect, text, Qt::AlignHCenter | Qt::AlignVCenter);
}
