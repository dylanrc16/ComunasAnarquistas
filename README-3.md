# Bibliotecas anarquistas

Proyecto 2 de Estructuras de Datos. Extiende el Proyecto 1, *Una sociedad solarpunk de comunas anarquistas*: una simulación por turnos (cada turno es un día) de una sociedad de 500 personas organizada en comunas que se ayudan entre sí **sin dinero y sin mercado**. El éxito de la sociedad se mide con la **satisfacción** de sus habitantes.

En este proyecto las personas saben **servicios**, las comunas pueden **disolverse**, la persona usuaria administra **3 comunas** y cada comuna tiene una **biblioteca** donde su gente aprende servicios leyendo libros. La biblioteca usa un **AVL** para buscar libros por nombre y un **árbol splay** como caché de búsquedas de texto. Las personas se guardan en una **tabla hash**.

**Integrantes:**

Dylan Rodríguez
Duvan Asenjo

---

## Contenido

1. [Motivación](#motivación)
2. [Cómo compilar y ejecutar](#cómo-compilar-y-ejecutar)
3. [Cómo se juega](#cómo-se-juega)
4. [Organización del código](#organización-del-código)
5. [Pruebas](#pruebas)
6. [Estructuras de datos](#estructuras-de-datos)
7. [La biblioteca](#la-biblioteca)
8. [Un día de simulación](#un-día-de-simulación)
9. [Modelo de la sociedad](#modelo-de-la-sociedad)
10. [Índices](#índices)
11. [Disolución de comunas](#disolución-de-comunas)
12. [Emergencias](#emergencias)
13. [La economía: regalo y ayuda mutua](#la-economía-regalo-y-ayuda-mutua)
14. [Parámetros de diseño](#parámetros-de-diseño)

---

## Motivación

Imaginar otros mundos posibles es parte de la labor de la ingeniería. Este proyecto plantea una sociedad donde las comunas intercambian bienes y servicios sin que medie el dinero. Para eso necesitamos modelar qué le falta a cada comuna, cómo se siente su gente y de qué manera las comunas pueden ayudarse sin crear una deuda ni un sustituto de la moneda.

En el Proyecto 2 el conocimiento también circula sin dinero: cada comuna tiene una biblioteca pública y cualquier persona puede aprender un servicio (atención médica, reparación, agricultura...) leyendo libros, sin pagar matrícula ni pedir permiso.

## Cómo compilar y ejecutar

Todo se compila desde la carpeta del proyecto, donde están también los archivos de datos (`personas`, `bienes`, `servicios`, `comunas`). Los archivos de datos se leen desde el directorio actual, así que el programa debe ejecutarse desde esa carpeta.

En Linux:

```
gcc -Wall -Wextra -o proyecto Main.c Juego.c Comunas.c Consumo.c Economia.c Emergencias.c Indices.c Persona.c Produccion.c Recurso.c Sociedad.c Avl.c Splay.c Biblioteca.c TablaPersonas.c Jugador.c Celular.c
./proyecto
```

En Windows es el mismo comando; el ejecutable queda como `proyecto.exe` y se corre con `.\proyecto.exe`.

Dos cuidados:

- **No usar `*.c`**: incluiría también `pruebas.c`, que tiene su propio `main`.
- **El ejecutable no puede llamarse `comunas`**: así se llama el archivo de datos de las comunas y lo sobrescribiría.

### Los libros (el corpus)

Al iniciar, el programa pregunta la carpeta de la biblioteca:

```
Carpeta de la biblioteca (Enter para usar "corpus"):
```

- Si se presiona **Enter**, usa la carpeta `corpus`, dentro del proyecto. Ahí va el corpus real de libros que entrega el profesor. Está en el `.gitignore` porque pesa varios gigas y no se sube al repositorio ni al `.tar.gz`.
- También se puede escribir cualquier otra ruta, por ejemplo `corpus_prueba` o `/home/usuario/libros`.
- Se cargan los archivos `.txt` de esa carpeta. Si no hay ninguno, vuelve a preguntar.

Para probar sin el corpus real, el proyecto incluye `corpus_prueba`, con 5 libros cortos.

## Cómo se juega

Al iniciar, el programa carga los archivos de datos y la biblioteca, y pide:

1. La carpeta de la biblioteca (ver arriba).
2. La cantidad de comunas (al menos 3, porque la persona usuaria administra 3), de bienes y de servicios.
3. **Las 3 comunas que va a administrar.** En el estado del día aparecen marcadas con `*`.

Cada día se muestra el estado de las comunas (personas, necesidad, satisfacción y reciprocidad), las vidas que quedan y la **satisfacción promedio de la sociedad**. Luego aparece este menú:

```
1. Hacer un regalo entre comunas
2. Ver los recursos de una comuna
3. Ver el estado de las comunas
4. Pasar al siguiente dia
5. Buscar un libro por nombre (AVL)
6. Buscar un texto en los libros (cache splay)
7. Ver la cache de busquedas
8. Ver las personas de una comuna
9. Buscar una persona por nombre (tabla hash)
10. Ir a la biblioteca (aprender un servicio)
11. Pasar varios dias
12. Abrir el celular de una de sus comunas
0. Salir
```

- **Regalar (1).** Solo desde una de sus comunas. Un bien se regala por cantidad; un servicio se regala **mudando a una persona** que lo sabe a la otra comuna.
- **Biblioteca (5, 6, 7 y 10).** La opción 5 busca un libro por su nombre de archivo en el AVL. La 6 busca un texto usando la caché splay de una comuna y la 7 dibuja esa caché. En la 10 una persona elige un servicio para estudiar y busca textos; cada búsqueda con resultados le hace leer un libro nuevo, y al leer 3 aprende el servicio.
- **Personas (8 y 9).** La 8 lista las personas de una comuna con sus servicios. La 9 busca a una persona por nombre en la tabla hash y dice en qué casilla está.
- **Celular (12).** Abre el celular solar de una de sus comunas (la funcionalidad propia, explicada más abajo).

Donde hay que elegir una comuna, un recurso o una persona, se elige **por número** de una lista. La única excepción es la opción 9, que pide escribir el nombre, porque su propósito es mostrar la búsqueda en la tabla hash; si se presiona Enter sin escribir nada, primero muestra la lista de una comuna.

**Cómo se pierde.** Una comuna en crisis por 3 días seguidos se disuelve. Si es una de las 3 comunas de la persona usuaria, pierde una vida y elige otra comuna para reemplazarla. La partida se pierde al perder **3 vidas** o si en la sociedad quedan **menos de 3 comunas**.

## Organización del código

**Estructuras y carga de datos**

| Archivo | Contenido |
|---|---|
| `Persona.c/.h` | Persona: nombre, comuna, lista de servicios que sabe, servicio que estudia y libros leídos. |
| `TablaPersonas.c/.h` | Tabla hash de las 500 personas (función djb2, encadenamiento). |
| `Recurso.c/.h` | Lista simple de recursos (`nombre`, `existencia`, `maximo`). Sirve para bienes y servicios. |
| `Comunas.c/.h` | Lista doble de comunas. Cada comuna tiene su biblioteca. |
| `Avl.c/.h` | Árbol AVL de nombres de libros, con sus 4 rotaciones. |
| `Splay.c/.h` | Árbol splay de búsquedas de texto (la caché), con zig, zig-zig y zig-zag. |
| `Biblioteca.c/.h` | Catálogo de libros (AVL), búsqueda de texto en los archivos y uso de la caché. |
| `Jugador.c/.h` | Lista simple de las comunas que administra la persona usuaria y sus vidas. |

**Simulación**

| Archivo | Contenido |
|---|---|
| `Sociedad.c/.h` | Crea la sociedad, reparte las personas y sus servicios, y disuelve comunas. |
| `Consumo.c/.h` | Cada día baja la existencia de los bienes. |
| `Produccion.c/.h` | Cada día sube la existencia de los bienes. |
| `Emergencias.c/.h` | Emergencias aleatorias que reducen recursos y afectan a las comunas cercanas. |
| `Economia.c/.h` | Regalos de bienes y servicios, y la ayuda mutua autónoma entre comunas. |
| `Indices.c/.h` | Índices de necesidad, satisfacción y reciprocidad. |

**Programa**

| Archivo | Contenido |
|---|---|
| `Juego.c/.h` | Ciclo de días, menús y entrada de datos del usuario. |
| `Celular.c/.h` | El celular solar de cada comuna (funcionalidad propia). |
| `Main.c` | Solo llama a `iniciarJuego()`. |
| `pruebas.c` | Programa aparte que prueba las estructuras de datos. |

## Pruebas

`pruebas.c` es un programa aparte, con su propio `main`, que verifica cada estructura de datos. Cada prueba imprime `[OK]` o `[FALLO]`, y al final dice si todas pasaron.

```
gcc -Wall -Wextra -o pruebas_bin pruebas.c Persona.c Recurso.c Comunas.c Sociedad.c Avl.c Splay.c TablaPersonas.c Jugador.c
./pruebas_bin
```

| Sección | Qué verifica |
|---|---|
| Lista simple: Persona y Recurso | Orden, enlaces, búsquedas, validación de datos y carga desde archivos. |
| Lista doble: Comunas | Enlaces `siguiente` y `anterior`, recorrido hacia atrás y búsquedas. |
| Sociedad | Que las comunas sumen exactamente 500 personas. |
| AVL | Que 1000 nombres insertados en orden queden balanceados (altura baja), que no se repitan nombres y que las búsquedas funcionen. |
| Splay | El ejemplo hecho en papel: buscar `A` deja el árbol `A(.,F(B(.,D(C,E)),G))`. También aciertos y fallos de la caché. |
| Tabla hash | Función hash, insertar, buscar, nombres repetidos (`Ana Rojas 2`) y colisiones resueltas por encadenamiento. |
| Jugador | Máximo 3 comunas, sin repetir, quitar una (sin liberar la comuna) y agregar un reemplazo. |


## Estructuras de datos

| Estructura | Dónde se usa | Por qué |
|---|---|---|
| **Lista simple** | Recursos de cada comuna, servicios de cada persona, libros leídos, resultados de una búsqueda y comunas del jugador | Solo se recorren de principio a fin, así que basta un puntero `siguiente`. |
| **Lista doble** | Comunas de la sociedad | Para conocer a las vecinas hay que moverse hacia adelante **y hacia atrás** (2 anteriores y 2 siguientes). |
| **Tabla hash** | Las 500 personas | Encontrar a una persona por nombre sin recorrer a las 500. |
| **AVL** | Catálogo de libros (nombres de archivo) | Buscar un libro por nombre en O(log n), aunque haya miles. |
| **Árbol splay** | Caché de búsquedas de texto de cada comuna | Las búsquedas que se repiten quedan cerca de la raíz y no hay que volver a leer los libros. |

### Lista doble de comunas y disolución

Cada comuna apunta a la `anterior` y a la `siguiente`. Cuando una comuna se disuelve, `disolverComuna` (`Sociedad.c`):

1. Muda a sus personas, en ronda, a las comunas vecinas.
2. La saca de la lista reenganchando a sus vecinas: `anterior->siguiente = siguiente` y `siguiente->anterior = anterior`. Si era la primera, la cabeza pasa a ser la siguiente; por eso la función devuelve la nueva cabeza.
3. Libera la comuna: sus bienes, sus servicios y su caché.

### Tabla hash de personas (`TablaPersonas.c`)

- **Tamaño:** 509 casillas, un número primo cercano a las 500 personas, para que en promedio quede una persona por casilla.
- **Función hash djb2:** empieza en 5381 y, por cada letra del nombre, multiplica por 33 y suma la letra. Al final se toma el residuo entre 509.
- **Colisiones por encadenamiento:** cada casilla tiene una lista simple de personas. La persona nueva se inserta al inicio de la lista de su casilla, así que insertar es O(1).
- **Nombres repetidos:** el archivo `personas` tiene 96 nombres repetidos. Para que cada nombre sea único, el segundo se guarda como `Nombre 2`, el tercero como `Nombre 3`, y así.
- **Con el archivo real:** las 500 personas usan 308 casillas, 192 personas cayeron en una casilla ocupada y la lista más larga tiene 6. Esas estadísticas se imprimen al iniciar.

Buscar a una persona cuesta O(1) en promedio: se calcula su casilla y se recorre solo esa lista corta.

### AVL de libros (`Avl.c`)

Es un árbol binario de búsqueda ordenado por nombre de archivo, que se mantiene balanceado: en cada nodo, la altura del lado izquierdo y la del derecho difieren a lo sumo en 1. Al insertar, cuando un nodo queda con factor de balance 2 o −2 se aplica una de 4 rotaciones:

| Caso | Se arregla con |
|---|---|
| Izquierda-izquierda | Una rotación a la derecha |
| Derecha-derecha | Una rotación a la izquierda |
| Izquierda-derecha | Rotación a la izquierda en el hijo, luego a la derecha en el nodo |
| Derecha-izquierda | Rotación a la derecha en el hijo, luego a la izquierda en el nodo |

Los diagramas de cada rotación están en los comentarios de `Avl.c`. Si los nombres llegan ordenados (por ejemplo `libro0001.txt`, `libro0002.txt`...), un árbol sin balanceo se volvería una lista de 1000 niveles; con el AVL, 1000 libros quedan con altura de a lo sumo 14 (un árbol perfecto necesitaría 10).

### Árbol splay: la caché (`Splay.c`)

Cada nodo guarda un texto buscado y la lista de libros donde aparece. Al buscar un texto se **chaflanea**: el nodo buscado (o el último del camino, si no está) sube hasta la raíz. Se hace de abajo hacia arriba, igual que en papel:

1. Se baja desde la raíz hasta el texto y se guarda el camino en un arreglo.
2. Desde ese nodo se sube de dos en dos niveles:
   - **Zig:** el padre es la raíz. Una rotación.
   - **Zig-zig:** el nodo y su padre están del mismo lado. Se rota **primero el abuelo** y luego el padre.
   - **Zig-zag:** el nodo y su padre están en lados distintos. Se rota el padre y luego el abuelo.

**Ejemplo (el mismo que verifica `pruebas.c`):** buscar `A` en este árbol. El camino es F → D → B → A.

```
Antes:

        F
       / \
      D   G
     / \
    B   E
   / \
  A   C

Zig-zig: A y su padre B son hijos izquierdos.
Se rota primero el abuelo D y luego B:

        F
       / \
      A   G
       \
        B
         \
          D
         / \
        C   E

Zig: el padre de A es la raiz F. Una rotacion:

    A
     \
      F
     / \
    B   G
     \
      D
     / \
    C   E
```

El resultado es `A(.,F(B(.,D(C,E)),G))`. Las opciones 6 y 10 imprimen cada zig, zig-zig o zig-zag que se hace.

## La biblioteca

Hay **un solo catálogo** (el AVL con los nombres de los libros), compartido por toda la sociedad, y **una caché por comuna** (un splay en `struct Biblioteca`, dentro de cada comuna). Cada comuna tiene su propia caché porque cada una busca cosas distintas.

**Buscar un texto** (`buscarTexto`, en `Biblioteca.c`):

1. El texto se pasa a minúsculas, así "Amor" y "amor" son la misma búsqueda.
2. Se busca en la caché de la comuna. Si está, es un **acierto**: el resultado ya está en la raíz y no se lee ningún archivo.
3. Si no está, es un **fallo**: se abre cada libro y se lee línea por línea, en minúsculas. En cuanto un libro tiene el texto, se anota y se pasa al siguiente libro, sin leerlo completo.
4. El resultado se guarda en la caché con `insertarSplay`, y el texto nuevo queda en la raíz.

**Aprender un servicio:** una persona elige un servicio para estudiar (opción 10). Cada búsqueda con resultados le hace leer un libro de la lista que todavía no había leído. Al leer **3 libros** aprende el servicio, y la comuna lo tiene en cuenta desde ese momento. Un libro ya leído no vuelve a contar.

## Un día de simulación

Cada día (`simularDia`, en `Juego.c`) ocurre en este orden:

1. `consumirRecursos`: las personas gastan bienes.
2. `producirRecursos`: las comunas producen bienes.
3. `cargarCelulares`: el sol carga la batería del celular de cada comuna.
4. `decaerEfectos`: la penalización, la solidaridad y el apoyo recibido bajan 2 puntos.
5. `recalcularServicios`: los servicios se vuelven a contar a partir de lo que saben las personas.
6. `ayudaAutonoma`: las comunas que no administra la persona usuaria se ayudan solas.
7. `avanzarEmergencias`: si toca, ocurre una emergencia.
8. `actualizarIndices`: se recalculan necesidad, reciprocidad y satisfacción.
9. `revisarDisoluciones`: se cuentan los días en crisis y se disuelven las comunas que llevan 3.

Después se muestra el estado y la persona usuaria decide qué hacer. Después de cada regalo o de aprender un servicio, los índices se recalculan.

## Modelo de la sociedad

**Población.** Son 500 personas, cargadas en la tabla hash y repartidas en ronda entre las comunas elegidas, así que siempre suman 500. Cada persona guarda un puntero a su comuna, así que mudarse es solo cambiar ese puntero.

**Bienes.** Cada comuna tiene los bienes elegidos, con un máximo de **2 por persona** y una existencia inicial entre 40% y 80% del máximo. Cada día se consumen y se producen:

- Cada comuna tiene su propio porcentaje de consumo y de producción, sorteados al iniciar entre 2.00% y 5.00%.
- Cada día, cada bien baja `personas × consumo / 100` y sube `personas × producción / 100`, sin pasar de su máximo.
- `redondearAlAzar` (en `Recurso.c`) usa la parte decimal como probabilidad de redondear hacia arriba: un gasto de 2.3 resta 3 con 30% de probabilidad y 2 con 70%. Así, en promedio, cada comuna gasta exactamente lo que le corresponde.

**Servicios: los saben las personas.** En el Proyecto 2 un servicio no es un número que sube y baja, sino **cuántas personas de la comuna lo saben hacer**:

- Al iniciar, cada persona recibe servicios al azar: 30% no sabe ninguno, 50% sabe uno y 20% sabe dos.
- El máximo de cada servicio es **1 por cada 10 personas** (lo que la comuna necesita). La existencia es cuántas personas lo saben, con tope en ese máximo: tener 3 doctoras no compensa que falte transporte.
- Por eso los servicios no se consumen ni se producen. Cambian cuando alguien se muda, cuando alguien aprende uno en la biblioteca o cuando una emergencia los interrumpe ese día.

## Índices

### Necesidad

Mide cuánto falta para llegar al máximo, sumando todos los bienes y servicios de la comuna:

```
necesidad = 100 × (suma de lo que falta) / (suma de los máximos)
```

Se limita al rango **5 a 95**: nunca llega a 100 (siempre hay necesidades adicionales) ni a 0. **La necesidad no cambió respecto al Proyecto 1.**

### Reciprocidad

Mide si una comuna da y recibe en equilibrio a lo largo del tiempo. No es una deuda: nadie le debe nada a nadie, pero una comuna que solo recibe o solo da está menos integrada.

```
balance       = 1 − |dados − recibidos| / (dados + recibidos)
participacion = (dados + recibidos) / 6, con tope de 1
reciprocidad  = 50 + 50 × participacion × (2 × balance − 1)
```

- Sin intercambios vale **50** (neutral), para no castigar a nadie al inicio.
- Un regalo aislado casi no la mueve; lo que cuenta es el patrón. Con 6 o más intercambios, una comuna equilibrada llega a 100 y una que solo da o solo recibe baja a 0.

### Satisfacción

Es el índice que cambia en el Proyecto 2:

```
satisfaccion = 0.4 × (100 − necesidad propia)
             + 0.2 × promedio de (100 − necesidad) de las vecinas
             + 0.2 × promedio de la satisfacción anterior de las vecinas
             + 0.2 × reciprocidad
             + apoyo recibido + solidaridad − penalizacion
```

Se limita al rango 0 a 100.

- **Vecinas:** las 2 comunas anteriores y las 2 siguientes en la lista doble.
- **Satisfacción de las vecinas:** se usa la del cálculo anterior. Si no, la de A dependería de la de B y la de B de la de A, en círculo. `actualizarIndices` guarda primero una copia de todas las satisfacciones y después calcula las nuevas, así el orden de la lista no cambia el resultado. Al iniciar se llama 2 veces para que los valores se asienten.
- **Solidaridad:** +5 para quien regala. **Apoyo recibido:** +5 para quien recibe. Ambos tienen tope de 15.
- **Penalización:** +10 cuando una comuna vecina sufre una emergencia.
- Solidaridad, apoyo y penalización bajan 2 puntos por día, así que sus efectos se apagan con el tiempo.

**La sociedad se evalúa con el promedio de satisfacción de todas las comunas**, que se muestra cada día.

## Disolución de comunas

Una comuna está **en crisis** si su necesidad es **85 o más** o su satisfacción es **25 o menos**. Si pasa **3 días seguidos** en crisis, se disuelve:

1. Sus personas se mudan en ronda a las comunas vecinas (y se llevan sus servicios y lo que han estudiado).
2. Se saca de la lista doble y se libera.
3. Si era de la persona usuaria, se saca de su lista y pierde una vida. Si todavía le quedan vidas, elige otra comuna para administrar.

Si sale de la crisis antes de los 3 días, la cuenta vuelve a empezar. La partida se pierde con **3 vidas perdidas** o si en la sociedad quedan **menos de 3 comunas**.

## Emergencias

Cada **3 a 7 días** (sorteado cada vez) ocurre una emergencia:

1. Se elige una comuna al azar.
2. Cada uno de sus bienes y servicios tiene **50%** de probabilidad de ser afectado, y si lo es pierde entre **10% y 50%** de su existencia. Todo es aleatorio: la comuna, cuáles recursos y cuánto.
   Los bienes perdidos hay que volver a producirlos. Los servicios, en cambio, solo se **interrumpen ese día**: al día siguiente `recalcularServicios` los vuelve a contar a partir de las personas, porque la gente no olvida lo que sabe.
3. Las 2 comunas anteriores y las 2 siguientes reciben **+10 de penalización**, lo que baja su satisfacción.

## La economía: regalo y ayuda mutua

### En la vida real

- **Economía del regalo.** El antropólogo Marcel Mauss la estudió en su *Ensayo sobre el don* (1925). En muchas sociedades las cosas circulan como regalos y no como mercancías: dar crea vínculos y prestigio social, y no una deuda monetaria. Ejemplos clásicos son el potlatch de pueblos de la costa noroeste de Norteamérica y el anillo Kula en las islas Trobriand.
- **Ayuda mutua.** El anarquista Piotr Kropotkin la desarrolló en *La ayuda mutua: un factor de la evolución* (1902). Sostiene que la cooperación es tan importante como la competencia para la supervivencia, y de ahí viene el principio comunista libertario de "de cada quien según su capacidad, a cada quien según su necesidad".

### En el código (`Economia.c`)

**Regalar un bien** (`regalarRecurso`):

- **Solo se regala el excedente.** A quien regala nunca le queda menos del **40% de su máximo**. Es la parte de "según su capacidad": nadie se queda sin lo básico por dar.
- **Quien recibe no puede pasar de su máximo.** Si el regalo es mayor, se recorta a lo que le cabe.

**Regalar un servicio** (`regalarServicio`): como el servicio lo sabe una persona, regalarlo es que **una persona se mude** a la otra comuna, con todo lo que sabe. La comuna que da debe conservar al menos otra persona que sepa ese servicio.

**Cada regalo** suma 1 a los regalos dados de una comuna y a los recibidos de la otra (para la reciprocidad), da +5 de solidaridad a quien regala y +5 de apoyo a quien recibe. No se guarda **quién** le dio qué a quién, para no reproducir el dinero con otro nombre.

**Ayuda mutua autónoma** (`ayudaAutonoma`): las comunas que no administra la persona usuaria se ayudan solas cada día.

- Si una comuna tiene un bien sobre el **70%** de su máximo, regala el **10%** de su máximo a la comuna que más lo necesite, siempre que esa comuna esté **bajo el 40%**.
- Si en una comuna hay más personas que saben un servicio de las que necesita, una se muda a una comuna donde nadie lo sabe (a lo sumo una mudanza por día).
- Solo se anuncian en pantalla los regalos que reciben las comunas de la persona usuaria.

Esto crea un incentivo sin obligación: las comunas autónomas ayudan a las de la persona usuaria, y la reciprocidad de esas comunas sube si ellas también devuelven ayuda.

## Parámetros de diseño

Todos son decisiones nuestras y están pensados para ajustarse fácilmente.

| Parámetro | Valor | Dónde |
|---|---|---|
| Rango de la necesidad | 5 a 95 | `Indices.c` |
| Pesos de la satisfacción (propia / vecinas / satisfacción vecinas / reciprocidad) | 0.4 / 0.2 / 0.2 / 0.2 | `Indices.c` |
| Intercambios para que la reciprocidad pese completa | 6 | `Indices.c` |
| Comunas cercanas | 2 anteriores y 2 siguientes | `Indices.c`, `Emergencias.c` |
| Bajada diaria de penalización, solidaridad y apoyo | 2 puntos | `Indices.c` |
| Crisis | necesidad ≥ 85 o satisfacción ≤ 25 | `Sociedad.h` |
| Días en crisis para disolverse | 3 | `Sociedad.h` |
| Comunas administradas / vidas | 3 / 3 | `Jugador.h` |
| Días entre emergencias | 3 a 7 | `Emergencias.c` |
| Probabilidad de que un recurso sea afectado | 50% | `Emergencias.c` |
| Pérdida por emergencia | 10% a 50% | `Emergencias.c` |
| Penalización a las comunas cercanas | +10 | `Emergencias.c` |
| Reserva de quien regala un bien | 40% del máximo | `Economia.c` |
| Bono de solidaridad / de apoyo | +5 cada uno (tope 15) | `Economia.c` |
| Ayuda autónoma: excedente / necesidad / regalo | 70% / 40% / 10% del máximo | `Economia.c` |
| Consumo / producción de bienes por comuna | 2.00% a 5.00%, al azar | `Consumo.c`, `Produccion.c` |
| Máximo de un bien / de un servicio | 2 por persona / 1 por cada 10 | `Sociedad.c` |
| Servicios por persona al iniciar | 0 (30%), 1 (50%), 2 (20%) | `Sociedad.c` |
| Libros para aprender un servicio | 3 | `Persona.h` |
| Casillas de la tabla hash | 509 | `TablaPersonas.h` |

## Limitaciones conocidas

- La lista de personas se carga desde el archivo, pero las comunas solo usan **cuántas** personas tienen, no sus nombres.
- Solo se usan dos estructuras de datos (lista simple y lista doble). No se implementó tabla de dispersión.
- El aleatorio usa `rand()` con semilla por hora, así que cada partida es distinta.
- La producción no se pidió en la especificación. La agregamos para que la sociedad no se quede sin bienes de forma inevitable, ya que sin ella solo hay procesos que consumen.
- Los parámetros de la tabla anterior son decisiones de diseño y no están calibrados con datos reales.