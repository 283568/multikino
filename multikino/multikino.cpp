// multikino.cpp : Defines the entry point for the application.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAZOV 100
#define MAX_ZANER 50
#define MAX_SALOV 10

typedef struct {
    int radek;
    int sedadlo;
    int obsazeno;
} Sedadlo;

typedef struct {
    int cislo;
    int radku;
    int sedadel_na_radu;
    Sedadlo*** sedadla;
} Sal;

typedef struct {
    char nazev[MAX_NAZOV];
    int delka;
    char zanr[MAX_ZANER];
} Film;

Sal** saly = NULL;
int pocet_salov = 0;

void nacitat_saly() {
    // Najprv vytvorime aspon jeden zakladny sal
    pocet_salov = 2; // Dva sály pre testovanie

    saly = (Sal**)malloc(pocet_salov * sizeof(Sal*));

    for (int i = 0; i < pocet_salov; i++) {
        saly[i] = (Sal*)malloc(sizeof(Sal));
        saly[i]->cislo = i + 1;
        saly[i]->radku = 5 + i; // Prvy sal 5 radov, druhy 6
        saly[i]->sedadel_na_radu = 8 + i; // Prvy sal 8 sedadiel, druhy 9

        // Alokujeme pamäť pre sedadlá
        saly[i]->sedadla = (Sedadlo***)malloc(saly[i]->radku * sizeof(Sedadlo**));

        for (int r = 0; r < saly[i]->radku; r++) {
            saly[i]->sedadla[r] = (Sedadlo**)malloc(saly[i]->sedadel_na_radu * sizeof(Sedadlo*));

            for (int s = 0; s < saly[i]->sedadel_na_radu; s++) {
                saly[i]->sedadla[r][s] = (Sedadlo*)malloc(sizeof(Sedadlo));
                saly[i]->sedadla[r][s]->radek = r;
                saly[i]->sedadla[r][s]->sedadlo = s;
                saly[i]->sedadla[r][s]->obsazeno = 0;
            }
        }

        printf("Vytvoreny sal %d: %d radov, %d sedadiel na radu\n",
            saly[i]->cislo, saly[i]->radku, saly[i]->sedadel_na_radu);
    }
}

void zobraz_sal(int cislo_salu) {
    if (cislo_salu < 1 || cislo_salu > pocet_salov) {
        printf("Neplatne cislo salu!\n");
        return;
    }

    Sal* sal = saly[cislo_salu - 1];
    printf("\n=== SAL %d ===\n", cislo_salu);
    printf("   ");
    for (int s = 0; s < sal->sedadel_na_radu; s++) {
        printf("%2d ", s + 1);
    }
    printf("\n");

    for (int r = 0; r < sal->radku; r++) {
        printf("%c: ", 'A' + r);
        for (int s = 0; s < sal->sedadel_na_radu; s++) {
            if (sal->sedadla[r][s]->obsazeno) {
                printf("[X]");
            }
            else {
                printf("[ ]");
            }
        }
        printf("\n");
    }
    printf("=============\n");
}

void zobraz_filmy() {
    printf("\n=== DOSTUPNE FILMY ===\n");
    printf("1. Avatar (19:00) - Sal 1\n");
    printf("2. Matrix (21:30) - Sal 1\n");
    printf("3. Avengers (18:15) - Sal 2\n");
    printf("======================\n");
}

void rezervovat_miesta() {
    printf("\n=== REZERVACIA MIEST ===\n");

    if (pocet_salov == 0) {
        printf("Nie su k dispozicii ziadne saly!\n");
        return;
    }

    zobraz_filmy();

    int vyber_filmu;
    printf("Zvolte film (1-3): ");
    scanf("%d", &vyber_filmu);

    // Automaticky priradíme sál podľa filmu
    int cislo_salu;
    if (vyber_filmu == 3) {
        cislo_salu = 2; // Avengers v sále 2
    }
    else {
        cislo_salu = 1; // Ostatné filmy v sále 1
    }

    printf("Film sa promita v sale %d\n", cislo_salu);

    // Zobrazenie sálu
    zobraz_sal(cislo_salu);

    // Výber miest
    int radek, sedadlo;
    printf("Zadajte rad (1-%d): ", saly[cislo_salu - 1]->radku);
    scanf("%d", &radek);
    printf("Zadajte sedadlo (1-%d): ", saly[cislo_salu - 1]->sedadel_na_radu);
    scanf("%d", &sedadlo);

    // Kontrola a rezervácia
    if (radek < 1 || radek > saly[cislo_salu - 1]->radku ||
        sedadlo < 1 || sedadlo > saly[cislo_salu - 1]->sedadel_na_radu) {
        printf("Neplatne miesto!\n");
        return;
    }

    Sedadlo* vybrane_sedadlo = saly[cislo_salu - 1]->sedadla[radek - 1][sedadlo - 1];

    if (vybrane_sedadlo->obsazeno) {
        printf("Sedadlo je uz obsadene!\n");
    }
    else {
        vybrane_sedadlo->obsazeno = 1;
        printf("Sedadlo %c%d uspesne rezervovane!\n", 'A' + radek - 1, sedadlo);

        // Generovanie kódu
        char kod[20];
        sprintf(kod, "KINO%d%c%d", cislo_salu, 'A' + radek - 1, sedadlo);
        printf("Vas rezervacny kod: %s\n", kod);
    }
}

void zobraz_hlavne_menu() {
    printf("\n=== HLAVNE MENU ===\n");
    printf("1. Zobraz filmy a casy\n");
    printf("2. Rezervovat miesta\n");
    printf("3. Koniec\n");
    printf("===================\n");
}

int main() {
    printf("=== SYSTEM REZERVACIE MULTIKINO ===\n");

    // Načítame sály
    nacitat_saly();

    // Hlavná slučka programu
    int volba;
    do {
        zobraz_hlavne_menu();
        printf("Zvolte moznost: ");
        scanf("%d", &volba);

        switch (volba) {
        case 1:
            zobraz_filmy();
            break;
        case 2:
            rezervovat_miesta();
            break;
        case 3:
            printf("Koniec programu.\n");
            break;
        default:
            printf("Neplatna volba!\n");
        }
    } while (volba != 3);

    return 0;
}
