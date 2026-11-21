#ifndef FUNCIONES_H
#define FUNCIONES_H

#define MAX_BLOQUES  10
#define MAX_RECURSOS 8

typedef struct
{
    char nombre[50];
    float cantidad;     
    int unidad;         
} Recurso;

typedef struct
{
    int id;
    char nombre[50];
    int demanda;               
    int periodo;              
    int bloquesPorQuintal;     
    float minutosPorQuintal;  
    int numRecursos;
    Recurso recursos[MAX_RECURSOS];
    int estado;                
} Bloque;


typedef struct
{
    int periodo;
    float horasNecesarias;
    float horasDisponibles;
    float horasPerdidas;               
    int planificados[MAX_BLOQUES];     
    int porTiempo[MAX_BLOQUES];        
    int perdidoClima[MAX_BLOQUES];     
} Plan;

float validarFloatRango(float a, float b);
void leerCadena(char *cadena, int n);

void registrarBloque(Bloque *bloques, int *cont);
void mostrarBloques(Bloque *bloques, int inicio, int fin);
int buscarBloque(Bloque *bloques, int cont);
void editarBloque(Bloque *bloques, int cont);
void eliminarBloque(Bloque *bloques, int cont);
void estudioTiempos(Bloque *bloques, int cont);
int calcularProduccion(Bloque *bloques, int cont, Plan *plan);
void verificarCapacidad(Bloque *bloques, int cont, Plan *plan);

#endif
