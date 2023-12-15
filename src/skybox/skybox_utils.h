float skyboxVertices[] =
    {
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
    {
        // Right
        6, 2, 1,
        1, 5, 6,
        // Left
        7, 4, 0,
        0, 3, 7,
        // Top
        6, 5, 4,
        4, 7, 6,
        // Bottom
        2, 3, 0,
        0, 1, 2,
        // Back
        5, 1, 0,
        0, 4, 5,
        // Front
        6, 7, 3,
        3, 2, 6
};


int lightTypeToNum(LightType light_type){
    int return_num;
    switch(light_type){
        case LightType::LIGHT_DIRECTIONAL:
            return_num = 0;
            break;
        case LightType::LIGHT_POINT:
            return_num = 1;
            break;
        case LightType::LIGHT_SPOT:
            return_num = 2;
            break;
    }
    return return_num;
}