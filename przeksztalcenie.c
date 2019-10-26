#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MNOZNIK 3
#define DZIELNIK 2

/* Funkcja zwalnia zaalokowana pamiec. */
void zwolnij_pamiec(char **plansza, int zaalokowane_kolumny) {
    for (int j = 0; j < zaalokowane_kolumny; j++) {
        free(plansza[j]);
    }
    free(plansza);
}

/* Funkcja wypisuje informacje o bledzie na stderr oraz zwalnia zaalokowana pamiec. */
void fatal(char **plansza, int zaalokowane_kolumny) {
    fprintf(stderr, "Blad alokacji");
    zwolnij_pamiec(plansza, zaalokowane_kolumny);
    exit(-1);
}

/* Funckja wczytuje dane i w razie potrzeby powieksza plansze. */
char **wczytaj(int *wiersze, int *kolumny, int *zaalokowane_kolumny) {
    char **plansza = NULL;
    int i, j;
    int liczba_w = 0, liczba_k = 0;
    char c;

    plansza = (char **) malloc(sizeof(int *));
    if (plansza == NULL) {
        fatal(plansza, liczba_k);
    }
    plansza[0] = NULL;

    for (i = 0; (c = getchar()) != EOF; ++i) {
        if (i == liczba_w) {
            liczba_w = 1 + liczba_w * MNOZNIK / DZIELNIK;
            for (int k = 0; k < liczba_k; k++) {
                plansza[k] = (char *) realloc(plansza[k], liczba_w * sizeof(int));
                if (plansza[k] == NULL) {
                    fatal(plansza, liczba_k);
                }
                for (int x = i; x < liczba_w; x++) {
                    plansza[k][x] = ' ';
                }
            }
        }
        for (j = 0; c != '\n'; ++j) {
            if (j == liczba_k) {
                liczba_k = 1 + liczba_k * MNOZNIK / DZIELNIK;
                plansza = (char **) realloc(plansza, liczba_k * sizeof(int *));
                if (plansza == NULL) {
                    fatal(plansza, liczba_k);
                }
                for (int k = j; k < liczba_k; ++k) {
                    plansza[k] = (char *) malloc(liczba_w * sizeof(int));
                    if (plansza[k] == NULL) {
                        fatal(plansza, liczba_k);
                    }
                    memset(plansza[k], ' ', liczba_w);
                }
            }
            plansza[j][i] = c;
            c = getchar();
            *kolumny = (j > *kolumny) ? j : *kolumny;
        }
        *wiersze = (i > *wiersze) ? i : *wiersze;
    }
    *wiersze += 1;
    *kolumny += 1;
    *zaalokowane_kolumny = liczba_k;
    return plansza;
}

/* Funkcja zamienia ze soba dane dwie wartosci z planszy. */
void zamien_wartosci(char **plansza, int j, int wiersz1, int wiersz2) {
    char pom = plansza[j][wiersz1];
    plansza[j][wiersz1] = plansza[j][wiersz2];
    plansza[j][wiersz2] = pom;
}

/* Funkcja odpowiedzialna za "opadanie" klockow. */
void grawitacja(char **plansza, int wiersze, int kolumny) {
    for (int j = 0; j < kolumny; j++) {
        for (int i = wiersze - 2; i >= 0; i--) {
            int k = i;
            while ((k + 1 < wiersze) && (plansza[j][k + 1] == ' ')) {
                zamien_wartosci(plansza, j, k, k + 1);
                k++;
            }
        }
    }
}

/* Funkcja sprawdza czy dana kolumna jest pusta. */
bool czy_pusta_kolumna(char **plansza, int j, int wiersze) {
    for (int i = 0; i < wiersze; i++) {
        if (plansza[j][i] != ' ') {
            return false;
        }
    }
    return true;
}

/* Funkcja zamienia ze soba dane dwie kolumny z planszy. */
void zamien_kolumny(char **plansza, int kolumna1, int kolumna2) {
    char *pom = plansza[kolumna1];
    plansza[kolumna1] = plansza[kolumna2];
    plansza[kolumna2] = pom;
}

/* Funkcja przesuwa puste kolumny na koniec planszy. */
void usun_puste_kolumny(char **plansza, int wiersze, int kolumny) {
    for (int j = 1; j < kolumny; j++) {
        int k = j;
        while ((k - 1 >= 0) && (czy_pusta_kolumna(plansza, k - 1, wiersze))) {
            zamien_kolumny(plansza, k, k - 1);
            k--;
        }
    }
}

/* Funkcja usuwa cala grupe klockow o podanym znaku, zaczynajaca sie w podanym 
miejscu. Jesli sasiadujace pole znajduje sie w zakresie planszy oraz jest na nim
okreslony klocek, to funkcja wywoluje sie rekurencyjnie z wartosciami z tego pola. */
void usun_grupe(char **plansza, int i, int j, int wiersze, int kolumny, int znak) {
    plansza[j][i] = ' ';
    if ((j + 1 < kolumny) && (plansza[j + 1][i] == znak)) {
        usun_grupe(plansza, i, j + 1, wiersze, kolumny, znak);
    }
    if ((j - 1 >= 0) && (plansza[j - 1][i] == znak)) {
        usun_grupe(plansza, i, j - 1, wiersze, kolumny, znak);
    }
    if ((i - 1 >= 0) && (plansza[j][i - 1] == znak)) {
        usun_grupe(plansza, i - 1, j, wiersze, kolumny, znak);
    }
    if ((i + 1 < wiersze) && (plansza[j][i + 1] == znak)) {
        usun_grupe(plansza, i + 1, j, wiersze, kolumny, znak);
    }
}

/* Funkcja wypisuje plansze, pomijajac przy tym puste wiersze oraz spacje na koncu wierszy. */
void wypisz(char **plansza, int wiersze, int kolumny) {
    int k;
    for (int i = 0; i < wiersze; i++) {
        k = kolumny - 1;
        while (k >= 0 && plansza[k][i] == ' ') {
            k--;
        }
        if (k != -1) {
            for (int j = 0; j <= k; j++) {
                printf("%c", plansza[j][i]);
            }
            printf("\n");
        }
    }
}

/* Funkcja prowadzi rozgrywke. */
void rozgrywka(char **plansza, int wiersze, int kolumny) {
    if (plansza[0] != NULL) {
        grawitacja(plansza, wiersze, kolumny);
        usun_puste_kolumny(plansza, wiersze, kolumny);
        if (plansza[0][wiersze - 1] != ' ') {
            usun_grupe(plansza, wiersze - 1, 0, wiersze, kolumny, plansza[0][wiersze - 1]);
        }
        grawitacja(plansza, wiersze, kolumny);
        usun_puste_kolumny(plansza, wiersze, kolumny);
        wypisz(plansza, wiersze, kolumny);
    }
}

int main(void) {
    char **plansza;
    int zaalokowane_kolumny = 0; /* liczba zaalokowanych kolumn */
    int wiersze = 0, kolumny = 0; /* liczba wczytanych wierszy i liczba wczytanych kolumn */

    plansza = wczytaj(&wiersze, &kolumny, &zaalokowane_kolumny);

    rozgrywka(plansza, wiersze, kolumny);

    zwolnij_pamiec(plansza, zaalokowane_kolumny);

    return 0;
}
