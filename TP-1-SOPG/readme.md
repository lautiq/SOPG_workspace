# Comunicación entre procesos en Linux usando FIFO

Estos dos archivos en C implementan una comunicación entre procesos utilizando un FIFO (First In, First Out) o tubería con nombre en un entorno de Linux. 
Una breve explicacion de su funcionamiento: 

## `writer.c`

1. **Creación del FIFO:**
   - Intenta crear un FIFO llamado "fifotp" usando `mknod`.
   - Si hay un error al crear el FIFO, imprime un mensaje de error y sale del programa.

2. **Apertura del FIFO para escritura:**
   - Espera hasta que otro proceso (lector) abra el FIFO para lectura.
   - Si hay un error al abrir el FIFO, imprime un mensaje de error y sale del programa.

3. **Manejo de señales:**
   - Instala manejadores de señales para `SIGUSR1` y `SIGUSR2`. Cuando se recibe una de estas señales, envía un mensaje al FIFO indicando la señal recibida.

4. **Bucle principal:**
   - Entra en un bucle infinito.
   - Lee la entrada del usuario desde la consola.
   - Si la entrada es una nueva línea (ENTER), formatea el mensaje como "DATA:" seguido del texto y lo escribe en el FIFO.

## `reader.c`

1. **Creación del FIFO:**
   - Intenta crear un FIFO llamado "fifotp" usando `mknod`.
   - Si hay un error al crear el FIFO, imprime un mensaje de error y sale del programa.

2. **Apertura del FIFO para lectura:**
   - Espera hasta que otro proceso (escritor) abra el FIFO para escritura.
   - Si hay un error al abrir el FIFO, imprime un mensaje de error y sale del programa.

3. **Manejo de señales:**
   - Instala un manejador de señales para `SIGINT` (interrupción del usuario). Cuando se recibe esta señal, imprime un mensaje y sale del programa.

4. **Apertura de archivos de registro:**
   - Abre dos archivos de registro, uno para datos (`log.txt`) y otro para señales (`signal.txt`), en modo de escritura al final.

5. **Bucle principal:**
   - Entra en un bucle hasta que la lectura del FIFO devuelve un valor menor o igual a 0.
   - Lee datos del FIFO.
   - Si el mensaje comienza con "DATA:", lo registra en el archivo de registro de texto.
   - Si el mensaje comienza con "SIGN:", lo registra en el archivo de registro de señales.
   - Imprime en la consola el mensaje leído.

6. **Manejo de señales:**
   - Si se recibe la señal `SIGINT` (Ctrl+C), imprime un mensaje y sale del programa.

7. **Cierre de archivos:**
   - Cierra los archivos de registro.

Este proyecto permite la transferencia de mensajes de texto entre procesos y registra las señales recibidas en archivos de registro correspondientes.
