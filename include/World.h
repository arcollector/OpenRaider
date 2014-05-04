/*!
 * \file include/World.h
 * \brief The game world (model)
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _WORLD_H_
#define _WORLD_H_

#include <list>
#include <vector>

#include "Room.h"
#include "Sprite.h"

#include "WorldData.h"

/*!
 * \brief The game world (model)
 */
class World {
public:

    /*!
     * \brief Deconstructs an object of World
     */
    ~World();

    /*!
     * \brief Adds mesh to world
     * \param model mesh to add
     */
    void addMesh(model_mesh_t *model);

    /*!
     * \brief Adds entity to world
     * \param e entity to add
     */
    void addEntity(entity_t *e);

    /*!
     * \brief Adds model to world.
     * \param model model to add
     * \returns next model ID or -1 on error
     */
    int addModel(skeletal_model_t *model);

    /*!
     * \brief Move entity in given direction unless collision occurs
     * \param e entity to move
     * \param movement direction of movement ('f', 'b', 'l' or 'r')
     */
    void moveEntity(entity_t *e, char movement);

    model_mesh_t *getMesh(int index);
    skeletal_model_t *getModel(int index);
    std::vector<entity_t *> *getEntities();

    void addRoom(Room &room);

    unsigned int sizeRoom();

    Room &getRoom(unsigned int index);

    void addSprite(SpriteSequence &sprite);

    unsigned int sizeSprite();

    SpriteSequence &getSprite(unsigned int index);

    /*!
     * \brief Find room a location is in.
     *
     * If it fails to be in a room it gives closest overlapping room.
     * \param index Guessed room index
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     * \returns correct room index or -1 for unknown
     */
    int getRoomByLocation(int index, float x, float y, float z);

    /*!
     * \brief Find room a location is in.
     *
     * If it fails to be in a room it gives closest overlapping room.
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     * \returns correct room index or -1 for unknown
     */
    int getRoomByLocation(float x, float y, float z);

    /*!
     * \brief Looks for portal crossings from xyz to xyz2 segment
     * from room[index]
     * \param index valid room index
     * \param x X coordinate of first point
     * \param y Y coordinate of first point
     * \param z Z coordinate of first point
     * \param x2 X coordinate of second point
     * \param y2 Y coordinate of second point
     * \param z2 Z coordinate of second point
     * \returns index of adjoined room or -1
     */
    int getAdjoiningRoom(int index,
                            float x, float y, float z,
                            float x2, float y2, float z2);

    /*!
     * \brief Gets the sector index of the position in room
     * \param room valid room index
     * \param x X coordinate in room
     * \param z Z coordinate in room
     * \returns sector index of position in room
     */
    int getSector(int room, float x, float z);
    int getSector(int room, float x, float z, float *floor, float *ceiling);

    unsigned int getRoomInfo(int room);

    /*!
     * \brief Check if sector is a wall
     * \param room valid room index
     * \param sector valid sector index
     * \returns true if this sector is a wall
     */
    bool isWall(int room, int sector);

    /*!
     * \brief Get the world height at a position
     * \param index valid room index
     * \param x X coordinate
     * \param y will be set to world height in that room
     * \param z Z coordinate
     */
    void getHeightAtPosition(int index, float x, float *y, float z);

    /*!
     * \brief Clears all data in world
     * \todo in future will check if data is in use before clearing
     */
    void destroy();

private:

    // Old World
    std::vector<entity_t *> mEntities;       //!< World entities
    std::vector<model_mesh_t *> mMeshes;     //!< Unanimated meshes
    std::vector<skeletal_model_t *> mModels; //!< Skeletal animation models

    // New World
    std::vector<Room *> mRooms;
    std::vector<SpriteSequence *> mSprites;

};

#endif
