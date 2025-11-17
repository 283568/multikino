// multikino.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#ifndef KINO_H
#define KINO_H

#define MAX_NAZOV 100
#define MAX_ZANER 50

typedef struct {
    int radek;
    int sedadlo;
    int obsazeno;
} Sedadlo;

typedef struct {
    int cislo;
    int radku;
    int sedadel_na_radu;
    Sedadlo*** sedadla; // 3D pointer - dôvod vysvetlím neskôr
} Sal;

typedef struct {
    char nazev[MAX_NAZOV];
    int delka;
    char zanr[MAX_ZANER];
} Film;
// Deklarácie funkcií
void nacitat_saly();
void zobraz_hlavne_menu();
void zobraz_filmy();
void rezervovat_miesta();
void zobraz_sal(int cislo_salu);
#define MAX_SALOV 10
#endif

// TODO: Reference additional headers your program requires here.
