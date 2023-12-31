#include "context.h"
#include "GLFW/glfw3.h"
#include "image.h"
#include <imgui.h>

#include <ios>
#include <iostream>

#include <vector>

void print4by4(glm::mat4 m)
{
    std::cout.width(10);
    std::cout << std::right << m[0][0];
    std::cout.width(10);
    std::cout << std::right << m[1][0]; 
    std::cout.width(10);
    std::cout << std::right << m[2][0];
    std::cout.width(10);
    std::cout << std::right << m[3][0] << std::endl;

    std::cout.width(10);
    std::cout << std::right << m[0][1];
    std::cout.width(10);
    std::cout << std::right << m[1][1]; 
    std::cout.width(10);
    std::cout << std::right << m[2][1];
    std::cout.width(10);
    std::cout << std::right << m[3][1] << std::endl;

    std::cout.width(10);
    std::cout << std::right << m[0][2];
    std::cout.width(10);
    std::cout << std::right << m[1][2]; 
    std::cout.width(10);
    std::cout << std::right << m[2][2];
    std::cout.width(10);
    std::cout << std::right << m[3][2] << std::endl;

    std::cout.width(10);
    std::cout << std::right << m[0][3];
    std::cout.width(10);
    std::cout << std::right << m[1][3]; 
    std::cout.width(10);
    std::cout << std::right << m[2][3];
    std::cout.width(10);
    std::cout << std::right << m[3][3] << std::endl;
}

ContextUPtr Context::Create() 
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow* window) {
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;
    
    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;
    
    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::MouseMove(double x, double y)
{
    if (!m_cameraControl)
        return ;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.05f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f) m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f) m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_cameraControl = false; 
        }
    }
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

bool Context::Init() 
{
    m_parse = Parse::Load("./resource/42.obj");
    if (!m_parse)
        return false;
    auto materials = m_parse->getMaterials();
    if (materials.size() > 0)
        m_material.attribute = materials[0]; // material을 담아놓긴 하는데, 어떤 방식으로 사용할지가 미정;;
    auto vertices = m_parse->getVBO();
    // auto indices = m_parse->getIndices();

    m_vertexArrayObject = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices.get(), m_parse->getFace().size() * 3 * 8 * sizeof(float));

    m_vertexArrayObject->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    m_vertexArrayObject->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3); // 컬러값이 시작되는 부분이 어디서 부터인가?의 값이 sizeof(float) * 3이다
    // stride는 vertex와 vertex 사이의 간격이다.
    // offset 얼만큼의 offset만큼 띄워서 시작하는 것인지를 뜻한다.
    m_vertexArrayObject->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6);

    // EBO생성
    // m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices.get(), sizeof(uint32_t) * (m_parse->getFace().size() * 3));

    

    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if (!m_program)
        return false;
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // Texture
    m_material.texDiffuse = Texture::CreateFromImage(Image::Load("./images/earth.png").get());
    m_material.texSpecular = Texture::CreateFromImage(Image::Load("./images/container2_specular.png").get());
    if (m_material.texDiffuse == nullptr || m_material.texSpecular == nullptr) {
        return false;
    }

    auto model = glm::rotate(glm::mat4(1.0f),
        glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)
    );
    // auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width/(float)m_height, 0.01f, 10.0f);
    auto transform = projection * view * model;
    m_program->SetUniform("transform", transform);

    return true;
}

void Context::Render()
{
    if (ImGui::Begin("ui window")) {
        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))) {
            glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        }
        ImGui::Separator();
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")) {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.5f, 0.0f, 180.0f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 3000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
        }
        
        if (ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::ColorEdit3("m.ambient", glm::value_ptr(m_material.attribute.ambient));
            ImGui::ColorEdit3("m.diffuse", glm::value_ptr(m_material.attribute.diffuse));
            ImGui::ColorEdit3("m.specular", glm::value_ptr(m_material.attribute.specular));
            ImGui::DragFloat("m.shininess", &m_material.attribute.shininess, 1.0f, 1.0f, 256.0f);
        }
        ImGui::Checkbox("animation", &m_animation);
        ImGui::Checkbox("texture", &m_texture);
    }
    ImGui::End();

    // 색상버퍼 초기화
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_program->Use();
    glm::vec4 tmp = glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    m_cameraFront = glm::vec3(tmp.x, tmp.y, tmp.z);

    m_light.position = m_cameraPos;
    m_light.direction = m_cameraFront;

    auto projection = glm::perspective(glm::radians(45.0f),
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 100.0f);
    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", m_light.position);
    m_program->SetUniform("light.direction", m_light.direction);
    m_program->SetUniform("light.cutoff", glm::vec2(cosf(glm::radians(m_light.cutoff[0])), cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);

    m_program->SetUniform("material.ambient", m_material.attribute.ambient);
    m_program->SetUniform("material.diffuse", m_material.attribute.diffuse);
    m_program->SetUniform("material.TexDiffuse", 0);
    
    m_program->SetUniform("material.specular", m_material.attribute.specular);
    m_program->SetUniform("material.shininess", m_material.attribute.shininess);
    m_program->SetUniform("m_texture", m_texture);

    // Texture
    glActiveTexture(GL_TEXTURE0);// glActiveTexture는 앞으로 내가 건드릴 텍스트 슬롯의 번호를 알려준다.
    m_material.texDiffuse->Bind();// 0번 슬롯에 할당할 텍스처는 2d 텍스처이고 그 텍스처를 할당한다.
    glActiveTexture(GL_TEXTURE1);
    m_material.texSpecular->Bind();

    auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
    model = glm::rotate(model, 
        glm::radians((m_animation ? (float)glfwGetTime() : 0.0f) * 120.0f ),
        glm::vec3(0.0f, 1.0f, 0.0f));
    auto transform = projection * view * model;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", model);
    glDrawArrays(GL_TRIANGLES, 0, m_parse->getFace().size() * 3 * 8);
}