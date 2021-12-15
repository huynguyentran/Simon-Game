/* Huy Nguyen u1315096
 * William Erignac u1308418
 *
 * Simon Game model handles all the game logic and pass the
 * information to the UI to display. The model will create and
 * add on to the sequence. The model will remember what the player
 * has choosed, determine if it is the correct sequence or not, and
 * display result accordingly.
 */

#include "model.h"
#include <math.h>
#include <iostream>

SimonModel::SimonModel(QObject *parent) : QObject(parent){
    //Initializing values.
    //animationIndex indicates what is the current
    //index of the playing sequence.
    animationIndex = 0;
    //playerProgressIndex indicates where on the
    //sequence that the player need to insert input.
    playerProgressionIndex = 0;
}

/**
 * @brief Adds random color to the end of the sequence.
 */
void SimonModel::addRandomColor(){
    //Random between 1 and 0. If
    //1, we add color Red to the sequence.
    //Else, we add color Blue.
    int num = rand()%2+1;
    if (num ==1)
        sequence.push_back(Red);
    else
        sequence.push_back(Blue);
}

/**
 * @brief Sets up the sequence and the flash period
 *        when the game first starts.
 */
void SimonModel::startGame(){
    //Clears the sequence and adds
    //2 random colors at the start.
    sequence.clear();
    addRandomColor();
    addRandomColor();

    //Emit the range to the UI.
    //The progress bar will have displayed
    //the progress of the player based on
    //sequence size.
    emit setRange(sequence.size());

    //Formula for the flash period, the larger
    //the sequence size, the quicker the flash
    //appears and disappears
    int flashPeriod = 1000 - ((-1/(sequence.size()*10.0f+150)*100000) + 700);
    //Emit the information to the UI.
    emit calculatedFlashPeriod(flashPeriod);

    //Plays the sequence.
    animateSequence();
}


/**
 * @brief Plays the sequence.
 */
void SimonModel::animateSequence(){
        //If the UI has not finished playing the sequence.
        if (animationIndex < sequence.size()){
            //Emits to the UI the color that needs to
            //be flashed and increase the animationIndex
            Color color = sequence[animationIndex];
            animationIndex++;
            emit flashColor(color);
       }
        else{
            //Sets the player progress at 0, set the range
            //for the progress bar, and let the player play
            //the game.
            playerProgressionIndex = 0;
            emit setRange(sequence.size());
            emit enterPlaystate();
        }
}


/**
 * @brief Handles the logic between the player and color buttons.
 * @param input-the color button that the player clicked.
 */
void SimonModel::buttonClicked(Color input){
    //If the player presses the correct button on the sequence.
    Color expected = sequence[playerProgressionIndex];
    if (input == expected)
    {
        //Update Progress Bar
        playerProgressionIndex++;
        emit updateProgress(playerProgressionIndex);
        //If the player has reached the end of the sequence.
        if (playerProgressionIndex == sequence.size()){
            //Add another color to the sequence.
            addRandomColor();
            //Set the player progress back to 0.
            animationIndex=0;
            //Update the flash period of the color buttons.
            int flashPeriod = (int) (800*(pow(1.3, -1 * (double) (sequence.size() - 2)) + 0.25));
            emit calculatedFlashPeriod(flashPeriod);
            //Display the success message to the player.
            emit success();
        }
    }
    else
    {
        //Set the progress of the player back to 0.
        animationIndex=0;
        //Display the fail message to the player.
        emit fail();
    }
}
