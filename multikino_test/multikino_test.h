#ifndef KINO_H
#define KINO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// preddefinovane konstanty
#define MAX_NAZOV 100
#define MAX_KOD 20
#define MAX_FILMY 10
#define MAX_RAD 15
#define MAX_SEDADLA 20
#define MAX_CASOV 3
#define MAX_SAL 15

// struktura pre film
typedef struct {
    char nazov[MAX_NAZOV];
    int cislo_saly;
	char casy[MAX_CASOV][6]; //dvojrozmerne pole pre casy
	int pocet_casov; // realny pocet casov pre dany film
} Film;

// struktura pre sedadlo
typedef struct {
    int rad;
    int sedadlo;
	int rezervovane; // 0 = volne, 1 = rezervovane
    char kod_vstupenky[MAX_KOD];
} Sedadlo;

// struktura pre salu
typedef struct {
    int cislo_saly;
    char cas[6]; 
    int rady; 
    int sedadla;
    Sedadlo sedadlo_pole[MAX_RAD][MAX_SEDADLA];  // typ bunky: sedadlo, dvojrozmerne pole celej saly
} Sala;

// struktura pre cely system
typedef struct {
	Film filmy[MAX_FILMY]; //pole filmov
	Sala saly[MAX_FILMY][MAX_CASOV]; //dvojrozmerne pole sal pre kazdy film a jeho casy
    int pocet_filmov;
} Kinosystem;

// Deklarácie funkcií
void nacitaj_filmy(Kinosystem* system, const char* nazov_suboru);
void uloz_filmy(Kinosystem* system, const char* nazov_suboru);
void nacitaj_salu(Sala* sala, const char* nazov_suboru);
void uloz_salu(const Sala* sala, const char* nazov_suboru);
void zobraz_filmy(Kinosystem* system);
void inicializuj_salu(Sala* sala, int cislo_saly, const char* cas, int rady, int sedadla);
void zobraz_salu(const Sala* sala);
void rezervuj_sedadlo(Sala* sala, const char* cas);
void generuj_kod_vstupenky(char* kod);
void zrus_rezervaciu(Sala* sala);
Sala* najdi_salu_podla_cisla_a_casu(Kinosystem* system, int cislo_saly, const char* cas);
void pridaj_film(Kinosystem* system);

#endif // KINO_H