#include <stdio.h>
#include <string.h>
#include <psl1ght/lv2.h>
#include <io/pad.h>
#include <rsx/rsx.h>
#include <io/console.h>

#define MAX_MODOS 40
#define MAX_LINEA 512

char modos[MAX_MODOS][MAX_LINEA];
int total_modos = 0;
int seleccion = 0;

void cargar_datos() {
    FILE *f = fopen("/dev_hdd0/game/FUCHIBOL_V13/USRDIR/DATOS.txt", "r");
    if (!f) {
        // Si no lo encuentra en la PS3, prueba en USB
        f = fopen("/dev_usb000/FUCHIBOL_V13/DATOS.txt", "r");
    }
    if (!f) return;

    char linea[MAX_LINEA];
    int en_modos = 0;

    while (fgets(linea, sizeof(linea), f)) {
        // Cuando encuentra la sección de modos, empieza a guardar
        if (strstr(linea, "37 MODOS") || strstr(linea, "MODOS:")) {
            en_modos = 1;
            continue;
        }
        // Cuando termina la sección de modos, corta
        if (en_modos && (strstr(linea, "ESTADIOS") || strstr(linea, "ONLINE") || strstr(linea, "JUGADORES"))) {
            break;
        }
        if (en_modos) {
            // Solo guarda líneas que empiecen con número tipo "1. Amistoso"
            if (linea[0] >= '0' && linea[0] <= '9' && total_modos < MAX_MODOS) {
                strcpy(modos[total_modos], linea);
                total_modos++;
            }
        }
    }
    fclose(f);

    // Si no encontró nada, pone los modos a mano para que no quede vacío
    if (total_modos == 0) {
        strcpy(modos[0], "1. Amistoso");
        strcpy(modos[1], "2. Modo Street");
        strcpy(modos[2], "3. Modo Nieve");
        strcpy(modos[3], "4. Carrera");
        strcpy(modos[4], "5. Ultimate Team");
        strcpy(modos[5], "6. Liga Argentina");
        total_modos = 6;
    }
}

void dibujar_menu() {
    console_init();
    printf("\n\n");
    printf("  FUCHIBOL V3 - FINAL COMPLETO\n");
    printf("  Por Lauti - Temperley, Argentina\n");
    printf("  118 Equipos - 2360 Jugadores - 37 Modos\n");
    printf("  ----------------------------------\n\n");

    for (int i = 0; i < total_modos; i++) {
        if (i == seleccion) printf("  > %s", modos[i]);
        else printf("    %s", modos[i]);
    }
    printf("\n  Cruz = Elegir - Arriba/Abajo = Mover\n");
}

int main() {
    padInfo padinfo;
    padData paddata;

    cargar_datos();
    dibujar_menu();

    while (1) {
        ioPadGetInfo(&padinfo);
        if (padinfo.status[0]) {
            ioPadGetData(0, &paddata);
            if (paddata.BTN_UP) {
                seleccion--;
                if (seleccion < 0) seleccion = total_modos - 1;
                dibujar_menu();
            }
            if (paddata.BTN_DOWN) {
                seleccion++;
                if (seleccion >= total_modos) seleccion = 0;
                dibujar_menu();
            }
            if (paddata.BTN_CROSS) {
                console_init();
                printf("\n\n  Elegiste: %s\n", modos[seleccion]);
                printf("  Cargando partido...\n");
                // Acá después conectamos con tu DATOS.txt para cargar equipos
            }
        }
        sysUtilCheckCallback();
    }
    return 0;
}