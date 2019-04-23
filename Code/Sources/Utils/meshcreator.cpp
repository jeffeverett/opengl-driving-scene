#include "Utils/meshcreator.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace Utils
{
    std::shared_ptr<Assets::Mesh> MeshCreator::create()
    {
        return std::make_shared<Assets::Mesh>(mPositions, mNormals, mTexCoords);
    }

    int MeshCreator::addOpenCylinder(float tDiff, float height1, float height2, float radius)
    {
        int partitions = 10;
        float theta = 360;
        for (int i = 1; i <= partitions; i++) {
            // Positions
            glm::vec3 leftTop = glm::vec3(radius*glm::cos(glm::radians((i-1)*theta/partitions)), height2, radius*glm::sin(glm::radians((i-1)*theta/partitions)));
            glm::vec3 leftBottom = glm::vec3(radius*glm::cos(glm::radians((i-1)*theta/partitions)), height1, radius*glm::sin(glm::radians((i-1)*theta/partitions)));
            glm::vec3 rightTop = glm::vec3(radius*glm::cos(glm::radians(i*theta/partitions)), height2, radius*glm::sin(glm::radians(i*theta/partitions)));
            glm::vec3 rightBottom = glm::vec3(radius*glm::cos(glm::radians(i*theta/partitions)), height1, radius*glm::sin(glm::radians(i*theta/partitions)));

            // Create first triangle
            mPositions.push_back(leftTop);
            mPositions.push_back(leftBottom);
            mPositions.push_back(rightBottom);
            mNormals.push_back(glm::vec3(leftTop[0], 0, leftTop[2]));
            mNormals.push_back(glm::vec3(leftBottom[0], 0, leftBottom[2]));
            mNormals.push_back(glm::vec3(rightBottom[0], 0, rightBottom[2]));
            mTexCoords.push_back(glm::vec2(mTextureScaleS*(i-1)/partitions, mTextureScaleT*(height1+tDiff)));
            mTexCoords.push_back(glm::vec2(mTextureScaleS*(i-1)/partitions, mTextureScaleT*height1));
            mTexCoords.push_back(glm::vec2(mTextureScaleS*i/partitions, mTextureScaleT*height1));

            // Create second triangle
            mPositions.push_back(leftTop);
            mPositions.push_back(rightBottom);
            mPositions.push_back(rightTop);
            mNormals.push_back(glm::vec3(leftTop[0], 0, leftTop[2]));
            mNormals.push_back(glm::vec3(rightBottom[0], 0, rightBottom[2]));
            mNormals.push_back(glm::vec3(rightTop[0], 0, rightTop[2]));
            mTexCoords.push_back(glm::vec2(mTextureScaleS*(i-1)/partitions, mTextureScaleT*(height1+tDiff)));
            mTexCoords.push_back(glm::vec2(mTextureScaleS*i/partitions, mTextureScaleT*height1));
            mTexCoords.push_back(glm::vec2(mTextureScaleS*i/partitions, mTextureScaleT*(height1+tDiff)));
        }

        return 6*partitions;
    }

    void MeshCreator::addCircularFace(float xOffset, float radius)
    {
        // Create circular faces
        int thetaPartitions = 20;
        float theta0 = 0;
        for (int i = 1; i<= thetaPartitions; i++) {
            float theta = glm::radians(360.0f*i/thetaPartitions);

            // Positions
            glm::vec3 center(xOffset, 0, 0);
            glm::vec3 left(xOffset, radius*glm::sin(theta0), radius*glm::cos(theta0));
            glm::vec3 right(xOffset, radius*glm::sin(theta), radius*glm::cos(theta));

            // Create triangle
            mPositions.push_back(left);
            mPositions.push_back(center);
            mPositions.push_back(right);
            mNormals.push_back(glm::vec3(0, left[1], left[2]));
            mNormals.push_back(glm::vec3(0, center[1], center[2]));
            mNormals.push_back(glm::vec3(0, right[1], right[2]));
            mTexCoords.push_back(glm::vec2(left[1]/radius, left[2]/radius));
            mTexCoords.push_back(glm::vec2(center[1]/radius, center[2]/radius));
            mTexCoords.push_back(glm::vec2(right[1]/radius, right[2]/radius));

            theta0 = theta;
        }
    }

    void MeshCreator::addRotatedOpenCylinder(float height1, float height2, float radius, float x1, float x2, float angle1, float angle2)
    {
        int verticesUsed = addOpenCylinder(x2-x1, height1, height2, radius);
        unsigned long startIdx = mPositions.size()-verticesUsed;

        // Rotate vertices created in previous call
        glm::mat3 rot1 = glm::eulerAngleZ(-angle1);
        glm::mat3 rot2 = glm::eulerAngleZ(-angle2);
        for (unsigned long i = startIdx; i < mPositions.size(); i+=6) {
            mPositions[i] -= glm::vec3(0, height2, 0);
            mPositions[i] = rot2*mPositions[i];
            mPositions[i] += glm::vec3(0, height2, 0);

            mPositions[i+1] -= glm::vec3(0, height1, 0);
            mPositions[i+1] = rot1*mPositions[i+1];
            mPositions[i+1] += glm::vec3(0, height1, 0);

            mPositions[i+2] -= glm::vec3(0, height1, 0);
            mPositions[i+2] = rot1*mPositions[i+2];
            mPositions[i+2] += glm::vec3(0, height1, 0);

            mPositions[i+3] -= glm::vec3(0, height2, 0);
            mPositions[i+3] = rot2*mPositions[i+3];
            mPositions[i+3] += glm::vec3(0, height2, 0);

            mPositions[i+4] -= glm::vec3(0, height1, 0);
            mPositions[i+4] = rot1*mPositions[i+4];
            mPositions[i+4] += glm::vec3(0, height1, 0);

            mPositions[i+5] -= glm::vec3(0, height2, 0);
            mPositions[i+5] = rot2*mPositions[i+5];
            mPositions[i+5] += glm::vec3(0, height2, 0);
        }

        // Apply x offset to vertices created in previous call
        for (unsigned long i = startIdx; i < mPositions.size(); i+=6) {
            mPositions[i] += glm::vec3(x2, 0, 0);
            mPositions[i+1] += glm::vec3(x1, 0, 0);
            mPositions[i+2] += glm::vec3(x1, 0, 0);
            mPositions[i+3] += glm::vec3(x2, 0, 0);
            mPositions[i+4] += glm::vec3(x1, 0, 0);
            mPositions[i+5] += glm::vec3(x2, 0, 0);
        }
    }

    void MeshCreator::addSphere(float maxPhi, glm::vec3 offset, float radius, float rotation)
    {
        int thetaPartitions = 10;
        int phiPartitions = 10;

        float theta0 = 0;
        float phi0 = 0;

        // Draw lowest partition
        glm::mat3 rot = glm::eulerAngleZ(rotation);
        for (int i = 0; i <= phiPartitions; i++) {
            float phi = glm::radians(maxPhi*i/phiPartitions);
            for (int j = 0; j <= thetaPartitions; j++) {
                float theta = glm::radians(360.0f*j/thetaPartitions);
                // Positions
                glm::vec3 leftTop = glm::vec3(radius*glm::sin(theta0)*glm::sin(phi0), radius*glm::cos(phi0), radius*glm::cos(theta0)*glm::sin(phi0));
                glm::vec3 rightTop = glm::vec3(radius*glm::sin(theta)*glm::sin(phi0), radius*glm::cos(phi0), radius*glm::cos(theta)*glm::sin(phi0));
                glm::vec3 leftBottom = glm::vec3(radius*glm::sin(theta0)*glm::sin(phi), radius*glm::cos(phi), radius*glm::cos(theta0)*glm::sin(phi));
                glm::vec3 rightBottom = glm::vec3(radius*glm::sin(theta)*glm::sin(phi), radius*glm::cos(phi), radius*glm::cos(theta)*glm::sin(phi));


                // Create first triangle
                mPositions.push_back(offset+rot*leftTop);
                mPositions.push_back(offset+rot*leftBottom);
                mPositions.push_back(offset+rot*rightBottom);
                mNormals.push_back(leftTop);
                mNormals.push_back(leftBottom);
                mNormals.push_back(rightBottom);
                mTexCoords.push_back(glm::vec2(theta0/glm::radians(360.0f), phi0/glm::radians(maxPhi)));
                mTexCoords.push_back(glm::vec2(theta0/glm::radians(360.0f), phi/glm::radians(maxPhi)));
                mTexCoords.push_back(glm::vec2(theta/glm::radians(360.0f), phi/glm::radians(maxPhi)));

                // Create second triangle
                mPositions.push_back(offset+rot*leftTop);
                mPositions.push_back(offset+rot*rightBottom);
                mPositions.push_back(offset+rot*rightTop);
                mNormals.push_back(leftTop);
                mNormals.push_back(rightBottom);
                mNormals.push_back(rightTop);
                mTexCoords.push_back(glm::vec2(theta0/glm::radians(360.0), phi0/glm::radians(maxPhi)));
                mTexCoords.push_back(glm::vec2(theta/glm::radians(360.0), phi/glm::radians(maxPhi)));
                mTexCoords.push_back(glm::vec2(theta/glm::radians(360.0), phi0/glm::radians(maxPhi)));

                theta0 = theta;
            }
            phi0 = phi;
        }
    }

    void MeshCreator::addCylinder(float height, float radius)
    {
        // Create circular faces
        addCircularFace(height, radius);
        addCircularFace(-height, radius);
    }

    void MeshCreator::addEllipticalSegment(float firstS, float secondS, float a, float b, float theta1, float theta2, float height, float depth)
    {
        // Positions
        glm::vec3 leftTopBack = glm::vec3((a+depth/2)*glm::cos(theta1),  1.0f, (b+depth/2)*glm::sin(theta1));
        glm::vec3 leftTopFront = glm::vec3((a-depth/2)*glm::cos(theta1),  1.0f, (b-depth/2)*glm::sin(theta1));
        glm::vec3 leftBottomBack = glm::vec3((a+depth/2)*glm::cos(theta1),  0.0f, (b+depth/2)*glm::sin(theta1));
        glm::vec3 leftBottomFront = glm::vec3((a-depth/2)*glm::cos(theta1),  0.0f, (b-depth/2)*glm::sin(theta1));
        glm::vec3 rightTopBack = glm::vec3((a+depth/2)*glm::cos(theta2),  1.0f, (b+depth/2)*glm::sin(theta2));
        glm::vec3 rightTopFront = glm::vec3((a-depth/2)*glm::cos(theta2),  1.0f, (b-depth/2)*glm::sin(theta2));
        glm::vec3 rightBottomBack = glm::vec3((a+depth/2)*glm::cos(theta2),  0.0f, (b+depth/2)*glm::sin(theta2));
        glm::vec3 rightBottomFront = glm::vec3((a-depth/2)*glm::cos(theta2),  0.0f, (b-depth/2)*glm::sin(theta2));

        // Front wall, first triangle
        mPositions.push_back(glm::vec3(1,height,1)*leftTopFront);
        mPositions.push_back(glm::vec3(1,height,1)*leftBottomFront);
        mPositions.push_back(glm::vec3(1,height,1)*rightBottomFront);
        mNormals.push_back(glm::vec3(leftTopFront[0], 0, leftTopFront[2]));
        mNormals.push_back(glm::vec3(leftBottomFront[0], 0, leftBottomFront[2]));
        mNormals.push_back(glm::vec3(rightBottomFront[0], 0, rightBottomFront[2]));
        mTexCoords.push_back(glm::vec2(firstS, leftTopFront[1]));
        mTexCoords.push_back(glm::vec2(firstS, leftBottomFront[1]));
        mTexCoords.push_back(glm::vec2(secondS, rightBottomFront[1]));

        // Front wall, second triangle
        mPositions.push_back(glm::vec3(1,height,1)*leftTopFront);
        mPositions.push_back(glm::vec3(1,height,1)*rightBottomFront);
        mPositions.push_back(glm::vec3(1,height,1)*rightTopFront);
        mNormals.push_back(glm::vec3(leftTopFront[0], 0, leftTopFront[2]));
        mNormals.push_back(glm::vec3(rightBottomFront[0], 0, rightBottomFront[2]));
        mNormals.push_back(glm::vec3(rightTopFront[0], 0, rightTopFront[2]));
        mTexCoords.push_back(glm::vec2(firstS, leftTopFront[1]));
        mTexCoords.push_back(glm::vec2(secondS, rightBottomFront[1]));
        mTexCoords.push_back(glm::vec2(secondS, rightTopFront[1]));

        // Back wall, first triangle
        mPositions.push_back(glm::vec3(1,height,1)*leftTopBack);
        mPositions.push_back(glm::vec3(1,height,1)*leftBottomBack);
        mPositions.push_back(glm::vec3(1,height,1)*rightBottomBack);
        mNormals.push_back(glm::vec3(leftTopBack[0], 0, leftTopBack[2]));
        mNormals.push_back(glm::vec3(leftBottomBack[0], 0, leftBottomBack[2]));
        mNormals.push_back(glm::vec3(rightBottomBack[0], 0, rightBottomBack[2]));
        mTexCoords.push_back(glm::vec2(secondS, leftTopBack[1]));
        mTexCoords.push_back(glm::vec2(secondS, leftBottomBack[1]));
        mTexCoords.push_back(glm::vec2(firstS, rightBottomBack[1]));

        // Back wall, second triangle
        mPositions.push_back(glm::vec3(1,height,1)*leftTopBack);
        mPositions.push_back(glm::vec3(1,height,1)*rightBottomBack);
        mPositions.push_back(glm::vec3(1,height,1)*rightTopBack);
        mNormals.push_back(glm::vec3(leftTopBack[0], 0, leftTopBack[2]));
        mNormals.push_back(glm::vec3(rightBottomBack[0], 0, rightBottomBack[2]));
        mNormals.push_back(glm::vec3(rightTopBack[0], 0, rightTopBack[2]));
        mTexCoords.push_back(glm::vec2(secondS, leftTopBack[1]));
        mTexCoords.push_back(glm::vec2(firstS, rightBottomBack[1]));
        mTexCoords.push_back(glm::vec2(firstS, rightTopBack[1]));

        // Top, first triangle
        mPositions.push_back(glm::vec3(1,height,1)*leftTopBack);
        mPositions.push_back(glm::vec3(1,height,1)*leftTopFront);
        mPositions.push_back(glm::vec3(1,height,1)*rightTopFront);
        mNormals.push_back(glm::vec3(leftTopBack[0], 0, leftTopBack[2]));
        mNormals.push_back(glm::vec3(leftTopFront[0], 0, leftTopFront[2]));
        mNormals.push_back(glm::vec3(rightTopFront[0], 0, rightTopFront[2]));
        mTexCoords.push_back(glm::vec2(0, 0));
        mTexCoords.push_back(glm::vec2(0, 0));
        mTexCoords.push_back(glm::vec2(0, 0));

        // Top, second triangle
        mPositions.push_back(glm::vec3(1,height,1)*leftTopBack);
        mPositions.push_back(glm::vec3(1,height,1)*rightTopFront);
        mPositions.push_back(glm::vec3(1,height,1)*rightTopBack);
        mNormals.push_back(glm::vec3(leftTopBack[0], 0, leftTopBack[2]));
        mNormals.push_back(glm::vec3(rightTopFront[0], 0, rightTopFront[2]));
        mNormals.push_back(glm::vec3(rightTopBack[0], 0, rightTopBack[2]));
        mTexCoords.push_back(glm::vec2(0, 0));
        mTexCoords.push_back(glm::vec2(0, 0));
        mTexCoords.push_back(glm::vec2(0, 0));
    }
}