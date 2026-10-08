# Una sociedad solarpunk de comunas anarquistas

Proyecto 1 de Estructuras de Datos. Simulación por turnos (cada turno es un día) de una sociedad de 500 personas organizada en comunas que se ayudan entre sí **sin dinero y sin mercado**. El éxito de la sociedad se mide con la **satisfacción** de sus habitantes.

**Integrantes:** 

Dylan Rodríguez
Duvan Asenjo

---

## Contenido

1. [Motivación](#motivación)
2. [Cómo compilar y ejecutar](#cómo-compilar-y-ejecutar)
3. [Cómo se juega](#cómo-se-juega)
4. [Estructuras de datos](#estructuras-de-datos)
5. [Organización del código](#organización-del-código)
6. [Un día de simulación](#un-día-de-simulación)
7. [Modelo de la sociedad](#modelo-de-la-sociedad)
8. [Índices de necesidad y satisfacción](#índices-de-necesidad-y-satisfacción)
9. [Emergencias](#emergencias)
10. [La economía: regalo y ayuda mutua](#la-economía-regalo-y-ayuda-mutua)
11. [Parámetros de diseño](#parámetros-de-diseño)
12. [Pruebas](#pruebas)
13. [Limitaciones conocidas](#limitaciones-conocidas)

---

## Motivación

Imaginar otros mundos posibles es parte de la labor de la ingeniería. Este proyecto plantea una sociedad donde las comunas intercambian bienes y servicios sin que medie el dinero. Para eso necesitamos modelar qué le falta a cada comuna, cómo se siente su gente y de qué manera las comunas pueden ayudarse sin crear una deuda ni un sustituto de la moneda.

## Cómo compilar y ejecutar

Desde la carpeta del proyecto, donde están también los archivos de datos (`personas`, `bienes`, `servicios`, `comunas`):

```
gcc -Wall -Wextra -o comunas *.c
./comunas
```

Los archivos de datos se leen desde el directorio actual, por lo que el programa debe ejecutarse desde la carpeta que los contiene.

## Cómo se juega

Al iniciar, el programa carga los cuatro archivos, muestra cuántos datos hay de cada uno y pide:

- cantidad de comunas (entre 1 y 50, y no más de las que hay en el archivo),
- cantidad de bienes,
- cantidad de servicios.

Luego se crea la sociedad y comienza el día 1. Cada día se muestra el estado de las comunas (necesidad, satisfacción y la **satisfacción promedio de la sociedad**) y aparece este menú:

```
1. Hacer un regalo entre comunas
2. Ver los recursos de una comuna
3. Ver el estado de las comunas
4. Pasar al siguiente dia
0. Salir
```

Para regalar se elige si es un bien o un servicio, la comuna que regala, la que recibe, el recurso y la cantidad. Todo se elige por número.

## Estructuras de datos

No se usan arreglos para guardar nombres. Todo está en estructuras lineales enlazadas:

| Estructura | Dónde se usa | Por qué |
|---|---|---|
| **Lista simple** | Personas (`struct Persona`) y recursos, o sea bienes y servicios (`struct Recurso`) | Solo se recorren de principio a fin, por lo que basta un puntero `siguiente`. |
| **Lista doble** | Comunas (`struct Comuna`) | Para conocer a las comunas cercanas necesitamos movernos hacia adelante **y hacia atrás**: las 2 anteriores y las 2 siguientes. Para eso se usa el puntero `anterior`. |

Cada comuna, a su vez, tiene dos listas simples de recursos: una de bienes y otra de servicios.

## Organización del código

**Carga y estructuras**

| Archivo | Contenido |
|---|---|
| `Persona.c/.h` | Lista simple de personas. Carga desde el archivo `personas`. |
| `Recurso.c/.h` | Lista simple de recursos (`nombre`, `existencia`, `maximo`). Sirve para bienes y servicios. |
| `Comunas.c/.h` | Lista doble de comunas. Carga desde el archivo `comunas`. |

**Simulación**

| Archivo | Contenido |
|---|---|
| `Sociedad.c/.h` | Reparte las 500 personas entre las comunas y asigna los recursos iniciales. |
| `Consumo.c/.h` | Cada día baja la existencia de los bienes. |
| `Produccion.c/.h` | Cada día sube la existencia de bienes y servicios. |
| `Emergencias.c/.h` | Emergencias aleatorias que reducen recursos y afectan a las comunas cercanas. |
| `Economia.c/.h` | El intercambio entre comunas (`regalarRecurso`). |
| `Indices.c/.h` | Cálculo de los índices de necesidad y satisfacción. |

**Programa**

| Archivo | Contenido |
|---|---|
| `Juego.c/.h` | Ciclo de días, menús y entrada de datos del usuario. |
| `main.c` | Solo llama a `iniciarJuego()`. |
| `pruebas/pruebas.c` | Programa aparte que demuestra el funcionamiento de las listas. |

## Un día de simulación

A partir del día 2, el programa ejecuta en este orden:

1. `consumirBienes`: las personas gastan bienes.
2. `producirRecursos`: las comunas producen.
3. `avanzarEmergencias`: si toca, ocurre una emergencia.
4. `actualizarIndices`: se recalculan necesidad y satisfacción.
5. Se muestra el estado y la persona usuaria decide qué intercambios hacer. Después de cada regalo se recalculan los índices.

El día 1 muestra el estado inicial, sin consumo ni producción.

## Modelo de la sociedad

**Población.** Son 500 personas, repartidas en partes iguales entre las comunas elegidas. Las que sobran se reparten de a una a las primeras comunas, así que siempre suman 500.

**Recursos.** Cada comuna tiene los bienes y servicios elegidos, cada uno con una existencia y un máximo calculados a partir de su población:

- Máximo de un bien: 2 por persona.
- Máximo de un servicio: 1 por cada 10 personas, redondeado hacia arriba.
- Existencia inicial: entre 40% y 80% del máximo. Varía de una comuna a otra y de un recurso a otro.

**Consumo.** Cada comuna tiene su propio porcentaje de consumo, entre 2% y 5%. Cada día, cada bien baja `personas × porcentaje / 100` (mínimo 1). Solo se consumen bienes.

**Producción.** Cada comuna tiene su propio porcentaje de producción, entre 2% y 5%. Cada día, cada bien sube `personas × porcentaje / 100` (mínimo 1), y cada servicio sube 1. Nada pasa de su máximo.

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
- **Solidaridad:** sube cuando la comuna regala algo.
- Ambas bajan 2 puntos por día, así que sus efectos se apagan con el tiempo.

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
- **Quien regala gana solidaridad** (+5), que sube su satisfacción un tiempo. Es un reconocimiento social, no algo que se pueda acumular ni canjear.
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
| Bono de solidaridad por regalar | +5 | `Economia.c` |
| Consumo / producción por comuna | 2% a 5% | `Consumo.c`, `Produccion.c` |
| Recuperación de servicios | +1 por día | `Produccion.c` |
| Máximo de un bien / de un servicio | 2 por persona / 1 por cada 10 | `Sociedad.c` |

## Pruebas

`pruebas/pruebas.c` es un programa aparte, con su propio `main`, que verifica las listas: orden de los elementos, enlaces `siguiente` y `anterior` (incluido el recorrido hacia atrás), búsquedas, validación de datos, carga desde archivos y reparto de las 500 personas. Cada prueba imprime `[OK]` o `[FALLO]`.

Se compila y ejecuta desde la carpeta principal:

```
gcc -Wall -Wextra -I. -o pruebas_bin pruebas/pruebas.c Persona.c Recurso.c Comunas.c Sociedad.c
./pruebas_bin
```

## Limitaciones conocidas

- La lista de personas se carga desde el archivo, pero las comunas solo usan **cuántas** personas tienen, no sus nombres.
- Solo se usan dos estructuras de datos (lista simple y lista doble). No se implementó tabla de dispersión.
- El aleatorio usa `rand()` con semilla por hora, así que cada partida es distinta.
- La producción no se pidió en la especificación. La agregamos para que la sociedad no se quede sin bienes de forma inevitable, ya que sin ella solo hay procesos que consumen.
- Los parámetros de la tabla anterior son decisiones de diseño y no están calibrados con datos reales.