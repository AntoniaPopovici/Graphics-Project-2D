# include <GL/glew.h>
# include <GL/freeglut.h> 
# include "loadShaders.h"
# include "glm/gtx/transform.hpp"
# include "glm/gtc/type_ptr.hpp"
# include <vector>
# include "cmath"
# define M_PI           3.14159265358979323846
# define MIN_Y			-20.0f

// important variables
GLuint
EboId,
birdVaoId,
birdVboId,
GrassVboId,
GrassVaoId,
TrunkVboId,
TrunkVaoId,
CrownVaoId,
CrownVboId,
ProgramId,
myMatrixLocation;

glm::mat4
myMatrix, resizeMatrix;

const float separationDistance = .02;
const float alignmentDistance = 0.6;
const float cohesionDistance = .7;
float xMin = -80, xMax = 80.f, yMin = -60.f, yMax = 60.f;
float Ox = -20.f;
float Oy = 10.275f;
int windowWidth = 900;
int windowHeight = 900;
bool right = false;
bool left = false;
bool up = false;
bool down = false;


class Boid {
public:
	float px, py;  // position
	float vx, vy;  // velocity

	Boid(float _x, float _y) : px(_x), py(_y), vx(0.0), vy(0.0) {}
};

std::vector<Boid> boids;


void shaders(void)
{
	ProgramId = loadShaders("shader.vert", "shader.frag");
	glUseProgram(ProgramId);
}


// functions for keyboard: press and release keys

void keyboardP(unsigned char key, int x, int y) {
	switch (key) {
	case 'd':
		right = true;
		break;
	case 'a':
		left = true;
		break;
	case 'w':
		up = true;
		break;
	case 's':
		down = true;
		break;
	case 'q':
		exit(0);
		break;
	}
}

void keyboardR(unsigned char key, int x, int y) {
	switch (key) {
	case 'd':
		right = false;
		break;
	case 'a':
		left = false;
		break;
	case 'w':
		up = false;
		break;
	case 's':
		down = false;
		break;
	}
}


// functions for boids
void initBoids() {
	for (int i = 0; i < 50; i++) {
		float x = static_cast<float>(rand()) / RAND_MAX;
		float y = static_cast<float>(rand()) / RAND_MAX;
		boids.push_back(Boid(x, y));
	}
}
std::vector<float> rotationAngles;

void updateBoids(int value) {

	for (Boid& boid : boids) {
		// separation - boids don't get too close
		float sepX = 0.0;
		float sepY = 0.0;
		for (const Boid& other : boids) {
			if (&boid != &other) {
				float dx = other.px - boid.px;
				float dy = other.py - boid.py;
				float d = std::sqrt(dx * dx + dy * dy);
				if (d < separationDistance) {
					sepX -= dx / d;
					sepY -= dy / d;
				}
			}
		}

		// alignment - boids align their velocities with neighboring boids
		float avgVx = 0.0;
		float avgVy = 0.0;
		int count = 0;
		for (const Boid& other : boids) {
			if (&boid != &other) {
				float dx = other.px - boid.px;
				float dy = other.py - boid.py;
				float d = std::sqrt(dx * dx + dy * dy);
				if (d < alignmentDistance) {
					avgVx += other.vx;
					avgVy += other.vy;
					count++;
				}
			}
		}
		if (count > 0) {
			avgVx /= count;
			avgVy /= count;
		}

		// cohesion - boids move towards the center of mass of neighbors
		float avgX = 0.0;
		float avgY = 0.0;
		count = 0;
		for (const Boid& other : boids) {
			if (&boid != &other) {
				float dx = other.px - boid.px;
				float dy = other.py - boid.py;
				float d = std::sqrt(dx * dx + dy * dy);
				if (d < cohesionDistance) {
					avgX += other.px;
					avgY += other.py;
					count++;
				}
			}
		}
		if (count > 0) {
			avgX /= count;
			avgY /= count;
		}

		// update velocity with speed limit
		boid.vx += sepX + 0.1 * avgVx + 0.01 * (avgX - boid.px);
		boid.vy += sepY + 0.1 * avgVy + 0.01 * (avgY - boid.py);

		float speedLimit = 0.001f;
		float speed = std::sqrt(boid.vx * boid.vx + boid.vy * boid.vy);
		if (speed > speedLimit) {
			boid.vx = (boid.vx / speed) * speedLimit;
			boid.vy = (boid.vy / speed) * speedLimit;
		}

		// update position with the grass as limit
		boid.px += boid.vx;
		boid.py += boid.vy;

		if (boid.py < MIN_Y) {
			boid.py = MIN_Y;
			boid.vy = -boid.vy; // reverse y velocity to bounce off the bottom boundary
		}

		float defaultSpeed = 0.1f;
		for (Boid& boid : boids) {
			if (!right && !left && !up && !down) {
				// Aplică o viteză prestabilită în direcția dorită
				boid.vx += defaultSpeed;
				boid.vy += 0.0; // Poți ajusta această valoare în funcție de direcția dorită
			}
		}
	}
	if (right) {
		// 'd' - move to the right
		for (Boid& boid : boids) {
			boid.vx += .01;
		}
	}
	else if (left) {
		// 'a' - move to the left
		for (Boid& boid : boids) {
			boid.vx -= .01;
		}
	}
	if (up) {
		// 'w' - move up
		for (Boid& boid : boids) {
			boid.vy += .01;
		}
	}
	else if (down) {
		// 's' - move down
		for (Boid& boid : boids) {
			boid.vy -= .01;
		}
	}

	glutPostRedisplay();

	glutTimerFunc(16, updateBoids, 0);
}
void grassVBO(void)
{
	static const GLfloat grassVertices[] = {
		-100.0f, -50.0f, 0.0f, 1.0f,
		100.0f, -50.0f, 0.0f, 1.0f,
		100.0f, -30.f, 0.0f, 1.0f,
		-100.0f, -30.f, 0.0f, 1.0f,
	};

	int objectTypes[4];
	for (int i = 0; i < 4; i++) {
		objectTypes[i] = 2; // type 2 => green color
	}

	glGenVertexArrays(1, &GrassVaoId);
	glBindVertexArray(GrassVaoId);


	glGenBuffers(1, &GrassVboId);
	glBindBuffer(GL_ARRAY_BUFFER, GrassVboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices) + sizeof(objectTypes), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(grassVertices), grassVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(grassVertices), sizeof(objectTypes), objectTypes);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_INT, 0, (const GLvoid*)sizeof(grassVertices));
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_INT, 0, (const GLvoid*)sizeof(grassVertices));
}

void trunkVBO(void)
{
	static  GLfloat trunkVertices[] = {
		-20.0f, -3.0f, 0.0f, 1.0f,
		-25.0f, -35.0f, 0.0f, 1.0f,
		-15.0f, -35.f, 0.0f, 1.0f,
	};

	int objectTypes[3];
	for (int i = 0; i < 3; i++) {
		objectTypes[i] = 0; // type 0 => black color
	}

	// translation matrix - moves the trunk to the left
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, 0.0f));

	for (int i = 0; i < sizeof(trunkVertices) / sizeof(trunkVertices[0]); i += 4) {
		glm::vec4 vertex(trunkVertices[i], trunkVertices[i + 1], trunkVertices[i + 2], trunkVertices[i + 3]);
		vertex = translationMatrix * vertex;
		trunkVertices[i] = vertex.x;
		trunkVertices[i + 1] = vertex.y;
		trunkVertices[i + 2] = vertex.z;
		trunkVertices[i + 3] = vertex.w;
	}

	glGenVertexArrays(1, &TrunkVaoId);
	glBindVertexArray(TrunkVaoId);

	glGenBuffers(1, &TrunkVboId);
	glBindBuffer(GL_ARRAY_BUFFER, TrunkVboId);

	glBufferData(GL_ARRAY_BUFFER, sizeof(trunkVertices) + sizeof(objectTypes), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(trunkVertices), trunkVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(trunkVertices), sizeof(objectTypes), objectTypes);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_INT, 0, (const GLvoid*)sizeof(trunkVertices));
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_INT, 0, (const GLvoid*)sizeof(trunkVertices));
}

void calculateCrownVertices(float* crownVertices, int n, float r1, const glm::vec3& translation) {
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);

	crownVertices[0] = Ox;
	crownVertices[1] = Oy;
	crownVertices[2] = 0.0f;
	crownVertices[3] = 1.0f;

	// drawing the crown using triangles (=> hexagon of n vertices)
	for (int k = 1; k <= n; k++) {
		float angle = (2 * M_PI * k) / n;
		glm::vec4 vertex(Ox + r1 * cos(angle), Oy + r1 * sin(angle), 0.0f, 1.0f);
		vertex = translationMatrix * vertex;

		crownVertices[4 * k] = vertex.x;
		crownVertices[4 * k + 1] = vertex.y;
		crownVertices[4 * k + 2] = vertex.z;
		crownVertices[4 * k + 3] = vertex.w;
	}
}

void crownVBO(void)
{
	static const int n = 11; // if modified, the indices[] should be modified too
	static const float r1 = 20.0f;
	glm::vec3 translation(-10.0f, 0.0f, 0.0f);

	GLfloat crownVertices[5 * n + 1];

	int objectTypes[5 * n + 1];
	for (int i = 0; i < 5 * n + 1; i++) {
		objectTypes[i] = 1; // type 1 => red color
	}

	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		0, 7, 8,
		0, 8, 9,
		0, 9, 10,
		0, 10, 11,
		0, 11, 1
	};

	calculateCrownVertices(crownVertices, n, r1, translation);

	glGenVertexArrays(1, &CrownVaoId);
	glBindVertexArray(CrownVaoId);

	glGenBuffers(1, &CrownVboId);
	glBindBuffer(GL_ARRAY_BUFFER, CrownVboId);

	glBufferData(GL_ARRAY_BUFFER, sizeof(crownVertices) + sizeof(objectTypes), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(crownVertices), crownVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(crownVertices), sizeof(objectTypes), objectTypes);

	glGenBuffers(1, &EboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_INT, 0, (const GLvoid*)sizeof(crownVertices));
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_INT, 0, (const GLvoid*)sizeof(crownVertices));
}

void destroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void birdVBO(Boid boid)
{
	float wingspan = 1.0f;

	GLfloat birdVertices[] = {
		boid.px, boid.py, 0.0f, 1.0f,
		boid.px - wingspan / 2, boid.py, 0.0f, 1.0f,
		boid.px + wingspan / 2, boid.py, 0.0f, 1.0f,

		boid.px, boid.py, 0.0f, 1.0f,
		boid.px - wingspan / 2, boid.py - wingspan / 2, 0.0f, 1.0f,
		boid.px + wingspan / 2, boid.py - wingspan / 2, 0.0f, 1.0f,
	};

	int objectTypes[6];
	for (int i = 0; i < 6; i++) {
		objectTypes[i] = 0; // type 0 => color for birds
	}

	glGenVertexArrays(1, &birdVaoId);
	glBindVertexArray(birdVaoId);

	glGenBuffers(1, &birdVboId);
	glBindBuffer(GL_ARRAY_BUFFER, birdVboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(birdVertices) + sizeof(objectTypes), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(birdVertices), birdVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(birdVertices), sizeof(objectTypes), objectTypes);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_INT, 0, (const GLvoid*)sizeof(birdVertices));
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_INT, 0, (const GLvoid*)sizeof(birdVertices));
}

void destroyVBO(void)
{
	glDisableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &CrownVboId);
	glDeleteBuffers(1, &EboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &CrownVaoId);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &birdVaoId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &GrassVaoId);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &TrunkVaoId);
}

void cleanup(void)
{
	destroyShaders();
	destroyVBO();
}


void initialize(void)
{
	glClearColor(0.529f, 0.808f, 0.922f, 1.0f); // blue sky

	grassVBO();
	trunkVBO();
	crownVBO();
	shaders();
	initBoids();

	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);

}


void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	// birds
	for (size_t i = 0; i < boids.size(); ++i) {
		float rotationAngle = atan2(boids[i].vy, boids[i].vx);
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::degrees(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(boids[i].px, boids[i].py, 0.0f));
		myMatrix = translationMatrix * rotationMatrix * resizeMatrix;

		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

		birdVBO(boids[i]);
		glBindVertexArray(birdVaoId);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	// grass
	glBindVertexArray(GrassVaoId);
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// trunk
	glBindVertexArray(TrunkVaoId);
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// crown
	glBindVertexArray(CrownVaoId);
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 33, GL_UNSIGNED_INT, (void*)(0));

	glFlush();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(400, -100);
	glutInitWindowSize(900, 900);
	glutCreateWindow("2D SCENE: Autumn Landscape");

	glewInit();

	initialize();
	glutKeyboardFunc(keyboardP);
	glutKeyboardUpFunc(keyboardR);
	glutDisplayFunc(render);
	glutTimerFunc(0, updateBoids, 0); // start animation
	glutMainLoop();

	glutCloseFunc(cleanup);

	return 0;
}
