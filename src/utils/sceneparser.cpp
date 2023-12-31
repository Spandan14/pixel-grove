#include "sceneparser.h"

#include <glm/gtx/transform.hpp>

#include <iostream>


void SceneParser::populateRenderData(RenderData *renderData, SceneNode *root, glm::mat4 currentCTM) { // NOLINT(*-no-recursion)
    glm::mat4 nodeCTM = glm::mat4(1.0f); // identity matrix

    for (int i = root->transformations.size() - 1; i > -1; i--) {
        nodeCTM = SceneParser::getTransformationMatrix(root->transformations[i]) * nodeCTM;
    }

    currentCTM = currentCTM * nodeCTM;

    for (auto & primitive : root->primitives) {
        RenderShapeData shape;
        shape.primitive = *primitive;
        shape.ctm = currentCTM;
        shape.inverseCTM = glm::inverse(currentCTM);

        renderData->shapes.push_back(shape);
    }

    for (auto light : root->lights) {
        SceneLightData lightData{};
        lightData.id = light->id;
        lightData.type = light->type;

        lightData.color = light->color;
        lightData.function = light->function;

        switch (light->type) {
            case LightType::LIGHT_DIRECTIONAL:
                lightData.dir = currentCTM * light->dir;
            case LightType::LIGHT_POINT:
                lightData.pos = currentCTM * glm::vec4{0, 0, 0, 1};
            case LightType::LIGHT_SPOT:
                lightData.dir = currentCTM * light->dir;
                lightData.pos = currentCTM * glm::vec4{0, 0, 0, 1};
                lightData.penumbra = light->penumbra;
                lightData.angle = light->angle;
        }

        renderData->lights.push_back(lightData);
    }

    for (auto & i : root->children) {
        populateRenderData(renderData, i, currentCTM);
    }
}


glm::mat4 SceneParser::getTransformationMatrix(SceneTransformation *transform) {
    switch (transform->type) {
        case TransformationType::TRANSFORMATION_MATRIX:
            return transform->matrix;
        case TransformationType::TRANSFORMATION_ROTATE:
            return transform->rotate == glm::vec3(0, 0, 0) ?
                   glm::identity<glm::mat4>() :
                   glm::rotate(transform->angle, glm::normalize(transform->rotate));
        case TransformationType::TRANSFORMATION_SCALE:
            return glm::scale(transform->scale);
        case TransformationType::TRANSFORMATION_TRANSLATE:
            return glm::translate(transform->translate);
    }
}
