/*!
 * \file include/SkeletalModel.h
 * \brief This is the factored out skeletal model class
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _SKELETALMODEL_H_
#define _SKELETALMODEL_H_

#include <vector>

#include "TombRaider.h"

class BoneTag {
  public:
    BoneTag(int m, float o[3], float r[3], char f);

    BoneTag(TombRaider& tr, unsigned int index, unsigned int j, unsigned int* l,
            unsigned int frame_offset);
    void display();

    void getOffset(float o[3]);
    void getRotation(float r[3]);
    char getFlag();

  private:
    int mesh;
    float off[3];
    float rot[3];
    char flag;
};

class BoneFrame {
  public:
    BoneFrame(float p[3]);

    BoneFrame(TombRaider& tr, unsigned int index, unsigned int frame_offset);
    ~BoneFrame();

    void getPosition(float p[3]);

    unsigned long size();
    BoneTag& get(unsigned long i);
    void add(BoneTag* t);

  private:
    float pos[3];
    std::vector<BoneTag*> tag;
};

class AnimationFrame {
  public:
    AnimationFrame(char r);

    AnimationFrame(TombRaider& tr, unsigned int index, int a, unsigned int* frame_offset,
                   int frame_step);
    ~AnimationFrame();

    unsigned long size();
    BoneFrame& get(unsigned long i);
    void add(BoneFrame* f);

  private:
    char rate;
    std::vector<BoneFrame*> frame;
};

class SkeletalModel {
  public:
    SkeletalModel(int i);

    SkeletalModel(TombRaider& tr, unsigned int index, int objectId);
    ~SkeletalModel();
    void display(unsigned long aframe, unsigned long bframe);

    int getId();
    void setPigTail(bool b);
    void setPonyPos(float x, float y, float z, float angle);

    unsigned long size();
    AnimationFrame& get(unsigned long i);
    void add(AnimationFrame* f);

  private:
    int id;
    bool tr4Overlay;
    bool pigtails;
    long ponytailId;
    float ponytail[3];
    long ponytailMeshId;
    unsigned int ponytailNumMeshes;
    float ponytailAngle;
    float ponyOff;
    float ponyOff2;
    std::vector<AnimationFrame*> animation;
};

#endif
