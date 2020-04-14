# CheckersGame

Author: Bryce Dombrowski

Date: January 13th, 2018

Blog post: https://brycedombrowski.com/2018/01/13/winter-2017-18-checkers-game-gui/

Info: 
 * This project began as an exercise to keep in coding shape over the winter break. After finishing a rudimentary version of checkers that took place in the console, I very quickly became tired of manually typing every move. This is when I decided to add a clickable and shiny front end. Over the course of this project I learned improved coding practices, which I hope is reflected in this final product, even if it is not perfect. If you have any questions or have any feedback, please contact me! I would love to hear your input.

Features:
 * Mouse-based interface with drag and drop movement for piece movement
 * Move legality checking for movement and "jumping"
 * Ability to exit the program, reset the board, change between some built in layouts
 * Custom board layout creator
 * A fancy installer (not necessary, but was interesting to learn!)
 * A simple AI

Photo:

![Checkers image](https://brycedombrowski.files.wordpress.com/2018/01/checkers.png)

Things to add/improve:
 * Currently, the whole scene is discarded and redrawn every time a move is made. There is probably a better way to do this, by somehow keeping track of all objects in the scene and adding or removing them as necessary.
 * Animation of "jumps"
 * ~~AI for a computer opponent~~ Completed
 * Deployment on Linux / Mac

Known bug:
 * If a game piece is dragged and dropped outside of the gameboard or window, the piece is not redrawn on the board. It can be redrawn by making some sort of move (legal or not, does not matter) or by pressing the "refresh" button. This behaviour is caused by the mouseReleaseEvent not triggering over any of the objects of the scene. If you know a solution, please let me know.

Acknowledgements:
 * This project had help from many of the Qt examples, especially: http://doc.qt.io/qt-5/qtwidgets-graphicsScene-dragdroprobot-example.html
