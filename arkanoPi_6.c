/** File Name          : arkanoPi_3.c
  * Description        : Main program body
  */
/* Includes ------------------------------------------------------------------*/
#include "arkanoPi_6.h"

/* Private variables ---------------------------------------------------------*/
static volatile tipo_juego juego;

volatile int flags = 0;

timer_t timerDisplay;
timer_t timerPelota;
timer_t timerRaqueta;

tipo_pantalla pantalla_inicial = {
		{
			{1,0,0,0,0,0,1},
			{1,1,0,0,0,1,1},
			{0,1,1,0,1,1,0},
			{0,0,1,1,1,0,0},
			{1,0,0,1,0,0,1},
			{1,1,0,0,0,1,1},
			{0,1,1,0,1,1,0},
			{0,0,1,1,1,0,0},
			{0,0,0,1,0,0,0},
			{0,0,0,0,0,0,0},
		}
};
tipo_pantalla pantalla_gameover = {
		{
			{0,0,0,0,0,0,0},
			{1,0,0,0,0,0,1},
			{1,1,0,0,0,1,1},
			{0,1,1,1,1,1,0},
			{0,0,1,1,1,0,0},
			{0,0,1,1,1,0,0},
			{0,1,1,1,1,1,0},
			{1,1,0,0,0,1,1},
			{1,0,0,0,0,0,1},
			{0,0,0,0,0,0,0},
		}
};
tipo_pantalla pantalla_victoria = {
		{
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
			{0,1,1,0,1,1,0},
			{1,1,0,0,1,1,0},
			{1,1,0,0,0,0,0},
			{1,1,0,0,0,0,0},
			{1,1,0,0,1,1,0},
			{0,1,1,0,1,1,0},
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
		}
};
/* Defines -------------------------------------------------------------------*/
#define GPIO_FILA_1 0
#define GPIO_FILA_2 1
#define GPIO_FILA_3 2
#define GPIO_FILA_4 3
#define GPIO_FILA_5 4
#define GPIO_FILA_6 7
#define GPIO_FILA_7 23

#define GPIO_COL_1 14
#define GPIO_COL_2 17
#define GPIO_COL_3 18
#define GPIO_COL_4 22

static int GPIO_FILAS [7] = {GPIO_FILA_1, GPIO_FILA_2, GPIO_FILA_3, GPIO_FILA_4, GPIO_FILA_5, GPIO_FILA_6, GPIO_FILA_7};
static int GPIO_COLUMNAS [4] = {GPIO_COL_1, GPIO_COL_2, GPIO_COL_3, GPIO_COL_4};

//Constantes temporizadores
#define DEBOUNCE_TIME 50
int debounceTime = 0;
//Pines de las interrupciones
#define GPIO_PALA_DERECHA 20
#define GPIO_PALA_IZQUIERDA 21
//Constantes configuracion SPI
#define SPI_ADC_CH 0
#define SPI_ADC_FREQ 1000000

//Primera columna a encender
int columna = 0;

int areaPala(tipo_raqueta *p_pala){
	int ancho = p_pala->ancho;
	int alto = p_pala->alto;
	return  ancho*alto;
}
/**
 * @brief			Funcion encargada de parar la ejecucion del programa por un tiempo determinado
 * @param	next    Momento en el que seguir con el programa
 */
void delay_until (unsigned int next) {
	unsigned int now = millis();

	if (next > now) {
		delay (next - now);
    }
}
//------------------------------------------------------
// FUNCIONES DE ENCENDIDO DE LEDS
//------------------------------------------------------
/**
 * @brief			Funcion encargada de escribir en los pines GPIO correspondientes el valor que selecciona la columna indicada por el parametro
 * @param	numero  Columna que se busca excitar. En binario determina cada uno de los bits a poner a nivel alto o bajo.
 * @note			las columnas estan codificadas en 4 bits. Van de 0 a 9
 */
void enciende_columna(int numero){
	digitalWrite(GPIO_COLUMNAS[0],numero & 0x01);//Bit de seleccion 1
	digitalWrite(GPIO_COLUMNAS[1],numero & 0x02);//Bit de seleccion 2
	digitalWrite(GPIO_COLUMNAS[2],numero & 0x04);//Bit de seleccion 3
	digitalWrite(GPIO_COLUMNAS[3],numero & 0x08);//Bit de seleccion 4
	/**switch (numero) {
		case 0:
			digitalWrite(GPIO_COLUMNAS[0],0);//Bit de seleccion 1
			digitalWrite(GPIO_COLUMNAS[1],0);//Bit de seleccion 2
			digitalWrite(GPIO_COLUMNAS[2],0);//Bit de seleccion 3
			digitalWrite(GPIO_COLUMNAS[3],0);
			break;
		case 1:
					digitalWrite(GPIO_COLUMNAS[0],1);//Bit de seleccion 1
					digitalWrite(GPIO_COLUMNAS[1],0);//Bit de seleccion 2
					digitalWrite(GPIO_COLUMNAS[2],0);//Bit de seleccion 3
					digitalWrite(GPIO_COLUMNAS[3],0);
					break;
		case 2:
					digitalWrite(GPIO_COLUMNAS[0],0);//Bit de seleccion 1
					digitalWrite(GPIO_COLUMNAS[1],1);//Bit de seleccion 2
					digitalWrite(GPIO_COLUMNAS[2],0);//Bit de seleccion 3
					digitalWrite(GPIO_COLUMNAS[3],0);
					break;
		case 3:
					digitalWrite(GPIO_COLUMNAS[0],1);//Bit de seleccion 1
					digitalWrite(GPIO_COLUMNAS[1],1);//Bit de seleccion 2
					digitalWrite(GPIO_COLUMNAS[2],0);//Bit de seleccion 3
					digitalWrite(GPIO_COLUMNAS[3],0);
					break;
		case 4:
					digitalWrite(GPIO_COLUMNAS[0],0);//Bit de seleccion 1
					digitalWrite(GPIO_COLUMNAS[1],0);//Bit de seleccion 2
					digitalWrite(GPIO_COLUMNAS[2],1);//Bit de seleccion 3
					digitalWrite(GPIO_COLUMNAS[3],0);
					break;
		case 5:
					digitalWrite(GPIO_COLUMNAS[0],1);//Bit de seleccion 1
					digitalWrite(GPIO_COLUMNAS[1],0);//Bit de seleccion 2
					digitalWrite(GPIO_COLUMNAS[2],1);//Bit de seleccion 3
					digitalWrite(GPIO_COLUMNAS[3],0);
					break;
		case 6:
					digitalWrite(GPIO_COLUMNAS[0],0);//Bit de seleccion 1
					digitalWrite(GPIO_COLUMNAS[1],1);//Bit de seleccion 2
					digitalWrite(GPIO_COLUMNAS[2],1);//Bit de seleccion 3
					digitalWrite(GPIO_COLUMNAS[3],0);
					break;
		case 7:
					digitalWrite(GPIO_COLUMNAS[0],1);//Bit de seleccion 1
					digitalWrite(GPIO_COLUMNAS[1],1);//Bit de seleccion 2
					digitalWrite(GPIO_COLUMNAS[2],1);//Bit de seleccion 3
					digitalWrite(GPIO_COLUMNAS[3],0);
					break;
		case 8:
					digitalWrite(GPIO_COLUMNAS[0],0);//Bit de seleccion 1
					digitalWrite(GPIO_COLUMNAS[1],0);//Bit de seleccion 2
					digitalWrite(GPIO_COLUMNAS[2],0);//Bit de seleccion 3
					digitalWrite(GPIO_COLUMNAS[3],1);
					break;
		case 9:
					digitalWrite(GPIO_COLUMNAS[0],1);//Bit de seleccion 1
					digitalWrite(GPIO_COLUMNAS[1],0);//Bit de seleccion 2
					digitalWrite(GPIO_COLUMNAS[2],0);//Bit de seleccion 3
					digitalWrite(GPIO_COLUMNAS[3],1);
					break;
	}**/
}
/**
 * @brief			Funcion encargada de poner a nivel bajo una fila determinada
 * @param	fila    Fila que se quiere poner a nivel bajo.
 */
void enciende_fila(int fila){
	digitalWrite(GPIO_FILAS[fila],0);
}
/**
 * @brief			Funcion encargada de poner a nivel alto una fila determinada
 * @param	fila    Fila que se quiere poner a nivel alto.
 */
void apaga_fila(int fila){
	digitalWrite(GPIO_FILAS[fila],1);
}
//------------------------------------------------------
// FUNCIONES DE ATENCION A INTERRUPCIONES
//------------------------------------------------------
/**
 * @brief			Subrutina de atencion a la interrupcion del pulsador de movimiento de la pala.
 * Activa el flag FLAG_RAQUETA_DERECHA para que la maquina de estados ejecute la rutina adecuada
 */
void int_pala_derecha(){
	if (millis () < debounceTime)
	{
	debounceTime = millis () + DEBOUNCE_TIME ;
	return;
	}
	piLock (FLAGS_KEY);
	flags |= FLAG_RAQUETA_DERECHA;
	flags |= FLAG_TECLA;
	piUnlock (FLAGS_KEY);
	printf("Interrupcion pala derecha.");
	fflush(stdout);
	while (digitalRead (6) == HIGH) {//Pin 6 asociado a teclado matricial
	delay (1) ;
	}
	debounceTime = millis () + DEBOUNCE_TIME ;
}
/**
 * @brief			Subrutina de atencion a la interrupcion del pulsador de movimiento de la pala.
 * Activa el flag FLAG_RAQUETA_IZQUIERDA para que la maquina de estados ejecute la rutina adecuada
 */
void int_pala_izquierda(){
	if (millis () < debounceTime)
		{
		debounceTime = millis () + DEBOUNCE_TIME ;
		return;
	}
	piLock (FLAGS_KEY);
	flags |= FLAG_RAQUETA_IZQUIERDA;
	flags |= FLAG_TECLA;
	piUnlock (FLAGS_KEY);
	printf("Interrupcion pala izquierda.");
	fflush(stdout);
	while (digitalRead (5) == HIGH) {//Pin 5 asociado a teclado matricial
		delay (1) ;
	}
	debounceTime = millis () + DEBOUNCE_TIME ;
}
//------------------------------------------------------
// FUNCIONES DE COMPROBACION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
/**
 * @brief			Comprueba el estado del flag FLAG_TECLA
 * @return			1 si el flag esta activo. 0 en caso contrario
 *
 */
int comprueba_tecla_pulsada (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_TECLA);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
 * @brief			Comprueba el estado del flag FLAG_PELOTA
 * @return			1 si el flag esta activo. 0 en caso contrario
 *
 */
int comprueba_tecla_pelota (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_PELOTA);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
 * @brief			Comprueba el estado del flag FLAG_RAQUETA_DERECHA
 * @return			1 si el flag esta activo. 0 en caso contrario
 *
 */
int comprueba_tecla_raqueta_derecha (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_DERECHA);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
 * @brief			Comprueba el estado del flag FLAG_RAQUETA_IZQUIERDA
 * @return			1 si el flag esta activo. 0 en caso contrario
 *
 */
int comprueba_tecla_raqueta_izquierda (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_IZQUIERDA);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
 * @brief			Comprueba el estado del flag FLAG_FINAL_JUEGO
 * @return			1 si el flag esta activo. 0 en caso contrario
 *
 */
int comprueba_final_juego (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_FINAL_JUEGO);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
 * @brief			Comprueba el estado del flag FLAG_JOYSTICK
 * @return			1 si el flag esta activo. 0 en caso contrario
 *
 */
int comprueba_joystick (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_JOYSTICK);
	piUnlock (FLAGS_KEY);

	return result;
}
//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------

/**
 * @brief			Funcion encargada de reiniciar el juego, o llevarlo al estado inicial en caso de que sea la primera ejecucion.
 * Esta funcion se llama desde la maquina de estados cuando se pulsa cualquier tecla en el estado WAIT_START
 */
void InicializaJuego (fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);
	// A completar por el alumno...

	//PintaMensajeInicialPantalla((tipo_pantalla *)&juego.arkanoPi.pantalla, (tipo_pantalla *)&pantalla_inicial);
	//PintaPantallaPorTerminal((tipo_pantalla *)&juego.arkanoPi.pantalla);
	InicializaArkanoPi((tipo_arkanoPi *) &juego.arkanoPi);
	PintaPantallaPorTerminal((tipo_pantalla *)&juego.arkanoPi.pantalla);
	timer_pelota_start(500);
	timer_raqueta_start(100);
	juego.estado = WAIT_PUSH;

}

/**
 * @brief			Funcion encargada de actualizar la posicion de la raqueta una unidad a la izquierda. Adicionalmente
 * comprueba que no exceda los limites de la pantalla no haciendo nada ese caso.
 */
void MueveRaquetaIzquierda (fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_KEY);
	int ancho = juego.arkanoPi.raqueta.ancho;
	int x = juego.arkanoPi.raqueta.x;
	if(x + (ancho-1) <= 0){
		return;
	}
	piLock(JUEGO_KEY);
	juego.arkanoPi.raqueta.x = juego.arkanoPi.raqueta.x - 1;
	ActualizaPantalla((tipo_arkanoPi*)&juego.arkanoPi);
	piUnlock(JUEGO_KEY);
	PintaPantallaPorTerminal((tipo_pantalla *)&juego.arkanoPi.pantalla);

}

/**
 * @brief			Funcion encargada de actualizar la posicion de la raqueta una unidad a la derecha. Adicionalmente
 * comprueba que no exceda los limites de la pantalla no haciendo nada ese caso.
 */
void MueveRaquetaDerecha (fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	piUnlock (FLAGS_KEY);
	// A completar por el alumno...
		int x = juego.arkanoPi.raqueta.x;
		if(x >= 9){
			return;
		}
		piLock(JUEGO_KEY);
		juego.arkanoPi.raqueta.x = juego.arkanoPi.raqueta.x + 1;
		ActualizaPantalla((tipo_arkanoPi*)&juego.arkanoPi);
		piUnlock(JUEGO_KEY);
		PintaPantallaPorTerminal((tipo_pantalla *)&juego.arkanoPi.pantalla);

}
/**
 * @brief	Funcion encargada de mover la raqueta a una posicion determinada. Sera llamada por
 * 			la rutina que controla el Joystick.
 */
void MueveRaqueta(int posicion){
	int ancho=juego.arkanoPi.raqueta.ancho;
	if(posicion + (ancho-1) <= 0 || posicion > MATRIZ_ANCHO-1){
		printf("Posicion de raquera erronea");
		return;
	}
	juego.arkanoPi.raqueta.x = posicion;
	ActualizaPantalla((tipo_arkanoPi*)&juego.arkanoPi);

}

/**
 * @brief			Funcion de actualizacion del movimiento de la pelota. Primero comprueba si hay rebote contra
 * alguna pared. Llevando al fin del juego si es la pared inferior. Luego comprueba si hay rebote contra algun ladrillo,
 * llevando al fin del juego con mensake de victoria si no quedan mas. Por ultimo aplica los rebotes especiales que suceden en cada
 * punto de la raqueta.
 * @note			Usa la variable global juego para modificar la posicion y velocidad de la pelota
 *
 */
void MovimientoPelota (fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_PELOTA;
	piUnlock (FLAGS_KEY);
	// A completar por el alumno...
	int x = juego.arkanoPi.pelota.x;
	int y = juego.arkanoPi.pelota.y;
	int xv = juego.arkanoPi.pelota.xv;
	int yv = juego.arkanoPi.pelota.yv;

	int izqPala = juego.arkanoPi.raqueta.x;
	int derPala = juego.arkanoPi.raqueta.x + juego.arkanoPi.raqueta.ancho -1;
	tipo_pantalla ladrillos = juego.arkanoPi.ladrillos;

	int sigX = x + xv;
	int sigY = y + yv;
	int sigXV = xv;
	int sigYV = yv;

	//Rebote contra paredes y fin del juego si supera limite inferior
	if(sigX > MATRIZ_ANCHO -1 || sigX < 0){
		sigXV = -xv;
		sigX = x + sigXV;

	}
	if(sigY < 0){
		sigYV = -yv;
		sigY = y + sigYV;
	}
	if(sigY > MATRIZ_ALTO -1){
		printf("Game Over...\n");
		piLock (FLAGS_KEY);
		flags |= FLAG_FINAL_JUEGO;
		piUnlock (FLAGS_KEY);
		piLock(JUEGO_KEY);
		PintaMensajeInicialPantalla((tipo_pantalla *)&juego.arkanoPi.pantalla,&pantalla_gameover);
		piUnlock(JUEGO_KEY);
		return;
	}

	//Rebote contra ladrillo
	if(ladrillos.matriz[sigX][sigY] == 1){
		juego.arkanoPi.ladrillos.matriz[sigX][sigY]=0;
		int restantes = CalculaLadrillosRestantes((tipo_pantalla*) &juego.arkanoPi.ladrillos);
		if(restantes <=15){
			printf("Has ganado! \n");
			piLock (FLAGS_KEY);
			flags |= FLAG_FINAL_JUEGO;
			piUnlock (FLAGS_KEY);
			piLock(JUEGO_KEY);
			PintaMensajeInicialPantalla((tipo_pantalla *)&juego.arkanoPi.pantalla,&pantalla_victoria);
			piUnlock(JUEGO_KEY);
			return;
		}
		sigYV = -yv;
		sigY = y + sigYV;
	}

	//Rebote contra pala
	if(sigY == 6){
		if(sigX == izqPala){
			sigXV = -1;
			sigX = x + sigXV;
			sigYV = -yv;
			sigY = y + sigYV;
		}
		if(sigX == derPala){
			sigXV = 1;
			sigX = x + sigXV;
			sigYV = -yv;
			sigY = y + sigYV;
		}
		if(sigX>izqPala && sigX<derPala){
			sigXV = 0;
			sigX = x + sigXV;
			sigYV = -yv;
			sigY = y + sigYV;
		}
	}

	piLock(JUEGO_KEY);
	juego.arkanoPi.pelota.x = sigX;
	juego.arkanoPi.pelota.y = sigY;
	juego.arkanoPi.pelota.xv = sigXV;
	juego.arkanoPi.pelota.yv = sigYV;

	ActualizaPantalla((tipo_arkanoPi*)&juego.arkanoPi);
	piUnlock(JUEGO_KEY);

	PintaPantallaPorTerminal((tipo_pantalla *)&juego.arkanoPi.pantalla);

}

/**
 * @brief			Lleva el juego al estado final. Pinta por pantalla mensaje de fin de juego
 */
void FinalJuego (fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_FINAL_JUEGO;
	piUnlock (FLAGS_KEY);
	timer_delete(timerPelota);
	printf("Fin del juego.\nPulsa una tecla para volver a empezar\n");
	juego.estado=WAIT_END;


}

/**
 * @brief			Devuelve el juego al estado de espera inicial.
 */
void ReseteaJuego (fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);
	//InicializaArkanoPi((tipo_arkanoPi *) &juego.arkanoPi);
	//PintaPantallaPorTerminal((tipo_pantalla *)&juego.arkanoPi.pantalla);
	printf("A la espera de recomenzar. Pulse una tecla para continuar...");
	piLock (JUEGO_KEY);
	PintaMensajeInicialPantalla((tipo_pantalla *)&juego.arkanoPi.pantalla,&pantalla_inicial);
	juego.estado=WAIT_START;
	piUnlock (JUEGO_KEY);
}

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------

/**
 * @brief			Inicializa el HW necesario para el programa: Interrupcion periodica del display,
 * inicializa el modo de los pines GPIO, crea thread de lectura del teclado, configura interrupciones externas
 *
 */

int systemSetup (void) {
	int x = 0;
		piLock (STD_IO_BUFFER_KEY);

		// sets up the wiringPi library
		if (wiringPiSetupGpio() < 0) {
			printf ("Unable to setup wiringPi\n");
			piUnlock (STD_IO_BUFFER_KEY);
			return -1;
	    }
		//sets up wiringPiSPI protocol
		if (wiringPiSPISetup (SPI_ADC_CH, SPI_ADC_FREQ) < 0) { //Conexion del canal 0(GPIO 08 en numeracion BCM) a 1 MHz
			 printf ("No se pudo inicializar el dispositivo SPI (CH 0)") ;
			 exit (1);
			 return -2;
		}
		//Configuracion pines GPIO entrada
		pinMode(GPIO_PALA_DERECHA,INPUT);
		pinMode(GPIO_PALA_IZQUIERDA,INPUT);

		pullUpDnControl(GPIO_PALA_DERECHA,PUD_DOWN);
		pullUpDnControl(GPIO_PALA_IZQUIERDA,PUD_DOWN);

		wiringPiISR(GPIO_PALA_DERECHA,INT_EDGE_RISING,&int_pala_derecha);
		wiringPiISR(GPIO_PALA_IZQUIERDA,INT_EDGE_RISING,&int_pala_izquierda);

		//Configuracion para uso de teclado matricial como entrada
		//Pines asociados a teclado matricial
		pinMode(5,INPUT);
		pinMode(6,INPUT);
		pinMode(0,OUTPUT);//Salida para tener siempre una columna a nivel alto

		pullUpDnControl(5,PUD_DOWN);
		pullUpDnControl(6,PUD_DOWN);

		wiringPiISR(6,INT_EDGE_RISING,&int_pala_derecha);
		wiringPiISR(5,INT_EDGE_RISING,&int_pala_izquierda);
		digitalWrite(0,HIGH);
		//Configuracion pines GPIO salida
		//Pines filas
		pinMode(GPIO_FILA_1,OUTPUT);
		pinMode(GPIO_FILA_2,OUTPUT);
		pinMode(GPIO_FILA_3,OUTPUT);
		pinMode(GPIO_FILA_4,OUTPUT);
		pinMode(GPIO_FILA_5,OUTPUT);
		pinMode(GPIO_FILA_6,OUTPUT);
		pinMode(GPIO_FILA_7,OUTPUT);
		//Pines columnas
		pinMode(GPIO_COL_1,OUTPUT);
		pinMode(GPIO_COL_2,OUTPUT);
		pinMode(GPIO_COL_3,OUTPUT);
		pinMode(GPIO_COL_4,OUTPUT);

		//Inicializacion temporizador refresco pantalla
		timer_display_start(1);

		// Lanzamos thread para exploracion del teclado convencional del PC
		x = piThreadCreate (thread_explora_teclado);

		if (x != 0) {
			printf ("it didn't start!!!\n");
			piUnlock (STD_IO_BUFFER_KEY);
			return -1;
	    }

		piUnlock (STD_IO_BUFFER_KEY);

		return 1;
}
/**
 * @brief			Inicializa la máquina de estados poniendo a 0 todos los flags
 * @param	luz_fsm
 */
void fsm_setup(fsm_t* fsm_arkanoPi) {
	piLock (FLAGS_KEY);
	flags = 0;
	piUnlock (FLAGS_KEY);

	//Inicio juego
	//InicializaJuego((fsm_t*)&fsm_arkanoPi);
	//InicializaJuego(fsm_arkanoPi);



	piLock (STD_IO_BUFFER_KEY);
	printf("\n'p' para mover la pelota\n'i' para mover a la izquierda\n'o' para mover a la derecha\n");
	piUnlock (STD_IO_BUFFER_KEY);
}

/**
 * @brief			Rutina de atencion a la interrupcion periodica para la acrtualizacion del display.
 * Para cada columna comprueba que filas deben estar encendidas y ejecuta las subrutinas adecuadas.
 */
static void timer_display_isr (union sigval arg) {
	if(columna >9){
		columna =0;
	}
	int i;
	tipo_pantalla pantalla = juego.arkanoPi.pantalla;
	for(i = 0; i < MATRIZ_ALTO ; i++){
		if(pantalla.matriz[columna][i]){
			enciende_fila(i);
		} else {
			apaga_fila(i);
		}
	}
	enciende_columna(columna);
	columna++;

}
/**
 * @brief			Inicializa el temporizador asociado a la interrupcion periodica del display
 * @param	ms		Intervalo de tiempo entre interrupciones
 */
static int timer_display_start(int ms){

	int result = 2;

	struct itimerspec spec;
	struct sigevent se;

	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = &timerDisplay;
	se.sigev_notify_function = timer_display_isr;
	se.sigev_notify_attributes = NULL;

	spec.it_value.tv_sec = ms / 1000;
	spec.it_value.tv_nsec = (ms % 1000) * 1000000;
	spec.it_interval.tv_sec = ms / 1000;
	spec.it_interval.tv_nsec = (ms % 1000) * 1000000;

	result = timer_create (CLOCK_REALTIME, &se, &timerDisplay); /* o CLOCK_MONOTONIC si se soporta */
	printf("result after create = %d\n", result);
	fflush(stdout);
	result = timer_settime (timerDisplay, 0, &spec, NULL);
	printf("result after settime = %d\n", result);
	fflush(stdout);

	return result;
}
/**
 * @brief			Rutina de atencion a la interrupcion periodica para la acrtualizacion del display.
 * Para cada columna comprueba que filas deben estar encendidas y ejecuta las subrutinas adecuadas.
 */
static void timer_pelota_isr (union sigval arg) {
	piLock (FLAGS_KEY);
	flags |= FLAG_PELOTA;
	piUnlock (FLAGS_KEY);
}
/**
 * @brief			Inicializa el temporizador asociado a la interrupcion periodica del display
 * @param	ms		Intervalo de tiempo entre interrupciones
 */
static int timer_pelota_start(int ms){

	int result = 2;

	struct itimerspec spec;
	struct sigevent se;

	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = &timerPelota;
	se.sigev_notify_function = timer_pelota_isr;
	se.sigev_notify_attributes = NULL;

	spec.it_value.tv_sec = ms / 1000;
	spec.it_value.tv_nsec = (ms % 1000) * 1000000;
	spec.it_interval.tv_sec = ms / 1000;
	spec.it_interval.tv_nsec = (ms % 1000) * 1000000;

	result = timer_create (CLOCK_REALTIME, &se, &timerPelota); /* o CLOCK_MONOTONIC si se soporta */
	printf("result after create = %d\n", result);
	fflush(stdout);
	result = timer_settime (timerPelota, 0, &spec, NULL);
	printf("result after settime = %d\n", result);
	fflush(stdout);

	return result;
}
/**
 * @brief			Rutina de atencion a la interrupcion periodica que comprueba la tension en el ADC para
 * 					mover la raqueta
 * Para cada columna comprueba que filas deben estar encendidas y ejecuta las subrutinas adecuadas.
 */
static void timer_raqueta_isr (union sigval arg) {

	float voltaje = lecturaADC();
	float max = 1.3;
	float intervalo = max/10;

	int posicion = (int) voltaje/intervalo;
	MueveRaqueta(posicion);
}
/**
 * @brief			Inicializa el temporizador asociado a la interrupcion periodica de la raqueta
 * @param	ms		Intervalo de tiempo entre interrupciones
 */
static int timer_raqueta_start(int ms){

	int result = 2;

	struct itimerspec spec;
	struct sigevent se;

	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = &timerRaqueta;
	se.sigev_notify_function = timer_raqueta_isr;
	se.sigev_notify_attributes = NULL;

	spec.it_value.tv_sec = ms / 1000;
	spec.it_value.tv_nsec = (ms % 1000) * 1000000;
	spec.it_interval.tv_sec = ms / 1000;
	spec.it_interval.tv_nsec = (ms % 1000) * 1000000;

	result = timer_create (CLOCK_REALTIME, &se, &timerRaqueta); /* o CLOCK_MONOTONIC si se soporta */
	printf("result after create = %d\n", result);
	fflush(stdout);
	result = timer_settime (timerRaqueta, 0, &spec, NULL);
	printf("result after settime = %d\n", result);
	fflush(stdout);

	return result;
}
/**
 * @brief			Realiza la lectura del ADC en la entrenadora mediante el protocolo SPI
 */
float lectura_ADC(void){
	unsigned char ByteSPI[3]; //Buffer lectura escritura SPI
	int resultado_SPI = 0; //Control operacion SPI
	float voltaje_medido = 0.0; //Valor medido. A calcular a partir del buffer

	ByteSPI[0] = 0b10011111; // Configuracion ADC (10011111 unipolar, 0-2.5v,canal 0, salida 1), bipolar 0b10010111
	ByteSPI[1] = 0b0;
	ByteSPI[2] = 0b0;

	resultado_SPI = wiringPiSPIDataRW (SPI_ADC_CH, ByteSPI, 3);//Enviamos y leemos tres bytes (8+12+4 bits)
	usleep(20);

	int salida_SPI = ((ByteSPI[1] << 5) | (ByteSPI[2] >> 3)) & 0xFFF;

	/*Caso unipolar */
	voltaje_medido = 2*2.50 * (((float) salida_SPI)/4095.0);
	return voltaje_medido;
}
/**
 * @brief			Crea thread adicional para la lectura del teclado
 */
PI_THREAD (thread_explora_teclado) {
	int teclaPulsada;

	while(1) {
		delay(10); // Wiring Pi function: pauses program execution for at least 10 ms

		piLock (STD_IO_BUFFER_KEY);

		if(kbhit()) {
			teclaPulsada = kbread();

			switch(teclaPulsada) {
				case 'i':
					piLock (FLAGS_KEY);
					flags |= FLAG_RAQUETA_IZQUIERDA;
					flags |= FLAG_TECLA;
					piUnlock (FLAGS_KEY);
					break;
				case 'o':
					piLock (FLAGS_KEY);
					flags |= FLAG_RAQUETA_DERECHA;
					flags |= FLAG_TECLA;
					piUnlock (FLAGS_KEY);
					break;
				case 'p':
					piLock (FLAGS_KEY);
					flags |= FLAG_PELOTA;
					flags |= FLAG_TECLA;
					piUnlock (FLAGS_KEY);
					break;
				case 'q':
					exit(0);
					break;

				default:
					printf("INVALID KEY!!!\n");
					break;
			}
		}

		piUnlock (STD_IO_BUFFER_KEY);
	}
}

/**
 * @brief			Programa principal para ejecutar. Bucle infinito que ejecuta una maquina de estados
 */
int main ()
{
	// Configuracion e inicializacion del sistema
	unsigned int next;

	// Maquina de estados: lista de transiciones
	// {EstadoOrigen,FunciónDeEntrada,EstadoDestino,FunciónDeSalida}
	fsm_trans_t tabla_juego[] = {
			{ WAIT_START,   comprueba_tecla_pulsada,  WAIT_PUSH, InicializaJuego },
			{ WAIT_END,   comprueba_tecla_pulsada,  WAIT_START, ReseteaJuego },
			{ WAIT_PUSH,   comprueba_tecla_pelota,  WAIT_PUSH, MovimientoPelota },
			{ WAIT_PUSH,   comprueba_tecla_raqueta_izquierda,  WAIT_PUSH, MueveRaquetaIzquierda },
			{ WAIT_PUSH,   comprueba_tecla_raqueta_derecha,  WAIT_PUSH, MueveRaquetaDerecha },
			{ WAIT_PUSH,   comprueba_joystick,  WAIT_PUSH, MueveRaquetaDerecha },
			{ WAIT_PUSH,   comprueba_final_juego,  WAIT_END, FinalJuego },
			{ -1, NULL, -1, NULL },
	};

	fsm_t* fsm_arkanopi = fsm_new (WAIT_START, tabla_juego, NULL);

	//Setup general del sistema
	systemSetup();
	//Setup maquina de estados
	fsm_setup(fsm_arkanopi);
	//Pinta pantalla inicial
	piLock (JUEGO_KEY);
	PintaMensajeInicialPantalla((tipo_pantalla *)&juego.arkanoPi.pantalla,&pantalla_inicial);
	piUnlock (JUEGO_KEY);

	//Bucle infinito
	next = millis();
	while (1) {
		fsm_fire (fsm_arkanopi);
		next += CLK_MS;
		delay_until (next);
	}
	fsm_destroy (fsm_arkanopi);
	return 0;
}