/**

  @file    eterrain.cpp
  @brief   Earth crust or other walk on surface.
  @author  Sofie Lehtikoski, Pekka Lehtikoski
  @version 1.0
  @date    13.5.2016

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "egui/egui3d.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define ETERR_RAND(r) (os_float)(r * ((double)rand() / (double)RAND_MAX - 0.5))

/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
eTerrain3D::eTerrain3D(
	eObject *parent,
	os_int oid,
	os_int flags)
    : eObject3D(parent, oid, flags)
{
    m_world_connection = OS_NULL;
    m_res_x = 1.1;
    m_res_y = 1.1;
    m_w = 129;
    m_h = 129;
    m_elev = 0;
    m_elev_sz = 0;
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eTerrain3D::~eTerrain3D()
{
    os_free(m_elev, m_elev_sz);
}


/**
****************************************************************************************************

  @brief Connect terrain to world.

  Terrains use different more global collision detection that most objects. This is handled 
  globally trough eworld. The connect_to_world function maps this.

  @return  None.

****************************************************************************************************
*/
void eTerrain3D::connect_to_world()
{
    eObject3D
        *world;

    world = this;

    /* Find world. If not found, make sure that we do not have world connection.
     */
    while ((world->flags() & EOBJ3D_WORLD) == 0)
    {
        world = eObject3D::cast(world->parent());
        if (world == OS_NULL)
        {
            delete m_world_connection;
            m_world_connection = OS_NULL;
            return;
        }
    }

    if (m_world_connection == OS_NULL) 
    {
        m_world_connection = new ePointer(this, EOID_ITEM, EOBJ_IS_ATTACHMENT);
//        m_world_connection->setflags(EOBJ_IS_ATTACHMENT);
    }

    m_world_connection->set(eWorld3D::cast(world)->m_terrains);
}


/**
****************************************************************************************************

  @brief Generate elevation map for.

  X...

  @return  None.

****************************************************************************************************
*/
void eTerrain3D::generate_elev_map(
    os_int sz,
    os_double res,
    os_float variation_m,
    os_float dampening,
    os_float adjust_1_m)
{
    int 
        xs,
        ys,
        cx,
        cy,
        x,
        y;

    os_float
        sum,
        val,
        r,
        *ele;

    static os_uint ulle = 0;

    time_t
        tt;

    bool
        isfirst;

    time(&tt);
    srand((unsigned int)tt % 10000 + ulle++);
    srand(rand());
    
    m_w = m_h = sz;
    m_res_x = m_res_y = res;

    /* Release old elevation map, if any.
     */
    os_free(m_elev, m_elev_sz);
    m_elev_sz = m_w * m_h * sizeof(float);

    /* Allocate elevation map.
     */
    m_elev = (float*)os_malloc(m_elev_sz, 0);
    os_memclear(m_elev, m_elev_sz);

    isfirst = adjust_1_m > 0;

    xs = m_w - 1;
    ys = m_h - 1;
    r = variation_m;
    while (xs >= 2 || ys >= 2)
    {
        for (y=0; y<m_h-1; y += ys)
        {
            for (x=0; x<m_w-1; x += xs)
            {
                sum = 
                    m_elev[x + y*m_w]
                  + m_elev[x + xs + y*m_w]
                  + m_elev[x + (y+ys)*m_w]
                  + m_elev[x + xs + (y+ys)*m_w];
               
                ele = &m_elev[x + xs/2 + (y+ys/2)*m_w];
                *ele = sum/4.0F + ETERR_RAND(r);
                //m_elev[x + xs/2 + (y+ys/2)*m_w] =
                //    sum/4.0F + ETERR_RAND(r);

                if (isfirst) 
                {
                    *ele += adjust_1_m;
                    if (*ele < adjust_1_m/2) *ele = adjust_1_m/2;
                }
            }
        }
        for (y=0; y<m_h-1; y += ys)
        {
            cy = y + ys/2;
            for (x=0; x<m_w-1; x += xs)
            {
                cx = x + xs/2;
                val = m_elev[cx + cy*m_w];

                ele = &m_elev[cx + y * m_w];
                *ele = val + ETERR_RAND(r);
                if (isfirst) *ele -= (os_float)(0.25 * adjust_1_m);
                ele = &m_elev[x + cy * m_w];
                *ele = val + ETERR_RAND(r);
                if (isfirst) *ele -= (os_float)(0.25 * adjust_1_m);
                ele = &m_elev[x + xs + cy * m_w];
                *ele = val + ETERR_RAND(r);
                if (isfirst) *ele -= (os_float)(0.25 * adjust_1_m);
                ele = &m_elev[cx + (y+ys)*m_w];
                *ele = val + ETERR_RAND(r);
                if (isfirst) *ele -= (os_float)(0.25 * adjust_1_m);

                /* m_elev[cx + y * m_w] = val + ETERR_RAND(r);
                m_elev[x + cy * m_w] = val + ETERR_RAND(r);
                m_elev[x + xs + cy * m_w] = val + ETERR_RAND(r);
                m_elev[cx + (y+ys)*m_w] = val + ETERR_RAND(r);
                 */
            }
        }
        if (xs >= 2) xs /= 2;
        if (ys >= 2) ys /= 2;
        r *= dampening;
        isfirst = false;
    }
}


/**
****************************************************************************************************

  @brief Make ground under sea sea deeper.

  X...

  @return  None.

****************************************************************************************************
*/
void eTerrain3D::steep_banks()
{
    os_int
        x,
        y;

    float
        depth;

    for (y = 0; y < m_h; y++)
    {
        for (x = 0; x < m_w; x++)
        {
            depth = m_elev[x + y*m_w];
            if (depth >= 0) 
            {
                depth += 0.1F;
            }
            else 
            {
                depth = 12.5F * depth - 0.2F;
            }
            m_elev[x + y*m_w] = depth;
        }
    }
}


/**
****************************************************************************************************

  @brief Make ground under sea sea deeper.

  X...

  @return  None.

****************************************************************************************************
*/
void eTerrain3D::avoid_square_island(
    os_double drop_m,
    os_int limit_dist)
{
    os_int
        x,
        y;


    int 
        dist;

    for (y = 0; y < m_h; y++)
    {
        for (x = 0; x < m_w; x++)
        {
            dist = y;
            if (m_h-y-1 < dist) dist = m_h-y-1;
            if (x < dist) dist = x;
            if (m_w-x-1 < dist) dist = m_w-x-1;

            if (dist < limit_dist)
            {
                m_elev[x + y*m_w] -= (os_float)(drop_m * (limit_dist - dist) / (os_double)limit_dist);
            }
        }
    }
}



/**
****************************************************************************************************

  @brief Make ground under sea sea deeper.

  X...

  @return  None.

****************************************************************************************************
*/
void eTerrain3D::above_sea_level(
    os_double above_precents,
    os_double step_m)
{
    os_int
        x,
        y,
        ix,
        total_n,
        limit_n;

    os_double
        level;

    #define AS_HISTO_N 10000
    
    os_int 
        histo[AS_HISTO_N],
        sum;

    os_memclear(histo, sizeof(histo));
    for (y = 0; y < m_h; y++)
    {
        for (x = 0; x < m_w; x++)
        {
            level = m_elev[x + y*m_w];
            ix = (os_int)(level/step_m + AS_HISTO_N/2);
            if (ix < 0) ix = 0;
            if (ix >= AS_HISTO_N) ix = AS_HISTO_N-1;
            histo[ix]++;
        }
    }

    total_n = m_w * m_h;
    limit_n = (os_int)(0.01 * (100.0 - above_precents) * total_n);
    sum = 0;
    level = 0;
    for (ix = 0; ix<AS_HISTO_N; ix++)
    {
        sum += histo[ix];
        if (sum >= limit_n)
        {
            level = (ix - AS_HISTO_N/2) * step_m;
            break;
        }
    }

    for (y = 0; y < m_h; y++)
    {
        for (x = 0; x < m_w; x++)
            m_elev[x + y*m_w] -= (os_float)level;
    }
}

/**
****************************************************************************************************

  @brief Blue elevation map.

  X...

  @return  None.

****************************************************************************************************
*/
void eTerrain3D::blur(
    os_int blur_r)
{
    os_int
        x,
        y,
        fx,
        fy,
        lx,
        ly,
        xx,
        yy;

    os_double
        sum,
        wsum;

    float
        *elev,
        w;

    elev = (float*)os_malloc(m_elev_sz, 0);

    for (y = 0; y < m_h; y++)
    {
        fy = y > blur_r ? y - blur_r : 0;
        ly = (y < m_h - blur_r) ? y + blur_r : m_h - 1;

        for (x = 0; x < m_w; x++)
        {
            fx = x > blur_r ? x - blur_r : 0;
            lx = (x < m_w - blur_r) ? x + blur_r : m_w - 1;

            sum = 0;
            wsum = 0;
            for (yy = fy; yy<=ly; yy++)
            {
                for (xx = fx; xx<=lx; xx++)
                {
                    w = (os_float)(1.0/(1.0/blur_r + abs(x-xx) + abs(y-yy)));
                    sum += w * m_elev[xx + yy * m_w];
                    wsum += w;
                }
            }
            elev[x + y * m_w] = (float)(sum / wsum);
        }
    }

    os_free(m_elev, m_elev_sz);
    m_elev = elev;
}


/**
****************************************************************************************************

  @brief Generate mesh for drawing.

  X...

  @return  None.

****************************************************************************************************
*/
void eTerrain3D::generate_mesh(
    eContainer *meshes,
    os_double red,
    os_double green,
    os_double blue)
{
    eMesh3D
        *mesh;

    os_int
        x,
        y,
        fx,
        lx,
        fy,
        ly;

    os_uint
        *e,
        i;

    glm::dvec3 
        X,
        Y,
        P;

    os_double
        norm;

    e3VertexAttrs
        *v;

    /* If no elevation map, we can do nothing.
     */
    if (m_elev == 0) return;

    /* Generate mesh.
     */
    mesh = emesh3d_new(meshes);
    m_first_mesh = mesh;

    /* Allocate vertex and element buffers.
     */
    mesh->m_vertices_sz = m_w * m_h * sizeof(e3VertexAttrs);
    mesh->m_vertices = (e3VertexAttrs*)os_malloc(mesh->m_vertices_sz, OS_NULL);
    os_memclear(mesh->m_vertices, mesh->m_vertices_sz);

    mesh->m_elements_sz = 2 /* squares to triangles */ * (m_w-1) * (m_h-1) * 3 /* mNumIndices */ * sizeof(os_uint);
    mesh->m_elements = (os_uint*)os_malloc(mesh->m_elements_sz, OS_NULL);
    os_memclear(mesh->m_elements, mesh->m_elements_sz);

    v = mesh->m_vertices;
    for (y = 0; y<m_h; y++)
    {
        fy = y > 0 ? y-1 : 0;
        ly = y < m_h-1 ? y + 1 : m_h - 1;
        for (x = 0; x<m_w; x++)
        {
            fx = x > 0 ? x-1 : 0;
            lx = x < m_w-1 ? x + 1 : m_w - 1;

            v->x = (os_float)((x - (m_w-1)/2) * m_res_x);
            v->z = (os_float)((y - (m_h-1)/2) * m_res_y);
            v->y = m_elev[y * m_w + x];
//  + 0.5*sin(2.5 * v->x) + 0.7*cos(1.5 * v->z);
            v->r = (os_uchar)(red * 255.0);
            v->g = (os_uchar)(green * 255.0);
            v->b = (os_uchar)(blue * 255.0);
            v->a = 255;
        
            X.x =  (lx - fx) * m_res_x;
            X.y =  m_elev[y * m_w + lx] - m_elev[y * m_w + fx];
            X.z = 0;
            Y.x =  0; 
            Y.y =  m_elev[ly * m_w + x] - m_elev[fy * m_w + x];
            Y.z = (ly - fy) * m_res_y;
            // P = glm::cross(X, Y);
            P = glm::cross(Y, X);
            norm = P.x * P.x + P.y * P.y + P.z * P.z;
            if (norm < 1.0e-5) norm = 1.0e-5;
            norm = sqrt(norm);
            v->nx = (os_float)(P.x/norm);
            v->ny = (os_float)(P.y/norm);
            v->nz = (os_float)(P.z/norm);
            v++;
        }
    }

    e = mesh->m_elements;
    for (y = 0; y<m_h-1; y++)
    {
        for (x = 0; x<m_w-1; x++)
        {
            i = x + y * m_w;
            *(e++) = i;
            *(e++) = i + 1;
            *(e++) = i + m_w;
            *(e++) = i + m_w;
            *(e++) = i + 1;
            *(e++) = i + m_w + 1;
        }
    }
}


/**
****************************************************************************************************

  @brief Get elevation at specific point of terrain.

  X...
  @param  x Coordinate in terrain mesh coordinates.
  @param  y Coordinate in terrain mesh coordinates.

  @return Elevation, m. -10000.0 m if not within terrain (10 km undergound).

****************************************************************************************************
*/
os_double eTerrain3D::get_elevation_at(
    os_double x, 
    os_double y)
{
    os_int
        ix,
        iy;

    os_double
        ox,
        oy,
        e1,
        e2;

    os_float
        *e;

    /* Adjust by center point of the terrain.
     */
    x += ((m_w-1)/2) * m_res_x;
    y += ((m_h-1)/2) * m_res_y;

    /* Calculate element coordinates.
     */
    ix = (os_int)(x / m_res_x);
    iy = (os_int)(y / m_res_y);
    if (ix < 0 || iy < 0 || ix >= m_w-1 || iy >= m_h-1) return -10000.0;

    ox = (x - ix * m_res_x) / m_res_x;
    oy = (y - iy * m_res_y) / m_res_y;

    e = m_elev + ix + iy * m_w;
    e1 = e[0]*(1.0-ox) + e[1]*ox;
    e2 = e[m_w]*(1.0-ox) + e[m_w + 1]*ox;
    
    return e1*(1.0-oy) + e2*oy;
}


/**
****************************************************************************************************

  @brief Check if object collides with terrain.

  X...

  @return  true if collides.

****************************************************************************************************
*/
bool eTerrain3D::collides_with_terrain(
    eObject3D *o /* ,
    glm::dvec3 *collision_point */)
{
    if (o->m_bb_2_world.y < get_elevation_at(o->m_bb_2_world.x, o->m_bb_2_world.z)) return true;

    return o->m_bb_1_world.y < get_elevation_at(o->m_bb_1_world.x, o->m_bb_1_world.z);
}
    
