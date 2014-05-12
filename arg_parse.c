/** argparse - un parser 'simple' para c
* Copyright (C) 2013 nickcis https://github.com/nickcis/
*
* This file is part of argparse.
*
* argparse is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* argparse is distributed in the hope that it will be useful,
*     but WITHOUT ANY WARRANTY; without even the implied warranty of
*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*     GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public License
*     along with Foobar. If not, see <http://www.gnu.org/licenses/>.
**/

#include "arg_parse.h"
#include <string.h>
#include <stdio.h>

typedef struct {
	TParseArgFunc func;
	char nombre_corto;
	char *nombre_largo;
	void* def;
	size_t def_tam;
	int encontre;
	char* buf;
} TArg;

struct TParseArg {
	int c_max, c;
	TArg *args;
};

TParseArg* ParseArg_new(int c){
	TParseArg* this = NULL;
	if(!c)
		return NULL;

	this = (TParseArg*) calloc(1, sizeof(TParseArg));
	this->c_max = c;

	this->args = (TArg*) calloc(c, sizeof(TArg));

	return this;
}

int ParseArg_addArg(TParseArg* this, TParseArgFunc func, char corto, char* largo, void* defecto, size_t tam){
	if(!this || this->c >= this->c_max)
		return 1;

	this->args[this->c].func = func;
	this->args[this->c].nombre_corto = corto;
	this->args[this->c].nombre_largo = strcpy((char*) calloc(strlen(largo)+1, sizeof(char)), largo);
	if(tam && defecto){
		this->args[this->c].def = calloc(1, tam);
		memcpy(this->args[this->c].def, defecto, tam);
		this->args[this->c].def_tam = tam;
	}

	this->c++;
	return 0;
}

/** Devuelve el puntero al argumento si lo encuentra en la lsita, o NULL
 */
TArg* encontrar_argumento_corto(TParseArg* this, char c){
	int i=0;
	for(i=0; i < this->c; i++){
		if(this->args[i].nombre_corto == c)
			return &(this->args[i]);
	}

	return NULL;
}

/** Devuelve el puntero al argumento si lo encuentra en la lsita, o NULL
 */
TArg* encontrar_argumento_largo(TParseArg* this, char* largo){
	int i=0;
	for(i=0; i < this->c; i++){
		if(strcmp(this->args[i].nombre_largo, largo) == 0)
			return &(this->args[i]);
	}

	return NULL;
}

int ParseArg_parse(TParseArg* this, int argc, char* argv[]){
	int i=0;
	if(!this)
		return 1;

	for(i=1; i < argc; i++){
		TArg* arg = NULL;
		if(argv[i][0] == '-'){
			if(argv[i][1] == '-')
				arg = encontrar_argumento_largo(this, argv[i]+2);
			else
				arg = encontrar_argumento_corto(this, argv[i][1]);

			if(!arg)
				continue;

			arg->encontre = 1;

			if(arg->func == NULL) // Es flag
				continue;

			if(i+1 < argc){
				i++;
				arg->buf = strcpy((char*) calloc(strlen(argv[i])+1, sizeof(char)), argv[i]);
			}
		}
	}

	return 0;
}

void* ParseArg_getArg(TParseArg* this, char corto){
	TArg* arg = NULL;
	if(!this)
		return NULL;

	arg = encontrar_argumento_corto(this, corto);

	if(!arg)
		return NULL;

	if(arg->encontre){
		if(arg->func == NULL){
			return (void*) 1;
		}

		if(arg->buf == NULL)
			return NULL;

		return arg->func(arg->buf);
	}

	return arg->def;
}


int ParseArg_delete(TParseArg* this){
	int i=0;
	if(!this)
		return 1;

	for(i=0; i < this->c; i++){
		if(this->args[i].nombre_largo)
			free(this->args[i].nombre_largo);
		if(this->args[i].def)
			free(this->args[i].def);
		if(this->args[i].buf)
			free(this->args[i].buf);
	}
	free(this->args);
	free(this);

	return 0;
}

void *ParseArg_parseStr(char* str){
	return strcpy((char*) calloc(strlen(str)+1, sizeof(char)), str);
}

