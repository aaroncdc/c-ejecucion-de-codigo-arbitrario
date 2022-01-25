/* Ejecución de código arbitrario en C para Windows de 64 bits
*                   Por Aarón C.d.C
*   
*   https://elinformati.co | https://github.com/aaroncdc   
*
* COMPILAR Y EJECUTAR CON MINGW / SE RECOMIENDA USAR GDB
*
* El siguiente código solicita una página de memoria con el bit
* de no ejecución (NX) desactivado, de modo que es posible ejecutar
* código arbitrario desde la memoria. No hay muchas aplicaciones
* prácticas para éste proceso ya que es inseguro (Se podría inyectar
* código malicioso en las páginas de memoria desprotegidas), pero
* demuestra que es posible hacerlo. Recomiendo ejecutarlo en GDB
* con MinGW.
*
* Este programa demuestra el uso del bit NX y la forma en la que
* protege las páginas de memoria contra ejecución de código
* malicioso. Primero se deshabilita el bit NX para permitir la
* ejecución de código, y después se habilita para forzar un
* error de segmentación en la memoria al intentar ejecutar el
* mísmo código.
* (https://elinformati.co/index.php/2022/01/17/la-memoria-del-ordenador-a-fondo/)
*
* Se compila con GCC (MINGW64) con el siguiente comando:
* gcc -g cace64.c -o cace -D__DEBUG
*
* ¡EJECUTALO USANDO GDB!
* $ gdb cace64
* (gdb) r
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <memoryapi.h>
#include <sysinfoapi.h>
#include <errhandlingapi.h>

/* El siguiente código binario se traduce en la siguiente rutina:
    int func(int a, int b) { return b - a; }*/
char funcData[18] = {0x55, 0x48, 0x89, 0xe5, 0x89, 0x4d, 0x10, 0x89, 0x55,
    0x18, 0x8b, 0x45, 0x18, 0x2b, 0x45, 0x10, 0x5d, 0xc3};

int main()
{
    SYSTEM_INFO sysinfo;            // Estructura SYSTEM_INFO
    int (*arCode)(int,int) = 0x0;   // Puntero al código binario (int func(int,int))
    int vpr = 0, r = 0;             // Valores de retorno
    DWORD ec = 0x0;                 // Valor de retorno de GetLastError()

    /* Obtener la información del sistema.
    * De dicha información, extraemos el tamaño máximo de cada página de memoria. */
    GetSystemInfo((LPSYSTEM_INFO)&sysinfo);
    printf("dwPageSize: %#0x\n", sysinfo.dwPageSize);

    /* Solicitar al sistema una página de memoria entera, con el bit de no ejecución
    * (NX) desactivado, mediante la bandera PAGE_EXECUTE_READWRITE. */
    void* pAddress = VirtualAlloc(
        NULL,
        (size_t)sysinfo.dwPageSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );

    // Comprobar que se ha devuelto una página de memoria.
    if(pAddress == 0L)
    {
        ec = GetLastError();
        printf(
            "Error al solicitar la página de memoria (GetLastError = %u)\n",
            ec);
        return 1;
    }

    printf("pAddress: %#0x\n", pAddress);

    // Copiar el código a ejecutar en la página de memoria.
    memcpy((void*)pAddress, (void*)funcData, 18);

    // Convertir la dirección de la página a un puntero de rutina.
    arCode = (void*)pAddress;
    
    // Llamada al código arbitrario en la memoria. Esto ejecuta la siguiente línea: int r = func(2,3).
    r = (*arCode)(2,3);

    // Si todo es correcto, se imprime el siguiente texto en la consola: "3 - 2 = 1"
    printf("3 - 2 = %i \n",r);

    // Activar el bit de no ejecución y proteger la página contra escritura.
    vpr = VirtualProtect(
        pAddress,
        (size_t)sysinfo.dwPageSize,
        PAGE_READONLY,
        (void*)arCode
    );

    // Asegurar que la anterior operación se ha realizado con éxito.
    if(vpr == 0L)
    {
        ec = GetLastError();
        printf(
            "No se ha podido cambiar la protección de la página (GetLastError = %u)\n",
            ec);
        return 1;
    }

    /* Comprueba que el siguiente código produce un error (Ejecutar sólo en GDB por seguridad.
    * Debes definir __DEBUG manualmente durante la compilación, por ejemplo: gcc -g -D__DEBUG). */
    #ifdef __DEBUG
        printf("-- AQUI SE FUERZA UN ERROR DE SEGMENTACION DELIBERADAMENTE (SIGSEGV) --\n");
        fflush(stdout); // Limpiar STDOUT. Esto imprimirá todos los mensajes en la consola.
        r = (*arCode)(1,4); // Se ejecuta código desde una página de memoria con el bit NX activado.
        printf("4 - 1 = %i \n",r);
    #endif

    // El siguiente código sólo se ejecuta cuando el código en __DEBUG no ha sido ejecutado.

    /* Liberar la página de memoria. dwSize debe ser 0 cuando dwFreeType
    * tiene valor MEM_RELEASE. */
    vpr = VirtualFree(pAddress, 0, MEM_RELEASE);

    // Comprobar que la operación no falla
    if(!vpr)
    {
        ec = GetLastError();
        printf("No se ha podido liberar la memoria (GetLastError = %u)\n",ec);
        return 1;
    }

    // Final del programa.
    return 0;
}