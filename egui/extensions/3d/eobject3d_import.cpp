/**

  @file    eobject3d_import.cpp
  @brief   Import 3D model as object.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    11.5.2016

  Use assimp library to import 3D model.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "egui/egui3d.h"


// assimp library include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"
#include <assimp/cimport.h>

#define REVREV
#define REVREV2

static void eobj3d_recursive_convert(
    eObject3D *eobj,
    const struct aiScene *sc, 
    const struct aiNode* nd,
    float scale,
    eContainer *meshes,
    os_int flags);


/**
****************************************************************************************************

  @brief Import 3d model from file.

  The import function uses assimp library to load 3D model from file, and to convert it to
  eObject3D presentation. This function clears any model content associated with eobject,
  and may create child objects.

  @param   path Path to file to import.
  @return  None.

****************************************************************************************************
*/
void eObject3D::import(
    const os_char *path,
    eContainer *meshes,
    os_int flags)
{
    const struct aiScene 
        *scene;

    m_flags |= EOBJ3D_OBJECT_ROOT;

	/* Import the scene.
     */
	scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene == OS_NULL)
    {
	    osal_debug_error("Loading model failed");
        return;
    }

    /* Convert scene to our object/mesh format.
     */
    eobj3d_recursive_convert(this, scene, scene->mRootNode, 0.5, meshes, flags);

    /* Release original scene from memory.
     */    
    aiReleaseImport(scene);
}

/* void eObject3D::import(
    const os_char *path,
    eContainer *meshes)
{
    m_mesh = emesh3d_new(meshes);
} */


/**
****************************************************************************************************

  @brief Convert assimp scene to our object/mesh structure.

  The recursive_convert...

  @param   path Path to file to import.
  @return  None.

****************************************************************************************************
*/
static void eobj3d_recursive_convert(
    eObject3D *eobj,
    const struct aiScene *sc, 
    const struct aiNode* nd,
    float scale,
    eContainer *meshes,
    os_int flags)
{
    eMesh3D
        *emesh,
        *emp;

    eObject3D 
        *new_eobj;

    e3VertexAttrs
        *v;

    os_uint
        *e;

	unsigned int i, n=0, t, nix, n_triangles;
	aiMatrix4x4 m = nd->mTransformation;

	aiMatrix4x4 m2;
	aiMatrix4x4::Scaling(aiVector3D(scale, scale, scale), m2);
	m = m * m2;

	// update transform
	m.Transpose();
	glPushMatrix();
	glMultMatrixf((float*)&m);

	// draw all meshes assigned to this node
	for (; n < nd->mNumMeshes; ++n)
	{
		const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
        if (mesh->mNumFaces <= 0) continue;

		// apply_material(sc->mMaterials[mesh->mMaterialIndex]);


		/* if(mesh->mNormals == NULL)
		{
			glDisable(GL_LIGHTING);
		}
		else
		{
			glEnable(GL_LIGHTING);
		}

		if(mesh->mColors[0] != NULL)
		{
			glEnable(GL_COLOR_MATERIAL);
		}
		else
		{
			glDisable(GL_COLOR_MATERIAL);
		} */

        /* Create new mesh and join to chain of meshes for this object.
            */
        emesh = emesh3d_new(meshes);
        if (eobj->m_first_mesh)
        {
            emp = eobj->m_first_mesh;
            while (emp->m_next_mesh)
                emp = emp->m_next_mesh;
            emp->m_next_mesh = emesh;
        }
        else
        {   
            eobj->m_first_mesh = emesh;
        }

        /* Calculate number of triangels.
         */
        n_triangles = 0;
		for (t = 0; t < mesh->mNumFaces; ++t) 
        {
			const struct aiFace* face = &mesh->mFaces[t];
            switch (face->mNumIndices)
            {
                case 3: n_triangles++; break;
                case 4: n_triangles += 2; break;
                default: break;
            }
        }

        /* Allocate vertex and element buffers.
         */
        emesh->m_vertices_sz = mesh->mNumVertices * sizeof(e3VertexAttrs);
        emesh->m_vertices = (e3VertexAttrs*)os_malloc(emesh->m_vertices_sz, OS_NULL);
        os_memclear(emesh->m_vertices, emesh->m_vertices_sz);

        emesh->m_elements_sz = n_triangles * 3 /* mNumIndices */ * sizeof(os_uint);
        emesh->m_elements = (os_uint*)os_malloc(emesh->m_elements_sz, OS_NULL);
        os_memclear(emesh->m_elements, emesh->m_elements_sz);

        /* Fill in vertices structure.
         */
        for (i = 0, v = emesh->m_vertices; i < mesh->mNumVertices; i++, v++)
        {
            v->x = mesh->mVertices[i].x;

            if (flags & EOBJ3D_FLIP_YZ)
            {
                v->z = mesh->mVertices[i].y;
                v->y = mesh->mVertices[i].z;
            }
            else
            {
                v->y = mesh->mVertices[i].y;
                v->z = mesh->mVertices[i].z;
            }

			if(mesh->mColors[0] != NULL)
            {
				v->r = (os_uchar)(255 * mesh->mColors[0][i].r);
				v->g = (os_uchar)(255 * mesh->mColors[0][i].g);
				v->b = (os_uchar)(255 * mesh->mColors[0][i].b);
				v->a = (os_uchar)(255 * mesh->mColors[0][i].a);
				v->a = 255;
            }
            else
            {
				v->r = 155;
				v->a = 255;
            }

    		if (mesh->mNormals != NULL)
            {
                v->nx = mesh->mNormals[i].x;

                if (flags & EOBJ3D_FLIP_YZ)
                {
                    v->nz = mesh->mNormals[i].y;
                    v->ny = mesh->mNormals[i].z;
                }
                else
                {
                    v->ny = mesh->mNormals[i].y;
                    v->nz = mesh->mNormals[i].z;
                }
            }
        }

        /* Fill in elements
         */
        e = emesh->m_elements;
		for (t = 0; t < mesh->mNumFaces; ++t) 
        {
			const struct aiFace* face = &mesh->mFaces[t];
            nix = face->mNumIndices;
            if (nix != 3 && nix != 4) continue;

			/* GLenum face_mode;
			switch(face->mNumIndices)
			{
				case 1: face_mode = GL_POINTS; break;
				case 2: face_mode = GL_LINES; break;
				case 3: face_mode = GL_TRIANGLES; break;
				default: face_mode = GL_POLYGON; break;
			} */
			for(i = 0; i < 3; i++)
				*(e++) = face->mIndices[i];

            if (nix == 4)
            {
			    for(i = 0; i < 3; i++)
				    *(e++) = face->mIndices[(i+2)%4];
            }
        }
	}

	// Convert all children
	for (n = 0; n < nd->mNumChildren; ++n)
	{
        new_eobj = eObject3D::newobj(eobj);
        if (eobj->flags() & EOBJ3D_MOVING) 
        {
            new_eobj->m_flags |= EOBJ3D_MOVING;
        }
        eobj3d_recursive_convert(new_eobj, sc, nd->mChildren[n], scale, meshes, flags);
	}

	glPopMatrix();
}

