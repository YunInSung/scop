#include "context.h"

ContextUPtr Context::Create() 
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init() 
{
    float vertices[] = {
        // 0.5f, 0.5f, 0.0f,
        // 0.5f, -0.5f, 0.0f,
        // -0.5f, 0.5f, 0.0f,

        // 0.5f, -0.5f, 0.0f,
        // -0.5f, -0.5f, 0.0f,
        // -0.5f, 0.5f, 0.0f

        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top right, red
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right, green
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left, blue
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f // top left, yellow
    };

    uint32_t indices[] ={
        0, 1, 3,
        1, 2, 3,
    };

    m_vertexArrayObject = VertexLayout::Create();

    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, 24 * sizeof(float));

    // m_vertexArrayObject->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    m_vertexArrayObject->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0); // 다음 점의 postion은 sizeof(float) * 6 만큼 뒤에 존재함을 뜻한다. 즉 다음 점의 postion의 위치는 현재 점의 첫번째 위치에서 sizeof(float) * 6만큼 뒤에 있다는 의미이다.
    m_vertexArrayObject->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, sizeof(float) * 3); // 컬러값이 시작되는 부분이 어디서 부터인가?의 값이 sizeof(float) * 3이다
    // stride는 vertex와 vertex 사이의 간격이다.
    // offset 얼만큼의 offset만큼 띄워서 시작하는 것인지를 뜻한다.

    // EBO생성
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, 6 * sizeof(uint32_t ));

    ShaderPtr vertShader = Shader::CreateFromFIle("./shader/per_vertex_color.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFIle("./shader/per_vertex_color.fs", GL_FRAGMENT_SHADER);

    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    // auto loc = glGetUniformLocation(m_program->Get(), "color");
    // m_program->Use();
    // glUniform4f(loc, 0.0f, 1.0f, 0.0f, 1.0f);

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // 정점 하나만 만들 때 사용했음,,! // vertex array 를 만들고 바인딩
    // uint32_t vao = 0;
    // glGenVertexArrays(1, &vao);
    // glBindVertexArray(vao);

    return true;

}

void Context::Render()
{
    // 색상버퍼 초기화
    glClear(GL_COLOR_BUFFER_BIT);

    // static float time = 0.0f;
    // float t = sinf(time) * 0.5f + 0.5f;
    // auto loc =  glGetUniformLocation(m_program->Get(), "color");
    m_program->Use();
    // glUniform4f(loc, t*t, 2.0f * t * (1.0f -t), (1.0f -t)*(1.0f -t), 1.0f);
    
    // 그림그리는 코드
    // m_program->Use(); // glUseProgram(m_program->Get());
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // glDrawArrays(GL_LINE_STRIP, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // time += 0.016f;
}