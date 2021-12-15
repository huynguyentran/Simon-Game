/* Huy Nguyen u1315096
 * William Erignac u1308418
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "model.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,  model(this)
    , bubbleUpdater(this)
	//The bubble manager will be updated every 30th of a second.
	, bubbleManager(size().width(), size().height(), 1000/30)
{
	flashPeriod = 1000;

	//Set up the ui for the start of the game.
    ui->setupUi(this);
    ui-> blueButton->setEnabled(false);
    ui-> redButton->setEnabled(false);
    dimButtons();

	//Bridge the connection between various UI elements and the model.
    connect(ui->startButton,&QPushButton::clicked,&model,&SimonModel::startGame);

	connect(&model,&SimonModel::updateProgress,this,&MainWindow::updateProgressBar);
	connect(&model,&SimonModel::setRange,this,&MainWindow::setRange);

	//Connect the view and the model such that the view will correctly display
	//the state of the model.
	connect(&model,&SimonModel::calculatedFlashPeriod,this,&MainWindow::setFlashPeriod);

    connect(&model,&SimonModel::flashColor,this,&MainWindow::startFlash);
    connect(&model,&SimonModel::enterPlaystate,this,&MainWindow::afterSequencePlayed);

    connect(&model,&SimonModel::success, this, &MainWindow::playerSucceeded);
    connect(&model,&SimonModel::fail, this, &MainWindow::playerFailed);

	//Bubble Particles Setup
	QColor outlineColor(41,231,231,255);
    bubblePen.setWidth(2);
	bubblePen.setColor(outlineColor);

	QColor mainFillColor(41,231,231,150);
	bubbleBrush.setColor(mainFillColor);
    bubbleBrush.setStyle(Qt::SolidPattern);

	QColor highlightFillColor(255,255,255,175);
	smallerBubbleBrush.setColor(highlightFillColor);
    smallerBubbleBrush.setStyle(Qt::SolidPattern);

	//Set up the bubbleManager to be updated every 30th of a second.
    connect(&bubbleUpdater, &QTimer::timeout, &bubbleManager, &BubbleManager::updatePositions);
	//Get ready to take particles to draw from the bubbleManager.
	connect(&bubbleManager, &BubbleManager::drawParticles, this, &MainWindow::updatePainter);

    bubbleUpdater.start(1000/30);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_redButton_clicked()
{
   model.buttonClicked(SimonModel::Red);
}

void MainWindow::on_blueButton_clicked()
{
   model.buttonClicked(SimonModel::Blue);
}

void MainWindow::on_startButton_clicked()
{
	//Set up the UI for the first animation phase of this game.
    ui-> startButton->setEnabled(false);
    ui-> blueButton->setEnabled(false);
    ui-> redButton->setEnabled(false);
    ui->playerProgress->setValue(0);
}


void MainWindow::afterSequencePlayed()
{
	//Set up the UI for the input phase of the game.
    ui-> blueButton->setEnabled(true);
    ui-> redButton->setEnabled(true);
    ui->playerProgress->setValue(0);
    ui->gameMessage->setText("Play the Sequence back");
    ui->redButton->setStyleSheet( QString("QPushButton {background-color: rgb(175,50,50);} QPushButton:pressed {background-color: rgb(225,50,50);}"));
    ui->blueButton->setStyleSheet( QString("QPushButton {background-color: rgb(50,50,175);} QPushButton:pressed {background-color: rgb(50,50,225);}"));
}

void MainWindow::setRange(int range){
    ui->playerProgress->setRange(0,range);
}

void MainWindow::updateProgressBar(int value){
    ui->playerProgress->setValue(value);
}

void MainWindow::setFlashPeriod(int period){
    flashPeriod = period;
}

/**
 * @brief Flashes a color when called by the model.
 */
void MainWindow::startFlash(SimonModel::Color color){
	//Set up the UI for the flash animation.
	ui->gameMessage->setText("Remember this Sequence...");
    ui->playerProgress->setValue(0);

	//Brighten the button that corresponds to the type of color provided.
	//Only the background-color needs to change because neither button can be pressed.
    switch(color){
	case SimonModel::Red:
        ui->redButton->setStyleSheet( QString("QPushButton {background-color: rgb(225,50,50);} QPushButton:pressed {background-color: rgb(225,50,50);}"));
        break;
    case SimonModel::Blue:
        ui->blueButton->setStyleSheet( QString("QPushButton {background-color: rgb(50,50,225);} QPushButton:pressed {background-color: rgb(50,50,225);}"));
        break;
    }

	//Pause between flashes.
    QTimer::singleShot(flashPeriod,this,&MainWindow::finishFlash);
}

/**
 * @brief Dim the color of the buttons to have a breif pause
 *			between color flashes.
 */
void MainWindow::finishFlash(){
    dimButtons();
	//Move on to the next color to flash, or enter the input state.
    QTimer::singleShot(flashPeriod,&model,&SimonModel::animateSequence);
}

void MainWindow::playerSucceeded(){
    ui->gameMessage->setText("Congratulations!");
    ui-> blueButton->setEnabled(false);
    ui-> redButton->setEnabled(false);
    dimButtons();
	//Give some time for the player to read the success
	//message before moving to the animation stage.
    QTimer::singleShot(1000,&model,&SimonModel::animateSequence);
}

void MainWindow::playerFailed(){
	ui->gameMessage->setText("You Lose!");
    ui-> startButton->setEnabled(false);
    ui-> blueButton->setEnabled(false);
    ui-> redButton->setEnabled(false);
    dimButtons();
	//Give some time for the player to read the fail
	//message before accepting input from the start button.
    QTimer::singleShot(2000,this, &MainWindow::resetWaitForStartState);
}


void MainWindow::resetWaitForStartState(){
	//Set up the UI to take input from the start button.
    ui->gameMessage->setText("Press Play to Start");
    ui->playerProgress->setValue(0);
    ui-> startButton->setEnabled(true);
}

/**
 * @brief Lower the color of the button to indicate to
 *        the player that the buttons is being disabled.
 */
void MainWindow::dimButtons(){
    ui->redButton->setStyleSheet( QString("QPushButton {background-color: rgb(150,50,50);} QPushButton:pressed {background-color: rgb(225,50,50);}"));
    ui->blueButton->setStyleSheet( QString("QPushButton {background-color: rgb(50,50,150);} QPushButton:pressed {background-color: rgb(50,50,225);}"));
}

/**
 * @brief Draws the bubbles effect on the MainWindow.
 */
void MainWindow::paintEvent(QPaintEvent *event){
    //Sets the bubble painter to this window.
    QPainter bubblePainter(this);
    //Sets the pen
    bubblePainter.setPen(bubblePen);
    //LocksMakes sure that the list does not
    //get modified while being iterate through.
	bubbleLock.lock();
	for (std::tuple<float, float>& coor : particleToDraw){
        //For each coordinates in particleToDraw,
        //draws the bubles.
		QRect bubble((int) std::get<0>(coor), (int) std::get<1>(coor), 60,60);
        QRect smallerBubble((int) std::get<0>(coor)+13, (int) std::get<1>(coor)+13, 10,10);
        bubblePainter.setBrush(bubbleBrush);
        bubblePainter.drawEllipse(bubble);
        bubblePainter.setBrush(smallerBubbleBrush);
        bubblePainter.drawEllipse(smallerBubble);
    }
	bubbleLock.unlock();
}

/**
 * @brief Updates the particle to draw list with new coordinates or
 *        new particle entity.
 * @param list-the updated coordiantes list from BubbleManager.
 */
void MainWindow::updatePainter(std::vector<std::tuple<float, float>> list){
    //Updates the particle list. Makes sure that the list does not
    //get modified while being iterate through.
    bubbleLock.lock();
	particleToDraw = list;
    bubbleLock.unlock();
    //Updates the UI to display the bubbles.
    update();
}
