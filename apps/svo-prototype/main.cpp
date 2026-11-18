
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include <iostream>

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antiAliasingLevel = 0;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    sf::Window window(
        sf::VideoMode({1280, 720}),
        "SVO Prototype",
        sf::Style::Default,
        sf::State::Windowed,
        settings
    );

    if (!window.setActive(true))
    {
        std::cerr << "Failed to activate OpenGL context\n";
        return -1;
    }

    // Initial OpenGL state
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1280, 720);

    // Main loop
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* resized =
                event->getIf<sf::Event::Resized>())
            {
                glViewport(
                    0, 0,
                    resized->size.x,
                    resized->size.y
                );
            }
        }

        glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -4.0f);
        glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(35.0f, 0.0f, 1.0f, 0.0f);

        glBegin(GL_QUADS);

        // Front
        glColor3f(1, 0, 0);
        glVertex3f(-1, -1,  1);
        glVertex3f( 1, -1,  1);
        glVertex3f( 1,  1,  1);
        glVertex3f(-1,  1,  1);

        // Back
        glColor3f(0, 1, 0);
        glVertex3f(-1, -1, -1);
        glVertex3f(-1,  1, -1);
        glVertex3f( 1,  1, -1);
        glVertex3f( 1, -1, -1);

        // Left
        glColor3f(0, 0, 1);
        glVertex3f(-1, -1, -1);
        glVertex3f(-1, -1,  1);
        glVertex3f(-1,  1,  1);
        glVertex3f(-1,  1, -1);

        // Right
        glColor3f(1, 1, 0);
        glVertex3f( 1, -1, -1);
        glVertex3f( 1,  1, -1);
        glVertex3f( 1,  1,  1);
        glVertex3f( 1, -1,  1);

        // Top
        glColor3f(1, 0, 1);
        glVertex3f(-1,  1, -1);
        glVertex3f(-1,  1,  1);
        glVertex3f( 1,  1,  1);
        glVertex3f( 1,  1, -1);

        // Bottom
        glColor3f(0, 1, 1);
        glVertex3f(-1, -1, -1);
        glVertex3f( 1, -1, -1);
        glVertex3f( 1, -1,  1);
        glVertex3f(-1, -1,  1);

        glEnd();

        window.display();
    }

    return 0;
}
