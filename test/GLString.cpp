/*!
 * \file test/GLString.cpp
 * \brief Open GL rendering font/string Unit Test
 *
 * \author Mongoose
 */

#include <math.h>
#include <SDL/SDL.h>
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <Texture.h>
#include <GLString.h>

GLString *TEXT;
SDL_Surface *SDL_WINDOW = NULL;
Texture gTexture;

void swap_buffers();

void event_resize(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    GLfloat aspect = (GLfloat)width/(GLfloat)height;

    // Mongoose 2002.01.01, Setup view volume, with a nice FOV
    // xythobuz:
    // gluPerspective is deprecated!
    // gluPerspective(40.0, aspect, 1, 2000);
    // fix: http://stackoverflow.com/a/2417756
    GLfloat fH = tanf(40.0f / 360.0f * 3.14159f);
    GLfloat fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, 1, 2000);

    glMatrixMode(GL_MODELVIEW);
}

void event_display(int width, int height) {
    static float x = 0.0f, y = 0.0f, z = -150.0f, r = 0.0f;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -20.0f);
    glRotatef((float)cosf(r)*180.0f, 0.0f, 0.0f, 1.0f);
    r += 0.01f;

    // Mongoose 2002.01.01, Render color quad
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(x + 50, y + 50, z);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(x - 50, y + 50, z);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(x + 50, y - 50, z);
    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(x - 50, y - 50, z);
    glEnd();

    // Mongoose 2002.01.01, Render text
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glColor3f(0.75, 0.5, 1.0);

    glEnterMode2d(width, height);
    TEXT->Render();
    glExitMode2d();

    glFlush();
    swap_buffers();
}

void swap_buffers() {
    SDL_GL_SwapBuffers();
}


void shutdown_gl() {
    SDL_Quit();
}

void init_gl(unsigned int width, unsigned int height) {
    int i;
    const char *errorText = "TEXT->glPrintf> ERROR code %i\n";

    // Setup GL
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    event_resize(width, height);

    // Mongoose 2002.01.01, Texture setup
    gTexture.reset();
    gTexture.setFlag(Texture::fUseMipmaps);
    gTexture.setMaxTextureCount(32);

    gTexture.loadFontTTF("data/test.ttf", 32, 126 - 32);  // ASCII

    TEXT->Init(4);
    i = TEXT->glPrintf((width/2)-50, height/2-32, "OpenRaider");
    if (i) {
        printf(errorText, i);
    }
    i = TEXT->glPrintf((width/2)-50, height/2, "GLString");
    if (i) {
        printf(errorText, i);
    }
    TEXT->Scale(1.2f);
    i = TEXT->glPrintf((width/2)-100, height/2+32, "Unit Test by Mongoose");
    if (i) {
        printf(errorText, i);
    }
    i = TEXT->glPrintf((width/2)-100, height/2+64, "ported to TTF by xythobuz");
    if (i) {
        printf(errorText, i);
    }
    TEXT->setActive(0, true);
    TEXT->setActive(1, true);
    TEXT->setActive(2, true);
    TEXT->setActive(3, true);
}

[[noreturn]] void main_gl() {
    SDL_Event event;
    unsigned int mkeys, mod, key;
    int flags;
    unsigned int width = 640;
    unsigned int height = 480;
    bool fullscreen = false;
#ifndef __APPLE__
    char *driver = NULL;
#endif

    // Setup clean up on exit
    atexit(shutdown_gl);

    // Create GL context
    SDL_Init(SDL_INIT_VIDEO);

#ifndef __APPLE__
    if (!driver || !driver[0] || SDL_GL_LoadLibrary(driver) < 0) {
        SDL_ClearError();

        // Fallback 1
        if (SDL_GL_LoadLibrary("libGL.so") < 0) {
            SDL_ClearError();

            // Fallback 2
            if (SDL_GL_LoadLibrary("libGL.so.1") < 0) {
                fprintf(stderr, "main_gl> SDL_GL_LoadLibrary failed!\n");
                fprintf(stderr, "main_gl> Error is [%s].\n", SDL_GetError());
                exit(1);
            }
        }
    }
#endif

    flags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER;

    if (fullscreen) {
        flags |= SDL_FULLSCREEN;
        SDL_ShowCursor(SDL_DISABLE);
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_WINDOW = SDL_SetVideoMode(width, height, 16, flags);
    SDL_WM_SetCaption("GLString Test", "GLString Test");
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    // Init rendering
    init_gl(width, height);

    for (;;) {
        // Pause for 10-20 ms
        SDL_Delay(10);

        event_display(width, height);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    exit(0);
                case SDL_MOUSEMOTION:
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    break;
                case SDL_KEYDOWN:
                    mkeys = (unsigned int)SDL_GetModState();
                    mod = 0;

                    if (mkeys & KMOD_LSHIFT)
                        mod |= KMOD_LSHIFT;

                    if (mkeys & KMOD_RSHIFT)
                        mod |= KMOD_RSHIFT;

                    if (mkeys & KMOD_LCTRL)
                        mod |= KMOD_LCTRL;

                    if (mkeys & KMOD_RCTRL)
                        mod |= KMOD_RCTRL;

                    if (mkeys & KMOD_LALT)
                        mod |= KMOD_LALT;

                    if (mkeys & KMOD_RALT)
                        mod |= KMOD_RALT;

                    if (mkeys & KMOD_LMETA)
                        mod |= KMOD_LMETA;

                    if (mkeys & KMOD_RMETA)
                        mod |= KMOD_RMETA;

                    key = event.key.keysym.sym;

                    switch (key)
                    {
                        case 0x1B: // 27d, ESC
                            exit(0);
#ifdef __APPLE__
                        case 113: // q
                            if ((mod & KMOD_RMETA) || (mod & KMOD_LMETA))
                                exit(0);
                            break;
#endif
                        case 114: // r
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    break;
                case SDL_VIDEORESIZE:
                    width = event.resize.w;
                    height = event.resize.h;
                    event_resize(width, height);
                    event_display(width, height);
                    break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        TEXT = new GLString();
        main_gl();
    } else {
        printf("Usage:\n\t%s\n", argv[0]);
        return 1;
    }
}

