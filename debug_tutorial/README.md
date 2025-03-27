# Iniciación a la depuración en C++ con VSCode

## Introducción

En este tutorial vamos a ver cómo depurar en C++ con Visual Studio Code. Utilizaremos para ello el software de la práctica 2 de esta asignatura.

## Pre-requisitos

Para poder seguir este tutorial necesitaremos el IDE de Visual Studio Code y el software de la práctica ya instalado y compilado. Trabajaremos en un entorno Linux, más adelante se explica cómo hacerlo en MAC.

En primer lugar, debemos abrir la carpeta de la práctica desde la opción de `Archivo -> Abrir carpeta...` de Visual Studio Code. Es importante tener abierta la carpeta de la práctica para poder acceder a todos los archivos de la práctica, en lugar de abrir solo archivos sueltos.

Si al abrir la carpeta se nos muestra una imagen como la siguiente, debemos seleccionar la opción "Indeterminado", "Sin especificar" o "Unspecified" (dependiendo de la versión y el idioma nos saldrá con uno de estos nombres).

![Selección de tipo de proyecto](images/unspecified.png)

Con la carpeta abierta, debemos asegurarnos de tener instaladas todas las extensiones necesarias para compilar y depurar en C++. Para ello, debemos ir a la pestaña de `Extensiones` en la barra lateral izquierda y buscar las extensiones `C/C++` y `CMake Tools`. Si no las tenemos instaladas, debemos instalarlas.

![Instalación de extensiones: C++](images/extension_cpp.png)
![Instalación de extensiones: CMake Tools](images/extension_cmake.png)

## El archivo launch.json

A continuación, vamos a colocar el archivo `launch.json` que tenemos en este mismo directorio en la carpeta oculta `.vscode` del directorio de la práctica. Podemos hacerlo a mano, aunque la barra de archivos de VSCode no nos va a mostrar esa carpeta. Para hacerlo por terminal, podemos ejecutar el siguiente comando (asumiendo que estamos en el directorio raíz de la práctica):

```bash
cp debug_tutorial/launch.json .vscode/
```

Echemos un vistazo rápido al archivo. Vemos que hay una serie de claves con valores asociados. Los que nos interesan principalmente son `program`, donde se encuentra el nombre del programa que queremos ejecutar (en este caso, `"{workspaceFolder}/practica2"`), y `args`, donde se encuentran los argumentos que queremos pasarle al programa. En este caso, no hay ninguno.

Nos puede interesar más adelante modificar estos parámetros, bien para usar el programa sin gráfica (`practica2SG`), o bien para lanzar cualquiera de los dos programas con argumentos específicos. Si queremos pasar argumentos, debemos añadirlos a la lista de `args` en el archivo `launch.json`. Los argumentos se añaden como elementos de una lista, separados por comas. Por ejemplo, para el ejemplo del tutorial parte 1, `./mapas/mapa30.map 0 0 17 5 0 17 17 0 3 3 0`, añadiríamos los argumentos de la siguiente forma:

```json
"args": ["./mapas/mapa30.map", "0", "0", "17", "5", "0", "17", "17", "0", "3", "3", "0"]
```

## Cambiando la localización por defecto de los ejecutables.

El software está diseñado para generar los ejecutables sobre el directorio raíz de la práctica. En cambio, VSCode por defecto asume que se generan en una carpeta `build/` dentro de dicho directorio. Si queremos compilar y depurar desde VSCode podemos modificar esta ruta por defecto, solo en este proyecto. Para ello:

1. Abrimos la paleta de comandos de VSCode con `Ctrl+Shift+P`.
2. Buscamos la opción `Workspace settings`, `Workspaces settings (JSON)`, `Preferencias: Abrir Configuración del Área de Trabajo`, `Preferencias: Abrir configuración de área de trabajo (JSON)` o similar.
3. Si hemos abierto el json, añadimos la siguiente línea:

```json
"cmake.buildDirectory": "${workspaceFolder}",
```

![Configuración del área de trabajo](images/build_directory_json.png)

4. Si hemos abierto el editor de configuración, buscamos la opción `cmake.buildDirectory` y la modificamos para que quede como en el punto anterior (quitar la parte de `build/`).

![Configuración del área de trabajo](images/build_directory_gui.png)

Es importante cambiar esta opción dentro de la configuración del área de trabajo y no en las configuraciones de usuario o globales, si no este cambio nos afectará a cualquier otro futuro proyecto que abramos en VSCode.

## Compilando directamente desde VSCode

Si hemos seguido los pasos anteriores, ya deberíamos tener todo listo para compilar y depurar desde VSCode. Para compilar, bastará con pulsar el botón que tenemos en la barra inferior, en el que pone "Versión de compilación".

![Botón de compilación](images/compilar.png)

Si antes no nos salió el menú desplegable para seleccionar el kit de compilación, tendremos que hacerlo ahora. Nos aparecerá el menú desplegable descrito anteriormente, donde tendremos que elegir la opción "Indeterminado", "Sin especificar" o "Unspecified" (dependiendo de la versión y el idioma nos saldrá con uno de estos nombres).

![Selección de tipo de proyecto](images/unspecified.png)

Tras esto, cada vez que pulsemos el botón se nos compilará automáticamente el proyecto, sin necesidad de escribir `make` en la terminal.

Si por algún motivo no elegimos la opción correcta al mostrar la lista desplegable, podemos solucionarlo yéndonos al menú de CMake en la barra lateral izquierda. En la pestaña que se nos abre, buscamos la opción "Configurar", y pinchamos justo debajo para cambiar el kit. Esto nos volverá a mostrar la lista desplegable.

![Selección de kit de compilación](images/retry_cmake.png)

## Depurando en VSCode

Ya estamos en condiciones de empezar a depurar. Antes de nada, tenemos que asegurarnos de que nuestro programa se está compilando en modo `Debug`. El software está preparado para compilarse en dos modos diferentes:

- `Debug`: Se compila con información de depuración, lo que nos permite depurar el programa, poner breakpoints y trazar errores. En este modo las ejecuciones suelen ser bastante más lentas.
- `Release`: Se compila sin información de depuración y con optimizaciones, lo que hace que las ejecuciones sean más rápidas. No podemos depurar en este modo.

Para cambiar el modo de compilación, debemos ir a la pestaña de CMake en la barra lateral izquierda, y pinchar, dentro de la pestaña de "Configurar", en la opción "Debug" o "Release". Esto nos abrirá un menú desplegable donde podremos seleccionar el modo de compilación. Se recomienda usar `Release` cuando estéis ejecutando el software en serio, y `Debug` cuando estéis buscando errores o problemas en el código.

![Selección de modo de compilación](images/debug_release.png)

Tras esto, ya podemos empezar a depurar. Nos vamos de nuevo a la barra lateral izquierda, y pinchamos la pestaña de "Ejecución y depuración". Nos deberíamos encontrar en una pestaña como la que se muestra a continuación:

![Menú de depuración](images/start_debug.png)

Si nos saliera una cosa diferente o nos dijera que tenemos que crear un archivo `launch.json`, debemos revisar los pasos anteriores. Seguramente, no hayamos copiado el archivo `launch.json` correctamente en la carpeta `.vscode` de la práctica.

Estando en este menú solo tenemos que darle al botón del play, y se nos abrirá el programa. Tanto el programa que se ejecute como los argumentos que se le pasen son los que tengamos especificados en el archivo `launch.json`. Cuando queramos depurar con otros parámetros, tendremos que modificar este archivo.


## Breakpoints y excepciones

## El camino difícil: depurando en MAC

Próximamente...