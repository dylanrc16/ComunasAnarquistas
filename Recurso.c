struct Recurso *crearRecurso(char nombre, int existencia, int maximo){

    struct Recurso *nuevo = calloc(1, sizeof(Recurso));

    if (nuevo == NULL){
        return NULL;
    }

    snprintf(nuevo->nombre, sizeof(nuevo->nombre), "%s", nombre);
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

struct Recurso *buscarRecurso(struct Recurso *inicio, char *nombre) {
    struct Recurso *actual = inicio;

    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }

        actual = actual->siguiente;
    }

    return NULL;
}

void imprimirRecursos(struct Recurso *inicio) {
    struct Recurso *actual = inicio;

    while (actual != NULL) {
        printf("%s: %d/%d\n",
               actual->nombre,
               actual->existencia,
               actual->maximo);

        actual = actual->siguiente;
    }
}

void liberarRecursos(struct Recurso *inicio) {
    struct Recurso *actual = inicio;

    while (actual != NULL) {
        struct Recurso *siguiente = actual->siguiente;
        free(actual);
        actual = siguiente;
    }
}

