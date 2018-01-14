/*
 * File: gamepiece.h
 * Author: Bryce Dombrowski
 *
 * Date: January 13th, 2018
 *
 * Sites:   brycedombrowski.com
 *          github.com/fwacer
 *
 * Contact: brycetdombrowski@gmail.com
 */

#ifndef GAMEPIECE_H
#define GAMEPIECE_H

#include <Qt>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QDrag>


class GamePiece : public QGraphicsItem
{
private:
    std::pair<char, char> square = std::make_pair('a', '1');
    int x = 0;
    int y = 0;
    QColor colour = Qt::black;
    bool king = false;

public:
    GamePiece(){

    }
    GamePiece(int x, int y, QColor colour, std::pair<char, char> square, bool king = false){
        this->x = x;
        this->y = y;
        this->colour = colour;
        this->king = king;
        this->square = square;
        //std::cout<<"Created game piece at "<<x<<y<<std::endl;

        setToolTip(QString("Click and drag to move"));
        setCursor(Qt::OpenHandCursor);
        setAcceptedMouseButtons(Qt::LeftButton);
    }
    void setPos(int x, int y){
        this->x = x;
        this->y = y;
    }
    void setPos(QPointF p){
        this->x = p.x();
        this->y = p.y();
    }
private:
    void mousePressEvent(QGraphicsSceneMouseEvent * event){
        setCursor(Qt::ClosedHandCursor);
        event->setAccepted(true);
        update();
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event)
    {
        if (QLineF(event->screenPos(),
                   event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance()) {
            return;
        }
        QRect rect = boundingRect().toRect();

        //This section of code is a bit janky and could be done better
        QPixmap pixmap(rect.size());
        pixmap.fill(Qt::transparent);
        pixmap.scaled(20,20);
        QPainter painter(&pixmap);

        painter.translate(-rect.topLeft());

        painter.setBrush(Qt::lightGray);

        painter.drawEllipse(QPoint(x+75/3, y+75/3), 75/3, 75/3);

        painter.setBrush(colour);
        painter.drawEllipse(QPoint(x+75/3, y+75/3), 71/3, 71/3);
        painter.setBrush(Qt::black);
        painter.end();

        QDrag *drag = new QDrag(event->widget());
        drag->setHotSpot( QPoint( 75/3, 75/3 ) );
        drag->setPixmap(pixmap);

        QMimeData *mime = new QMimeData;
        QString s;
        s += square.first;
        s += square.second;

        mime->setText(s);
        drag->setMimeData(mime);

        hide();
        update();
        drag->exec();
        //Returns here once the drag has finished executing
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
    {
        show();
        setCursor(Qt::OpenHandCursor);
        event->setAccepted(true);
    }

    QRectF boundingRect() const
    {
        return QRectF(x, y, 75, 75);
    }
    QRectF interior() const
    {
        return QRectF(x+2, y+2, 71, 71);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0){
        Q_UNUSED(option);
        Q_UNUSED(widget);

        painter->setBrush(Qt::lightGray);

        painter->drawEllipse(boundingRect());

        painter->setBrush(colour);
        painter->drawEllipse(interior());
        if(king){
            const QPointF kingPoints[7] = { //the king's crown
                QPointF(x+18.0, y+45.0),
                QPointF(x+58.0, y+45.0),
                QPointF(x+60.0, y+15.0),
                QPointF(x+48.0, y+30.0),
                QPointF(x+38.0, y+15.0),
                QPointF(x+28.0, y+30.0),
                QPointF(x+16.0, y+15.0)
            };
            painter->setBrush(Qt::darkYellow);
            painter->drawPolygon(kingPoints, 7);
        }
        painter->setBrush(Qt::black);

    }
};


#endif
