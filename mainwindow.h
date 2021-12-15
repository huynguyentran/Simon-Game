/* Huy Nguyen u1315096
 * William Erignac u1308418
 *
 * MainWindow dislay the application.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "model.h"
#include "BubbleManager.h"
#include <QPen>
#include <QPainter>
#include <mutex>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    //Slots to indicate the player's actions.
    void on_redButton_clicked();
    void on_startButton_clicked();
    void on_blueButton_clicked();

    //Slots to take in the flash period of the buttons
    //that are calculated and sent by the model.
    void setFlashPeriod(int period);
    void startFlash(SimonModel::Color color);
    void finishFlash();

    //Slot to enable the buttons after the random sequence
    //has been played.
    void afterSequencePlayed();

    //Slots to update the progress bar accordingly.
    //setRange will set the range of the progress bar
    //based on the information sent by the model.
    void setRange(int size);
    void updateProgressBar(int update);

    //Updates the label bar to indicate the game's instructions to the player.
    void playerSucceeded();
    void playerFailed();

    //If the player fails, instructs the UI to wait a little bit until
    //allow the player to start the new game.
    void resetWaitForStartState();

    //Updates the painters each second to draw the floating bubbles.
	void updatePainter(std::vector<std::tuple<float, float>> list);
    void paintEvent(QPaintEvent *event);

private:
    Ui::MainWindow *ui;

    //The Simon Game model which contains
    //the necessary information to pass to
    //the ui. Model will instruct what the UI
    //should do.
    SimonModel model;

    //The flash period of a button.
    int flashPeriod;
    //Returns the bubbton to the its
    //original state.
    void dimButtons();

    //Timer to update the bubbles on
    //the main window.
    QTimer bubbleUpdater;
    //BubbleManager object that handles all
    //bubbles coordinate calculation and remove
    //the bubbles if they reach certain height.
    BubbleManager bubbleManager;

    //Pen and Brush objects to draw the bubble.
    QPen bubblePen;
    QBrush bubbleBrush;
    QBrush smallerBubbleBrush;

    //A vector that contains the coordinates needed
    //to draw the bubble.
	std::vector<std::tuple<float, float>> particleToDraw;

    //A lock to make sure the vector of bubbles does not get
    //modified while the UI is drawing the bubble.
	std::mutex bubbleLock;
};
#endif // MAINWINDOW_H
