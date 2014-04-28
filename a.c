#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "listaligada.h"

#define NUMEV					10000000
#define PI					3.14159265358979323846
#define LAMBDA 					1
#define CANAIS 					7
#define PROP					0.3
#define DLNORM					5.0
#define DLPRI					1.0
#define DELTAN  				0.0
#define DELTAP					(-0.2)
#define DEADLINE(x) 				(x==PRIORITARIA?DLPRI:DLNORM)
#define DELTA(x) 				(x==PRIORITARIA?DELTAP:DELTAN)
#define PRECOCES 				0.05

#define MU 					1.5
#define STDEV 					0.6

#define PRIORITARIA 				0
#define NORMAL					1

#define CHEGADA					0
#define TERMINO					1

#define PRECOCES 				0.05

double gentimeexp(double lambda);
double gentimelog(double mu,double sigma);
char desistiu(double tempo);

int main()
{	
	lista * fila = NULL;
	lista * eventos = NULL;
	int terminos=0;
	int processadas[2] = {};
	int adicionadas[2] = {};
	int chegadas[2] = {};
	int perdidas[2] = {};
	int atrasadas[2] = {};
	int ativas = 0;
	int i;	
	char tipo;

	srand((unsigned) time(NULL));
	
	// Adicionar primeiro evento
	eventos=adicionar(eventos,CHEGADA,gentimeexp(LAMBDA));
	
	for(i=0;i<NUMEV;i++)
	{
		// Chegada
		if(eventos->tipo == CHEGADA)
		{
			// Gerar nova chegada
			eventos=adicionar(eventos,CHEGADA,eventos->tempo+gentimeexp(LAMBDA));
			
			// Chegada Prioritaria
			if(rand() < RAND_MAX*PROP)
			{
				tipo = PRIORITARIA;
			}

			// Chegada Normal
			else
			{
				tipo = NORMAL;
			}
			
			chegadas[tipo]++;

			// Caso não haja canais livres
			if(ativas >= CANAIS)
			{
				// Desistencias Permaturas
				if(rand() < PRECOCES*RAND_MAX)
				{
					perdidas[tipo]++;	
				}
				
				// Adicionar à fila de espera	
				else
				{
					fila=adicionar(fila,tipo,eventos->tempo+DEADLINE(tipo)+DELTA(tipo));
					adicionadas[tipo]++;				
				}
			}

			// Caso haja canais livres
			else
			{
				processadas[tipo]++;
				eventos=adicionar(eventos,TERMINO,eventos->tempo+gentimelog(MU,STDEV));
				ativas++;
			}
		}
		
		// Termino
		else if(eventos->tipo == TERMINO)
		{
			terminos++;

			// Verficar desistencia
			while(fila != NULL && desistiu(eventos->tempo-fila->tempo+DEADLINE(fila->tipo)+DELTA(fila->tipo)))
			{
				perdidas[fila->tipo]++;
				fila=remover(fila);
			}
			if(fila != NULL)
			{
				// Verificar se Deadline excedida
				if(fila->tempo - DELTA(fila->tipo) < eventos->tempo)
				{
					atrasadas[fila->tipo]++;
				}
				processadas[fila->tipo]++;	
				eventos=adicionar(eventos,TERMINO,eventos->tempo+gentimelog(MU,STDEV));
				fila=remover(fila);
			}
			else
			{
				ativas--;
			}
		}
		eventos=remover(eventos);			
	}
	for(i=0;i<2;i++)
	{
		i==NORMAL?printf("Normais:\n"):printf("\n\nPrioritarias:\n");
		printf("Chegadas: %d\nProcessadas: %d\nAtrasadas: %d\nPerdidas: %d\nAdicionadas à fila: %d\n",
			chegadas[i],processadas[i],atrasadas[i],perdidas[i],adicionadas[i]);
		printf("Percentagem de atendidas no tempo definido: %f%%\nTaxa de perdas: %f%%\n\n",
			100-((double) atrasadas[i])/chegadas[i]*100-((double) perdidas[i])/chegadas[i]*100, ((double) perdidas[i])/chegadas[i]*100);
	}	
	printf("Terminos: %d\n",terminos);
		
}

// Gerador aleatório exponencial
double gentimeexp(double lambda)
{
	double u = ((double) rand()+1)/((double) RAND_MAX+1);
	return -log(u)/lambda;
}

// Gerador aleatório lognormal
double gentimelog(double mu,double sigma)
{
	double u1 = ((double) rand()+1)/((double) RAND_MAX+1);
	double u2 = ((double) rand()+1)/((double) RAND_MAX+1);
	return exp(mu+sigma*sqrt(-2*log(u1))*cos(2*PI*u2));
}

// Decidir se houve desistência
char desistiu(double tempo)
{
	if(tempo<1.0)
	{
		return 0;
	}
	if(tempo>6.0)
	{
		return 1;
	}
	if(rand() < RAND_MAX*((tempo-1.0)/5.0))
	{
		return 1;
	}
	return 0;
}

