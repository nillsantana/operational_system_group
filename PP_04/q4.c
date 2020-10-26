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
int a_pagar[50];
int c_pagar = 0;
int caixa_ocupado = 0;

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
	sem_wait(&contador_clientes); //numero maximo de clientes dentro
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
	sem_wait(&contador_cortar);
	int j = c_cortar;
	a_cortar[j] = 1;
	c_cortar = ((c_cortar + 1)%3);
	sem_post(&contador_cortar);

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

	sem_wait(&contador_caixa);
	k = c_pagar;
	c_pagar++;
	sem_post(&contador_caixa);

	sem_wait(&fila_caixa);
	a_pagar[k] = 1;
	sem_wait(&fila_caixa);

	c_caixa++;

	while(a_pagar[k] != 0);{
		printf("cliente %i esperando para pagar.\n", tid);
	}

	sem_post(&contador_clientes);
	printf("cliente %i pagou e deixou a barbearia.\n", tid);

}

void *barbeiro(void *id){

	int tid;
	tid = (int)id;

	while(TRUE){
		int ocupado = 0;
		while((c_caixa == 0)&&(c_cadeira == 0)){
			sleep(5);
		}

		sem_wait(&acesso_caixa);
		if((c_caixa != 0) && (caixa_ocupado == 0)){
			ocupado = 1;
			caixa_ocupado = 1;
			sem_post(&acesso_caixa);
			
			printf("barbeiro %i recebendo pagamento.\n", tid);

		}

		if (ocupado == 0){
			sem_wait(&cadeira_1);
			if(cadeiras[0] != 0){
				ocupado = 1;
				printf("cortou cabelo.\n");
				cadeiras[0] = 0;
				sem_post(&cadeira_1);

				sem_wait(&contador_cadeira);
				c_cadeira--;
				sem_post(&contador_cadeira);

				sem_post(&cadeira);
			}
		}

		if (ocupado == 0){
			sem_wait(&cadeira_2);
			if(cadeiras[1] != 0){
				ocupado = 1;
				printf("cortou cabelo.\n");
				cadeiras[1] = 0;
				sem_post(&cadeira_2);
	
				sem_wait(&contador_cadeira);
				c_cadeira--;
				sem_post(&contador_cadeira);
			}
		}

		if (ocupado == 0){
			sem_wait(&cadeira_3);
			if(cadeiras[2] != 0){
				ocupado = 1;
				printf("cortou cabelo.\n");
				cadeiras[2] = 0;
				sem_post(&cadeira_3);

				sem_wait(&contador_cadeira);
				c_cadeira--;
				sem_post(&contador_cadeira);
			}
		}

		ocupado = 0;

		//sem_wait(&contador_cadeira);
		//if(c_cadeira != 0){
		//	printf("cortando cabelo");
		//	c_cadeira--;
		//}
		//sem_post(&contador_cadeira);

	}

}

void *caixa(void *vargp){

	int n_cliente = 0;

	while(TRUE){
		while(caixa_ocupado == 0){
			sleep(5);
		}

		n_cliente = a_pagar[c_pagar];

		a_pagar[c_pagar] = 0;
		dinheiro = dinheiro + 20;

		printf("Emitido o recibo ao cliente %i.\n", n_cliente);
		
		n_cliente = 0;

		sem_wait(&acesso_caixa);
		caixa_ocupado = 0;
		sem_post(&acesso_caixa);
		printf("Barbeiro deixou caixa.\n");

		sem_post(&caixa);

	}

}

int main(int argc, char *argv[]){

	sem_init(&caixa, 0, 1);
	sem_init(&cadeira, 0, 3);
	sem_init(&sofa, 0, 4);
	sem_init(&em_pe, 0, 16);
	sem_init(&fila_fora, 0, 30);

	sem_init(&contador_caixa, 0, 1);
	sem_init(&fila_caixa, 0, 1);
	sem_init(&acesso_caixa, 0, 1);

	sem_init(&contador_cadeira, 0, 1);
	sem_init(&contador_cortar, 0, 1);
	sem_init(&a_cortar, 0, 3);

	sem_init(&cadeira_1, 0, 1);
	sem_init(&cadeira_2, 0, 1);
	sem_init(&cadeira_3, 0, 1);

	sem_init(&contador_clientes, 0, 20);

	int i = 1;

	pthread_t thread_clientes;
	int clientes;
	clientes = pthread_create(&threads_clientes, NULL, cliente, (void *)&i);
	pthread_join(threads_clientes, NULL);

	pthread_t thread_barbeiros;
	int barbeiros;
	barbeiros = pthread_create(&threads_barbeiros, NULL, cliente, (void *)&i);
	pthread_join(threads_barbeiros, NULL);

	pthread_t thread_caixa;
	int caixa;
	caixa = pthread_create(&thread_caixa, NULL, caixa, NULL);
	pthread_join(thread_caixa, NULL);

	//pthread_exit(NULL);

	//return(0);

}
