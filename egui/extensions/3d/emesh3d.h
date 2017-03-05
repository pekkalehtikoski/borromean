/**

  @file    emesh3d.h
  @brief   3D mesh base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    12.5.2016

  Mesh base class. Mesh consists of vertices and triangles? texture?

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EMESH3D_INCLUDED
#define EMESH3D_INCLUDED

/** OpenGL buffer object enumeration. Index to World's m_glbuf array.
 */
typedef enum
{
    E3GL_STATIC_VERTEX_BUF = 0,
    E3GL_DYNAMIC_VERTEX_BUF = 1,
    E3GL_STATIC_ELEMENT_BUF = 2,

    E3GL_NRO_BUFFERS
}
e3GlBufferEnum;

/** Vertex attribute format
 */
typedef struct
{
    os_float x, y, z;
    os_uchar r, g, b, a;

// Change to GL_INT_2_10_10_10_REV
    os_float nx, ny, nz;
} 
e3VertexAttrs;


/**
****************************************************************************************************

  @brief 3D mesh base class.

  The eObject3D is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eMesh3D : public eObject
{
	/** 
	************************************************************************************************

	  @name Constructors and destructor

	  X...

	************************************************************************************************
	*/
	/*@{*/

public:
        /* Constructor.
		 */
		eMesh3D(
			eObject *parent,
			os_int oid = EOID_ITEM,
            os_int flags = EOBJ_DEFAULT);

		/* Virtual destructor.
		 */
		virtual ~eMesh3D();

        /* Casting eObject pointer to eMesh3D pointer.
         */
		inline static eMesh3D *cast(
			eObject *o) 
		{ 
            e_assert_type(o, EGUICLASSID_MESH3D)
            return (eMesh3D*)o;
		}

		/* Get class identifier.
         */
		virtual os_int getclassid() 
        { 
            return EGUICLASSID_MESH3D; 
        }

        /* Static function to add class to propertysets and class list.
         */
        static void setupclass();

		/* Static constructor function for generating instance by class list.
         */
		static eMesh3D *newobj(
			eObject *parent,
			os_int oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT)
		{
			return new eMesh3D(parent, oid, flags);
		}

        void calculate_bounding_box_mesh(
            glm::fvec4 *bb_1,
            glm::fvec4 *bb_2,
            bool *initialized);

    /*@}*/

	/** 
	************************************************************************************************

	  @name Mesh data

	  Vertices, triangles?

	************************************************************************************************
	*/
	/*@{*/
        /* Mesh vertices.
         */
        e3VertexAttrs *m_vertices;

        /* Size of vertices buffer in bytes.
         */
        os_int m_vertices_sz;

        /* Vertex offset, set by upload_mesh_data().
         */
        os_int m_vertex_offset;

        /* Vertex buffer bumber, set by upload_mesh_data().
         */
        e3GlBufferEnum m_gl_vertex_buf_nr;

        /* Mesh elements.
         */
        os_uint *m_elements;

        /* Size of elements buffer in bytes.
         */
        os_int m_elements_sz;

        /* Elements offset, set by upload_mesh_data().
         */
        os_int m_element_offset;

        /* Element buffer bumber, set by upload_mesh_data().
         */
        e3GlBufferEnum m_gl_element_buf_nr;

        /** Pointer to the next mesh for this object.
         */
        eMesh3D *m_next_mesh;
    /*@}*/
};

/* Allocate new empty mesh from the mesh container.
 */
eMesh3D *emesh3d_new(
    eContainer *meshes);
#endif
