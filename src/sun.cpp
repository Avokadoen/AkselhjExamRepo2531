
#include "sun.hpp"

Sun::Sun() {
	time = DAY;
	skyColor = DaySkyColor;
	lerpPos = 0;
	secondsInADay = 5.0f;
	currentLight = Day;
	timeRunning = true;
}

void Sun::update(float deltaTime) {
	if (timeRunning) {
		if (time == DAY) {
			if (lerpPos < 1.0f) {
				currentLight = lerpDirLight(Day, Dusk, lerpPos);
				skyColor = lerpVec(DaySkyColor, DuskSkyColor, lerpPos);
				lerpPos += deltaTime / secondsInADay;
			}
			else {
				time = DUSK;
				lerpPos = 0;
			}
		}

		if (time == DUSK) {
			if (lerpPos < 1.0f) {
				currentLight = lerpDirLight(Dusk, Night, lerpPos);
				skyColor = lerpVec(DuskSkyColor, NightSkyColor, lerpPos);
				lerpPos += deltaTime / secondsInADay;
			}
			else {
				time = NIGHT;
				lerpPos = 0;
			}
		}

		if (time == NIGHT) {
			if (lerpPos < 1.0f) {
				currentLight = lerpDirLight(Night, Dawn, lerpPos);
				skyColor = lerpVec(NightSkyColor, DawnSkyColor, lerpPos);
				lerpPos += deltaTime / secondsInADay;
			}
			else {
				time = DAWN;
				lerpPos = 0;
			}
		}

		if (time == DAWN) {
			if (lerpPos < 1.0f) {
				currentLight = lerpDirLight(Dawn, Day, lerpPos);
				skyColor = lerpVec(DawnSkyColor, DaySkyColor, lerpPos);
				lerpPos += deltaTime / secondsInADay;
			}
			else {
				time = DAY;
				lerpPos = 0;
			}
		}
	}
}

void Sun::syncWithShader(Shader shader) {
	shader.setBool("dirSet", true);
	shader.setVec3("dirLight.direction", currentLight.direction);
	shader.setVec3("dirLight.ambient", currentLight.ambient);
	shader.setVec3("dirLight.diffuse", currentLight.diffuse);
	shader.setVec3("dirLight.specular", currentLight.specular);
}

glm::vec3 Sun::getSkyColor() {
	return skyColor;
}

void Sun::toggleTime() {
	timeRunning = !timeRunning;
}

void Sun::setTime(TimeOfTheDay time, float lerpPos) {
	this->time = time;
	this->lerpPos = lerpPos;
}

DirLight Sun::lerpDirLight(DirLight v0, DirLight v1, float t) {
	DirLight outPut;
	outPut.direction	= lerpVec(v0.direction, v1.direction, t);
	outPut.ambient		= lerpVec(v0.ambient, v1.ambient, t);
	outPut.diffuse		= lerpVec(v0.diffuse, v1.diffuse, t);
	outPut.specular		= lerpVec(v0.specular, v1.specular, t);
	return outPut;
}

glm::vec3 Sun::lerpVec(glm::vec3 v0, glm::vec3 v1, float t) {
	glm::vec3 outPut;
	outPut.x = lerp(v0.x, v1.x, t);
	outPut.y = lerp(v0.y, v1.y, t);
	outPut.z = lerp(v0.z, v1.z, t);
	return outPut;
}

float Sun::lerp(float v0, float v1, float t){
	return (1 - t) * v0 + t * v1;
}

