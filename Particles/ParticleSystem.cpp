#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{

}

ParticleSystem::ParticleSystem(Camera& cam, int numParticles)
{
	// Set camera
	this->camera = &cam;

	// Resize particle vector and add camera to each particle
	Resize(numParticles);
}

void ParticleSystem::Begin()
{
	// Initialize all particle instances
	for (int i = 0; i < particles.size(); i++)
		InitParticle(particles[i]);
}

void ParticleSystem::InitParticle(Particle& particle)
{
	// Initialize / Reset particle instance with random emission properties
	float inclination = ((rand() / (float)RAND_MAX) * (maxInclination - minInclination)) + minInclination;
	inclination = toRadians(inclination);	// convert to radians

	float azimuth = ((rand() / (float)RAND_MAX) * (maxAzimuth - minAzimuth)) + minAzimuth;
	azimuth = toRadians(azimuth);		// convert to radians

	float radius = ((rand() / (float)RAND_MAX) * (maxRadius - minRadius)) + minRadius;
	float speed = ((rand() / (float)RAND_MAX) * (maxSpeed - minSpeed)) + minSpeed;
	float lifetime = ((rand() / (float)RAND_MAX) * (maxLifetime - minLifetime)) + minLifetime;
	float size = ((rand() / (float)RAND_MAX) * (maxSize - minSize)) + minSize;

	// Generate random color
	float r = rand() / (float)RAND_MAX;
	float g = rand() / (float)RAND_MAX;
	float b = rand() / (float)RAND_MAX;
	float a = rand() / (float)RAND_MAX;	// Don't know why alpha channel doesnt do anything
	Color randColor = Color(r, g, b, a);

	// Convert Spherical coordinates to Cartesian
	float X = sinf(inclination) * cosf(azimuth);
	float Y = sinf(inclination) * sinf(azimuth);
	float Z = -cosf(inclination); // Face down -Z axis
	Vector3 coord(X, Y, Z);

	// Apply rotation (X-axis only)
	Matrix34 rot;
	rot.MakeRotateX(toRadians(rotation));
	rot.Transform(coord, coord);

	// Determine initial position and velocity
	particle.position = (coord * radius) + origin;  // apply raidal / origin offset
	particle.velocity = coord * speed;				// apply speed value

	// Set other properties
	particle.lifeTime = lifetime;
	particle.age = 0;
	particle.size = size;

	// Apply random color properties
	if (randomizeColor)
		particle.color = randColor;
	else
		particle.color = color;
}

void ParticleSystem::Update(float timeDelta, float floor_pos, Vector3 wind)
{
	for (int i = 0; i < particles.size(); i++)
	{
		// Age particle
		particles[i].age += timeDelta;

		// Reset particle instance if over lifetime
		if (particles[i].age > particles[i].lifeTime)
			InitParticle(particles[i]);

		// Apply gravitational force
		if (useGravity)
			particles[i].velocity = particles[i].velocity + (Gravity * timeDelta);

		// Apply wind force
		particles[i].velocity = particles[i].velocity + wind;

		// Move particle
		particles[i].position = particles[i].position + (particles[i].velocity * timeDelta);

		// Ground collision
		if (particles[i].position.y <= floor_pos)
			particles[i].GroundCollision(floor_pos);
	}
}

void ParticleSystem::Render()
{
	for (int i = 0; i < particles.size(); i++)
		particles[i].Draw();
}

void ParticleSystem::Resize(int numParticles)
{
	if (numParticles != particles.size())
		particles.resize(numParticles, Particle(*camera));
}

ParticleSystem::~ParticleSystem()
{

}

// Helper function. Convert degrees to radians
float ParticleSystem::toRadians(float degrees)
{
	return degrees * (PI / 180.0f);
}