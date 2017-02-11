/**

  @file    dragoworld.cpp
  @brief   World and all it contains.
  @author  Sofie Lehtikoski, Pekka Lehtikoski
  @version 1.0
  @date    9.5.2016

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "../drago.h"


/* // Shader sources
static const GLchar* vertexSource =
"#version 150 core\n"
"in vec3 position;"
"in vec4 color;"
"in vec3 normal;"
"out vec4 Color;"
"out vec3 Normal;"
"uniform mat4 MVP;"
"void main()"
"{"
"    Color = color;"
"    Normal = normal;"
"    gl_Position = MVP * vec4(position, 1.0);"
"}";
static const GLchar* fragmentSource =
"#version 150 core\n"
"in vec4 Color;"
"in vec3 Normal;"
"out vec4 outColor;"
"void main()"
"{"
"    outColor = vec4(Normal, 1.0);"
//"    outColor = vec4(Color, 1.0);"
//"    outColor = Color;"
"}"; */

// Shader sources
static const GLchar* vertexSource =
"#version 330 core\n"
"in vec3 position;"
"in vec4 color;"
"in vec3 normal;"
"out vec4 Color;"
"out vec3 FragPos;"
"out vec3 Normal;"
"uniform mat4 M;"
"uniform mat4 N;"
"uniform mat4 MVP;"
"void main()"
"{"
"    Color = color;"
"    Normal = vec3(N * vec4(normal, 1.0f));"
"    FragPos = vec3(M * vec4(position, 1.0f));"
"    gl_Position = MVP * vec4(position, 1.0);"
"}";

static const GLchar* fragmentSource =
"#version 330 core\n"
"in vec4 Color;"
"in vec3 FragPos;"
"in vec3 Normal;"
"out vec4 outColor;"
"uniform vec3 lightPos;"
"uniform vec3 lightColor;"
"void main()"
"{"
   "vec3 norm = normalize(Normal);"
   "vec3 lightDir = normalize(lightPos - FragPos);"
   "float diff = max(dot(norm, lightDir), 0.0);"
   "vec3 diffuse = diff * lightColor;"
   "float ambientStrength = 0.1f;"
   "vec3 ambient = ambientStrength * lightColor;"
   "outColor = vec4((ambient + diffuse), 1.0) * Color;"
"}";


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
DragoWorld::DragoWorld(
	eObject *parent,
	os_int oid)
    : eObject(parent, oid)
{
    // Create GL buffer objects.
    glGenBuffers(E3GL_NRO_BUFFERS, m_glbuf);

    m_world = eWorld3D::newobj(OS_NULL, EOID_ROOT);

    m_mountains = eTerrain3D::newobj(m_world);
    m_mountains->connect_to_world();
    m_mountains->generate_elev_map(257, 10.0, 1380.0F, 0.45F, 400.0F);
    m_mountains->avoid_square_island(400.0, 35);
    m_mountains->above_sea_level(30, 1.0);
//    m_mountains->steep_banks();
//    m_mountains->blur(1);
    m_mountains->generate_mesh(&m_meshes, 0.2, 0.12, 0.12);

    m_plains = eTerrain3D::newobj(m_world);
    m_plains->connect_to_world();
    m_plains->generate_elev_map(257, 10.0, 40.0F, 0.55F, 50.0F);
    m_plains->avoid_square_island(10.0, 30);
    m_plains->above_sea_level(60, 0.05);
    m_plains->steep_banks();
    m_plains->blur(1);
    m_plains->generate_mesh(&m_meshes, 0.3, 0.25, 0.15);

    // Sea has to ve very flat.
    m_sea = eTerrain3D::newobj(m_world);
//    m_sea->connect_to_world();
    m_sea->generate_elev_map(257, 200.0, 0.05F, 0.95F, 0.0F);
    m_sea->generate_mesh(&m_meshes, 0.1, 0.5, 1.0);


    m_camera[0] = new DragoCamera(m_world);

    m_box = eMovingObject3D::newobj(m_world);
    m_box2 = eMovingObject3D::newobj(m_world);
    m_box3 = eMovingObject3D::newobj(m_world);

    m_camera[1] = new DragoCamera(m_box);
    m_camera[1]->m_position.y = 13.7;
    m_camera[1]->m_position.z = -1;
    m_camera[2] = new DragoCamera(m_box2);
    m_camera[2]->m_position.y = 70;
//    m_camera[2]->m_rotate.x = -1.15;

    m_box->import("C:/coderoot/tools/assimp-3.2/test/models/BLEND/HUMAN2.blend", &m_meshes, EOBJ3D_BLENDER_DEFAULT_ORIENTATION);
    m_box2->import("C:/coderoot/tools/assimp-3.2/test/models-nonbsd/X/dwarf.x", &m_meshes, EOBJ3D_DEFAULT);
    m_box3->import("C:/coderoot/drago/models/CrocodilosaurusRex.blend", &m_meshes, EOBJ3D_BLENDER_DEFAULT_ORIENTATION);
    // m_box3->import("C:/coderoot/drago/models/BlenderFun.blend", &m_meshes, EOBJ3D_BLENDER_DEFAULT_ORIENTATION);
    m_box3->m_position.x = 105;
    m_box3->m_position.y = 309.1;
    m_box2->m_position.z = 300.0;
    // m_box3->m_rotate.x = -3.14/2;
    m_box3->m_velocity.x = 0.001;

//    m_box3->m_angular_velocity.y = 0.1;
  //  m_box3->m_angular_velocity.x = 0.005;
    // m_box3->m_scale = 45.3;
     m_box3->m_scale = 4.9;

    upload_mesh_data();


m_box2->m_position.x = 0.2;
m_box2->m_position.y = 300.1;
m_box2->m_position.z = 202.0;
//m_box2->m_angular_velocity.y = 1.1;
// m_box2->m_growth_rate = -0.1;
//m_box2->m_scale = 5.3;
 m_box2->m_scale = 0.5;

// m_box->m_rotate.z = 0.1;
m_box->m_position.y = 323.5;
m_box->m_position.x = -100.1;
m_box->m_position.z = 252.0;
m_box->m_velocity.z = -0.001;
m_box->m_velocity.y = 0.5;
//m_box->m_angular_velocity.z = 0.3;
//m_box->m_angular_velocity.x = 0.99;
//m_box->m_scale = 45.3;
 m_box->m_scale = 5.5;

m_camera[0]->m_position.z = 1553;
m_camera[0]->m_position.y = 531.7;
m_camera[0]->m_rotate.x = -0.3;

    // Create and compile the vertex shader
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertexShader, 1, &vertexSource, NULL);
    glCompileShader(m_vertexShader);

    // Create and compile the fragment shader
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(m_fragmentShader);

    // Link the vertex and fragment shader into a shader program
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);
    glBindFragDataLocation(m_shaderProgram, 0, "outColor");
    glLinkProgram(m_shaderProgram);
    glUseProgram(m_shaderProgram);

    // Specify the layout of the vertex data
    m_posAttrib = glGetAttribLocation(m_shaderProgram, "position");
    glEnableVertexAttribArray(m_posAttrib);
    glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(e3VertexAttrs), 0);

    m_colAttrib = glGetAttribLocation(m_shaderProgram, "color");
    glEnableVertexAttribArray(m_colAttrib);
    glVertexAttribPointer(m_colAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(e3VertexAttrs), (void*)(3 * sizeof(GLfloat)));

    m_mormalAttrib = glGetAttribLocation(m_shaderProgram, "normal");
    glEnableVertexAttribArray(m_mormalAttrib);
    glVertexAttribPointer(m_mormalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(e3VertexAttrs), (void*)(4 * sizeof(GLfloat))); 

    // Get a handle for our "MVP" uniform
    m_MVP_uniform = glGetUniformLocation(m_shaderProgram, "MVP");

    // Get a handle for our "M" uniform
    m_M_uniform = glGetUniformLocation(m_shaderProgram, "M");

    // Get a handle for our "N" uniform
    m_N_uniform = glGetUniformLocation(m_shaderProgram, "N");

    // Get a handle for our light uniform
    m_LightPos1_uniform = glGetUniformLocation(m_shaderProgram, "lightPos");
    m_LightColor1_uniform = glGetUniformLocation(m_shaderProgram, "lightColor");

    // Enables Smooth Shading
	/* These do nothing? glShadeModel(GL_SMOOTH);		 
    glEnable (GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); */

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);		 // Enables Smooth Shading
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

//	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
//	glClearDepth(1.0f);				// Depth Buffer Setup
//	glEnable(GL_DEPTH_TEST);		// Enables Depth Testing
//	glDepthFunc(GL_LEQUAL);			// The Type Of Depth Test To Do
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculation

//	glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);    // Uses default lighting parameters
	// glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_NORMALIZE);

	//glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	//glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	//glEnable(GL_LIGHT1);
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
DragoWorld::~DragoWorld()
{
    glDeleteProgram(m_shaderProgram);
    glDeleteShader(m_fragmentShader);
    glDeleteShader(m_vertexShader);

    // Delete GL buffer objects.
    glDeleteBuffers(E3GL_NRO_BUFFERS, m_glbuf);
}


/**
****************************************************************************************************

  @brief Upload all mesh data to graphics card.

  X...

  @return  None.

****************************************************************************************************
*/
void DragoWorld::upload_mesh_data()
{
    int
        bufsz[E3GL_NRO_BUFFERS],
        i;

    eMesh3D
        *mesh;

    // Calculate buffer sizes.
    os_memclear(bufsz, E3GL_NRO_BUFFERS * sizeof(os_int));
    for (mesh = eMesh3D::cast(m_meshes.first()); mesh; mesh=eMesh3D::cast(mesh->next()))
    {
        mesh->m_gl_vertex_buf_nr = E3GL_STATIC_VERTEX_BUF;
        mesh->m_vertex_offset = bufsz[mesh->m_gl_vertex_buf_nr];
        bufsz[mesh->m_gl_vertex_buf_nr] += mesh->m_vertices_sz;

        mesh->m_gl_element_buf_nr = E3GL_STATIC_ELEMENT_BUF;
        mesh->m_element_offset = bufsz[mesh->m_gl_element_buf_nr];
        bufsz[mesh->m_gl_element_buf_nr] += mesh->m_elements_sz;
    }

    // Allocate buffers.
    for (i = 0; i<E3GL_NRO_BUFFERS; i++)
    {
        if (bufsz[i] == 0) continue;

        switch(i)
        {
            case E3GL_STATIC_VERTEX_BUF:
            case E3GL_DYNAMIC_VERTEX_BUF:
                glBindBuffer(GL_ARRAY_BUFFER, m_glbuf[i]);
                glBufferData(GL_ARRAY_BUFFER, bufsz[i], 0, 
                    i == E3GL_STATIC_VERTEX_BUF ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
                break;

            case E3GL_STATIC_ELEMENT_BUF:
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glbuf[i]);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufsz[i], 0, GL_STATIC_DRAW);
                break;
        }
    }

    // Upload data to buffers.
    for (mesh = eMesh3D::cast(m_meshes.first()); mesh; mesh=eMesh3D::cast(mesh->next()))
    {
        if (mesh->m_vertices_sz) 
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_glbuf[mesh->m_gl_vertex_buf_nr]);
            glBufferSubData(GL_ARRAY_BUFFER, mesh->m_vertex_offset,  
                mesh->m_vertices_sz,  mesh->m_vertices);
        }

        if (mesh->m_elements_sz)
        {
            int n = mesh->m_elements_sz/sizeof(os_uint);
            for (i = 0; i < n; i++)
            {
                mesh->m_elements[i] += mesh->m_vertex_offset / sizeof(e3VertexAttrs);
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glbuf[mesh->m_gl_element_buf_nr]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,  mesh->m_element_offset,  
                mesh->m_elements_sz,  mesh->m_elements);
        }
    }
}

void DragoWorld::update_game(
    eKeyboardCtrl *kbctrl,
    os_long elapsed)
{
    double
        elapsed_sec;
    
    // Do not get out of whack if game is paused. Limit maximum loop time to 0.3 seconds.
    elapsed_sec = 0.001 * elapsed;
    if (elapsed_sec > 0.3) elapsed_sec = 0.3;
    m_world->update_game(kbctrl, elapsed_sec);
}

void DragoWorld::calculate_world_mtx(
    os_int camera_nr)
{
    m_world->calculate_world_mtx(m_camera[camera_nr]);
}

void DragoWorld::draw_world(
    os_int camera_nr)
{
//    glm::mat4 lp;
//    glUniformMatrix4fv(m_LightPos1_uniform, 1, GL_FALSE, &lp[0][0]);

    GLfloat LightPosition[]= { 3000.0f, 5000.0f, 0.0f, 1.0f };
    glUniform3fv(m_LightPos1_uniform, 1, LightPosition);

    GLfloat LightColor[]= { 0.8f, 0.8f, 0.8f, 1.0f };
    glUniform3fv(m_LightColor1_uniform, 1, LightColor);


    //GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
    //glUniform3fv(m_LightAmbient1_uniform, 1, LightAmbient);

    //GLfloat LightDiffuse[]= { 0.6f, 1.0f, 0.1f, 1.0f };
    //glUniform3fv(m_LightDiffuse1_uniform, 1, LightDiffuse);


    m_camera[camera_nr]->draw_world(this);
}
