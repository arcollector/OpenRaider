/*!
 * \file src/FontTRLE.cpp
 * \brief SDL Font implementation
 *
 * \author xythobuz
 */

#include <fstream>
#include <sstream>

#include "global.h"
#include "utils/strings.h"
#include "FontTRLE.h"

FontTRLE::FontTRLE() {
    mFontInit = false;
    mFontName = NULL;
    mFontTexture = 0;
}

FontTRLE::~FontTRLE() {
}

int FontTRLE::initialize() {
    //! \todo Font coloring not working when .pc has color?!?!

    assert(mFontInit == false);
    assert(mFontName != NULL);
    assert(mFontName[0] != '\0');
    assert(stringEndsWith(mFontName, ".pc") == true);

    // Load .pc file...
    std::ifstream file(mFontName, std::ios::in | std::ios::binary);
    unsigned char *pixels = new unsigned char[256 * 256 * 4];
    if (!file.read((char *)pixels, 256 * 256 * 4)) {
        delete [] pixels;
        return -1;
    }

    // Fix coloring
    for (unsigned int i = 0; i < (256 * 256 * 4); i += 4) {
        float y = (0.2126f * pixels[i + 2]);
        y += (0.7152f * pixels[i + 1]);
        y += (0.0722f * pixels[i]);
        pixels[i] = pixels[i + 1] = pixels[i + 2] = (unsigned char)y;
    }

    // ...into GL texture
    glGenTextures(1, &mFontTexture);
    glBindTexture(GL_TEXTURE_2D, mFontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
    delete [] pixels;

    // Try to load .lps file, overwriting default glyph positions
    char *lpsFile = stringReplace(mFontName, ".pc", ".lps");
    loadLPS(lpsFile);
    delete [] lpsFile;

    mFontInit = true;
    return 0;
}

void FontTRLE::loadLPS(const char *f) {
    std::ifstream file(f);
    if (!file)
        return;

    /*!
     * \todo This is probably the worlds most unreliable parser...
     */

    for (std::string line; std::getline(file, line);) {
        std::istringstream stream(line);
        std::string tok1, tok2;
        std::getline(stream, tok1, '=');
        std::getline(stream, tok2);

        // we are only interested in lines starting with
        // xxx=
        // Where xxx is a 3 digit number
        try {
            int index = std::stoi(tok1);
            if ((index >= 0) && (index <= 105)) {
                std::istringstream row(tok2);
                std::string a, b, c, d, e;
                std::getline(row, a, ',');
                std::getline(row, b, ',');
                std::getline(row, c, ',');
                std::getline(row, d, ',');
                std::getline(row, e);
                offsets[index][0] = std::stoi(a);
                offsets[index][1] = std::stoi(b);
                offsets[index][2] = std::stoi(c);
                offsets[index][3] = std::stoi(d);
                offsets[index][4] = std::stoi(e);
            }
        } catch (std::invalid_argument) {

        }
    }
}

#define SCALING 2.0f

void FontTRLE::writeChar(unsigned int index, unsigned int xDraw, FontString &s) {
    int width = (int)(((vec_t)offsets[index][2]) * s.scale * SCALING);
    int height = (int)(((vec_t)offsets[index][3]) * s.scale * SCALING);
    int offset = (int)(((vec_t)offsets[index][4]) * s.scale * SCALING);

    // screen coordinates
    int xMin = xDraw;
    int yMin = s.y + offset + (int)(10.0f * s.scale * SCALING);
    int xMax = xMin + width;
    int yMax = yMin + height;

    // texture part
    vec_t txMin = ((vec_t)offsets[index][0]) / 256.0f;
    vec_t txMax = ((vec_t)(offsets[index][0] + offsets[index][2])) / 256.0f;
    vec_t tyMin = ((vec_t)offsets[index][1]) / 256.0f;
    vec_t tyMax = ((vec_t)(offsets[index][1] + offsets[index][3])) / 256.0f;

    // draw
    glBindTexture(GL_TEXTURE_2D, mFontTexture);
    glColor4f(s.color[0], s.color[1], s.color[2], s.color[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(txMin, tyMin);
    glVertex2i(xMin, yMin);
    glTexCoord2f(txMin, tyMax);
    glVertex2i(xMin, yMax);
    glTexCoord2f(txMax, tyMax);
    glVertex2i(xMax, yMax);
    glTexCoord2f(txMax, tyMin);
    glVertex2i(xMax, yMin);
    glEnd();
}

void FontTRLE::writeString(FontString &s) {
    assert(mFontInit == true);
    assert(s.text != NULL);

    unsigned int x = s.x;
    unsigned int y = 0;

    for (unsigned int i = 0; s.text[i] != '\0'; i++) {
        // index into offset table
        int index = s.text[i] - '!';

        if (index == -1) // space
            x += (unsigned int)(14.0f * s.scale * SCALING);

        if ((index < 0) || (index > 105))
            continue; // skip unprintable chars

        writeChar((unsigned int)index, x, s);
        x += (int)((vec_t)(offsets[index][2] + 1) * s.scale * SCALING); // width

        if (y < (unsigned int)(((vec_t)offsets[index][3]) * s.scale * SCALING))
            y = (unsigned int)(((vec_t)offsets[index][3]) * s.scale * SCALING);
    }

    s.w = x - s.x;
    s.h = y;
}
