/**

  @file    dragomain.cpp
  @brief   Main program.
  @author  Sofie Lehtikoski, Pekka Lehtikoski
  @version 1.0
  @date    9.5.2016

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "drago.h"

static void drago_main_loop(
    sf::Window *window);


int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;

    sf::Window window(sf::VideoMode(1400, 1000, 32), "OpenGL", sf::Style::Titlebar | sf::Style::Close, settings);
//    sf::Window window(sf::VideoMode(1920, 1000, 32), "OpenGL", sf::Style::Titlebar | sf::Style::Close | sf::Style::Fullscreen, settings);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    drago_main_loop(&window);

    window.close();

    return 0;
}

static void drago_main_loop(
    sf::Window *window)
{
    DragoWorld
        world;

    sf::Clock 
        clock;

    sf::Time 
        elapsed;

    int
        camera_nr = 0;

    eKeyboardCtrl
        kbctrl;

	//eContainer ulle, dulle; 
	// eVariable ulle, dulle;

	// ulle = dulle;

    glEnable(GL_DEPTH_TEST);

    bool running = true;
    while (running)
    {
        ememory_clear(&kbctrl, sizeof(kbctrl));

        sf::Event windowEvent;
        while (window->pollEvent(windowEvent))
        {
            switch (windowEvent.type)
            {
            /* case sf::Event::KeyPressed:
                if (++camera_nr >= DRAGO_MAX_CAMERAS) camera_nr = 0;
                break; */

            case sf::Event::TextEntered:
                switch (windowEvent.text.unicode)
                {
                    case '1': camera_nr = 0; break;
                    case '2': camera_nr = 1; break;
                    case '3': camera_nr = 2; break;
                }

                if (camera_nr >= DRAGO_MAX_CAMERAS) camera_nr = 0;
                break;

            case sf::Event::Closed:
                running = false;
                break;
                //            case sf::Event::KeyPressed:
                //                c = 0.9;
                //                break;
            }
        }

        /* Fill in keyboard control.
         */
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) kbctrl.up = true;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) kbctrl.down = true;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) kbctrl.left = true;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) kbctrl.right = true;

        elapsed = clock.restart();
        world.update_game(&kbctrl, &elapsed);

        world.calculate_world_mtx(camera_nr);

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        world.draw_world(camera_nr);

        // Swap buffers
        window->display();

        sf::sleep(sf::seconds(0.03f));
    }
}


