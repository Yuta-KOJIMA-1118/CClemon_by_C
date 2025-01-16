#include "server.h"

void prepare_skills() {
    strcpy(skills[0].name, "lemon");
    skills[0].priority = 0;
    skills[0].energy = -1;
    skills[0].type = CHARGE;

    strcpy(skills[1].name, "fire");
    skills[1].priority = 1;
    skills[1].energy = 3;
    skills[1].type = ATTACK;

    strcpy(skills[2].name, "barrier");
    skills[2].priority = 2;
    skills[2].energy = 0;
    skills[2].type = DEFENCE;

    strcpy(skills[3].name, "change");
    skills[3].priority = 3;
    skills[3].energy = 1;
    skills[3].type = REFLECT;

    strcpy(skills[4].name, "gun");
    skills[4].priority = 4;
    skills[4].energy = 5;
    skills[4].type = ATTACK;
}