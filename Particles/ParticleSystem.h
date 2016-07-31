#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include "Particle.h"
#include "matrix34.h"

class ParticleSystem
{
public:
	ParticleSystem();
	ParticleSystem(Camera&, int);
	void Begin();
	void InitParticle(Particle&);
	void Update(float, float, Vector3);
	void Render();
	void Resize(int);
	~ParticleSystem();

	// Collection of all particle instances
	vector<Particle> particles;

	// Position of particle system
	Vector3 origin;

	// Rotation of particle system (applied about X-axis)
	float rotation;

	// Camera for particles to look at
	Camera* camera;

	// Can choose whether or not to use gravity
	Vector3 Gravity = Vector3(0.f, -9.8f, 0.f);
	bool useGravity = true;

	// Variance for particles
	Color color;
	bool randomizeColor = true;

	float minInclination;
	float maxInclination;

	float minAzimuth;
	float maxAzimuth;

	float minRadius;
	float maxRadius;

	float minSpeed;
	float maxSpeed;

	float minLifetime;
	float maxLifetime;

	float minSize;
	float maxSize;

private:
	float toRadians(float);
};


#endif