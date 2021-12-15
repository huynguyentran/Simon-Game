/* Huy Nguyen u1315096
 * William Erignac u1308418
 *
 * Simon Game model handles all the game logic and pass the
 * information to the UI to display. The model will create and
 * add on to the sequence. The model will remember what the player
 * has choosed, determine if it is the correct sequence or not, and
 * display result accordingly.
 */

#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <vector>

class SimonModel: public QObject{
 Q_OBJECT
public:
    explicit SimonModel(QObject *parent = nullptr);
    //An enum that contains Red and Blue to indicate
    //what the player has chose.
    enum Color{Red, Blue};
    //Add random color to the sequence.
    void addRandomColor();
    //A method to indicate what button has been clicked.
    void buttonClicked(Color input);

public slots:
    //Slot to indicate the start button has been clicked.
    void startGame();
    //Slot to send to the UI which button need to be flash.
    void animateSequence();

private:
    //The sequence of color.
    std::vector<Color> sequence;
    //Indicates the index of color that is being animated.
    int animationIndex;
    //Indicates where the player is at on the sequence.
    int playerProgressionIndex;

signals:
    //Send the signal of which color to flash to the UI.
    void flashColor(Color color);
    //Set the range of the progress bar based on the
    //size of the sequence.
    void setRange(int size);
    //Send the calculated flash period to the UI.
    void calculatedFlashPeriod(int period);
    //Send the signal to disables buttons when
    //the player starts the game.
    void enterPlaystate();
    //Update the progress bar if the player
    //press the correct button.
    void updateProgress(int value);
    //Send the signals to the UI whether the player
    //successful enters the correct sequence or not.
    void success();
    void fail();
};

#endif // MODEL_H
