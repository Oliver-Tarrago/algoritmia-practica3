/**
 * Nom:     Oriol
 * Cognoms: Benítez Bravo
 * NIU:     1566931
 *
 * Nom:     Oliver Einard
 * Cognoms: Tarragó Boardman
 * NIU:     1527541
 */

#include <stdio.h>
#include <stdlib.h>

typedef enum { SUCCESS, NO_FILE, EMPTY_FILE, NO_MEMORY } RETURN;

void flush() {
    for (int c = getchar(); c != EOF && c != '\n'; c = getchar());
}

typedef struct alu {
    int niu;
    float notes[5];
    struct alu *seg;
} Alu;

float mitjana(const float *dades, int n);

void imprimirllista(const Alu *llista_alumnes);

void llegir_alumne(FILE *des_de, Alu *alumne);

/**
 * Insereix un alumne a una llista ordenada d'alumnes mantenint un ordre creixent
 * @param alumne Alumne a inserir
 * @param llista_ordenada Llista d'alumnes on inserir
 * @return Un punter al primer element de la llista
 */
Alu * inserir(Alu *alumne, Alu *llista_ordenada);

/**
 * Versió d'inserir fent servir recursivitat. És més ràpid però pot haver un stackoverflow si la llista és massa
 * gran. (> 100k elements)
 *
 * @see Alu *inserir(Alu *alumne, Alu *llista_ordenada)
 *
 * @param alumne Alumne a inserir
 * @param llista_ordenada Llista d'alumnes on inserir
 * @return Un punter al primer element de la llista
 */
Alu *inserir_rec(Alu *alumne, Alu *llista_ordenada);

void demanar_alumne(Alu *alumne);

int menu();

int main() {
    int length = 0;

    FILE *fitxer_dades = fopen("Llista.txt", "r");
    if (fitxer_dades == NULL) {
        printf("\nNo s'ha accedit al fitxer de dades\n");
        return NO_FILE;
    }

    int niu;
    if (fscanf(fitxer_dades, "%i;", &niu) == EOF) {
        printf("\nFitxer buit\n");
        return EMPTY_FILE;
    }

    Alu *primer = (Alu *) malloc(sizeof(Alu));
    if (primer == NULL) {
        printf("Problema assignant espai de memoria\n");
        return NO_MEMORY;
    }
    ++length;
    primer->niu = niu;
    llegir_alumne(fitxer_dades, primer);
    primer->notes[4] = mitjana(primer->notes, 4);

    Alu *anterior;
    while (fscanf(fitxer_dades, "%i;", &niu) != EOF) {
        Alu *actual = (Alu *) malloc(sizeof(Alu));
        if (actual == NULL) {
            printf("Problema assignant espai de memoria\n");
            return NO_MEMORY;
        }

        length++;
        actual->niu = niu;
        llegir_alumne(fitxer_dades, actual);
        actual->notes[4] = mitjana(actual->notes, 4);

        if (actual->niu < primer->niu) {
            actual->seg = primer;
            primer = actual;
        } else {
            anterior = primer;
            while (anterior->seg != NULL && (anterior->seg)->niu < actual->niu) {
                anterior = anterior->seg;
            }
            actual->seg = anterior->seg;
            anterior->seg = actual;
        }
    }
    fclose(fitxer_dades);
    printf("\nS'ha llegit informacio de %d linies.\n\n", length);

    int opcio;
    while ((opcio = menu()) != 3) {
        if (opcio == 1) {
            Alu *alumne = (Alu *) malloc(sizeof(Alu));
            if (alumne == NULL) {
                printf("Problema assignant espai de memoria\n");
                return NO_MEMORY;
            }
            demanar_alumne(alumne);
            primer = inserir_rec(alumne, primer);
            ++length;
        } else if (opcio == 2) {
            imprimirllista(primer);
        }
    }

    return EXIT_SUCCESS;
}

int menu() {
    printf("\n1. Insertar alumne\n"
           "2. Llistar.\n"
           "3. Acabar.\n"
           "Introdueix una opció: ");
    int opcio = -1;
    while (opcio < 1 || opcio > 3) {
        scanf("%d", &opcio);
    }
    return opcio;
}

void llegir_alumne(FILE *des_de, Alu *alumne) {
    for (int i = 0; i < 4; i++) {
        fscanf(des_de, "%f", &alumne->notes[i]);
        fgetc(des_de);//llegeix i descarta els ; i el \n
    }
}

Alu * inserir(Alu *alumne, Alu *llista_ordenada) {
    Alu *actual = llista_ordenada, *anterior = NULL;
    while (actual->seg != NULL && actual->niu < alumne->niu) {
        anterior = actual;
        actual = actual->seg;
    }
    if (actual->niu < alumne->niu) { // actual->seg == NULL => actual is the last element
                                     // => alumne->niu is the biggest => alumne must be the last element
        actual->seg = alumne;
    } else { // actual->niu >= alumne->niu
        if (actual == llista_ordenada) { // if actual is the first element
            alumne->seg = llista_ordenada; // => alumne->niu is the lowest => alumne must be the first element
            llista_ordenada = alumne; // => alumne is the pointer to the first element now
        } else { // actual is not the first element => anterior != NULL
            anterior->seg = alumne; // actual->niu >= alumne->niu => anterior->seg = alumne
            alumne->seg = actual; // => alumne->seg = actual
        }
    }
    return llista_ordenada;
}

Alu *inserir_rec(Alu *alumne, Alu *llista_ordenada) {
    /** I a part de més ràpid, és més simple */
    if (alumne->niu < llista_ordenada->niu) {
        alumne->seg = llista_ordenada;
        return alumne;
    } else {
        llista_ordenada->seg = llista_ordenada->seg != NULL ? inserir_rec(alumne, llista_ordenada->seg) : alumne;
        return llista_ordenada;
    }
}

void demanar_alumne(Alu *alumne) {
    printf("\nIntrodueix separat per comes: niu, nota1, nota2, nota3, nota4: ");
    while (scanf("%d, %f, %f, %f, %f", &alumne->niu, &alumne->notes[0], &alumne->notes[1], &alumne->notes[2], &alumne->notes[3]) != 5) {
        printf("\nHeu d'entrar 5 valors, un enter (NIU) i quatre reals (notes): ");
        flush();
    }
    alumne->notes[4] = mitjana(alumne->notes, 4);
}

void imprimirllista(const Alu *llista_alumnes) {
    while (llista_alumnes != NULL) {
        printf("%d | ", llista_alumnes->niu);
        for (int i = 0; i < 4; i++) {
            printf("%5.1f", llista_alumnes->notes[i]);
        }
        printf(" |%6.2f", llista_alumnes->notes[4]);
        printf("\n");
        llista_alumnes = llista_alumnes->seg;
    }
}

float mitjana(const float *dades, int n) {
    float m = 0.f;
    for (int i = 0; i < n; i++) {
        m += dades[i];
    }

    return m / (float) n;
}
