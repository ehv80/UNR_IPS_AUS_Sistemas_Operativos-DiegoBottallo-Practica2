///////////////////////////////////////////////////////////////////////////////////////////
//
// ls.c
//
// Este programita muestra toda la informacion posible que se puede sacar de los archivos
// de un directorio, usando las estructuras de datos "struct dirent" y "struct stat"
//
///////////////////////////////////////////////////////////////////////////////////////////
//
// Diseño de Sistemas Operativos
// Curso 2003-2004
//
// Jose Juan Sanchez Hernandez
// jsh420@alboran.ual.es
//
///////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <string.h>


/*
struct dirent {
	long            d_ino;        * número i-nodo *
	off_t           d_off;        * despl. al siguiente dirent *
        unsigned short  d_reclen;     * long. de este registro *
	unsigned char   d_type;       * tipo de fichero *
        char            d_name[256];  * nombre del fichero *
};

                                                                                
struct stat
{
	dev_t         st_dev;      * dispositivo *
	ino_t         st_ino;      * inodo *
        mode_t        st_mode;     * protección *
        nlink_t       st_nlink;    * número de enlaces físicos *
        uid_t         st_uid;      * ID del usuario propietario *
        gid_t         st_gid;      * ID del grupo propietario *
        dev_t         st_rdev;     * tipo dispositivo (si es
                                      dispositivo inodo) *
        off_t         st_size;     * tamaño total, en bytes *
        unsigned long st_blksize;  * tamaño de bloque para el
                                      sistema de ficheros de E/S *
        unsigned long st_blocks;   * número de bloques asignados en bloques de 512 bytes*
        time_t        st_atime;    * hora último acceso *
        time_t        st_mtime;    * hora última modificación *
        time_t        st_ctime;    * hora último cambio *
};

* File types.  *
#define	__S_IFDIR	0040000	* Directory.  *
#define	__S_IFCHR	0020000	* Character device.  *
#define	__S_IFBLK	0060000	* Block device.  *
#define	__S_IFREG	0100000	* Regular file.  *
#define	__S_IFIFO	0010000	* FIFO.  *
#define	__S_IFLNK	0120000	* Symbolic link.  *
#define	__S_IFSOCK	0140000	* Socket.  *


* Protection bits.  *
#define	__S_ISUID	04000	* Set user ID on execution.  *
#define	__S_ISGID	02000	* Set group ID on execution.  *
#define	__S_ISVTX	01000	* Save swapped text after use (sticky).  *
#define	__S_IREAD	0400	* Read by owner.  *
#define	__S_IWRITE	0200	* Write by owner.  *
#define	__S_IEXEC	0100	* Execute by owner.  *

*/

char *imprimir_proteccion(int m);
char *imprimir_tipo_archivo(unsigned char c);
char * imprimir_fecha(const time_t *t);
void imprimir_basico();
void imprimir_ampliado();
void imprimir_todo();


int main(int argc, char *argv[])
{

	//comprobamos los parametros de entrada para llamar a la funcion adecuada
	if(argc == 1) imprimir_basico();
	if((argc == 2) && (strcmp(argv[1],"-la")==0)) imprimir_ampliado();
	if((argc == 2) && (strcmp(argv[1],"-todo")==0)) imprimir_todo();

	return 0;
}

// Esta funcion muestra toda la informacion posible que se puede sacar de los archivos
// de un directorio, usando las estructuras de datos "struct dirent" y "struct stat"
//
void imprimir_todo()
{
	DIR *dp;
	struct dirent *d;
	struct inode *inode;
	struct stat buf;

	//obtenemos un puntero al flujo del directorio actual
	dp = opendir(".");

	//comprobamos si se ha producido algun error
	if (dp == NULL) {
		printf("\nERROR: En la funcion opendir\n");
		exit(1);
	}

	//obtenemos un puntero a la estructura del directorio a partir 
	//del puntero del flujo del directorio
	d = readdir(dp);

	//examinamos cada entrada del directorio para devolver informacion de cada archivo
	while (d) {

		//informacion sacada de la estructura "struct dirent"
		printf("-----------------------------------------------------------------\n");
		printf("\n INFORMACION DE: struct dirent\n");
		printf("\t Nombre del fichero:\t\t %s\n", d->d_name);
		printf("\t Numero i-nodo:\t\t\t %d\n",d->d_ino);
		printf("\t Longitud de este registro:\t %d bytes\n",d->d_reclen);
		printf("\t Tipo de archivo:\t\t %s\n",imprimir_tipo_archivo(d->d_type));

		//obtenemos informacion del archivo en la estructura "struct stat"
		if(stat(d->d_name,&buf)==-1) {
			printf("\nERROR: En la funcion stat\n");
			exit(-1);
		}	

	    	//informacion sacada de la estructura "struct stat"
		printf("\n INFORMACION DE: struct stat\n");
    		printf("\t Dispositivo:\t\t\t %d\n",buf.st_dev);
    		printf("\t Inodo:\t\t\t\t %d\n",buf.st_ino);
    		printf("\t Proteccion:\t\t\t %s\n",imprimir_proteccion(buf.st_mode));
    		printf("\t Numero de enlaces fisicos:\t %d\n",buf.st_nlink);
    		printf("\t ID del usuario:\t\t %d\n",buf.st_uid);
    		printf("\t ID del grupo:\t\t\t %d\n",buf.st_gid);
    		printf("\t Tipo Dispositivo:\t\t %d\n",buf.st_rdev);
    		printf("\t Tamanyo total:\t\t\t %d bytes\n",buf.st_size);
    		printf("\t Tamanyo de bloque:\t\t %u bytes\n",buf.st_blksize);
    		printf("\t Numero de bloques asignados:\t %u\n",buf.st_blocks);
    		printf("\t Hora ultimo acceso:\t\t %s",ctime(&buf.st_atime));
    		printf("\t Hora ultima modificacion:\t %s",ctime(&buf.st_mtime));
    		printf("\t Hora ultimo cambio:\t\t %s",ctime(&buf.st_ctime));
		printf("\n");

		//obtenemos un puntero a la estructura del directorio a partir 
		//del puntero del flujo del directorio
    		d = readdir(dp);
	}

	//cerramos el flujo del directorio	
	closedir(dp);
}

// Entrada: El valor del campo "st_mode" (proteccion) de la estructura stat
// Salida: Una cadena que muestra el tipo de archivo que es, y los permisos
// de dicho archivo para el propietario, grupo del propietario y demas usuarios.
//
char *imprimir_proteccion(int m)
{
	static char cadena[10];

	//inicializamos la cadena con un valor inicial
	strcpy(cadena,"----------");

	//comprobamos de que tipo de archivo se trata. 
	//Comparamos con las mascaras que estan definidas en el archivo "stat.h"

	// tipo de archivo = archivo regular
	if ((m & S_IFREG) == S_IFREG)
		cadena[0] = '-';
	// tipo de archivo = directorio
	if ((m & S_IFDIR) == S_IFDIR)
		cadena[0] = 'd';
	// tipo de archivo = especial de caracter
	if ((m & S_IFCHR) == S_IFCHR)
		cadena[0] = 'c';
	// tipo de archivo = especial de bloque
	if ((m & S_IFBLK) == S_IFBLK)
		cadena[0] = 'b';
	// tipo de archivo = enlace simbólico
	if ((m & S_IFLNK) == S_IFLNK)
		cadena[0] = 'l';
	// tipo de archivo = socket
	if ((m & S_IFSOCK) == S_IFSOCK)
		cadena[0] = 's';
	// tipo de archivo = fifo
	if ((m & S_IFIFO) == S_IFIFO)
		cadena[0] = 'f';


	//comprobamos los permisos del archivo con respecto al propietario
	if (m & S_IREAD)
		cadena[1] = 'r';
	if (m & S_IWRITE)
		cadena[2] = 'w';
	if (m & S_IEXEC)
		cadena[3] = 'x';

	//desplazamos 3 bits a la izquierda el valor del modo para seguir comprobando
	//los siguientes 3 bits correspondientes al grupo al que pertenece el propietario
	m <<= 3;
	
	//comprobamos los permisos del archivo con respecto al grupo del propietario
	if (m & S_IREAD)
		cadena[4] = 'r';
	if (m & S_IWRITE)
		cadena[5] = 'w';
	if (m & S_IEXEC)
		cadena[6] = 'x';

	//desplazamos 3 bits a la izquierda el valor del modo para seguir comprobando
	//los siguientes 3 bits correspondientes al resto de usuarios del sistema
	m <<= 3;

	//comprobamos los permisos del archivo con respecto al resto de usuarios
	if (m & S_IREAD)
		cadena[7] = 'r';
	if (m & S_IWRITE)
		cadena[8] = 'w';
	if (m & S_IEXEC)
		cadena[9] = 'x';

	//devolvemos la cadena que hemos creado
	return(cadena);
}

// Entrada: El valor del campo "d_type" (tipo archivo) de la estructura dirent
// Salida: Una cadena que muestra el tipo de archivo que es (archivo regular, directorio,
// especial de caracter,especial de directorio,fifo)
//
char *imprimir_tipo_archivo(unsigned char c)
{
	//Tipo archivo		valor
	//---------------------------
	//regular		1000
	//directorio		0100
	//especial caracter	0010
	//especial bloque	0110
	//enlace simbolico	1010
	//pipe			0001

	static char cadena[32];
		
	//inicializamos la cadena
	strcpy(cadena,"TipoArchivo");

	// tipo de archivo = archivo regular
	if ((c & 8) == 8)
		strcpy(cadena,"Archivo regular");
	// tipo de archivo = directorio
	if ((c & 4) == 4)
		strcpy(cadena,"Directorio");
	// tipo de archivo = especial de caracter
	if ((c & 2) == 2)
		strcpy(cadena,"Especial de caracter");
	// tipo de archivo = especial de bloque
	if ((c & 6) == 6)
		strcpy(cadena,"Especial de bloque");
	// tipo de archivo = enlace simbólico
	if ((c & 10) == 10)
		strcpy(cadena,"Enlace Simbolico");
	// tipo de archivo = fifo
	if ((c & 1) == 1)
		strcpy(cadena,"Fifo");

	//devolvemos la cadena que hemos creado
	return cadena;
}

// Esta funcion muestra todos los nombres de los archivos que estan dentro de un directorio
// Solamente usamos la estructuras de datos "struct dirent"
//
void imprimir_basico()
{
	DIR *dp;
	struct dirent *d;

	//obtenemos un puntero al flujo del directorio actual
	dp = opendir(".");

	//comprobamos si se ha producido algun error
	if (dp == NULL) {
		printf("\nERROR: En la funcion opendir\n");
		exit(1);
	}

	//obtenemos un puntero a la estructura del directorio a partir 
	//del puntero del flujo del directorio
	d = readdir(dp);

	//examinamos cada entrada del directorio para devolver informacion de cada archivo
	while (d) {

		//informacion sacada de la estructura "struct dirent"
		printf(" %s\n", d->d_name);

		//obtenemos un puntero a la estructura del directorio a partir 
		//del puntero del flujo del directorio
    		d = readdir(dp);
	}

	//cerramos el flujo del directorio	
	closedir(dp);
}

// Esta funcion muestra alguna la informacion que se puede sacar de los archivos
// de un directorio, usando las estructuras de datos "struct dirent" y "struct stat"
//
void imprimir_ampliado()
{
	DIR *dp;
	struct dirent *d;
	struct stat buf;

	//obtenemos un puntero al flujo del directorio actual
	dp = opendir(".");

	//comprobamos si se ha producido algun error
	if (dp == NULL) {
		printf("\nERROR: En la funcion opendir\n");
		exit(1);
	}

	//obtenemos un puntero a la estructura del directorio a partir 
	//del puntero del flujo del directorio
	d = readdir(dp);

	//examinamos cada entrada del directorio para devolver informacion de cada archivo
	while (d) {

		//obtenemos informacion del archivo en la estructura "struct stat"
		if(stat(d->d_name,&buf)==-1) {
			printf("\nERROR: En la funcion stat\n");
			exit(-1);
		}	

	    	//informacion sacada de la estructura "struct stat"
		//mostramos los permisos del archivo
    		printf("%s ",imprimir_proteccion(buf.st_mode)); 
		//mostramos el numero de enlaces que tiene
    		printf("   %d ",buf.st_nlink);
		//mostramos el identificador del propietario
    		printf(" %d ",buf.st_uid);
		//mostramos el identificador el grupo
    		printf("\t %d ",buf.st_gid);
		//mostramos el tamanyo en bytes del archivo
    		printf("\t %d",buf.st_size);
		//mostramos la fecha de ultima modificacion
    		printf("\t %s ",imprimir_fecha(&buf.st_mtime));

		//informacion sacada de la estructura "struct dirent"
		printf(" %s \n", d->d_name);

		//obtenemos un puntero a la estructura del directorio a partir 
		//del puntero del flujo del directorio
    		d = readdir(dp);
	}

	//cerramos el flujo del directorio	
	closedir(dp);
}

// Entrada: Un argumento de tipo "time_t" que representa el tiempo de calendario.  
// Al ser interpretado como un valor de tiempo absoluto, representa el número de
// segundos transcurridos desde las 00:00:00 del 1 de Enero de 1970, en
// Tiempo Universal Coordinado, Coordinated Universal Time (UTC).
// Salida: Una cadena con la fecha
//
char * imprimir_fecha(const time_t *t)
{
	static char *cadena;

	// La función ctime() convierte el tiempo de calendario timep a una cadena
        // de la forma "Wed Jun 30 21:49:08 1993\n"
        cadena = ctime(t);

	// Eliminamos el salto de linea del final de la cadena por cuestiones de impresion
	// a la hora de mostrar toda la informacion
	cadena[strlen(cadena)-1] = '\0';
	
	// Devolvemos la cadena con la fecha
	return cadena;
}
