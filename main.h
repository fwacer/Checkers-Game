/*
 * File: main.h
 * Author: Bryce Dombrowski
 *
 * Date: January 13th, 2018
 *
 * Sites:   brycedombrowski.com
 *          github.com/fwacer
 *
 * Contact: brycetdombrowski@gmail.com
 */

#ifndef MAIN_H
#define MAIN_H
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsSceneDragDropEvent>

static const std::vector<std::string> gameStateVector = {"Invalid Move", "" /*Valid Move*/, "White Wins", "Black Wins", "Draw", };
static bool userCreatingBoardFlag = false;

typedef enum BoardLayout{
    Standard = 1,
    Kings,
    Jumpalicious,
    TwoRows,
    CustomBoardCreate,
    CustomBoardPlay
}BoardLayout;

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView(QGraphicsScene *scene) : QGraphicsView(scene){
        setAcceptDrops(true);
    }
public slots:
    //TBA?
    void zoomIn() { scale(1.2, 1.2); }
    void zoomOut() { scale(1 / 1.2, 1 / 1.2); }

private:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event){
        Q_UNUSED(event);
    }
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event){
        Q_UNUSED(event);
        update();
    }
    void dropEvent(QGraphicsSceneDragDropEvent *event){
        event->setAccepted(true);
        update();
    }
};

void drawSceneBoard( QGraphicsScene & scene);
void drawScenePieces(QGraphicsScene & scene, std::map<std::pair<char, char>, char> & gameBoard);
void redrawBoard(std::pair<char, char> from, std::pair<char, char> to, QGraphicsScene * scene);
void customBoardAddPiece(std::pair<char, char> square, int pieceType);
int main(int argc, char *argv[]);

#endif
