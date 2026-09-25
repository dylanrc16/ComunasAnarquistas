struct Recurso *crearRecurso(char nombre, int existencia, int maximo){

    struct Recurso *nuevo = calloc(1, sizeof(Recurso));

    if (nuevo == NULL){
        return -1
    }

    nuevo->nombre = nombre;
    nuevo->existencia = existencia;
    nuevo->maximo = maximo;

    return nuevo;  
}

struct Recurso *agregarFinal(struct Recurso *inicio, char *nombre, int existencia, int maximo){

    struct Recurso *nueva = crearRecurso(nombre, existencia, maximo);

    if (inicio == NULL){
        inicio = nueva;
    }

    struct Recurso *act = inicio;

    while(act->siguiente != NULL){
        act = act->siguiente;
    }
    act->siguiente = nueva;

    return inicio;
}


