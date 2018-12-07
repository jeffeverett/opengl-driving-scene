#include "globals.hpp"
#include "Physics/debugdrawer.hpp"


using namespace Physics;


DebugDrawer::DebugDrawer() {
    mShader = simpleShader;
}

void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
#ifdef DEBUG
    //std::cout << "Drawing line with color (" << color[0] << ", " << color[1] << ", " << color[2] << ")" << std::endl;
#endif

    // https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=11517
    GLuint VAO, VBO;
    GLfloat points[12];

    points[0] = from.x();
    points[1] = from.y();
    points[2] = from.z();
    points[3] = color.x();
    points[4] = color.y();
    points[5] = color.z();

    points[6] = to.x();
    points[7] = to.y();
    points[8] = to.z();
    points[9] = color.x();
    points[10] = color.y();
    points[11] = color.z();

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);

    glBindVertexArray(VAO);
    mShader->use();
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void DebugDrawer::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) {
    //TODO
}

void DebugDrawer::draw3dText (const btVector3 &location, const char *textString) {
    textRenderer->renderText(textString, 1, glm::vec3(0.5,0.5,0.5));
}

void DebugDrawer::reportErrorWarning(const char *warningString) {
    std::cerr << warningString << std::endl;
}

void DebugDrawer::setDebugMode(int debugMode) {
    mDebugMode = debugMode;
}

int DebugDrawer::getDebugMode() const {
    return mDebugMode;
}