#include "mesh.h"
#include <cstring>
#include <iostream>

Mesh::Mesh(const char* filepath)
{
    this->filepath = filepath;
    this->loadMesh();
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
    this->bindMesh();
}

GLuint Mesh::getVAO(){
    return this->m_vao;
}

void Mesh::freeMesh(){
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void Mesh::bindMesh(){
    glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->m_mesh.size(), this->m_mesh.data(), GL_STATIC_DRAW);
    glBindVertexArray(m_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(6*sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::drawMesh(){
    glBindVertexArray(this->m_vao);
    glDrawArrays(GL_TRIANGLES, 0, this->m_mesh.size() / 8);
    glBindVertexArray(0);
}

bool Mesh::loadMesh(){
    std::vector<int> vertices;
    std::vector<int> uvs;
    std::vector<int> normals;
    std::vector<glm::vec3> t_vertices;
    std::vector<glm::vec3> t_normals;
    std::vector<glm::vec2> t_uv;
    FILE * file = fopen(this->filepath, "r");
    if ( file == NULL ) {
        printf("File open error\n");
        return false;
    }
    glm::vec3 vert;

    while(true){
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;



        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            t_vertices.push_back(vertex);
        }
        else if(strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            t_uv.push_back(uv);
        }
        else if(strcmp( lineHeader, "vn") == 0){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            t_normals.push_back(normal);
        }
        else if(strcmp( lineHeader, "f") == 0){
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            vertices.push_back(vertexIndex[0]);
            vertices.push_back(vertexIndex[1]);
            vertices.push_back(vertexIndex[2]);
            uvs.push_back(uvIndex[0]);
            uvs.push_back(uvIndex[1]);
            uvs.push_back(uvIndex[2]);
            normals.push_back(normalIndex[0]);
            normals.push_back(normalIndex[1]);
            normals.push_back(normalIndex[2]);
        }


    }
    for(unsigned int i = 0; i < vertices.size(); ++i){

        glm::vec3 mesh_vertex = t_vertices[vertices[i] - 1];
        m_mesh.push_back(mesh_vertex.x);
        m_mesh.push_back(mesh_vertex.y);
        m_mesh.push_back(mesh_vertex.z);
        glm::vec3 mesh_normal = t_normals[normals[i] - 1];
        m_mesh.push_back(mesh_normal.x);
        m_mesh.push_back(mesh_normal.y);
        m_mesh.push_back(mesh_normal.z);
        glm::vec2 mesh_uv = t_uv[uvs[i] - 1];
        m_mesh.push_back(mesh_uv.x);
        m_mesh.push_back(mesh_uv.y);

    }

    return true;
}
