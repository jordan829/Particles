#include "Particle.h"

Particle::Particle(Camera& cam)
{
	camera = &cam;
}

void Particle::Draw()
{
	// Setup particle properties
	GLfloat mat[] = { color.r, color.g, color.b, 0.1f };
	GLfloat shine[] = { 0.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
	glMaterialfv(GL_FRONT, GL_SHININESS, shine);

	// Set position and look at camera
	glLoadIdentity();
	glTranslatef(position.x, position.y, position.z);
	glRotatef(-camera->GetAzimuth(), 0.0f, 1.0f, 0.0f);
	glRotatef(-camera->GetIncline(), 1.0f, 0.0f, 0.0f);

	// Draw particle
	DrawCircle();
	//DrawQuad();
}

void Particle::GroundCollision(float floor_pos)
{
	float elasticity = 0.2f;
	float friction = 0.2f;
	position.y = floor_pos - position.y;
	velocity.y = -elasticity * velocity.y;
	velocity.x = (1 - friction)* velocity.x;
	velocity.z = (1 - friction)* velocity.z;
}

void Particle::DrawQuad()
{
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
	glVertex3f(-size, -size, 0);
	glVertex3f(size, -size, 0);
	glVertex3f(size, size, 0);
	glVertex3f(-size, size, 0);
	glEnd();
}

void Particle::DrawCircle()
{
	glNormal3f(0, 0, 1);
	glBegin(GL_POLYGON);
	for (double i = 0; i < 2 * PI; i += PI / 12)
		glVertex3f(cos(i) * size, sin(i) * size, 0.0);
	glEnd();
}

Particle::~Particle()
{

}