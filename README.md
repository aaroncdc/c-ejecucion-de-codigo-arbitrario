# c-ejecucion-de-codigo-arbitrario
 Ejemplos de ejecución de código arbitrario en C para Windows (32 y 64 bits)

Demostraciones de ejecución de código binario arbitrario en la memoria para Windows con MinGW, de 32 y 64 bits. El archivo cace32.c contiene código para ejecutarse en un entorno Windows de 32 bits, mientras que cace64.c contiene código para ejecutarse en un entorno Windows de 64 bits. Ambos deben ser compilados con su correspondiente versión de MinGW (MINGW32 para cace32 y MINGW64 para cace64).

## Explicación

Al principio, durante la era de los procesadores de 8, 16 y 32 bits, era posible ejecutar código casi desde cualquier dirección de la memoria. Evidentemente, esto supone un problema de seguridad ya que cualquier aplicación puede inyectar código malicioso dentro de la memoria de otro proceso. Para intentar mitigar ésto, los sistemas de 32 bits incorporaron algunos métodos de control de acceso a la memoria y de ejecución. En el caso de Windows XP, se incorporó una característica denominada ["DEP" (Data Execution Prevention)](https://en.wikipedia.org/wiki/Executable_space_protection#Windows), cuyo funcionamiento puede causar problemas y en ocasiones viene desactivado por defecto.

Esto quiere decir que cualquier aplicación ejecutada en un entorno de 32 bits puede ejecutar código arbitrario desde cualquier dirección de la memoria, siempre y cuando tenga permiso de acceso a dicha región de la memoria. Lo cual supone una vulnerabilidad, y muchos atacantes se han aprovechado de ésto en el pasado para inyectar y ejecutar código malicioso en otros procesos.

Para evitar todo ésto de una manera eficiente, los fabricantes de procesadores incorporaron una extensión denominada [PAE (Physical Address Extension)](https://en.wikipedia.org/wiki/Physical_Address_Extension), incorporada en los procesadores desde el Intel Pentium Pro y los procesadores AMD Athlon de mediados de los 90. En los procesadores de 64 bits, se incorpora además una extensión de las tablas de paginación, que añade la posibilidad de usar el primer bit de cualquier entrada de la tabla como un bit de "no ejecución" ([NX](https://en.wikipedia.org/wiki/NX_bit)).

Cuando el bit NX de cualquier página de memoria está activado, no se podran realizar llamadas o ejecutar código desde éstas páginas de memoria, protegiendolas frente a ataques de inyección de código arbitrario.

Los ejemplos contenidos en éste proyecto no demuestran ningún tipo de ataque, pero demuestran cómo se ejecuta código arbitrario en ambos tipos de sistemas, cómo se puede cambiar la protección de la memoria en Windows (64-bits) mediante la API de Windows, y cómo protege el bit NX las páginas de memoria contra la ejecución de código arbitrario.

Puedes leer más información sobre la memoria y su funcionamiento en [mi blog](https://elinformati.co/index.php/2022/01/17/la-memoria-del-ordenador-a-fondo/).