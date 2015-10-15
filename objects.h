#ifndef OBJECTS_H
#define OBJECTS_H


// Obj Baseclass
class Object
{
public:
    Object();
    void setPosition(int x, int y);
    void resize(int width, int height);
    int getx() const;
    int gety() const;
    int getwidth() const;
    int getheight() const;

protected:
    int x, y;
    int width, height;
};


// Bgdat Object
class BgdatObject: public Object
{
public:
    BgdatObject();
    BgdatObject(int x, int y, int width, int height, int id);
    int getid() const;
protected:
    int id;
};


// Sprite Object
class Sprite: public Object
{
public:
    Sprite();
    Sprite(int x, int y, int id);
    void setSpriteOffset(int offsetx, int offsety);
    int getid() const;

protected:
    int offsetx, offsety;
    int id;
};

#endif // OBJECTS_H
