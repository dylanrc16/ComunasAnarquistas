struct Comuna *crearComuna(char *nombre, int cantidadPersonas){
    if(nombre == NULL || cantidadPersonas < 0){
        return -1;
    }
    struct Comuna *nueva = calloc(1, sizeof(Comuna));

    nueva->nombre = nombre;
    nueva->cantidadPersonas = cantidadPersonas;

    return nueva;
}

struct Comuna *agregarFinal(struct Comuna *inicio, char *nombre, int cantidadPersonas){

    struct Comuna *nueva = crearComuna(nombre, cantidadPersonas);

    if (inicio == NULL){
        inicio = nueva;
    }

    struct Comuna *act = inicio;

    while(act->siguiente != NULL){
        act = act->siguiente;
    }
    act->siguiente = nueva;

    return inicio;
}

void imprimir(struct Comuna *inicio) {

    struct Comuna *act = inicio;

    while (act != NULL) {

        printf("%d\n", act->nombre, act->cantidadPersonas);
        act = act->siguiente;
    }
}

struct Comuna *buscar(struct Comuna *inicio, char *nombre) {

    struct Comuna *act = inicio;
    while (act != NULL) {

        if (act->nombre == nombre) {
            return act;
        }
        act = act->siguiente;
    }
    return NULL;
}



