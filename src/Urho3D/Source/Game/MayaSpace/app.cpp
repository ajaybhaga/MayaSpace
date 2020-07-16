/*
 * The definition file for the default application object.
 *
 * Part of the Cyclone physics system.
 *
 * Copyright (c) Icosagon 2003. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */
#include <cstring>
#include <iostream>
#include <vector>
//#include "gl/ogl_headers.h"
#include "app.h"
//#include "cyclone/timing.h"

void Application::initGraphics()
{
/*
    glClearColor(1.0f, 0.95f, 0.8f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setView();
    */
}

void Application::setView()
{
    /*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)width/(double)height, 1.0, 500.0);
//    glMultMatrix( GLKMatrix4MakePerspective(60.0, (double)width/(double)height, 1.0, 500.0).m ); // << .m is the GLfloat* you are accessing
    glMatrixMode(GL_MODELVIEW);
     */
}

void Application::display()
{
    /*

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(18.0f, 0, 0,  0, 0, 0,  0, 1.0f, 0);
    glRotatef(-phi, 0, 0, 1);
    glRotatef(theta, 0, 1, 0);
    glTranslatef(0, -5.0f, 0);

     */
}

const char* Application::getTitle()
{
    return "AgentSim Demo v0.1";
}

void Application::deinit()
{
}

void Application::update() {

    /*
    // Find the duration of the last frame in seconds
    float duration = (float) TimingData::get().lastFrameDuration * 0.001f;
    if (duration <= 0.0f) return;
    else if (duration > 0.05f) duration = 0.05f;
*
    // Update the objects
    updateObjects(duration);

    // Perform the contact generation
    generateContacts();

    // Resolve detected contacts
    resolver.resolveContacts(
            cData.contactArray,
            cData.contactCount,
            duration
    );

    glutPostRedisplay();
     */
}


void Application::key(unsigned char key)
{
    /*
    switch(key)
    {
        case 'R': case 'r':
            // Reset the simulation
            reset();
            return;

        case 'C': case 'c':
            // Toggle rendering of contacts
            renderDebugInfo = !renderDebugInfo;
            return;

        case 'P': case 'p':
            // Toggle running the simulation
            pauseSimulation = !pauseSimulation;
            return;

        case ' ':
            // Advance one frame
            autoPauseSimulation = true;
            pauseSimulation = false;
    }
*/
}


void Application::resize(int width, int height)
{
    // Avoid the divide by zero.
    if (height <= 0) height = 1;

    // Set the internal variables and update the view
    Application::width = width;
    Application::height = height;

    /*
    glViewport(0, 0, width, height);
    setView();
     */
}

void Application::mouse(int button, int state, int x, int y)
{
    // Set the position
    last_x = x;
    last_y = y;
}

void Application::mouseDrag(int x, int y)
{
    // Update the camera
    theta += (x - last_x)*0.25f;
    phi += (y - last_y)*0.25f;

    // Keep it in bounds
    if (phi < -20.0f) phi = -20.0f;
    else if (phi > 80.0f) phi = 80.0f;

    // Remember the position
    last_x = x;
    last_y = y;
}

// The following methods aren't intended to be overloaded
void Application::renderText(float x, float y, const char *text, void *font)
{
    /*
    glDisable(GL_DEPTH_TEST);

    // Temporarily set up the view in orthographic projection.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)width, 0.0, (double)height, -1.0, 1.0);

    // Move to modelview mode.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Ensure we have a font
    if (font == NULL) {
        font = GLUT_BITMAP_HELVETICA_10;
    }

    // Loop through characters displaying them.
    size_t len = strlen(text);

    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(x, y);
    for (const char *letter = text; letter < text+len; letter++) {

        // If we meet a newline, then move down by the line-height
        // TODO: Make the line-height a function of the font
        if (*letter == '\n') {
            y -= 12.0f;
            glRasterPos2f(x, y);
        }
        glutBitmapCharacter(font, *letter);
    }

    // Pop the matrices to return to how we were before.
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
     */
}

void Application::renderPanel(float x, float y, float panelWidth, float panelHeight, const char *text)
{
/*
    void *font = GLUT_BITMAP_8_BY_13;
    glDisable(GL_DEPTH_TEST);

    // Temporarily set up the view in orthographic projection.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)width, 0.0, (double)height, -1.0, 1.0);

    // Move to modelview mode.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float px = panelWidth;
    float py = panelHeight;
    float wx = x;
    float wy = y-py+10.0f;

    glTranslatef(0,0,0); //the position of the element
    glPushMatrix();
    glBegin(GL_QUADS);
    glColor4f(0.1, 0.9, 0.4, 0.6f);
    glVertex3f(wx+px, wy, 0);
    glVertex3f(wx+px, wy+py, 0);
    glVertex3f(wx, wy+py, 0);
    glVertex3f(wx, wy, 0);
    glEnd();
    glPopMatrix();

    // Loop through characters displaying them.
    size_t len = strlen(text);

    float tx, ty;
    tx = x + 5.0f;
    ty = y - 8.0f;
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(tx, ty);
    for (const char *letter = text; letter < text+len; letter++) {

        // If we meet a newline, then move down by the line-height
        // TODO: Make the line-height a function of the font
        if (*letter == '\n') {
            ty -= (12.0f);
            glRasterPos2f(tx, ty);
            continue;
        }
        glutBitmapCharacter(font, *letter);
    }

    // Pop the matrices to return to how we were before.
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    */
}

void Application::renderParameters(float x, float y, std::vector<float> parameters)
{

    if (parameters[0] == NULL) {
        return;
    }
/*
    const char *text = "Weights:";
    void *font = GLUT_BITMAP_8_BY_13;
    glDisable(GL_DEPTH_TEST);

    // Temporarily set up the view in orthographic projection.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)width, 0.0, (double)height, -1.0, 1.0);

    // Move to modelview mode.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float width = 2.0f;
    float height = 20.0f;

    float px = width;
    float py = height;
    float wx = x+2.0f;
    float wy = y-py+10.0f;

    for (int i = 0; i < parameters.size(); i++) {
        glTranslatef(0, 0, 0); //the position of the element
        glPushMatrix();
        glBegin(GL_QUADS);

        glColor4f(parameters[i], 0.0, 1-parameters[i], 0.8f);


        glVertex3f((i*px)+wx + px, wy, 0);
        glVertex3f((i*px)+wx + px, wy + py, 0);
        glVertex3f((i*px)+wx, wy + py, 0);
        glVertex3f((i*px)+wx, wy, 0);
        glEnd();
        glPopMatrix();
    }

    // Ensure we have a font
    if (font == NULL) {
        font = GLUT_BITMAP_HELVETICA_10;
    }

    // Loop through characters displaying them.
    size_t len = strlen(text);

    float tx, ty;
    tx = x + 5.0f;
    ty = y + 14.0f;
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(tx, ty);
    for (const char *letter = text; letter < text+len; letter++) {

        // If we meet a newline, then move down by the line-height
        // TODO: Make the line-height a function of the font
        if (*letter == '\n') {
            ty -= (12.0f);
            glRasterPos2f(tx, ty);
            continue;
        }
        glutBitmapCharacter(font, *letter);
    }

    // Pop the matrices to return to how we were before.
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    */
}

Application::Application() {

}
/*
Application::Application()
:  theta(0.0f),
   phi(15.0f),
   resolver(maxContacts*8),
    renderDebugInfo(false),
    pauseSimulation(true),
    autoPauseSimulation(false)
{
    cData.contactArray = contacts;
}
*/

void Application::drawDebug()
{
    if (!renderDebugInfo) return;

    /*
    // Recalculate the contacts, so they are current (in case we're
    // paused, for example).
    generateContacts();

    // Render the contacts, if required
    glBegin(GL_LINES);
    for (unsigned i = 0; i < cData.contactCount; i++)
    {
        // Interbody contacts are in green, floor contacts are red.
        if (contacts[i].body[1]) {
            glColor3f(0,1,0);
        } else {
            glColor3f(1,0,0);
        }

        cyclone::Vector3 vec = contacts[i].contactPoint;
        glVertex3f(vec.x, vec.y, vec.z);

        vec += contacts[i].contactNormal;
        glVertex3f(vec.x, vec.y, vec.z);
    }

    glEnd();
     */
}
