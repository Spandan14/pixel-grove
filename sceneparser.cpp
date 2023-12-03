#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

void SceneParser::traverseSceneGraph(SceneNode* node, glm::mat4& cumulativeTransform, RenderData& renderData) {

    glm::mat4 cumulatedTransform = cumulativeTransform;
    for (SceneTransformation* transform : node->transformations) {
        if (transform->type == TransformationType::TRANSFORMATION_TRANSLATE){
            cumulatedTransform = cumulatedTransform * glm::translate(transform->translate) ;
        } else if(transform->type == TransformationType::TRANSFORMATION_SCALE){
            cumulatedTransform = cumulatedTransform * glm::scale(transform->scale);
        } else if(transform->type == TransformationType::TRANSFORMATION_ROTATE){
            cumulatedTransform =  cumulatedTransform *  glm::rotate(transform->angle, transform->rotate);
        } else{
            cumulatedTransform = transform->matrix * cumulatedTransform;
        }
    }

    for (ScenePrimitive* primitive : node->primitives) {
        RenderShapeData shapeData;
        shapeData.primitive = *primitive;
        shapeData.ctm = cumulatedTransform;
        renderData.shapes.push_back(shapeData);
    }

    for (SceneLight* light : node->lights) {
        SceneLightData lightData;
        lightData.id = light->id;
        lightData.type = light->type;
        lightData.color = light->color;
        lightData.function = light->function;
        lightData.dir = cumulatedTransform * light->dir;
        lightData.pos = cumulatedTransform * glm::vec4(0.f, 0.f, 0.f, 1.f);
        lightData.penumbra = light->penumbra;
        lightData.angle = light->angle;

        renderData.lights.push_back(lightData);
    }

    for (SceneNode* child : node->children) {
        traverseSceneGraph(child, cumulatedTransform, renderData);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    SceneNode *root = fileReader.getRootNode();
    renderData.shapes.clear();

    glm::mat4 cumulativeTransform(1.0f);
    traverseSceneGraph(root, cumulativeTransform, renderData);

    return true;
}

