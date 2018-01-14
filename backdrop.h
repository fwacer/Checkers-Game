#ifndef BACKDROP_H
#define BACKDROP_H

/*
 * File: backdrop.h
 * Author: Bryce Dombrowski
 *
 * Date: January 13th, 2018
 *
 * Sites:   brycedombrowski.com
 *          github.com/fwacer
 *
 * Contact: brycetdombrowski@gmail.com
 */

#include <QGraphicsItem>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include "checkersgame.h"

class Backdrop : public QGraphicsItem
{
public:
    Backdrop(){
        setAcceptDrops(true);
    }
private:
    QColor colour = Qt::white;

    void dragEnterEvent(QGraphicsSceneDragDropEvent *event){
        if (event->mimeData()->hasText()) {
            event->setAccepted(true);
            update();
        } else {
            event->setAccepted(false);
        }
    }
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event){
        Q_UNUSED(event);
        update();
    }
    void dropEvent(QGraphicsSceneDragDropEvent *event){
        event->setAccepted(true);
        if (event->mimeData()->hasText()){
            std::string s = event->mimeData()->text().toStdString();
            redrawBoard(std::make_pair(char(s.at(0)), char(s.at(1))), std::make_pair('z', 'z'), scene()); //invalid square
        }else{
            update();
        }
    }

    QRectF boundingRect() const
    {
        return QRectF(75, 85, 600, 600);
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0){
        Q_UNUSED(option);
        Q_UNUSED(widget);

        painter->setBrush(colour);
        painter->drawRect(boundingRect());
    }

};



#endif // BACKDROP_H
