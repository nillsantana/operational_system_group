/***************************************************************************//**
  @file         myshell.c
  @authors      Elivelton Rangel, Nilvan Santana, Matheus Nishimoto, Robson Sousa
  @brief        UNIVERSIDADE FEDERAL DA BAHIA - UFBA
                Projeto prático 04: Coordenação de Tarefas
                Sistemas Operacionais (MAT566)
*******************************************************************************/

#include "utility.c"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int c_caixa = 0;
int c_cadeira = 0;

int a_cortar[3] = {0, 0, 0};
int c_cortar = 0;

int dinhero = 0;

void *cliente(void *id){

	int tid;
	tid = (int)id;


	
	int pagou_corte = 0;

	printf("entrou na fila de fora.\n");
	sem_wait(&fila_fora);
	sem_wait(&contador_cadeira); //numero maximo de clientes dentro
	sem_wait(&em_pe);


	printf("entrou na fila em pe.\n");
	sem_post(&fila_fora);
	sem_wait(&sofa);

	printf("sentou sofa.\n");
	sem_post(&em_pe);
	sem_wait(&cadeira);
////////////////////////////////////////////////////
///corte do cabelo

	sem_wait(&a_cortar);
	int j = c_cortar;
	c_cortar = ((c_cortar + 1)%3);

	printf("sentou na cadeira");
	sem_post(&sofa);
	sem_wait(&contador_cadeira);
	c_cadeira++;
	sem_post(&contador_cadeira);

	while(cadeiras[j] != 0){
		printf("cliente %i cortando cabelo.\n", tid);
	}

	sem_post(&a_cortar);

	printf("cliente %i cortou cabelo.\n", tid);
//////////////////////////////////////////////////////
////pagamento

	printf("entrou na fila do caixa");
	sem_wait(&caixa);

	while(pagou_corte = 0);{
		printf("cliente %i esperando para pagar.\n", tid);
	}

	sem_post(&contador_cliente);
	printf("cliente %i pagou e deixou a barbearia.\n", tid);

}

void *barbeiro(void *id){

	int tid;
	tid = (int)id;

	while(TRUE){
		while((c_caixa == 0)&&(c_cadeira == 0)){
			sleep(5);
		}

		if(cadeiras[0] != 0){
			printf("cortou cabelo");
			cadeiras[0] = 0;

			sem_wait(&contador_cadeira);
			c_cadeira--;
			sem_post(&contador_cadeira);

			sem_post(&cadeira);
		}

		if(cadeiras[1] != 0){
			printf("cortou cabelo");
			cadeiras[1] = 0;

			sem_wait(&contador_cadeira);
			c_cadeira--;
			sem_post(&contador_cadeira);
		}

		if(cadeiras[2] != 0){
			printf("cortou cabelo");
			cadeiras[2] = 0;

			sem_wait(&contador_cadeira);
			c_cadeira--;
			sem_post(&contador_cadeira);
		}

		//sem_wait(&contador_cadeira);
		//if(c_cadeira != 0){
		//	printf("cortando cabelo");
		//	c_cadeira--;
		//}
		//sem_post(&contador_cadeira);

	}

}

void *caixa(){

	while(TRUE){
		while(caixa == 0){
			sleep(5);
		}
	}

	

}

int main(int argc, char *argv[]){

	sem_init(&caixa, 0, 1);
	sem_init(&cadeira, 0, 3);
	sem_init(&sofa, 0, 4);
	sem_init(&em_pe, 0, 16);
	sem_init(&fila_fora, 0, 30);

	sem_init(&contador_caixa, 0, 1);

	sem_init(&contador_cadeira, 0, 1);
	sem_init(&a_cortar, 0, 3);

	sem_init(&contador_clientes, 0, 20);


}
