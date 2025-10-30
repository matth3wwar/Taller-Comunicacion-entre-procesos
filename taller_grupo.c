/******************************************
* Pontificia Universidad Javeriana
* Autores: Mateo David Guerra, Ángel García
* Fecha: 27 de Octubre del 2025
* Materia: Sistemas Operativos
* Tema: Fork Pipe Named
* Descripción: Demostración de comunicación
* entre procesos por medio de Pipes.
*******************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

/*Método para la lectura de los archivos conteniendo los arreglos*/
int leerArchivo(const char *archivo, int N, int **salida) {

	/*Alerta si la cantidad de datos prevista es 0*/
	if (N == 0) {
		printf("\nAlerta de arreglo vacío en archivo '%s'\n", archivo);
		*salida = NULL;
		return 0;
	}

	/*Apertura del archivo en modo lectura*/
	FILE *f = fopen(archivo, "r");
	if (!f) {
		printf("\nNo se encuentra el archivo con el nombre \"%s\"\n\n", archivo);
		return 0;
	}

	/*Reserva del espacio para el arreglo*/
	int *arreglo = malloc(N * sizeof(int));

	/*Error en la reserva del espacio*/
	if (!arreglo) {
		printf("\nNo se pudo inicializar el arreglo del primer archivo\n");
		fclose(f);
		return 0;
	}

	/*Lectura de los contenidos del archivo*/
	size_t i = 0;
	while (i < N && fscanf(f, "%d", &arreglo[i]) == 1) {
		i++;
	}

	/*En caso de obtener un error durante la lectura del archivo, se almacena y se trata acá*/
	if (ferror(f)) {
		fprintf(stderr, "\nError al leer el archivo: %s\n", archivo);
		free(arreglo);
		fclose(f);
		return 0;
	}

	fclose(f);/*Se cierra el canal con el archivo*/
	*salida = arreglo;/*Se asigna el espacio del vector a la variable dada como parametro*/
	return 1;
}

/*Método de "toString" de los arreglos para ser impresos en consola*/
void imprimirArreglo(int *arr, int N) {
	if (arr == NULL) {
		/*Salida si el arreglo se encuentra vacío*/
		printf("\nArreglo Vacío\n");
	} else {
		/*Recorrido del arreglo para imprimir cada elemento*/
		printf("\nArreglo de tamaño %d:\n", N);
		for (int i = 0; i < N; i++) {
			char c = i == N - 1 ? ' ' : ',';
			printf(" %d%c", arr[i], c);
		}
		printf("\n");
	}
}

int main(char argc, char *argv[]) {
	if (argc < 5) {
	        fprintf(stderr, "Uso: %s N1 archivo00 N2 archivo01\n", argv[0]);
	        return 1;
	    }


	/*Conversión de los argumentos de N1 y N2 a cadena de caracteres*/
	char *endptr;

	/*Conversión de N1*/
	errno = 0;
	long n1 = strtol(argv[1], &endptr, 10);
	if (errno || *endptr != '\0' || n1 < 0) {
		fprintf(stderr, "N1 no es válido (%s)", argv[1]);
		return 1;
	}

	/*Conversion de N2*/
	errno = 0;
	long n2 = strtol(argv[3], &endptr, 10);
	if (errno || *endptr != '\0' || n1 < 0) {
		fprintf(stderr, "N1 no es válido (%s)", argv[1]);
		return 1;
	}

	/*Declaración de N1 y N2*/
	size_t N1 = (size_t)n1;
	size_t N2 = (size_t)n2;

	/*Inicialización de arreglos numéricos enteros*/
	int *arreglo1 = NULL;
	int *arreglo2 = NULL;

	/*Lectura de archivo conteniendo el primer arreglo*/
	if (!leerArchivo(argv[2], N1, &arreglo1)) {
		free(arreglo1);
		free(arreglo2);
		return 1;
	}

	/*Lectura de archivo conteniendo el segundo arreglo*/
	if (!leerArchivo(argv[4], N2, &arreglo2)) {
		free(arreglo1);
		free(arreglo2);
		return 1;
	}

	/*Impresión por consola de los contenidos de los archivos*/
	printf("\nArreglo 1:\n");
	imprimirArreglo(arreglo1, N1);

	printf("\nArreglo 2:\n");
	imprimirArreglo(arreglo2, N2);

	/*Creación de Pipe Resultado total*/
	int fd[2];

	if(pipe(fd) == -1) {
		perror("PIPE");
		exit(EXIT_FAILURE);
	}

	/*Creación de Pipe Resultado A
	int fdA[2];

	if(pipe(fdA) == -1) {
		perror("PIPE");
		exit(EXIT_FAILURE);
	}

	/*Creación de Pipe Resultado B
	int fdB[2];

	if(pipe(fdB) == -1) {
		perror("PIPE");
		exit(EXIT_FAILURE);
	}
*/
	pid_t pid_hijo = fork();


	if(pid_hijo < 0) {
		perror("FORK");
		exit(EXIT_FAILURE);
	}

	//Primer hijo (suma total desde los dos arreglos)
	if (pid_hijo == 0) {
		size_t j;
		int sumaT = 0;
		for (j = 0; j < N1; j++) {
			sumaT += arreglo1[j];
		}
		for (j = 0; j < N2; j++) {
			sumaT += arreglo2[j];
		}
		printf("\nSuma T = %d\n", sumaT);

		/*Envio de mensaje al padre*/

		close(fd[0]); /*Cerrar lectura*/
		write(fd[1], &sumaT, sizeof(sumaT));/*Escribir resultado*/
		//close(fd[1]); /*Cerrar escritura*/

		/*Creación segundo hijo*/
		pid_hijo = fork();
		if(pid_hijo == -1) {
			perror("FORK");
			exit(EXIT_FAILURE);
		}

		if (pid_hijo != 0) return 0;
	}

	//Segundo hijo (suma B de archivo01)
	if (pid_hijo == 0) {
		size_t j;
		int sumaB = 0;
		for (j = 0; j < N2; j++) {
			sumaB += arreglo2[j];
		}
		printf("\nSuma B = %d\n", sumaB);

		/*Envio de mensaje al padre*/

		close(fd[0]); /*Cerrar lectura*/
		write(fd[1], &sumaB, sizeof(sumaB));/*Escribir resultado*/
		//close(fd[1]); /*Cerrar escritura*/
		/*Creación grand hijo*/
		pid_hijo = fork();
		if(pid_hijo == -1) {
			perror("FORK");
			exit(EXIT_FAILURE);
		}

		if (pid_hijo != 0) return 0;
	}

	//Grand hijo (suma A de archivo01)
	if (pid_hijo == 0) {
		size_t j;
		int sumaA = 0;
		for (j = 0; j < N1; j++) {
			sumaA += arreglo1[j];
		}

		/*Envio de mensaje al padre*/

		close(fd[0]); /*Cerrar lectura*/
		write(fd[1], &sumaA, sizeof(sumaA));/*Escribir resultado*/
		//close(fd[1]); /*Cerrar escritura*/

		return 0;
	}
	wait(NULL);
	close(fd[1]);/*Cerrar escritura*/

	int resultado;
	/*Recepción de los mensajes en el padre*/
	for (int i = 0; i < 3; i++) {
		read(fd[0], &resultado, sizeof(resultado));/*leer resultado*/

		char *sumType = "Total";
		if (i == 1) sumType = "A";
		if (i == 2) sumType = "B";
		printf("\nResultado Suma %s: %d", sumType, resultado);
	}

	close(fd[0]); /*Cerrar lectura*/
	free(arreglo1);
	free(arreglo2);

	return 0;
}
