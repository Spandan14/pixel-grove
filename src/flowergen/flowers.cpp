#include "flowers.h"

void Flower::flattenLSystem(LSystemNode *&root, std::vector <FlowerData> &flattened) {
    std::vector<FlowerData> flattened_flower;
    nodeDFS(root, flattened_flower);
    flattened = flattened_flower;
}

void Flower::drawMesh(GLuint m_shader, std::vector<FlowerData> &flowers, glm::vec3 &location) {
//    for (int i = 0; i < flowers.size(); ++i) {
//
//        GLint shininess_Location = glGetUniformLocation(m_shader, "shininess");
//        glUniform1f(shininess_Location, 22.f);
//        GLint ambient_Location = glGetUniformLocation(m_shader, "ambientColor");
//        glUniform4fv(ambient_Location, 1, &flowers[i].mat.cAmbient[0]);
//
//        GLint diffuse_Location = glGetUniformLocation(m_shader, "diffuseColor");
//        glUniform4fv(diffuse_Location, 1, &flowers[i].mat.cDiffuse[0]);
//
//        GLint specular_Location = glGetUniformLocation(m_shader, "specularColor");
//        glUniform4fv(specular_Location, 1, &flowers[i].mat.cSpecular[0]);
//        glm::mat4 position = glm::mat4(
//                1, 0, 0, 0,
//                0, 1, 0, 0,
//                0, 0, 1, 0,
//                location.x, location.y, location.z, 1) * flowers[i].ctm;
//        GLint modelLocation = glGetUniformLocation(m_shader, "modelMatrix");
//        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &position[0][0]);
//
//
//        GLint inverseCTMLocation = glGetUniformLocation(m_shader, "inverseCTM");
//        glUniformMatrix3fv(inverseCTMLocation, 1, GL_FALSE, &flowers[i].ictm[0][0]);
//        glBindVertexArray(flowers[i].vao);
//        glDrawArrays(GL_TRIANGLES, 0, flowers[i].size);
//        glBindVertexArray(0);
//    }
}

void Flower::nodeDFS(LSystemNode *flower_data, std::vector<FlowerData> &to_render, glm::mat4 ctm) {
    ctm *= flower_data->ctm;
    FlowerData parsed;
    parsed.ctm = ctm;
    parsed.ictm = glm::transpose(glm::inverse(glm::mat3(ctm)));
    parsed.size = flower_data->size;
    parsed.mat = flower_data->mat;
    parsed.type = flower_data->type;
    switch (flower_data->type) {
        case (FlowerComponent::FLOWER):
            parsed.triangles = getFlowerVBO();
            break;
        case (FlowerComponent::STEM):
            parsed.triangles = getStemVBO();
            break;
        case (FlowerComponent::LEAF):
            parsed.triangles = getLeafVBO();
            break;
    }
    to_render.push_back(parsed);

    for (unsigned int i = 0; i < flower_data->children.size(); ++i) {
        nodeDFS(flower_data->children[i], to_render, ctm);
    }
}

FlowerCTMCollection Flower::collectCTMs(std::vector<FlowerData> &objects) {
    FlowerCTMCollection collection;

    for (auto & object : objects) {
        switch (object.type) {
            case (FlowerComponent::FLOWER):
                collection.flowerCTMs.push_back(object.ctm);
                collection.flowerInverseCTMS.push_back(object.ictm);
                collection.flowerMats.push_back(object.mat);
                break;
            case (FlowerComponent::STEM):
                collection.stemCTMs.push_back(object.ctm);
                collection.stemInverseCTMS.push_back(object.ictm);
                collection.stemMats.push_back(object.mat);
                break;
            case (FlowerComponent::LEAF):
                collection.leafCTMs.push_back(object.ctm);
                collection.leafInverseCTMS.push_back(object.ictm);
                collection.leafMats.push_back(object.mat);
                break;
        }
    }
    return collection;
}
