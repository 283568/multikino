// basic rundown:
// program pracuje so suborom filmy.txt odkial nacita filmy, saly a casy
// do neho ulozi pridane nove filmy
// pre kazdu salu vytvori subor sala_X_HHMM.txt kde X je cislo saly a HHMM je cas   
// rezervuje a rusi sedadla v sale
// uz teraz to ma cez 700 lines taze bonus neni :(

#include "multikino_test.h"
#include <stdio.h>  
#include <stdlib.h> // atoi, rand
#include <string.h> // strcpy, strtok
#include <time.h> // time

// komentare
// ->.. pristup k polu v struct cez pointer
// buffer.. docasna premenna na ulozenie retazca
// token.. cast retazca po rozdeleni
// atoi.. prevod retazca na cele cislo
// strtok.. funkcia na rozdelenie retazca na casti
// strcpy.. kopirovanie retazca
// strcspn.. najdenie indexu znaku v retazci
// strchr.. najdenie prveho vyskytu znaku v retazci
// sprintf.. formatovanie retazca do premennej (KOD + cislo)

void nacitaj_filmy(Kinosystem* system, const char* nazov_suboru) {
    //vytvorenie premennej subor na citanie
    FILE* subor = fopen(nazov_suboru, "r");

    //ak sa subor neda otvorit
    if (subor == NULL) {
        printf("Subor %s sa nenasiel!\n", nazov_suboru);
        system->pocet_filmov = 0; //nastavi pocet filmov na 0
        return;
    }

    system->pocet_filmov = 0;
    char riadok[256];

	// format filmu: nazov|cislo_saly|cas1,cas2,cas3
    // citanie riadkov zo suboru kym neprekrocim max pocet filmov
    while (fgets(riadok, sizeof(riadok), subor) && system->pocet_filmov < MAX_FILMY) {
        
        // nahradenie \n z konca riadku nulou (vymazanie)
		// strcspn.. index \n v riadku
        riadok[strcspn(riadok, "\n")] = 0;

		//preskocenie prazdnych riadkov
		int je_prazdny = 1; //1 - prazdny, 0 - plny
        for (int i = 0; riadok[i]; i++) {
            if (riadok[i] != ' ' && riadok[i] != '\t') {
                je_prazdny = 0;
                break;
            }
        }
        // preskocenie prazdnych riadkov
        if (je_prazdny) {
            continue;
        }

        // token je pointer
        // token = nazov film
        // strtok.. rozdeluje retazec na casti-tokeny podla |
        char* token = strtok(riadok, "|");

        // strcpy.. kopiruje retazec do noveho
        // ak token nie je NULL, ulozenie nazvu filmu do struktury Film z tokenu
        if (token) strcpy(system->filmy[system->pocet_filmov].nazov, token);

        token = strtok(NULL, "|"); // token = cislo saly

        // atoi prevadza retazec na cele cislo
        // ulozenie cisla saly
        if (token) system->filmy[system->pocet_filmov].cislo_saly = atoi(token);

        token = strtok(NULL, "|\n"); // token = casy
        if (token) {
            char* cas_token = strtok(token, ","); // rozdelenie casov podla ,
            int pocet = 0;
            while (cas_token && pocet < MAX_CASOV) {

                // ulozenie casu
                strcpy(system->filmy[system->pocet_filmov].casy[pocet], cas_token);
                pocet++;

                cas_token = strtok(NULL, ",");
            }
            // update poctu nacitanych casov
            system->filmy[system->pocet_filmov].pocet_casov = pocet;
        }
        // prechod na dalsi film
        system->pocet_filmov++;
    }
    fclose(subor);
}

void uloz_filmy(Kinosystem* system, const char* nazov_suboru) {
    // write mode
    FILE* subor = fopen(nazov_suboru, "w");
    if (subor == NULL) {
        printf("Chyba pri zapisovani do suboru %s\n", nazov_suboru);
        return;
    }

    // prepisanie suboru 
    for (int i = 0; i < system->pocet_filmov; i++) {
        if (system->filmy[i].pocet_casov > 0) {
            // zapis nazvu filmu a cisla saly 
            fprintf(subor, "%s|%d|", system->filmy[i].nazov, system->filmy[i].cislo_saly);

            for (int j = 0; j < system->filmy[i].pocet_casov; j++) {
                // zapis casov 
                fprintf(subor, "%s", system->filmy[i].casy[j]);

                // ak to nie je posledny cas prida ciarku
                if (j < system->filmy[i].pocet_casov - 1) {
                    fprintf(subor, ",");
                }
            }
			// ak to nie je posledny film prida novy riadok
            if (i < system->pocet_filmov - 1) {
                fprintf(subor, "\n");
            }
        }
    }
    fclose(subor);
}

void zobraz_filmy(Kinosystem* system) {
    printf("Filmy:\n");
	// vypise poradie, nazov filmu, cislo saly
    for (int i = 0; i < system->pocet_filmov; i++) {
        printf("%d. %s (Sala %d) - ", i + 1, system->filmy[i].nazov, system->filmy[i].cislo_saly);
        // vypise casy
        for (int j = 0; j < system->filmy[i].pocet_casov; j++) {
            printf("%s ", system->filmy[i].casy[j]);
        }
        printf("\n");
    }
}

void nacitaj_salu(Sala* sala, const char* nazov_suboru) {
    FILE* subor = fopen(nazov_suboru, "r");
    // ak subor neexistuje vytvori novu salu
    if (subor == NULL) {
        int cislo_saly = 1;
        char cas[6] = "10:30";

		// strstr.. najde slovo sala v nazve suboru
        const char* cislo_str = strstr(nazov_suboru, "sala");

		// skipnutie sala_ v nazve suboru
        if (cislo_str != NULL) {
            cislo_str += 4;
			if (*cislo_str == '_') cislo_str++; // preskoci _
            cislo_saly = atoi(cislo_str); // cislo saly

            const char* cas_str = strchr(cislo_str, '_');
            if (cas_str != NULL) {
                cas_str++;
                if (strlen(cas_str) >= 4) {
					sprintf(cas, "%.2s:%.2s", cas_str, cas_str + 2); // cas HH:MM
                }
            }
        }
        // vytvorenie saly s default rozmermi
        inicializuj_salu(sala, cislo_saly, cas, 8, 10); 
        return;
    }

    int cislo_saly, rady, sedadla;
    // ak nenacita 3 udaje
    if (fscanf(subor, "%d %d %d", &cislo_saly, &rady, &sedadla) != 3) {
        fclose(subor);
        inicializuj_salu(sala, 1, "10:30", 8, 10);
        return;
    }

    char cas[6] = "10:30";
    const char* cas_str = strstr(nazov_suboru, "sala");
    if (cas_str != NULL) {
        cas_str = strchr(cas_str, '_');
        if (cas_str != NULL) {
            cas_str = strchr(cas_str + 1, '_');
            if (cas_str != NULL) {
                cas_str++;
                if (strlen(cas_str) >= 4) {
                    sprintf(cas, "%.2s:%.2s", cas_str, cas_str + 2);
                }
            }
        }
    }
    inicializuj_salu(sala, cislo_saly, cas, rady, sedadla);

	// pre kazde sedadlo nacita stav rezervacie, nastavi kod vstupenky na ""
    for (int i = 0; i < sala->rady; i++) {
        for (int j = 0; j < sala->sedadla; j++) {
            if (fscanf(subor, "%d", &sala->sedadlo_pole[i][j].rezervovane) != 1) {
                sala->sedadlo_pole[i][j].rezervovane = 0;
            }
            strcpy(sala->sedadlo_pole[i][j].kod_vstupenky, "");
        }
    }
    fclose(subor);
}

void inicializuj_salu(Sala* sala, int cislo_saly, const char* cas, int rady, int sedadla) {
    // ulozenie cisla saly a casu
    sala->cislo_saly = cislo_saly;
    strcpy(sala->cas, cas);

	// ak su rozmery vacsie ako max pouzije max
    sala->rady = (rady <= MAX_RAD) ? rady : MAX_RAD;
    sala->sedadla = (sedadla <= MAX_SEDADLA) ? sedadla : MAX_SEDADLA;

	// nastavenie kazdeho sedadla na volne a kod vstupenky na ""
    for (int i = 0; i < sala->rady; i++) {
        for (int j = 0; j < sala->sedadla; j++) {
            sala->sedadlo_pole[i][j].rad = i + 1;
            sala->sedadlo_pole[i][j].sedadlo = j + 1;
            sala->sedadlo_pole[i][j].rezervovane = 0;
            strcpy(sala->sedadlo_pole[i][j].kod_vstupenky, "");
        }
    }
}

void zobraz_salu(Sala* sala) {
    printf("\nSala %d - %s\n", sala->cislo_saly, sala->cas);

    printf("   ");
	// vypis cisiel sedadiel
    for (int j = 0; j < sala->sedadla; j++) {
		printf("%2d ", j + 1); // cislo na 2 miesta = zarovnanie
    }
    printf("\n");

	// vypis radov 
    for (int i = 0; i < sala->rady; i++) {
        printf("%2d ", i + 1);

		// vypis stavu sedadiel v rade (O alebo X)
        for (int j = 0; j < sala->sedadla; j++) {
            printf(" %c ", sala->sedadlo_pole[i][j].rezervovane ? 'X' : 'O');
        }
        printf("\n");
    }
    printf("O=volne X=obsadene\n");
}
// sala v subore
void uloz_salu(const Sala* sala, const char* nazov_suboru) {
    FILE* subor = fopen(nazov_suboru, "w");
    if (subor == NULL) return;

	// ulozenie cisla saly, poctu radov a sedadiel
    fprintf(subor, "%d %d %d\n", sala->cislo_saly, sala->rady, sala->sedadla);

	// ulozenie stavu rezervacie kazdeho sedadla
    for (int i = 0; i < sala->rady; i++) {
        for (int j = 0; j < sala->sedadla; j++) {
            fprintf(subor, "%d ", sala->sedadlo_pole[i][j].rezervovane);
        }
        fprintf(subor, "\n");
    }
    fclose(subor);
}

Sala* najdi_salu_podla_cisla_a_casu(Kinosystem* system, int cislo_saly, const char* cas) {
	// prejde vsetky filmy a ich casy, 
    // ak sa zhoduju vrati pointer na salu
    for (int i = 0; i < system->pocet_filmov; i++) {
        for (int j = 0; j < system->filmy[i].pocet_casov; j++) {
            if (system->saly[i][j].cislo_saly == cislo_saly &&
                strcmp(system->saly[i][j].cas, cas) == 0) {
                return &system->saly[i][j];
            }
        }
    }
    return NULL;
}

void generuj_kod_vstupenky(char* kod) {
	// rand.. generacia nahodneho cisla (0-9999)
    // formatovanie na KOD + 4 cislice
    sprintf(kod, "KOD%04d", rand() % 10000);
}

void rezervuj_sedadlo(Sala* sala, const char* cas) {
	// input radu a sedadla
    int vybraty_rad, vybrate_sedadlo;
    printf("Rezervacia:\n");
    zobraz_salu(sala);
    printf("Rad (1-%d): ", sala->rady);
    scanf("%d", &vybraty_rad);
    printf("Sedadlo (1-%d): ", sala->sedadla);
    scanf("%d", &vybrate_sedadlo);

	// ked je cislo mimo rozsah
    if (vybraty_rad < 1 || vybraty_rad > sala->rady ||
        vybrate_sedadlo < 1 || vybrate_sedadlo > sala->sedadla) {
        printf("Neplatne cislo!\n");
        return;
    }
    int rad_index = vybraty_rad - 1;
    int sedadlo_index = vybrate_sedadlo - 1;

    // ked je rezervovane
    if (sala->sedadlo_pole[rad_index][sedadlo_index].rezervovane) {
        printf("Sedadlo je obsadene!\n");
        return;
    }

	// generovanie kodu
    char kod[MAX_KOD];
    sprintf(kod, "KOD%04d", rand() % 10000);

	// nastavenie sedadla ako rezervovane a ulozenie kodu
    sala->sedadlo_pole[rad_index][sedadlo_index].rezervovane = 1;
    strcpy(sala->sedadlo_pole[rad_index][sedadlo_index].kod_vstupenky, kod);
    printf("Rezervovane! Kod: %s\n", kod);
}

void zrus_rezervaciu(Sala* sala) {
    int vybraty_rad, vybrate_sedadlo;
    char zadany_kod[MAX_KOD];
    printf("Zrusenie rezervacie:\n");
    zobraz_salu(sala);

	// input radu, sedadla a kodu
    printf("Rad (1-%d): ", sala->rady);
    scanf("%d", &vybraty_rad);
    printf("Sedadlo (1-%d): ", sala->sedadla);
    scanf("%d", &vybrate_sedadlo);
    printf("Kod: ");
    scanf("%19s", zadany_kod);

	// cislo mimo rozsah
    if (vybraty_rad < 1 || vybraty_rad > sala->rady ||
        vybrate_sedadlo < 1 || vybrate_sedadlo > sala->sedadla) {
        printf("Neplatne cislo!\n");
        return;
    }
    int rad_index = vybraty_rad - 1;
    int sedadlo_index = vybrate_sedadlo - 1;

	// nie je rezervovane
    if (!sala->sedadlo_pole[rad_index][sedadlo_index].rezervovane) {
        printf("Sedadlo nie je rezervovane!\n");
        return;
    }

	// ak sa kody nezhoduju
    if (strcmp(sala->sedadlo_pole[rad_index][sedadlo_index].kod_vstupenky, zadany_kod) != 0) {
        printf("Nespravny kod!\n");
        return;
    }

	// nastavenie sedadla ako volne a kodu na ""
    sala->sedadlo_pole[rad_index][sedadlo_index].rezervovane = 0;
    strcpy(sala->sedadlo_pole[rad_index][sedadlo_index].kod_vstupenky, "");
    printf("Rezervacia zrusena!\n");
}

void pridaj_film(Kinosystem* system) {
    // kontrola miesta pre dalsi film
    if (system->pocet_filmov >= MAX_FILMY) {
        printf("Nemas miesto pre dalsi film! Maximalny pocet filmov je %d.\n", MAX_FILMY);
        return;
    }
	// index noveho filmu (koniec aktualneho poctu)
    int index_noveho_filmu = system->pocet_filmov;
	char buffer[100]; // pomocny buffer pre vstupy

    printf("Zadaj nazov noveho filmu: ");
    int c;
	while ((c = getchar()) != '\n' && c != EOF) {} // vycisti vstupny buffer

    // precitanie nazvu
    fgets(system->filmy[index_noveho_filmu].nazov, MAX_NAZOV, stdin);

	// odstrani \n z nazvu
    char* newline = strchr(system->filmy[index_noveho_filmu].nazov, '\n');
    if (newline) {
        *newline = '\0';
    }

    int cislo_saly;
    int platne_cislo_saly = 0;

    // opakovane zadavanie kym cislo saly neni platne
    while (!platne_cislo_saly) {
        printf("Zadaj cislo saly (1-15): ");
		fgets(buffer, sizeof(buffer), stdin); // buffer ma cislo saly

        // odstranenie \n z bufferu
        newline = strchr(buffer, '\n');
        if (newline) {
            *newline = '\0';
        }

        cislo_saly = atoi(buffer);

        // kontrola rozsahu 1-15
        if (cislo_saly < 1 || cislo_saly > MAX_SAL) {
            printf("Neplatne cislo saly! Cislo musi byt v rozsahu 1-%d.\n", MAX_SAL);
			continue; // spat na zaciatok while
        }

        // kontrola ci uz sala s tymto cislom existuje
        int sala_uz_existuje = 0;
        for (int i = 0; i < system->pocet_filmov; i++) {
            if (system->filmy[i].cislo_saly == cislo_saly) {
                sala_uz_existuje = 1;
                break;
            }
        }
        if (sala_uz_existuje) {
            printf("Sala cislo %d uz existuje! Vyber ine cislo saly.\n", cislo_saly);
        }
        else {
            platne_cislo_saly = 1;
        }
    }
	// ulozenie cisla saly do struct Film
    system->filmy[index_noveho_filmu].cislo_saly = cislo_saly;

    // 
    printf("Kolko casov pre film chces zadat? (max %d): ", MAX_CASOV);
    fgets(buffer, sizeof(buffer), stdin);
    newline = strchr(buffer, '\n');
    if (newline) {
        *newline = '\0';
    }

    int pocet_zadanych_casov = atoi(buffer);

	// kontrola rozsahu casov
    if (pocet_zadanych_casov < 1 || pocet_zadanych_casov > MAX_CASOV) {
        printf("Neplatny pocet casov! Pouzijem maximalny pocet %d.\n", MAX_CASOV);
        pocet_zadanych_casov = MAX_CASOV;
    }

    // zaciatok = 0
    system->filmy[index_noveho_filmu].pocet_casov = 0;
    // cyklus pre kazdy cas
    for (int i = 0; i < pocet_zadanych_casov; i++) {
        printf("Zadaj cas %d (format HH:MM): ", i + 1);
        fgets(buffer, sizeof(buffer), stdin);

        newline = strchr(buffer, '\n');
        if (newline) {
            *newline = '\0';
        }

		// : musi byt na 2. indexe a dlzka byt 5 
        if (strlen(buffer) == 5 && buffer[2] == ':') {
            strcpy(system->filmy[index_noveho_filmu].casy[i], buffer);
            system->filmy[index_noveho_filmu].pocet_casov++;
        }
        else {
            printf("Neplatny format casu! Pouzite HH:MM (napr. 14:30)\n");
            i--; // opakuj zadanie tohto casu
        }
    }

	// preddefinovane rozmery saly
    int predvolene_rady = 8;
    int predvolene_sedadla = 10;

    printf("Pre salu %d budu pouzite predvolene rozmery: %d radov, %d sedadiel v rade.\n",
        cislo_saly, predvolene_rady, predvolene_sedadla);

	// tvorba sal pre kazdy cas filmu
    for (int j = 0; j < system->filmy[index_noveho_filmu].pocet_casov; j++) {
        char nazov_suboru[50];
        char cas_bez_dvojbodky[6];

        // kopiruje cas
        strcpy(cas_bez_dvojbodky, system->filmy[index_noveho_filmu].casy[j]);

        // basically skipne : 
        int m = 0;
        for (int k = 0; cas_bez_dvojbodky[k]; k++) {
            if (cas_bez_dvojbodky[k] != ':') {
                cas_bez_dvojbodky[m++] = cas_bez_dvojbodky[k];
            }
        }
        cas_bez_dvojbodky[m] = '\0';

		// nazov suboru sala_X_HHMM.txt
        sprintf(nazov_suboru, "sala_%d_%s.txt",
            system->filmy[index_noveho_filmu].cislo_saly,
            cas_bez_dvojbodky);

		// tvorba saly s preddefinovanymi rozmermi
        inicializuj_salu(&system->saly[index_noveho_filmu][j],
            system->filmy[index_noveho_filmu].cislo_saly,
            system->filmy[index_noveho_filmu].casy[j],
            predvolene_rady, predvolene_sedadla);

		// ulozenie saly do suboru
        uloz_salu(&system->saly[index_noveho_filmu][j], nazov_suboru);
        printf("Vytvorena sala: %s (%d radov, %d sedadiel)\n",
            nazov_suboru, predvolene_rady, predvolene_sedadla);
    }

    system->pocet_filmov++;
	uloz_filmy(system, "filmy.txt"); // ulozenie do filmy.txt

    printf("Film '%s' bol uspesne pridany do saly %d!\n",
        system->filmy[index_noveho_filmu].nazov,
        system->filmy[index_noveho_filmu].cislo_saly);
}

void zobraz_dostupne_saly(Kinosystem* system) {
    int pouzite_saly[16] = { 0 };  // pole 16 cisel, vsetky 0 

	// prejde kazdy film a oznaci pouzite saly
    for (int i = 0; i < system->pocet_filmov; i++) {
        if (system->filmy[i].cislo_saly >= 1 && system->filmy[i].cislo_saly <= 15) {
            pouzite_saly[system->filmy[i].cislo_saly] = 1;
        }
    }

    printf("Dostupne saly: ");
    int prva_sala = 1;
    for (int i = 1; i <= MAX_SAL; i++) {
        if (!pouzite_saly[i]) { // ak neni pouzita
			if (!prva_sala) { // prida ciarku ak to nie je prva sala
                printf(", ");
            }
            printf("%d", i); // napise cislo saly
            prva_sala = 0;
        }
    }
    printf("\n");

    // to iste jak dostupne
    printf("Obsadene saly: ");
    prva_sala = 1;
    for (int i = 1; i <= 15; i++) {
        if (pouzite_saly[i]) {
            if (!prva_sala) {
                printf(", ");
            }
            printf("%d", i);
            prva_sala = 0;
        }
    }
    printf("\n");
}

int main() {
    Kinosystem system;
    int volba;
    int vybraty_film = -1;
    int vybraty_cas = -1;
    Sala* vybrata_sala = NULL;

    srand((unsigned int)time(NULL));

    printf("REZERVACNY SYSTEM\n");
    nacitaj_filmy(&system, "filmy.txt");

	// nacitanie sal pre kazdy film a jeho casy
    for (int i = 0; i < system.pocet_filmov; i++) {
        for (int j = 0; j < system.filmy[i].pocet_casov; j++) {
            char nazov_suboru[50];
            char cas_bez_dvojbodky[6]; 
            strcpy(cas_bez_dvojbodky, system.filmy[i].casy[j]);
            for (int k = 0; cas_bez_dvojbodky[k]; k++) {
                if (cas_bez_dvojbodky[k] == ':') {
                    for (int l = k; cas_bez_dvojbodky[l]; l++) {
                        cas_bez_dvojbodky[l] = cas_bez_dvojbodky[l + 1];
                    }
                }
            }
			// nazov suboru sala_X_HHMM.txt
            sprintf(nazov_suboru, "sala_%d_%s.txt", system.filmy[i].cislo_saly, cas_bez_dvojbodky);
            nacitaj_salu(&system.saly[i][j], nazov_suboru);
            system.saly[i][j].cislo_saly = system.filmy[i].cislo_saly;
            strcpy(system.saly[i][j].cas, system.filmy[i].casy[j]);
        }
    }

	do { // vypise moznosti
        printf("\nHLAVNE MENU:\n");
        printf("1. Zobraz filmy a vyber film\n");
        printf("2. Rezervuj sedadlo\n");
        printf("3. Zrus rezervaciu\n");
        printf("4. Zobraz salu\n");
        printf("5. Pridaj novy film\n");
        printf("6. Zobraz dostupne saly\n");  
        printf("0. Koniec\n");
        printf("Volba: ");
		scanf("%d", &volba); // nacita volbu

        // rozobera vsetky pripady
        switch (volba) {
        case 1:
            // zobrazenie a vyber 
            zobraz_filmy(&system);
            printf("Film (1-%d): ", system.pocet_filmov);
            scanf("%d", &vybraty_film);
            if (vybraty_film >= 1 && vybraty_film <= system.pocet_filmov) {
                vybraty_film--;
                printf("Vybrali ste film: %s\n", system.filmy[vybraty_film].nazov);

				// ak ma film 1 cas - automaticky vyber
                if (system.filmy[vybraty_film].pocet_casov == 1) {
                    vybraty_cas = 1; 
                    printf("Film ma iba jeden cas: %s\n", system.filmy[vybraty_film].casy[0]);

                    vybrata_sala = najdi_salu_podla_cisla_a_casu(&system,
                        system.filmy[vybraty_film].cislo_saly,
                        system.filmy[vybraty_film].casy[0]);

                    printf("Sala %d vybrana.\n", system.filmy[vybraty_film].cislo_saly);
                }
                // uzivatel vybera cas
                else if (system.filmy[vybraty_film].pocet_casov > 1) {
                    printf("Dostupne casy: ");
                    for (int j = 0; j < system.filmy[vybraty_film].pocet_casov; j++) {
                        printf("%d.%s ", j + 1, system.filmy[vybraty_film].casy[j]);
                    }
                    printf("\nCas (1-%d): ", system.filmy[vybraty_film].pocet_casov);
                    scanf("%d", &vybraty_cas);

                    if (vybraty_cas >= 1 && vybraty_cas <= system.filmy[vybraty_film].pocet_casov) {
                        vybraty_cas--;
                        vybrata_sala = najdi_salu_podla_cisla_a_casu(&system,
                            system.filmy[vybraty_film].cislo_saly,
                            system.filmy[vybraty_film].casy[vybraty_cas]);
                    }
                    // ak cas neni v rozsahu
                    else {
                        printf("Neplatny cas!\n");
                        vybraty_cas = -1;
                        vybrata_sala = NULL;
                    }
                }
				// ak nema ziadne casy
                else {
                    printf("Chyba: Film nema zadane ziadne casy!\n");
                    vybraty_film = -1;
                    vybraty_cas = -1;
                    vybrata_sala = NULL;
                }
            }
			// film nie je v rozsahu
            else {
                printf("Neplatny film!\n");
                vybraty_film = -1;
                vybraty_cas = -1;
                vybrata_sala = NULL;
            }
            break;

        case 2: // rezervuje ak sa uz vybral film (sala je dajake cislo)
            if (vybrata_sala != NULL) {
                rezervuj_sedadlo(vybrata_sala, system.filmy[vybraty_film].casy[vybraty_cas]);
            }
            else {
                printf("Najprv vyber film!\n");
            }
            break;

		case 3: // to iste ako 2
            if (vybrata_sala != NULL) {
                zrus_rezervaciu(vybrata_sala);
            }
            else {
                printf("Najprv vyber film!\n");
            }
            break;

		case 4: // to iste ako 2
            if (vybrata_sala != NULL) {
                zobraz_salu(vybrata_sala);
            }
            else {
                printf("Najprv vyber film!\n");
            }
            break;

		case 5: // ukaze dostupne saly a prida film
            zobraz_dostupne_saly(&system);  
            pridaj_film(&system);
            break;

		case 6: // dostupne saly
            zobraz_dostupne_saly(&system);
            break;

		case 0: // koniec
            break;

        default: 
            printf("Neplatna volba!\n");
        }
    } while (volba != 0); // opakuje kym neni volba 0

    // ulozenie zmien
    uloz_filmy(&system, "filmy.txt");
	// ulozenie sal do suborov pre kazdy film a cas
    for (int i = 0; i < system.pocet_filmov; i++) {
        for (int j = 0; j < system.filmy[i].pocet_casov; j++) {
            char nazov_suboru[50];
            char cas_bez_dvojbodky[6];
            strcpy(cas_bez_dvojbodky, system.filmy[i].casy[j]);
            for (int k = 0; cas_bez_dvojbodky[k]; k++) {
                if (cas_bez_dvojbodky[k] == ':') {
                    for (int l = k; cas_bez_dvojbodky[l]; l++) {
                        cas_bez_dvojbodky[l] = cas_bez_dvojbodky[l + 1];
                    }
                }
            }
            sprintf(nazov_suboru, "sala_%d_%s.txt", system.filmy[i].cislo_saly, cas_bez_dvojbodky);
            uloz_salu(&system.saly[i][j], nazov_suboru);
        }
    }
    printf("Program ukonceny. Vsetky zmeny ulozene.\n");
    return 0;
}