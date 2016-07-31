#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#define PI 3.14f

using namespace std;
#include <vector>
#include <iostream>
#include "Vector3.h"
#include "Color.h"
#include "camera.h"

class Particle
{
public:
	Particle(Camera&);
	void Draw();
	void GroundCollision(float);
	void DrawQuad();
	void DrawCircle();
	~Particle();

	Camera* camera;
	Vector3 position;
	Vector3 velocity;
	Color color;
	float size;
	float age;
	float lifeTime;
};


#endif