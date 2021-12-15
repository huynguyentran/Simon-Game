/* Huy Nguyen u1315096
 * William Erignac u1308418
 *
 * BubbleManager keeps track and updates the positions of
 * bubbles in the background.
 */

#include "BubbleManager.h"
#include <random>
#include <algorithm>


BubbleManager::BubbleManager(int screenW, int screenH, int _timeBetweenFrames){
    width = screenW;
    height = screenH;
	timeBetweenFrames = _timeBetweenFrames;
	//Since we want to immediately spawn particles,
	//timePassedSinceLastSpawn is set to timeBetweenSpawn.
	timePassedSinceLastSpawn = timeBetweenSpawn;
	particleList.clear();
}

/*Updates the positions of all active particles,
 *adds paticles when there's less than the max number of particles
 *and the timePassedSinceLastSpawn has reached timeBetweenSpawn,
 *and removes particles that have hit y = 0.
 */
void BubbleManager::updatePositions(){
	//A list of particles that have hit y = 0.
    std::vector<int> toRemove;

	particleLocker.lock();
	//If there's space to add particles, and the minimum amount of time has passed
	//we need to add particles.
	if(particleList.size() < maxParticles && timePassedSinceLastSpawn >= timeBetweenSpawn)
    {
		float horizontalMultiplier = ((float) rand()) / RAND_MAX; //float from 0 to 1
		std::tuple<float, float>* newParticle = new std::tuple<float, float>(std::make_tuple((int)(width * horizontalMultiplier),(int) height));
        particleList.push_back(newParticle);

		timePassedSinceLastSpawn = 0;
	}
	else if (timePassedSinceLastSpawn < timeBetweenSpawn)
		timePassedSinceLastSpawn += timeBetweenFrames;

	//This generator and normal distribution will define the horizontal
	//movement of every particle.
	std::default_random_engine generator;
	//The constructor arguments for the normal distribution are the mean and standard deviation.
	std::normal_distribution<float> distribution(0.0f,1.5f);

	//Update the positions of all the particles with time.
    for (int i = 0; i < particleList.size();i++){
		std::tuple<float, float> &particle = *particleList[i];

		//Apply the constant upwards displacement.
        std::get<1>(particle) -= (particleVelocity * (timeBetweenFrames / 1000.0f));
		//We clapm the horizontal movement as to prevent huge jumps.
		float horizontalMovement = std::clamp(distribution(generator),-3.0f,3.0f);
        std::get<0>(particle) += horizontalMovement;
		//Check if this particle needs to be deleted.
		if (std::get<1>(particle) <= 0)
             toRemove.push_back(i);
    }

	//Remove the particles that are at the top of the defined area.
	for (int i = toRemove.size()-1; i>=0;i--){
	   std::tuple<float, float>* particle = particleList[toRemove[i]];
	   particleList.erase(particleList.begin()+toRemove[i]);
	   delete particle;
    }

	//duplicate all the positions to be sent to the view.
	std::vector<std::tuple<float,float>> unlinkedPositions;
	for(std::tuple<float,float>* particlePtr : particleList)
	{
		std::tuple<float, float> copy = *particlePtr;
		unlinkedPositions.push_back(copy);
	}
	particleLocker.unlock();

	//Draw all active particles.
	emit drawParticles(unlinkedPositions);
}

BubbleManager::~BubbleManager()
{
	particleLocker.lock();
	for(std::tuple<float, float>* particle : particleList)
        delete particle;
	particleList.clear();
	particleLocker.unlock();
}
