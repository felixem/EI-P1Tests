# EI-P1Tests
Repositorio para pruebas unitarias de la práctica 1 de EI

INSTRUCCIONES PARA LA COMPILACIÓN

Copiar los .h y .cpp en las carpetas include y lib de la raiz del proyecto.

Ejecutar desde el directorio Compile del proyecto:

make

INSTRUCCIONES PARA LA EJECUCIÓN

Ejecutar desde la carpeta raiz del proyecto:

DirEjecutable/Ejecutable [-e directorioEntrada] [-r directorioReferencia] [-c directorioCopia]

Siendo:
directorioEntrada -> directorio donde se encuentran los ficheros a tokenizar.
directorioReferencia -> directorio donde se encuentran los ficheros de salida de referencia.
directorioCopia -> directorio donde se moverá la salida generada por el método TokenizarDirectorio.

Al finalizar la ejecución se creará en la raiz un fichero "reporte_tokenizacion" que resume las diferencias entre la salida generada y la salida esperada
