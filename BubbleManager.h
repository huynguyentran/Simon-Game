/* Huy Nguyen u1315096
 * William Erignac u1308418
 *
 * BubbleManager keeps track and updates the positions of
 * bubbles in the background.
 */


#ifndef BUBBLEMANAGER_H
#define BUBBLEMANAGER_H
#include <QObject>
#include <vector>
#include <tuple>
#include <mutex>


class BubbleManager:public QObject{
 Q_OBJECT

private:
	//The width of the space to spawn bubbles.
    int width;
	//The height of the space to spawn bubbles.
    int height;
	//The positions of all the active particles.
    std::vector<std::tuple<float, float>*> particleList;
	//The maximum number of particles on screen at any given time.
    int maxParticles = 15;
	//The amount of time that needs to pass before a new particle can be spawned (in ms).
    int timeBetweenSpawn = 200;
	//The amount of time that passes between updatePositions calls (in ms).
    int timeBetweenFrames;
	//The amount of time that has passed since a particle has been spawned.
    int timePassedSinceLastSpawn;
	//The constant velocity that the particles travel upwards in pixels / second.
    float particleVelocity = 200.0f;
	//A mutex that prevents particleList from being edited and edited / accessed on two different threads.
    std::mutex particleLocker;

signals:
	//A call at the end of updateParticles that provides a copy of the positions of the current particles.
	void drawParticles(std::vector<std::tuple<float, float>> list);

public slots:
	/*Updates the positions of all active particles,
	 *adds paticles when there's less than the max number of particles
	 *and the timePassedSinceLastSpawn has reached timeBetweenSpawn,
	 *and removes particles that have hit y = 0.
	 */
    void updatePositions();

public:
    BubbleManager(int width, int height, int _timeBetweenFrames);
    ~BubbleManager();
};

#endif // BUBBLEMANAGER_H
