#include <stdio.h>
#include "funciones.h"

int main (int argc, char *argv[]) {

    Bloque bloques[MAX_BLOQUES];
    Plan plan;
    int cont = 0, opc, opc2, indice;

    do
    {
        printf("\n========================================\n");
        printf("          FABRICA DE BLOQUES\n");
        printf("========================================\n");
        printf("  1. Registrar bloque\n");
        printf("  2. Mostrar bloques\n");
        printf("  3. Buscar bloque\n");
        printf("  4. Editar bloque\n");
        printf("  5. Eliminar bloque\n");
        printf("  6. Calcular produccion y verificar capacidad\n");
        printf("  7. Estudio de tiempos (quintales por periodo)\n");
        printf("----------------------------------------\n");
        printf("  Ingrese una opcion: ");

        opc = (int)validarFloatRango(1,7);
        switch (opc)
        {
        case 1:
            registrarBloque(bloques,&cont);
            break;
        case 2:
            mostrarBloques(bloques,0,cont);
            break;
        case 3:
            indice = buscarBloque(bloques,cont);
            if (indice == -1)
            {
                printf("Error: no existe el bloque\n");
            }
            else
            {
                mostrarBloques(bloques,indice,indice + 1);
            }
            break;
        case 4:
            editarBloque(bloques,cont);
            break;
        case 5:
            eliminarBloque(bloques,cont);
            break;
        case 6:
            if (calcularProduccion(bloques,cont,&plan))
            {
                verificarCapacidad(bloques,cont,&plan);
            }
            break;
        case 7:
            estudioTiempos(bloques,cont);
            break;
        }

        printf("\nDesea seleccionar otra opcion?\n");
        printf("  1. Si\n");
        printf("  2. No\n");
        printf("  Ingrese una opcion: ");
        opc2 = (int)validarFloatRango(1,2);
    } while (opc2 == 1);

    return 0;
}
