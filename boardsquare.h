/*
 * File: boardsquare.h
 * Author: Bryce Dombrowski
 *
 * Date: January 13th, 2018
 *
 * Sites:   brycedombrowski.com
 *          github.com/fwacer
 *
 * Contact: brycetdombrowski@gmail.com
 */

#ifndef BOARDSQUARE_H
#define BOARDSQUARE_H
#include <QGraphicsItem>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include "checkersgame.h"
#include "gamepiece.h"
#include "main.h"

class BoardSquare : public QGraphicsItem
{
private:
    int x = 0;
    int y = 0;
    std::pair<char, char> square;
    QColor colour = Qt::black;
    bool dragOver = false;

    //Variables for user creating a custom board

    int customEditingFlag = Empty;
public:
    BoardSquare(int x, int y, std::pair<char, char> square){
        this->x = x;
        this->y = y;
        this->square = square;
        setAcceptDrops(true);
        //std::cout<<"Created game tile at "<<x<<y<<std::endl;
    }
    void setPos(int x, int y){
        this->x = x;
        this->y = y;
    }
    std::pair<int, int> get(){
        return std::make_pair(x,y);
    }

private:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event){
        if (event->mimeData()->hasText()) {
            event->setAccepted(true);
            dragOver = true;
            update();
        } else {
            event->setAccepted(false);
        }
    }
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event){
        Q_UNUSED(event);
        dragOver = false;
        update();
    }
    void dropEvent(QGraphicsSceneDragDropEvent *event){
        dragOver = false;
        event->setAccepted(true);
        if (event->mimeData()->hasText()){
            std::string s = event->mimeData()->text().toStdString();
            redrawBoard(std::make_pair(char(s.at(0)), char(s.at(1))), square, scene());
        }else{
            update();
        }
    }

    QRectF boundingRect() const
    {
        return QRectF(x, y, 75, 75);
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0){
        Q_UNUSED(option);
        Q_UNUSED(widget);

        painter->setBrush(dragOver ? Qt::lightGray: colour);
        painter->drawRect(boundingRect());

        //for displaying what piece is selected by the user when creating custom board
        if (customEditingFlag != Empty){
            painter->setBrush(Qt::lightGray);
            painter->drawEllipse(QRectF(x, y, 75, 75)); //outline

            bool king = false;
            switch(customEditingFlag){
            case Black:
                painter->setBrush(Qt::black);
                break;
            case White:
                painter->setBrush(Qt::white);
                break;
            case BlackKing:
                painter->setBrush(Qt::black);
                king = true;
                break;
            case WhiteKing:
                painter->setBrush(Qt::white);
                king = true;
                break;
            }

            painter->drawEllipse(QRectF(x+2, y+2, 71, 71)); //interior
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
        }
    }


    //User creating a board:

    void mousePressEvent(QGraphicsSceneMouseEvent * event){
        if (userCreatingBoardFlag){
            event->setAccepted(true);
            if (event->button()==Qt::LeftButton){ //left click cycles through colours
                switch(customEditingFlag){
                case White:
                    customEditingFlag = Black;
                    break;
                case Black:
                    customEditingFlag = Empty;
                    break;
                case Empty:
                    customEditingFlag = White;
                    break;
                case WhiteKing:
                    customEditingFlag = BlackKing;
                    break;
                case BlackKing:
                    customEditingFlag = Empty;
                    break;
                }
            }else if(event->button()==Qt::RightButton){ //right click promotes and demotes
                switch(customEditingFlag){
                case White:
                    customEditingFlag = WhiteKing;
                    break;
                case Black:
                    customEditingFlag = BlackKing;
                    break;
                case WhiteKing:
                    customEditingFlag = White;
                    break;
                case BlackKing:
                    customEditingFlag = Black;
                    break;
                }
            }
            customBoardAddPiece(square, customEditingFlag); //adds piece to custom board
            update();
        }
    }
};


#endif // BOARDSQUARE_H
