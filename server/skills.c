#include "server.h"

void prepare_skills() {
    strcpy(skills[LEMON].name, "lemon");
    skills[LEMON].priority = 0;
    skills[LEMON].energy = -1;
    skills[LEMON].type = CHARGE;

    strcpy(skills[FIRE].name, "fire");
    skills[FIRE].priority = 1;
    skills[FIRE].energy = 3;
    skills[FIRE].type = ATTACK;

    strcpy(skills[BARRIER].name, "barrier");
    skills[BARRIER].priority = 2;
    skills[BARRIER].energy = 0;
    skills[BARRIER].type = DEFENCE;

    strcpy(skills[CHANGE].name, "change");
    skills[CHANGE].priority = 3;
    skills[CHANGE].energy = 1;
    skills[CHANGE].type = REFLECT;

    strcpy(skills[GUN].name, "gun");
    skills[GUN].priority = 4;
    skills[GUN].energy = 5;
    skills[GUN].type = ATTACK;
}