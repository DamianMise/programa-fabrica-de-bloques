#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "funciones.h"

#define NUM_UNIDADES 5

static const char *PERIODOS[] = {"Semanal", "Mensual", "Semestral", "Anual"};
static const int SEMANAS[] = {1, 4, 26, 52};
static const char *UNIDADES[] = {"carretillas", "porciones", "libras", "kg", "litros"};

float validarFloatRango(float a, float b){
    float num = 0;
    int val, c;
    do
    {
        val = scanf("%f",&num);
        if (val == EOF) exit(0);
        while ((c = getchar()) != '\n' && c != EOF);
        if (val != 1 || num < a || num > b)
        {
            printf("Error: el valor ingresado es incorrecto\n");
        }
    } while (val != 1 || num < a || num > b);
    return num;
}

void leerCadena(char *cadena, int n){
    if (fgets(cadena,n,stdin) == NULL)
    {
        cadena[0] = '\0';
        return;
    }
    int len = strlen(cadena);
    if (len > 0 && cadena[len - 1] == '\n')
    {
        cadena[len - 1] = '\0';
    }
    else
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

static int iguales(const char *a, const char *b){
    while (*a && *b)
    {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++;
        b++;
    }
    return *a == *b;
}

static int contiene(const char *texto, const char *busca){
    int n = strlen(busca), m = strlen(texto);
    if (n == 0) return 0;
    for (int i = 0; i + n <= m; i++)
    {
        int ok = 1;
        for (int j = 0; j < n; j++)
        {
            if (tolower((unsigned char)texto[i + j]) != tolower((unsigned char)busca[j]))
            {
                ok = 0;
                break;
            }
        }
        if (ok) return 1;
    }
    return 0;
}

static int contarActivos(Bloque *b, int cont){
    int n = 0;
    for (int i = 0; i < cont; i++)
    {
        if (b[i].estado != 3) n++;
    }
    return n;
}

static int seleccionarPeriodo(const char *titulo){
    printf("\n%s\n",titulo);
    for (int i = 0; i < 4; i++)
    {
        printf("  %d. %s\n",i + 1,PERIODOS[i]);
    }
    printf("  Ingrese una opcion: ");
    return (int)validarFloatRango(1,4) - 1;
}

static int seleccionarUnidad(void){
    printf("  Unidad de medida:\n");
    for (int i = 0; i < NUM_UNIDADES; i++)
    {
        printf("    %d. %s\n",i + 1,UNIDADES[i]);
    }
    printf("  Ingrese una opcion: ");
    return (int)validarFloatRango(1,NUM_UNIDADES) - 1;
}

static void registrarRecursos(Bloque *b){
    printf("Cuantos recursos distintos lleva 1 quintal (1-%d): ",MAX_RECURSOS);
    int n = (int)validarFloatRango(1,MAX_RECURSOS);
    for (int i = 0; i < n; i++)
    {
        printf("\n  --- Recurso %d ---\n",i + 1);
        printf("  Nombre (ej. chasqui, polvo, puzolana, cemento): ");
        leerCadena(b->recursos[i].nombre,50);
        b->recursos[i].unidad = seleccionarUnidad();
        printf("  Cantidad por quintal (en %s): ",UNIDADES[b->recursos[i].unidad]);
        b->recursos[i].cantidad = validarFloatRango(0.01,10000);
    }
    b->numRecursos = n;
}

void registrarBloque(Bloque *bloques, int *cont){
    int espacio = MAX_BLOQUES - *cont;
    if (espacio <= 0)
    {
        printf("Error: ya se registraron los %d tipos de bloque\n",MAX_BLOQUES);
        return;
    }
    printf("Cuantos tipos de bloque desea registrar (1-%d): ",espacio);
    int n = (int)validarFloatRango(1,espacio);

    for (int k = 0; k < n; k++)
    {
        Bloque *b = &bloques[*cont];
        printf("\n----- Bloque %d de %d -----\n",k + 1,n);
        b->id = *cont;
        printf("Nombre (ej. Bloque de 15, Bloque de 10): ");
        leerCadena(b->nombre,50);
        printf("Cantidad de bloques demandados: ");
        b->demanda = (int)validarFloatRango(0,1000000);
        b->periodo = seleccionarPeriodo("Periodo de la demanda:");
        printf("Bloques que salen de 1 quintal de mezcla: ");
        b->bloquesPorQuintal = (int)validarFloatRango(1,1000);
        printf("Estudio de tiempos: minutos que toma fabricar 1 quintal: ");
        b->minutosPorQuintal = validarFloatRango(0.1,10000);
        registrarRecursos(b);
        b->estado = 0;
        (*cont)++;
        printf("Bloque registrado\n");
    }
}


void mostrarBloques(Bloque *bloques, int inicio, int fin){
    int hay = 0;
    for (int i = inicio; i < fin; i++)
    {
        Bloque *b = &bloques[i];
        if (b->estado == 3) continue;
        hay = 1;

        float quintales = b->demanda / (float)b->bloquesPorQuintal;
        float horas = quintales * b->minutosPorQuintal / 60.0f;

        printf("\n========================================\n");
        printf("  %s\n",b->nombre);
        printf("========================================\n");
        printf("  Demanda: %d bloques (%s)\n",b->demanda,PERIODOS[b->periodo]);
        printf("  Rendimiento: %d bloques por quintal\n",b->bloquesPorQuintal);
        printf("  Tiempo: %.1f min por quintal (%.2f quintales/hora)\n",
               b->minutosPorQuintal,60.0f / b->minutosPorQuintal);
        printf("  Para cumplir la demanda se necesitan %.2f quintales y %.2f horas\n",
               quintales,horas);
        printf("  Recursos:\n");
        printf("    %-20s %-18s %s\n","Recurso","Por quintal","Para la demanda");
        for (int j = 0; j < b->numRecursos; j++)
        {
            Recurso *r = &b->recursos[j];
            printf("    %-20s %7.2f %-10s %7.2f %s\n",r->nombre,
                   r->cantidad,UNIDADES[r->unidad],
                   r->cantidad * quintales,UNIDADES[r->unidad]);
        }
    }
    if (!hay)
    {
        printf("No hay bloques registrados\n");
    }
}

int buscarBloque(Bloque *bloques, int cont){
    if (contarActivos(bloques,cont) == 0)
    {
        printf("No hay bloques registrados\n");
        return -1;
    }
    printf("\nComo desea buscar?\n");
    printf("  1. Elegir de la lista de bloques\n");
    printf("  2. Buscar por parte del nombre\n");
    printf("  Ingrese una opcion: ");
    int opc = (int)validarFloatRango(1,2);
    char texto[50];
    int lista[MAX_BLOQUES];
    int n = 0;

    if (opc == 2)
    {
        printf("  Escriba parte del nombre: ");
        leerCadena(texto,50);
    }
    for (int i = 0; i < cont; i++)
    {
        if (bloques[i].estado == 3) continue;
        if (opc == 2 && !contiene(bloques[i].nombre,texto)) continue;
        lista[n++] = i;
    }
    if (n == 0) return -1;
    if (n == 1 && opc == 2) return lista[0];

    printf("\nBloques:\n");
    for (int k = 0; k < n; k++)
    {
        printf("  %d. %s\n",k + 1,bloques[lista[k]].nombre);
    }
    printf("  Ingrese el numero del bloque: ");
    return lista[(int)validarFloatRango(1,n) - 1];
}


void editarBloque(Bloque *bloques, int cont){
    int idx = buscarBloque(bloques,cont);
    if (idx == -1)
    {
        printf("Error: no existe el bloque\n");
        return;
    }
    mostrarBloques(bloques,idx,idx + 1);
    printf("\nQue desea editar?\n");
    printf("  1. Nombre\n");
    printf("  2. Demanda y periodo\n");
    printf("  3. Bloques por quintal\n");
    printf("  4. Tiempo por quintal\n");
    printf("  5. Recursos\n");
    printf("  Ingrese una opcion: ");
    int opc = (int)validarFloatRango(1,5);
    Bloque *b = &bloques[idx];
    switch (opc)
    {
    case 1:
        printf("Nuevo nombre: ");
        leerCadena(b->nombre,50);
        break;
    case 2:
        printf("Nueva cantidad demandada: ");
        b->demanda = (int)validarFloatRango(0,1000000);
        b->periodo = seleccionarPeriodo("Periodo de la nueva demanda:");
        break;
    case 3:
        printf("Nuevos bloques por quintal: ");
        b->bloquesPorQuintal = (int)validarFloatRango(1,1000);
        break;
    case 4:
        printf("Nuevos minutos por quintal: ");
        b->minutosPorQuintal = validarFloatRango(0.1,10000);
        break;
    case 5:
        registrarRecursos(b);
        break;
    }
    printf("Bloque editado\n");
}

void eliminarBloque(Bloque *bloques, int cont){
    int idx = buscarBloque(bloques,cont);
    if (idx == -1)
    {
        printf("Error: no existe el bloque\n");
        return;
    }
    mostrarBloques(bloques,idx,idx + 1);
    printf("\nDesea borrar este bloque?\n  1. Si\n  2. No\n  Ingrese una opcion: ");
    if ((int)validarFloatRango(1,2) == 1)
    {
        bloques[idx].estado = 3;
        printf("Bloque borrado\n");
    }
}

void estudioTiempos(Bloque *bloques, int cont){
    int idx = buscarBloque(bloques,cont);
    if (idx == -1)
    {
        printf("Error: no existe el bloque\n");
        return;
    }
    Bloque *b = &bloques[idx];
    printf("Porcentaje de produccion real (1-100): ");
    float ef = validarFloatRango(1,100);

    int minutos[] = {15, 30, 60, 480, 540};
    const char *etiquetas[] = {"15 minutos", "30 minutos", "1 hora",
                               "Jornada de 8 h", "Jornada de 9 h"};

    printf("\n%s: %.1f min/quintal, eficiencia %.0f%%\n",b->nombre,b->minutosPorQuintal,ef);
    printf("%-16s %12s %12s\n","Tiempo","Quintales","Bloques");
    for (int i = 0; i < 5; i++)
    {
        float q = minutos[i] / b->minutosPorQuintal * ef / 100.0f;
        printf("%-16s %12.2f %12d\n",etiquetas[i],q,(int)(q * b->bloquesPorQuintal));
    }
}


int calcularProduccion(Bloque *bloques, int cont, Plan *p){
    if (contarActivos(bloques,cont) == 0)
    {
        printf("Error: no hay bloques registrados\n");
        return 0;
    }

    printf("\n============ CONDICIONES DEL PERIODO ============\n");
    p->periodo = seleccionarPeriodo("Periodo a planificar:");
    printf("Dias laborables por semana (1-7): ");
    int diasSemana = (int)validarFloatRango(1,7);
    printf("Horas por jornada (1-24): ");
    float horasJornada = validarFloatRango(1,24);
    int diasTotales = diasSemana * SEMANAS[p->periodo];
    printf("Dias sin trabajar en el periodo por lluvia u otras causas (0-%d): ",diasTotales);
    int diasPerdidos = (int)validarFloatRango(0,diasTotales);
    printf("Porcentaje de produccion real de la jornada (1-100): ");
    float ef = validarFloatRango(1,100);

    p->horasDisponibles = (diasTotales - diasPerdidos) * horasJornada * ef / 100.0f;
    p->horasPerdidas = diasPerdidos * horasJornada * ef / 100.0f;

    printf("\n============== DEMANDA Y PEDIDOS ==============\n");
    p->horasNecesarias = 0;
    for (int i = 0; i < cont; i++)
    {
        p->planificados[i] = 0;
        if (bloques[i].estado == 3) continue;
        Bloque *b = &bloques[i];
        int base = (int)(b->demanda * (float)SEMANAS[p->periodo] / SEMANAS[b->periodo] + 0.5f);
        printf("%s: demanda equivalente al periodo %s = %d bloques\n",
               b->nombre,PERIODOS[p->periodo],base);
        printf("  Pedidos adicionales confirmados (bloques, 0 si no hay): ");
        int extra = (int)validarFloatRango(0,1000000);
        p->planificados[i] = base + extra;
        p->horasNecesarias += p->planificados[i] / (float)b->bloquesPorQuintal
                              * b->minutosPorQuintal / 60.0f;
    }

    float fTiempo = 1, fSinPerdidas = 1;
    if (p->horasNecesarias > 0)
    {
        fTiempo = p->horasDisponibles / p->horasNecesarias;
        fSinPerdidas = (p->horasDisponibles + p->horasPerdidas) / p->horasNecesarias;
        if (fTiempo > 1) fTiempo = 1;
        if (fSinPerdidas > 1) fSinPerdidas = 1;
    }

    printf("\n===== PRODUCCION PLANIFICADA VS. REAL (%s) =====\n",PERIODOS[p->periodo]);
    printf("Horas necesarias: %.2f | disponibles: %.2f | perdidas por clima: %.2f\n",
           p->horasNecesarias,p->horasDisponibles,p->horasPerdidas);
    printf("%-18s %10s %10s %12s %10s\n","Bloque","Planif.","Alcanza","Clima","Jornada");
    for (int i = 0; i < cont; i++)
    {
        p->porTiempo[i] = 0;
        p->perdidoClima[i] = 0;
        if (bloques[i].estado == 3) continue;
        int sinPerdidas = (int)(p->planificados[i] * fSinPerdidas);
        p->porTiempo[i] = (int)(p->planificados[i] * fTiempo);
        p->perdidoClima[i] = sinPerdidas - p->porTiempo[i];
        printf("%-18s %10d %10d %12d %10d\n",bloques[i].nombre,
               p->planificados[i],p->porTiempo[i],p->perdidoClima[i],
               p->planificados[i] - sinPerdidas);
    }
    printf("(Clima = bloques no fabricados por dias perdidos; "
           "Jornada = no alcanzan las horas aun sin perdidas)\n");
    return 1;
}

void verificarCapacidad(Bloque *bloques, int cont, Plan *p){
    char nom[MAX_BLOQUES * MAX_RECURSOS][50];
    int uni[MAX_BLOQUES * MAX_RECURSOS];
    float nec[MAX_BLOQUES * MAX_RECURSOS];
    float disp[MAX_BLOQUES * MAX_RECURSOS];
    int nr = 0;

    for (int i = 0; i < cont; i++)
    {
        if (bloques[i].estado == 3) continue;
        float q = p->porTiempo[i] / (float)bloques[i].bloquesPorQuintal;
        for (int j = 0; j < bloques[i].numRecursos; j++)
        {
            Recurso *r = &bloques[i].recursos[j];
            int k;
            for (k = 0; k < nr; k++)
            {
                if (iguales(nom[k],r->nombre) && uni[k] == r->unidad) break;
            }
            if (k == nr)
            {
                strcpy(nom[k],r->nombre);
                uni[k] = r->unidad;
                nec[k] = 0;
                nr++;
            }
            nec[k] += q * r->cantidad;
        }
    }

    printf("\n============ VERIFICACION DE RECURSOS ============\n");
    float factor = 1;
    int limitante = -1;
    for (int k = 0; k < nr; k++)
    {
        printf("%s: se necesitan %.2f %s. Disponible: ",nom[k],nec[k],UNIDADES[uni[k]]);
        disp[k] = validarFloatRango(0,1000000);
        if (nec[k] > 0 && disp[k] / nec[k] < factor)
        {
            factor = disp[k] / nec[k];
            limitante = k;
        }
    }

    printf("\n============ RESULTADO FINAL (%s) ============\n",PERIODOS[p->periodo]);
    printf("%-18s %10s %10s %10s %10s\n","Bloque","Planif.","Fabricado","Pendiente","Quintales");
    int totPlan = 0, totFab = 0, totClima = 0, totJornada = 0, totRec = 0;
    for (int i = 0; i < cont; i++)
    {
        if (bloques[i].estado == 3) continue;
        int fab = (int)(p->porTiempo[i] * factor);
        int pend = p->planificados[i] - fab;
        printf("%-18s %10d %10d %10d %10.2f\n",bloques[i].nombre,p->planificados[i],
               fab,pend,fab / (float)bloques[i].bloquesPorQuintal);
        totPlan += p->planificados[i];
        totFab += fab;
        totClima += p->perdidoClima[i];
        totRec += p->porTiempo[i] - fab;
        totJornada += p->planificados[i] - p->porTiempo[i] - p->perdidoClima[i];
    }

    int pendiente = totPlan - totFab;
    printf("\nBloques planificados: %d | fabricados: %d | pendientes: %d\n",
           totPlan,totFab,pendiente);

    if (pendiente == 0)
    {
        printf("La fabrica SI puede cumplir con la demanda del periodo\n");
        return;
    }

    printf("La fabrica NO puede cumplir con la demanda del periodo\n");
    printf("Demanda pendiente (se arrastra al siguiente periodo): %d bloques\n",pendiente);
    printf("Motivo de los bloques no fabricados:\n");
    if (totClima > 0)
        printf("  - Dias perdidos por lluvia/otros: %d bloques (%.2f horas perdidas)\n",
               totClima,p->horasPerdidas);
    if (totJornada > 0)
        printf("  - Horas de jornada insuficientes: %d bloques (faltan %.2f horas)\n",
               totJornada,p->horasNecesarias - p->horasDisponibles - p->horasPerdidas > 0
               ? p->horasNecesarias - p->horasDisponibles - p->horasPerdidas : 0.0f);
    if (totRec > 0 && limitante != -1)
    {
        printf("  - Falta de recursos: %d bloques\n",totRec);
        for (int k = 0; k < nr; k++)
        {
            if (nec[k] > disp[k])
            {
                printf("      Faltan %.2f %s de %s\n",nec[k] - disp[k],UNIDADES[uni[k]],nom[k]);
            }
        }
    }
}
