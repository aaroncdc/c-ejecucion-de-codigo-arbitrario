/* Ejecución de código arbitrario en C para Windows de 32 bits
*                   Por Aarón C.d.C
*   
*   https://elinformati.co | https://github.com/aaroncdc   
*
*               COMPILAR Y EJECUTAR CON MINGW
*
* El siguiente código ejecuta código arbitrario directamente desde
* un puntero de funciones. Esto sólo es posible en sistemas Windows
* de 32-bits, ya que en 64 bits se fuerza el uso de la bandera de
* no ejecución (NX) en las páginas de memoria usadas por el programa.
*  
* En éste caso el código binario se ejecuta desde la sección .data del
* ejecutable binario. En los sistemas de 64 bits, ésta sección se copia
* en una región separada del código ejecutable (.text) y se protege contra
* ejecución, pero en 32 bits todo es ejecutable.
*
* Esto es inseguro, ya que habilita a un atacante a ejecutar código
* arbitrario malicioso inyectando código en la memoria mediante un ataque
* tipo buffer overflow o similar.
*
* Compilalo con GCC en un sistema Windows de 32 bits con MinGW de 32 bits:
* $ gcc cace32.c -o cace32
* $ ./cace32
*
*/

#include <stdio.h>

/* El siguiente código binario se traduce en la siguiente rutina:
    int func(int a, int b) { return b - a; }*/
char funcData[11] = {0x55,0x89,0xe5,0x8b,0x45,0x0c,0x2b,0x45,0x08,0x5d,0xc3};

int main()
{
    // Crea un puntero de función que apunta al código binario arbitrario.
    int (*func)(int,int) = (void*)&funcData;

    // Ejecuta el código arbitrario. Se traduce a: int r = func(2,3).
    int r = (*func)(4,16);

    // Si se ejecuta correctamente, debería imprimir el siguiente mensaje: "16 - 4 = 12"
    printf("16 - 4 = %i\n",r);

    // Fin
    return 0;
}