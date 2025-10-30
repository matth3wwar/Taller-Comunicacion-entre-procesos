# Comunicación entre procesos
## Autores
Este talles fue realizado por Mateo Guerra y Ángel Daniel García Santana de la Pontificia Universidad Javeriana para la materia de Sistemas Operativos.

## Descripción
El programa demuestra la comunicación entre procesos en C utilizando las llamadas al sistema fork() y pipe().
Recibe dos archivos con números enteros y, mediante la creación de procesos hijo y nieto, calcula tres resultados:

- Suma A: suma de los valores del primer archivo.
- Suma B: suma de los valores del segundo archivo.
- Suma Total: suma combinada de ambos arreglos.

Cada proceso realiza su cálculo de forma independiente y envía el resultado al proceso padre a través de un pipe, quien finalmente muestra todos los valores en pantalla.
El programa evidencia el uso de procesos concurrentes y comunicación interprocesos en sistemas tipo UNIX.

## Compilación
```bash
make taller_procesos

## Uso

# Terminal 
./taller_procesos [N1] [archivo1] [N2] [archivo2]
