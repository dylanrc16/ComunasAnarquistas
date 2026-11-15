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

No se usan arreglos para guardar nombres. Todo está en estructuras lineales enlazadas:

| Estructura | Dónde se usa | Por qué |
|---|---|---|
| **Lista simple** | Personas (`struct Persona`) y recursos, o sea bienes y servicios (`struct Recurso`) | Solo se recorren de principio a fin, por lo que basta un puntero `siguiente`. |
| **Lista doble** | Comunas (`struct Comuna`) | Para conocer a las comunas cercanas necesitamos movernos hacia adelante **y hacia atrás**: las 2 anteriores y las 2 siguientes. Para eso se usa el puntero `anterior`. |

Cada comuna, a su vez, tiene dos listas simples de recursos: una de bienes y otra de servicios.

## Un día de simulación

A partir del día 2, el programa ejecuta en este orden:

1. `consumirRecursos`: las personas gastan bienes y servicios.
2. `producirRecursos`: las comunas producen.
3. `decaerEfectos`: la penalización y la solidaridad bajan 2 puntos.
4. `avanzarEmergencias`: si toca, ocurre una emergencia.
5. `actualizarIndices`: se recalculan necesidad y satisfacción.
6. Se muestra el estado y la persona usuaria decide qué intercambios hacer. Después de cada regalo se recalculan los índices.

El día 1 muestra el estado inicial, sin consumo ni producción.

## Modelo de la sociedad

**Población.** Son 500 personas, repartidas en partes iguales entre las comunas elegidas. Las que sobran se reparten de a una a las primeras comunas, así que siempre suman 500.

**Recursos.** Cada comuna tiene los bienes y servicios elegidos, cada uno con una existencia y un máximo calculados a partir de su población:

- Máximo de un bien: 2 por persona.
- Máximo de un servicio: 1 por cada 10 personas, redondeado hacia arriba.
- Existencia inicial: entre 40% y 80% del máximo. Varía de una comuna a otra y de un recurso a otro.

**Consumo.** Cada comuna tiene su propio porcentaje de consumo, sorteado al iniciar entre 2.00% y 5.00%. Cada día, cada bien baja `personas × porcentaje / 100` y cada servicio baja la décima parte de eso (igual que su máximo, que es 1 por cada 10 personas).

**Producción.** Cada comuna tiene su propio porcentaje de producción, sorteado al iniciar entre 2.00% y 5.00%. Cada día, cada bien sube `personas × porcentaje / 100` y cada servicio la décima parte de eso. Nada pasa de su máximo.

**Redondeo al azar.** Las existencias son enteras, pero el gasto y la producción tienen decimales. Para no perder esos decimales, `redondearAlAzar` (en `Recurso.c`) usa la parte decimal como probabilidad de redondear hacia arriba: un gasto de 2.3 resta 3 con 30% de probabilidad y 2 con 70%. Así, en promedio, cada comuna gasta exactamente lo que le corresponde, y dos comunas con porcentajes distintos gastan distinto aunque tengan pocas personas.

Los porcentajes de consumo y producción se combinan de forma distinta en cada comuna. Así, algunas se mantienen estables, otras se van quedando sin bienes y otras acumulan excedente. Esa diferencia es lo que hace que regalar tenga sentido.

## Índices de necesidad y satisfacción

### Necesidad

Mide cuánto falta para llegar al máximo, sumando todos los bienes y servicios de la comuna:

```
necesidad = 100 × (suma de lo que falta) / (suma de los máximos)
```

El resultado se limita al rango **5 a 95**. Nunca llega a 100 (siempre hay necesidades adicionales o emergencias) y tampoco a 0.

### Satisfacción

Mide cómo se siente la gente con su propia comuna y con las comunas de alrededor:

```
satisfaccion = 0.6 × (100 − necesidad propia)
             + 0.4 × (promedio de 100 − necesidad de las comunas cercanas)
             + solidaridad
             − penalizacion
```

El resultado se limita al rango 0 a 100.

- **Comunas cercanas:** las 2 anteriores y las 2 siguientes en la lista doble, es decir, hasta 4 comunas (menos si la comuna está en un extremo de la lista).
- **Pesos 0.6 y 0.4:** la gente se siente bien sobre todo por lo que tiene su propia comuna, pero también influye cómo están las de alrededor.
- **Penalización:** sube cuando una comuna cercana sufre una emergencia.
- **Solidaridad:** sube cuando la comuna regala algo, con un tope de 15.
- Ambas bajan 2 puntos una vez por día (`decaerEfectos`), así que sus efectos se apagan con el tiempo. No bajan al recalcular los índices después de un regalo.

La satisfacción de una comuna depende de la necesidad de sus vecinas, por eso `actualizarIndices` calcula primero todas las necesidades y después todas las satisfacciones.

**La sociedad se evalúa con el promedio de satisfacción de todas las comunas**, que se muestra cada día.

## Emergencias

Cada **3 a 7 días** (sorteado cada vez) ocurre una emergencia:

1. Se elige una comuna al azar.
2. Cada uno de sus bienes y servicios tiene **50%** de probabilidad de ser afectado, y si lo es pierde entre **10% y 50%** de su existencia. Todo es aleatorio: la comuna, cuáles recursos y cuánto.
3. Las 2 comunas anteriores y las 2 siguientes reciben **+10 de penalización**, lo que baja su satisfacción.

## La economía: regalo y ayuda mutua

### En la vida real

- **Economía del regalo.** El antropólogo Marcel Mauss la estudió en su *Ensayo sobre el don* (1925). En muchas sociedades las cosas circulan como regalos y no como mercancías: dar crea vínculos y prestigio social, y no una deuda monetaria. Ejemplos clásicos son el potlatch de pueblos de la costa noroeste de Norteamérica y el anillo Kula en las islas Trobriand.
- **Ayuda mutua.** El anarquista Piotr Kropotkin la desarrolló en *La ayuda mutua: un factor de la evolución* (1902). Sostiene que la cooperación es tan importante como la competencia para la supervivencia, y de ahí viene el principio comunista libertario de "de cada quien según su capacidad, a cada quien según su necesidad".

### En el código

Está en `regalarRecurso` (`Economia.c`). Una comuna entrega parte de un bien o servicio a otra, y:

- **Solo se regala el excedente.** A quien regala nunca le queda menos del **40% de su máximo**. Es la parte de "según su capacidad": nadie se queda sin lo básico por dar.
- **Quien recibe no puede pasar de su máximo.** Si el regalo es mayor, se recorta a lo que le cabe.
- **No hay contrapartida ni deuda.** El programa no guarda quién le dio qué a quién, para no reproducir el dinero con otro nombre.
- **Quien regala gana solidaridad** (+5), que sube su satisfacción un tiempo. Es un reconocimiento social, no algo que se pueda acumular ni canjear: tiene un tope de 15 y baja 2 puntos por día.
- **Los intercambios los decide la persona usuaria**, como pide la especificación.

Un ejemplo: una comuna que tiene mucho de un bien (por ejemplo, porque produce más de lo que consume) puede regalar parte a otra que lo necesita. La que recibe sube su existencia, su necesidad baja y la satisfacción de ambas mejora.

## Parámetros de diseño

Todos son decisiones nuestras y están pensados para ajustarse fácilmente.

| Parámetro | Valor | Dónde |
|---|---|---|
| Rango de la necesidad | 5 a 95 | `Indices.c` |
| Peso propia / vecinas en la satisfacción | 0.6 / 0.4 | `Indices.c` |
| Comunas cercanas | 2 anteriores y 2 siguientes | `Indices.c`, `Emergencias.c` |
| Bajada diaria de penalización y solidaridad | 2 puntos | `Indices.c` |
| Días entre emergencias | 3 a 7 | `Emergencias.c` |
| Probabilidad de que un recurso sea afectado | 50% | `Emergencias.c` |
| Pérdida por emergencia | 10% a 50% | `Emergencias.c` |
| Penalización a las comunas cercanas | +10 | `Emergencias.c` |
| Reserva de quien regala | 40% del máximo | `Economia.c` |
| Bono de solidaridad por regalar | +5 (tope 15) | `Economia.c` |
| Consumo / producción por comuna | 2.00% a 5.00%, al azar | `Consumo.c`, `Produccion.c` |
| Consumo / producción de servicios | 1/10 de la de un bien | `Consumo.c`, `Produccion.c` |
| Máximo de un bien / de un servicio | 2 por persona / 1 por cada 10 | `Sociedad.c` |

## Limitaciones conocidas

- La lista de personas se carga desde el archivo, pero las comunas solo usan **cuántas** personas tienen, no sus nombres.
- Solo se usan dos estructuras de datos (lista simple y lista doble). No se implementó tabla de dispersión.
- El aleatorio usa `rand()` con semilla por hora, así que cada partida es distinta.
- La producción no se pidió en la especificación. La agregamos para que la sociedad no se quede sin bienes de forma inevitable, ya que sin ella solo hay procesos que consumen.
- Los parámetros de la tabla anterior son decisiones de diseño y no están calibrados con datos reales.