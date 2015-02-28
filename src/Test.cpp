/*Copyright (c) 2015 "F�lix Escalona Monchol�"
Unit tests for class Tokenizador

This file is part of https://github.com/felixem/EI-P1Tests.

EI-P1Tests is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include <sys/resource.h>
#include <sys/stat.h>
#include <fstream>
#include <cstring>

#include "tokenizador.h"
#include "tokenizadorClase.h"

//Constantes para ejecuci�n de pruebas
const bool UNIT_TESTS = true;
const bool DIRECTORY_TESTS = true;

//Constantes para el suite de pruebas
const char* LISTA_FICHEROS="lista_fich";
const char* RESULTADO="resultado";
const char* SALIDA_ESPERADA="salida_esperada";
const char* REPORTE_TOKEN="reporte_tokenizacion";
const char* DIRECTORIO_PRUEBAS ="entradas/prueba";
const char* DIRECTORIO_SALIDA ="salidas/prueba";
const char* DIRECTORIO_COPIA="salida_ejecucion";
const char* DIRECTORIO_DIFERENCIAS="diferencias";
const char* EXTENSION_TOKEN="tk";
const char* EXTENSION_DIFERENCIAS="diff";

//Declaraci�n de funciones
bool borrarListaFicheros(const string &listaFic);

//Mostrar resultado esperado y obtenido
void MostrarResultados(list<string> &tokens, list<string> &resultados)
{
	cout <<"Resultado esperado:"<<endl;
	for(list<string>::iterator it = resultados.begin(); it!=resultados.end(); ++it)
	{
		cout << (*it) << "\t";
	}
	cout <<endl;
	cout <<"Resultado obtenido:"<<endl;
	for(list<string>::iterator it = tokens.begin(); it!=tokens.end(); ++it)
	{
		cout << (*it) << "\t";
	}
}

//Comparar listas
void compararListas(list<string> &tokens, list<string> &resultados)
{
	//Igualdad de tama�os
	if(resultados.size() != tokens.size())
		//Mostrar resultados esperados
		MostrarResultados(tokens,resultados);

	ASSERT_EQUAL(resultados.size(), tokens.size());

	//Igualdad de contenido
	list<string>::iterator result = resultados.begin();
	for(list<string>::iterator it = tokens.begin(); it!=tokens.end(); ++it)
	{
		//Mostrar errores
		if((*result)!=(*it))
			MostrarResultados(tokens,resultados);

		ASSERT_EQUAL(*result, *it);
		++result;
	}
}

//Tests planteados por el profesor
namespace testsProfesor
{
//Palabras compuestas
void testCompuestas() {
	Tokenizador a("-#", true, false);
	list<string> tokens;
	list<string> resultados;

	a.Tokenizar("MS-DOS p1 p2 UN-DOS-TRES", tokens);
	// La lista de tokens a devolver deber�a contener: "MS-DOS, p1, p2, UN-DOS-TRES"
	resultados.merge({"MS-DOS","p1","p2","UN-DOS-TRES"});
	compararListas(tokens, resultados);

	// La lista de tokens a devolver deber�a contener: "pal1, MS-DOS, p1, p2"
	a.Tokenizar("pal1 -MS-DOS p1 p2", tokens);
	resultados.clear();
	resultados.merge({"pal1","MS-DOS","p1","p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 MS-DOS#p3 p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, MS-DOS, p3, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","MS-DOS","p3","p1","p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1#MS-DOS#p3 p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, MS-DOS, p3, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","MS-DOS","p3","p1","p2"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/ ");
	a.Tokenizar("MS-DOS p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "MS-DOS, p1, p2"
	resultados.clear();
	resultados.merge({"MS-DOS","p1","p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 -MS-DOS p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, -MS-DOS, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","-MS-DOS","p1","p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 MS-DOS#p3 p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, MS-DOS#p3, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","MS-DOS#p3","p1","p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1#MS-DOS#p3 p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1#MS-DOS#p3, p1, p2"
	resultados.clear();
	resultados.merge({"pal1#MS-DOS#p3","p1","p2"});
	compararListas(tokens, resultados);
}

//Tests de url
void testURLs()
{
	Tokenizador a(",", true, false);
	list<string> tokens, resultados;
	string s = "p0 http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es p1 p2";

	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver deber�a contener: " p0, http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"p0",
		"http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es",
		"p1","p2"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/ ");
	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver deber�a contener: " p0, http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es, p1, p2"
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/ &");
	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver deber�a contener: " p0, http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es, p1, p2"
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/&");
	s = "p0 hhttp://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013 p1 p2";
	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver deber�a contener: " p0, hhttp:,intime.dlsi.ua.es:8080, dossierct, index.jsp?lang=es, status=probable, date=22-01-2013, p1, p2"
	resultados.clear();
	resultados.merge({"p0","hhttp:", "intime.dlsi.ua.es:8080", "dossierct", "index.jsp?lang=es", "status=probable",
		"date=22-01-2013", "p1", "p2"});
	compararListas(tokens, resultados);
}

//Test emails
void testEmails()
{
	Tokenizador a(",", true, false);
	list<string> tokens, resultados;

	a.DelimitadoresPalabra("@.&");
	a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener:	"catedraTelefonicaUA@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"catedraTelefonicaUA@iuii.ua.es","p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 @iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 cat@iuii.ua.es@cd p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, cat, iuii.ua.es@cd, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","cat", "iuii.ua.es@cd", "p1", "p2"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("&.");
	a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "catedraTelefonicaUA@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"catedraTelefonicaUA@iuii.ua.es","p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 @iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, @iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","@iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1&@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, @iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","@iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1&catedra@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, catedra@iuii.ua.es,	p1, p2"
	resultados.clear();
	resultados.merge({"pal1","catedra@iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 cat@iuii.ua.es@cd p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, cat@iuii.ua.es@cd,	p1, p2"
	resultados.clear();
	resultados.merge({"pal1","cat@iuii.ua.es@cd", "p1", "p2"});
	compararListas(tokens, resultados);

}

//Test acr�nimos
void testAcronimos()
{
	Tokenizador a(",", true, false);
	list<string> tokens, resultados;

	a.DelimitadoresPalabra("@.&");
	a.Tokenizar("U.S.A p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver deber�a contener: "U.S.A, p1, e.g, p2, La"
	resultados.clear();
	resultados.merge({"U.S.A","p1", "e.g", "p2", "La"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("");
	a.Tokenizar("U.S.A .U.S.A .p1 p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver deber�a contener: "U.S.A, U.S.A, .p1, p1, e.g, p2., La"
	resultados.clear();
	resultados.merge({"U.S.A","U.S.A",".p1","p1", "e.g", "p2.", "La"});
	compararListas(tokens, resultados);

	a.Tokenizar("a&U.S.A p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver deber�a contener: "a&U.S.A, p1, e.g, p2., La"
	resultados.clear();
	resultados.merge({"a&U.S.A","p1", "e.g", "p2.", "La"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("&");
	a.Tokenizar("a&U.S.A p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver deber�a contener: "a, U.S.A, p1, e.g, p2.,	La"
	resultados.clear();
	resultados.merge({"a", "U.S.A","p1", "e.g", "p2.", "La"});
	compararListas(tokens, resultados);
}

//Test n�meros
void testNumeros()
{
	Tokenizador a(",", true, false);
	list<string> tokens, resultados;

	a.DelimitadoresPalabra("@.,&");
	a.Tokenizar("pal1 10.000,34 10,000.34 10.000.123.456.789.009,34 10,000,123,456,789,009.34 20.03 40,03 2005 10. 20, 10.0 20,0 La 20,12.456,7.8.9,", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1 10.000,34 10,000.34 10.000.123.456.789.009,34 10,000,123,456,789,009.34 20.03
	// 40,03 2005 10 20 10.0 20,0 La 20,12.456,7.8.9"
	resultados.clear();
	resultados.merge({"pal1","10.000,34", "10,000.34", "10.000.123.456.789.009,34", "10,000,123,456,789,009.34", "20.03",
		"40,03","2005", "10", "20", "10.0", "20,0", "La", "20,12.456,7.8.9"});
	compararListas(tokens, resultados);

	a.Tokenizar(".34 ,56", tokens);
	// La lista de tokens a devolver deber�a contener: "0.34 0,56"
	resultados.clear();
	resultados.merge({"0.34","0,56"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 10.35% 10,35% 23.000,3% 23� 23.05� 23,05� 11$ 11.05$ 3� 4�", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1 10.35 % 10,35 % 23.000,3 %	23 � 23.05 � 23,05 � 11	$
	// 11.05 $ 3 � 4 �"
	resultados.clear();
	resultados.merge({"pal1","10.35", "%", "10,35", "%", "23.000,3","%", "23", "�", "23.05", "�",
		"23,05", "�", "11", "$", "11.05", "$", "3", "�", "4", "�"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 10.00a 10.000.a.000 10/12/85 1,23E+10", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1 10.00a 10.000.a.000 10/12/85 1	23E+10"
	resultados.clear();
	resultados.merge({"pal1","10.00a", "10.000.a.000", "10/12/85", "1", "23E+10"});
	compararListas(tokens, resultados);

	/* PRUEBA INVALIDADA POR POSIBLE AMBIG�EDAD ENTRE EMAIL Y N�MERO
	a.Tokenizar("pal1&10.00@10.000&000@10/12/85", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1 10.00 10.000 000 10/12/85"
	resultados.clear();
	resultados.merge({"pal1","10.00","10.000","000", "10/12/85"});
	compararListas(tokens, resultados);*/

	a.Tokenizar(".34@@&,56", tokens);
	// La lista de tokens a devolver deber�a contener: "0.34 0,56"
	resultados.clear();
	resultados.merge({"0.34","0,56"});
	compararListas(tokens, resultados);

}

//Tokenizador01
void tokenizador01() {

	list<string> tokens;
	list<string> resultados;

	TokenizadorClase a(". /");
	stringstream ss;
	ss << a;
	ASSERT_EQUAL("DELIMITADORES: . /", ss.str());

	a.Tokenizar("MS DOS OS 2 high low", tokens);
	resultados.merge({"MS","DOS", "OS", "2", "high", "low"});
	compararListas(tokens, resultados);
}

//Tokenizador02
void tokenizador02() {

	list<string> tokens;
	list<string> resultados;

	bool kCasosEspeciales = true, kpasarAminusculas = false;
	Tokenizador p("/.;-", kCasosEspeciales, kpasarAminusculas );

	stringstream ss;
	ss << p;
	ASSERT_EQUAL("DELIMITADORES: /.;- TRATA CASOS ESPECIALES: 1 PASAR A MINUSCULAS Y SIN ACENTOS: 0", ss.str());

	p.Tokenizar("MS DOS OS 2 high low", tokens);
	resultados.merge({"MS","DOS", "OS", "2", "high", "low"});
	compararListas(tokens, resultados);

	p.CasosEspeciales (false);
	p.Tokenizar("MS DOS OS 2 high low", tokens);
	resultados.clear();
	resultados.push_back("MS DOS OS 2 high low");
	compararListas(tokens, resultados);

	list<string> lt1, lt2, lt3, lt4;
	Tokenizador a(" ", kCasosEspeciales, kpasarAminusculas );
	a.Tokenizar("MS DOS OS 2 high low", lt1);
	resultados.clear();
	resultados.merge({"MS","DOS", "OS", "2", "high", "low"});
	compararListas(lt1, resultados);

	Tokenizador b(" ", kCasosEspeciales, kpasarAminusculas  );
	b.Tokenizar("M", lt2);
	resultados.clear();
	resultados.push_back("M");
	compararListas(lt2, resultados);

	Tokenizador c(" ", kCasosEspeciales, kpasarAminusculas  );
	c.Tokenizar(" ", lt3);
	resultados.clear();
	compararListas(lt3, resultados);

	Tokenizador d(" ", kCasosEspeciales, kpasarAminusculas  );
	resultados.clear();
	compararListas(lt4, resultados);

}

//Tokenizador03
void tokenizador03() {

	list<string> resultados;

	bool kCasosEspeciales = true, kpasarAminusculas = false;

	list<string> lt1, lt2, lt3;

	Tokenizador a("[]# ", kCasosEspeciales, kpasarAminusculas);
	a.Tokenizar("MS#DOS OS_2 [high low]", lt1);
	resultados.merge({"MS","DOS", "OS_2", "high", "low"});
	compararListas(lt1, resultados);

	a.AnyadirDelimitadoresPalabra("_ []");
	a.Tokenizar("MS#DOS OS_2 [high low]", lt2);
	resultados.clear();
	resultados.merge({"MS","DOS", "OS", "2", "high", "low"});
	compararListas(lt2, resultados);

	a.DelimitadoresPalabra("_");
	a.Tokenizar("MS#DOS OS_2 [high low]", lt3);
	resultados.clear();
	resultados.merge({"MS#DOS", "OS", "2", "[high", "low]"});
	compararListas(lt3, resultados);
	a.DelimitadoresPalabra(" _");
	a.Tokenizar("MS#DOS 10 Espa�a �sp��� OS_2 [high low]", lt3);
	resultados.clear();
	resultados.merge({"MS#DOS", "10", "Espa�a", "�sp���", "OS", "2", "[high", "low]"});
	compararListas(lt3, resultados);
}

//Tokenizador04
void tokenizador04() {

	list<string> resultados;

	bool kCasosEspeciales = true, kpasarAminusculas = true;

	list<string> lt1, lt2, lt3;

	Tokenizador a("[]# ", kCasosEspeciales, kpasarAminusculas);
	a.Tokenizar("MS#DOS OS_2 [high low]", lt1);
	resultados.clear();
	resultados.merge({"ms", "dos", "os_2", "high", "low"});
	compararListas(lt1, resultados);

	a.AnyadirDelimitadoresPalabra("_ []");
	a.Tokenizar("MS#DOS OS_2 [high low]", lt2);
	resultados.clear();
	resultados.merge({"ms", "dos", "os", "2", "high", "low"});
	compararListas(lt2, resultados);

	a.DelimitadoresPalabra("_");
	a.Tokenizar("MS#DOS OS_2 [high low]", lt3);
	resultados.clear();
	resultados.merge({"ms#dos", "os", "2", "[high", "low]"});
	compararListas(lt3, resultados);

	a.DelimitadoresPalabra(" _");
	a.Tokenizar("MS#DOS 10 Espa�a �sp��� OS_2 [high low]", lt3);
	resultados.clear();
	resultados.merge({"ms#dos", "10", "espa�a", "espa�e", "os", "2", "[high", "low]" });
	compararListas(lt3, resultados);

	a.Tokenizar("����� ����� �� ", lt3);
	resultados.clear();
	resultados.merge({"aeiou", "aeiou", "��"});
	compararListas(lt3, resultados);
}

//Tokenizador 05
void tokenizador05()
{
	list<string> resultados;

	Tokenizador a("-#", true, false);
	list<string> tokens;

	a.Tokenizar("MS-DOS p1 p2 UN-DOS-TRES", tokens);
	// La lista de tokens a devolver deber�a contener: "MS-DOS, p1, p2, UN-DOS-TRES"
	resultados.clear();
	resultados.merge({"MS-DOS", "p1", "p2", "UN-DOS-TRES" });
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 -MS-DOS p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, MS-DOS, p1, p2"
	resultados.clear();
	resultados.merge({"pal1", "MS-DOS", "p1", "p2" });
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 MS-DOS#p3 p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, MS-DOS, p3, p1, p2"
	resultados.clear();
	resultados.merge({"pal1", "MS-DOS", "p3", "p1", "p2" });
	compararListas(tokens, resultados);

	a.Tokenizar("pal1#MS-DOS#p3 p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, MS-DOS, p3, p1, p2"
	resultados.clear();
	resultados.merge({"pal1", "MS-DOS", "p3", "p1", "p2" });
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/ ");
	a.Tokenizar("MS-DOS p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "MS-DOS, p1, p2"
	resultados.clear();
	resultados.merge({"MS-DOS", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 -MS-DOS p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, -MS-DOS, p1, p2"
	resultados.clear();
	resultados.merge({"pal1", "-MS-DOS", "p1", "p2" });
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 MS-DOS#p3 p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, MS-DOS#p3, p1, p2"
	resultados.clear();
	resultados.merge({"pal1", "MS-DOS#p3", "p1", "p2" });
	compararListas(tokens, resultados);

	a.Tokenizar("pal1#MS-DOS#p3 p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1#MS-DOS#p3, p1, p2"
	resultados.clear();
	resultados.merge({"pal1#MS-DOS#p3", "p1", "p2"});
	compararListas(tokens, resultados);

	a.PasarAminuscSinAcentos(true);
	a.Tokenizar("pal1#MS-DOSaA#p3 p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1#ms-dosaa#p3, p1, p2"
	resultados.clear();
	resultados.merge({"pal1#ms-dosaa#p3", "p1", "p2"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/-");
	a.CasosEspeciales (false);
	a.Tokenizar("pal1#MS-DOSaA#p3 p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1#ms, dosaa#p3 p1 p2"
	resultados.clear();
	resultados.merge({"pal1#ms", "dosaa#p3 p1 p2" });
	compararListas(tokens, resultados);
}

//Tokenizador06
void tokenizador06()
{
	list<string> resultados;

	Tokenizador a(",", true, false);
	list<string> tokens;
	string s = "p0 http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es p1 p2";

	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver deber�a contener: " p0, http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"p0", "http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es",
						"p1","p2"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/ ");
	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver deber�a contener: " p0, http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"p0", "http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es",
						"p1","p2"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/ &");
	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver deber�a contener: " p0, http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"p0", "http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es",
						"p1","p2"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/&");
	s = "p0 hhttp://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&DATE=22-01-2013 p1 p2";

	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver deber�a contener: " p0, hhttp:, intime.dlsi.ua.es:8080, dossierct, index.jsp?lang=es, status=probable, DATE=22-01-2013, p1, p2"
	resultados.clear();
	resultados.merge({"p0", "hhttp:", "intime.dlsi.ua.es:8080", "dossierct", "index.jsp?lang=es", "status=probable",
						"DATE=22-01-2013","p1", "p2"});
	compararListas(tokens, resultados);

	a.PasarAminuscSinAcentos(true);
	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver deber�a contener: " p0, hhttp:, intime.dlsi.ua.es:8080, dossierct, index.jsp?lang=es, status=probable, date=22-01-2013, p1, p2"
	resultados.clear();
	resultados.merge({"p0", "hhttp:", "intime.dlsi.ua.es:8080", "dossierct", "index.jsp?lang=es", "status=probable",
						"date=22-01-2013","p1", "p2"});
	compararListas(tokens, resultados);

	s = "p0 http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es p1 p2";
	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver deber�a contener: " p0, http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedratelefonicaua@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"p0", "http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedratelefonicaua@iuii.ua.es",
						"p1","p2"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/&");
	a.CasosEspeciales (false);
	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver deber�a contener: " p0 http:, intime.dlsi.ua.es:8080, dossierct, index.jsp?lang=es, status=probable, date=22-01-2013, newspaper=catedratelefonicaua@iuii.ua.es p1 p2"
	resultados.clear();
	resultados.merge({"p0 http:", "intime.dlsi.ua.es:8080", "dossierct", "index.jsp?lang=es", "status=probable",
						"date=22-01-2013","newspaper=catedratelefonicaua@iuii.ua.es p1 p2"});
	compararListas(tokens, resultados);
}

//Tokenizador07
void tokenizador07()
{
	Tokenizador a("-#", true, false);
	list<string> resultados;
	list<string> tokens;

	a.DelimitadoresPalabra("@.&");
	a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "catedraTelefonicaUA@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"catedraTelefonicaUA@iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 @iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	// ELIMINADO POR POSIBLE AMBIGUEDAD DE QUE SE DETECTA PRIMERO ACRONIMO O EMAIL
	//a.Tokenizar("pal1 cat@iuii.ua.es@cd p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, cat, iuii.ua.es, cd, p1, p2"
	//	imprimirListaSTL(tokens);

	a.DelimitadoresPalabra("&.");
	a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "catedraTelefonicaUA@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"catedraTelefonicaUA@iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 @iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, @iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","@iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1&@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, @iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","@iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1&catedra@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, catedra@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","catedra@iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	// ELIMINADO POR POSIBLE AMBIGUEDAD DE QUE SE DETECTA PRIMERO ACRONIMO O EMAIL
	//a.Tokenizar("pal1 cat@iuii.ua.es@cd p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1, cat@iuii, ua, es@cd, p1, p2"
	//	imprimirListaSTL(tokens);

	a.PasarAminuscSinAcentos(true);
	a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "catedratelefonicaua@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"catedratelefonicaua@iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("@.&");
	a.CasosEspeciales (false);
	a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver deber�a contener: "catedratelefonicaua, iuii, ua, es p1 p2"
	resultados.clear();
	resultados.merge({"catedratelefonicaua", "iuii", "ua","es p1 p2"});
	compararListas(tokens, resultados);
}

//Tokenizador08
void tokenizador08()
{
	Tokenizador a("-#", true, false);
	list<string> tokens;
	list<string> resultados;

	a.DelimitadoresPalabra("@.&");
	a.Tokenizar("U.S.A p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver deber�a contener: "U.S.A, p1, e.g, p2, La"
	resultados.clear();
	resultados.merge({"U.S.A", "p1", "e.g", "p2", "La"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("");
	a.Tokenizar("U.S.A .U.S.A .p1 p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver deber�a contener: "U.S.A, U.S.A, .p1, p1, e.g, p2., La"
	resultados.clear();
	resultados.merge({"U.S.A", "U.S.A", ".p1", "p1", "e.g", "p2.", "La"});
	compararListas(tokens, resultados);

	a.Tokenizar("a&U.S.A p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver deber�a contener: "a&U.S.A, p1, e.g, p2., La"
	resultados.clear();
	resultados.merge({"a&U.S.A", "p1", "e.g", "p2.", "La"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("&");
	a.Tokenizar("a&U.S.A p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver deber�a contener: "a, U.S.A, p1, e.g, p2., La"
	resultados.clear();
	resultados.merge({"a", "U.S.A", "p1", "e.g", "p2.", "La"});
	compararListas(tokens, resultados);

	a.PasarAminuscSinAcentos(true);
	a.Tokenizar("a&U.S.A p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver deber�a contener: "a, u.s.a, p1, e.g, p2., la"
	resultados.clear();
	resultados.merge({"a", "u.s.a", "p1", "e.g", "p2.", "la"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra(".&");
	a.CasosEspeciales (false);
	a.Tokenizar("a&U.S.A p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver deber�a contener: "a, u, s, a p1 e, g,  p2,  la"
	resultados.clear();
	resultados.merge({"a", "u", "s", "a p1 e", "g", " p2", " la"});
	compararListas(tokens, resultados);
}

//Tokenizador09
void tokenizador09()
{
	Tokenizador a("-#", true, false);
	list<string> tokens;
	list<string> resultados;

	a.DelimitadoresPalabra("@.,&");
	a.Tokenizar("pal1 10.000,34 10,000.34 10.000.123.456.789.009,34 10,000,123,456,789,009.34 20.03 40,03 2005 10. 20, 10.0 20,0 La 20,12.456,7.8.9,", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1	10.000,34	10,000.34	10.000.123.456.789.009,34	10,000,123,456,789,009.34	20.03	40,03	2005	10	20	10.0	20,0	La	20,12.456,7.8.9"
	resultados.clear();
	resultados.merge({"pal1", "10.000,34", "10,000.34", "10.000.123.456.789.009,34", "10,000,123,456,789,009.34", "20.03", "40,03",
					"2005", "10", "20", "10.0", "20,0", "La", "20,12.456,7.8.9"});
	compararListas(tokens, resultados);

	a.Tokenizar(".34 ,56", tokens);
	// La lista de tokens a devolver deber�a contener: "0.34	0,56"
	resultados.clear();
	resultados.merge({"0.34","0,56"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 10.35% 10,35% 23.000,3% 23� 23.05� 23,05� 11$ 11.05$ 3� 4�", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1	10.35	%	10,35	%	23.000,3	%	23	�	23.05	�	23,05	�
	//	11	$	11.05	$	3	�	4	�"
	resultados.clear();
	resultados.merge({"pal1", "10.35", "%", "10,35", "%", "23.000,3", "%",
					"23", "�", "23.05", "�", "23,05", "�", "11",
					"$", "11.05","$","3","�","4","�"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 10.00a 10.000.a.000 10/12/85 1,23E+10", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1	10.00a	10.000.a.000	10/12/85 1	23E+10	"
	resultados.clear();
	resultados.merge({"pal1", "10.00a", "10.000.a.000",
					"10/12/85", "1", "23E+10"});
	compararListas(tokens, resultados);

	//NO SE COMPRUEBA POR POSIBLE AMBIG�EDAD
	/*
	a.Tokenizar("pal1&10.00@10.000&000@10/12/85", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1	10.00	10.000	000	10/12/85"
	resultados.clear();
	resultados.merge({"pal1", "10.00", "10.000", "000", "10/12/85"});
	compararListas(tokens, resultados);*/

	a.Tokenizar(".34@@&,56", tokens);
	// La lista de tokens a devolver deber�a contener: "0.34	0,56"
	resultados.clear();
	resultados.merge({"0.34", "0,56"});
	compararListas(tokens, resultados);

	a.PasarAminuscSinAcentos(true);
	/*PRUEBA ANULADA POR AMBIG�EDAD
	a.Tokenizar("Pal1&10.00@10.000&000@10/12/85 La", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1	10.00	10.000	000	10/12/85	la"
	resultados.clear();
	resultados.merge({"pal1", "10.00", "10.000", "000", "10/12/85", "la"});
	compararListas(tokens, resultados);*/

	a.CasosEspeciales (false);
	a.Tokenizar("Pal1&10.00@10.000&000@10/12/85 La", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1	10	00	10	000	000	10/12/85 la"
	resultados.clear();
	resultados.merge({"pal1", "10", "00", "10", "000", "000", "10/12/85 la"});
	compararListas(tokens, resultados);
}
}

//Tests planteados por felixem
namespace testsFelixem
{
//Comprobar problemas con delimitadores
void delimitadorInicial()
{
	Tokenizador a("/", true, false);
	list<string> tokens, resultados;
	string s = "/holamundo";

	a.Tokenizar(s, tokens);
	resultados.merge({"holamundo"});
	compararListas(tokens, resultados);
}
//Comprobar problemas con delimitador al final
void delimitadorFinal()
{
	Tokenizador a("/", true, false);
	list<string> tokens, resultados;
	string s = "holamundo/";

	a.Tokenizar(s, tokens);
	resultados.merge({"holamundo"});
	compararListas(tokens, resultados);
}
//Comprobar problemas con delimitadores encadenados
void delimitadoresEncadenados()
{
	Tokenizador a("/", true, false);
	list<string> tokens, resultados;
	string s = "/////hola/mundo////";

	a.Tokenizar(s, tokens);
	resultados.merge({"hola", "mundo"});
	compararListas(tokens, resultados);
}

//Comprobar problemas con palabras compuestas formadas por n�meros
void compuestasNumeros()
{
	Tokenizador a("-", true, false);
	list<string> tokens, resultados;
	string s = "MS-DOS 1-1 23-2-22";

	/* PRUEBA INVALIDADA POR AMBIG�EDAD
	a.Tokenizar(s, tokens);
	resultados.merge({"MS-DOS", "1", "1", "23", "2", "22"});
	compararListas(tokens, resultados);
	*/
}

//Comprobar los a�adidos y borrados de los delimitadores
void delimitadores()
{
	Tokenizador a("./-a");
	ASSERT_EQUAL("./-a",a.DelimitadoresPalabra());

	a.AnyadirDelimitadoresPalabra("./ %$");
	ASSERT_EQUAL("./-a %$",a.DelimitadoresPalabra());
}

}

//Suite de tests unitarios
void runSuite(){
	cute::suite s;

	//Test casos especiales
	s.push_back(CUTE(testsProfesor::testCompuestas));
	s.push_back(CUTE(testsProfesor::testURLs));
	s.push_back(CUTE(testsProfesor::testEmails));
	s.push_back(CUTE(testsProfesor::testAcronimos));
	s.push_back(CUTE(testsProfesor::testNumeros));
	//Tests tokenizador
	s.push_back(CUTE(testsProfesor::tokenizador01));
	s.push_back(CUTE(testsProfesor::tokenizador02));
	s.push_back(CUTE(testsProfesor::tokenizador03));
	s.push_back(CUTE(testsProfesor::tokenizador04));
	s.push_back(CUTE(testsProfesor::tokenizador05));
	s.push_back(CUTE(testsProfesor::tokenizador06));
	s.push_back(CUTE(testsProfesor::tokenizador07));
	s.push_back(CUTE(testsProfesor::tokenizador08));
	s.push_back(CUTE(testsProfesor::tokenizador09));
	//Tests de felixem
	s.push_back(CUTE(testsFelixem::delimitadorInicial));
	s.push_back(CUTE(testsFelixem::delimitadorFinal));
	s.push_back(CUTE(testsFelixem::delimitadoresEncadenados));
	s.push_back(CUTE(testsFelixem::compuestasNumeros));
	s.push_back(CUTE(testsFelixem::delimitadores));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
}

//Calcular tiempos
double getcputime(void)
{
	struct timeval tim;
	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	tim=ru.ru_utime;
	double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
	tim=ru.ru_stime;
	t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
	return t;
}

//Borrar fichero seg�n su ruta
void borrarFichero(const string &nombreFic)
{
	struct stat dir;
	// Compruebo la existencia del fichero
	int err=stat(nombreFic.c_str(), &dir);
	if(err!=-1)
	{
		//Borrar fichero
		string str = "rm "+nombreFic;
		system(str.c_str());
	}
}

//Comprobar si la entrada se corresponde con un directorio
bool esDirectorio(const string &directorio)
{
	struct stat dir;
	// Compruebo la existencia del directorio
	int err=stat(directorio.c_str(), &dir);
	if(err==-1 || !S_ISDIR(dir.st_mode))
		return false;
	else
		return true;
}

//Volcar la indexaci�n de un directorio en un fichero seg�n un filtro de extensi�n de archivos
bool indexarDirectorioPorExtension(const string &directorio, const string &extension, const string &nomArchivo)
{
	//Comprobar si es un directorio
	if(!esDirectorio(directorio))
	{
		cerr<<"ERROR: No existe el directorio "<<directorio<<endl;
		return false;
	}
	else
	{
		string cmd="find "+directorio+" -type f -follow -name \"*."+extension+"\" |sort > "+nomArchivo;
		system(cmd.c_str());
		return true;
	}
}

//Limpiar el directorio de los ficheros con la extensi�n deseada
bool limpiarDirectorio(const string &dirAIndexar, const string &ext)
{
	//Comprobar si es un directorio
	if(!esDirectorio(dirAIndexar))
	{
		cerr<<"ERROR: No existe el directorio "<<dirAIndexar<<endl;
		return false;
	}
	else
	{
		string nombreFic = "borrar_"+ext;
		// Hago una lista de ficheros a borrar con find>fich
		indexarDirectorioPorExtension(dirAIndexar,ext,nombreFic);
		//Borrar la lista de ficheros que se generan en cada ejecuci�n
		borrarListaFicheros(nombreFic);
		//Borrar ficheros auxiliares
		borrarFichero(nombreFic);
	}

	return true;
}

//Borrar lista de ficheros
bool borrarListaFicheros(const string &listaFic)
{
	//Abrir fichero
	ifstream fic;
	bool sinfallos=true;
	fic.open(listaFic.c_str());

	//Fichero inexistente
	if(!fic.is_open())
	{
		cerr << "ERROR: No existe el archivo: "<< listaFic << endl;
		return false;
	}

	//Recorrer el fichero
	string cadena;
	while(getline(fic, cadena))
	{
		borrarFichero(cadena);
	}

	fic.close();
	return sinfallos;
}

//Quitar el primer directorio de la ruta
bool quitarDirectorioComienzo(string &ruta, const string &directorio)
{
	string::size_type pos = ruta.find(directorio+"/");

	//No se pudo quitar el directorio del comienzo
	if(pos == string::npos || pos !=0)
	{
		cerr <<"ERROR: No se pudo borrar el directorio " <<directorio<< " de la ruta "<<ruta<<endl;
		return false;
	}

	//Modificar la ruta
	string::size_type tam = directorio.length()+1;
	ruta.erase(pos,tam);
	return true;
}

//Comparar un fichero con otro y volcar la salida sobre otro fichero
void compararFicheros(const string &fic1, const string &fic2, const string &ficSalida)
{
	string str = "diff "+fic1+" "+fic2+" >> "+ficSalida;
	system(str.c_str());
}

//Borrar lista de ficheros
bool compararListaFicheros(const string &resultado, const string &esperado,
		const string &dirEntrada, const string &dirSalida)
{
	//Abrir fichero
	ifstream res,esp;
	res.open(resultado.c_str());

	//Fichero inexistente
	if(!res.is_open())
	{
		cerr << "ERROR: No existe el archivo: "<< resultado << endl;
		return false;
	}

	//Abrir segundo fichero
	esp.open(esperado.c_str());

	//Fichero inexistente
	if(!esp.is_open())
	{
		cerr << "ERROR: No existe el archivo: "<< esperado << endl;
		res.close();
		return false;
	}

	//Recorrer el fichero
	string cadenaRes;
	string cadenaEsp;
	string rutaRes;
	string rutaEsp;
	bool sinfallos=true;
	int i=0;

	//Comparar todos los ficheros
	while(!esp.eof() && !res.eof())
	{
		cadenaRes="";
		cadenaEsp="";

		//Leer l�nea
		getline(res, cadenaRes);
		getline(esp, cadenaEsp);

		//Comparar ficheros
		if(cadenaRes.length()!=0 && cadenaEsp.length()!=0)
		{
			//Comparar la ruta de los archivos sin sus directorios raiz
			rutaRes = cadenaRes;
			rutaEsp = cadenaEsp;
			quitarDirectorioComienzo(rutaRes,dirEntrada);
			quitarDirectorioComienzo(rutaEsp,dirSalida);

			//Error en la comparaci�n de rutas
			if(rutaRes != rutaEsp)
			{
				cerr<<"ERROR: El directorio de los ficheros a comparar no coincide."<<
				"Se esperaba "<<rutaEsp<<" pero se obtuvo "<<rutaRes<<endl;
				sinfallos=false;
			}
			//Comparar ficheros
			else
			{
				stringstream dirSalida;
				dirSalida<<DIRECTORIO_DIFERENCIAS<<"/"<<rutaEsp<<"."<<EXTENSION_DIFERENCIAS;
				compararFicheros(cadenaRes,cadenaEsp,dirSalida.str());
			}
		}
		++i;
	}

	//Mala generaci�n en la tokenizaci�n de ficheros
	if(!res.eof())
	{
		cerr<<"El n�mero de ficheros resultado es mayor del esperado"<<endl;
		sinfallos=false;
	}
	else if(!esp.eof())
	{
		cerr<<"El n�mero de ficheros resultado es menor del esperado"<<endl;
		sinfallos=false;
	}

	//Cerrar ficheros
	esp.close();
	res.close();
	return sinfallos;
}

//Comparar salidas de los ficheros de salida
bool compararSalidas(const string &dirEntrada,const string &dirSalida)
{
	string ficResultados=RESULTADO;
	string ficSalidaEsperada=SALIDA_ESPERADA;
	indexarDirectorioPorExtension(dirEntrada,EXTENSION_TOKEN,ficResultados);
	indexarDirectorioPorExtension(dirSalida,EXTENSION_TOKEN,ficSalidaEsperada);

	return compararListaFicheros(ficResultados,ficSalidaEsperada,dirEntrada,dirSalida);
}

//Borrar directorio completo
bool borrarDirectorio(const string &directorio)
{
	//Comprobar si es un directorio
	if(esDirectorio(directorio))
	{
		//Borrar directorio
		string cmd ="rm -r "+directorio;
		system(cmd.c_str());
		return true;
	}
	else
		return false;
}

//Calcar la estructura de directorios del origen al destino sin incluir los archivos contenidos
bool calcarEstructuraDirectorios(const string &origen, const string &destino)
{
	//Comprobar si es un directorio
	if(!esDirectorio(origen))
	{
		cerr<<"ERROR: No existe el directorio "<<origen<<endl;
		return false;
	}
	else
	{
		//Copiar estructura de directorio con la salida
		//rsync -r --max-size=1 salida/* diferencias
		string cmd =  "rsync -r --max-size=1 "+origen+"/* "+destino;
		system(cmd.c_str());
		return true;
	}
}

//Borrar archivos de una determinada extension
void borrarArchivosExtension(const string& directorio, const string& ext)
{
	//find prueba -follow -name \"*.tk" | xargs rm -rf
	string cmd="find "+directorio+" -type f -follow -name \"*."+ext+"\" | xargs rm -rf";
	system(cmd.c_str());
}

//Copiar los ficheros de una extension dentro de un directorio a otro directorio respectando la jerarqu�a de directorios interna
bool copiarArchivosExtension(const string& origen, const string&ext, const string&destino)
{
	//Comprobar si es un directorio
	if(!esDirectorio(origen))
	{
		cerr<<"ERROR: No existe el directorio "<<origen<<endl;
		return false;
	}
	else
	{
		//Copiar todos los ficheros que tengan la extensi�n de entrada dentro del directorio origen
		//rsync -rv --include '*/' --include '*.tk' --exclude '*' --prune-empty-dirs prueba/* salidaReal
		string cmd =  "rsync -r --include '*/' --include '*."+ext+"' --exclude '*' --prune-empty-dirs "+origen+"/* "+destino;
		system(cmd.c_str());
		//Eliminar los ficheros de la extension en el directorio origen
		borrarArchivosExtension(origen,ext);
		return true;
	}
}

//Limpiar ficheros temporales
void limpiarFicherosTemporales()
{
	borrarFichero(LISTA_FICHEROS);
	borrarFichero(RESULTADO);
	borrarFichero(SALIDA_ESPERADA);
}

//Limpiar directorios y ficheros temporales
void limpiarDirectoriosTemporales(const string& directorioPruebas, const string& directorioSalida,
		const string& directorioComparaciones, const string& directorioCopia)
{
	//Borrar los ficheros temporales generados en cada ejecucion
	limpiarFicherosTemporales();
	borrarFichero(REPORTE_TOKEN);
	cout<<endl;
	cout<<"Archivos temporales limpiados"<<endl;

	//Limpiar extensiones de ficheros temporales de los directorios
	if(limpiarDirectorio(directorioPruebas,EXTENSION_TOKEN))
		cout<<"Directorio "+directorioPruebas<<" limpiado de ."<<EXTENSION_TOKEN<<endl;

	//Truncar el directorio de diferencias completo
	if(borrarDirectorio(directorioComparaciones))
		cout<<"Directorio "+directorioComparaciones<<" borrado"<<endl;
	//Copiar estructura de directorio con la salida
	if(calcarEstructuraDirectorios(directorioPruebas,directorioComparaciones))
		cout<<"Directorio "+directorioComparaciones+" regenerado"<<endl;
	else
		cerr<<"No se pudo regenerar el directorio "<<directorioComparaciones<<endl;

	//Borrar el directorio de copia
	if(borrarDirectorio(directorioCopia))
		cout<<"Directorio "+directorioCopia<<" borrado"<<endl;

}

//M�todo para testear la tokenizaci�n de directorios
bool testDirectorio(const Tokenizador &tok, const string& directorioPruebas,
		const string& directorioSalida, long double &tiempoFinal)
{
	bool sinerrores = true;

	//Tokenizar el directorio
	if(tok.TokenizarDirectorio(directorioPruebas))
		tiempoFinal = getcputime();
	else
	{
		cerr<<"Hubo problemas graves en la tokenizaci�n del directorio"<<endl;
		sinerrores = false;
		tiempoFinal = getcputime();
	}

	return sinerrores;
}

//Saber si un fichero est� vac�o
bool esFicheroVacio(const string &fichero)
{
	struct stat dat;

	//Comprobar si se puede abrir el fichero
	if(stat(fichero.c_str(),&dat) == -1) {
		cerr<<"ERROR: no se pudo abrir el fichero "<<fichero<<endl;
		return true;
	}

	//Comprobar la cantidad de bytes
	if(dat.st_size == 0)
		return true;
	else
		return false;
}

//Escribir en el informe los ficheros que no est�n vac�os
bool reportarNoVacios(const string &listado, const string &nomSalida)
{
	ifstream ficEntrada;
	ofstream ficSalida;
	bool sinfallos=true;

	//Abrir listado de ficheros
	ficEntrada.open(listado.c_str());
	//Comprobar la apertura del listado
	if(ficEntrada.is_open())
	{
		ficSalida.open(nomSalida.c_str());
		//Comprobar creaci�n del archivo de reporte
		if(ficSalida.is_open())
		{
			//A�adir un t�tulo dentro del reporte
			ficSalida<<"Listado de ficheros de diferencia que reportan errores de tokenizaci�n:"<<"\n";

			//Recorrer el fichero mientras no se llegue al final
			string cadena;
			while(getline(ficEntrada,cadena))
			{
				//A�adir en el reporte los ficheros no vac�os
				if(!esFicheroVacio(cadena))
					ficSalida<<cadena<<"\n";
			}
		}
		//Error en el fichero de salida
		else
		{
			cerr<<"ERROR: No se pudo generar el fichero "<<listado<<endl;
			sinfallos=false;
		}
	}
	else
	{
		cerr<<"ERROR: No se pudo abrir el fichero "<<listado<<endl;
		sinfallos=false;
	}

	ficEntrada.close();
	ficSalida.close();

	return sinfallos;
}

//Genera un informe sobre los ficheros de diferencias situados en el directorio de entrada
bool generarInforme(const string &directorio,const string& reporte)
{
	string listado ="listado_diferencias";

	//Generar un listado con todos los archivos diff
	indexarDirectorioPorExtension(directorio,EXTENSION_DIFERENCIAS,listado);
	//Generar el reporte de diferencias
	bool sinfallos=reportarNoVacios(listado,reporte);
	//Eliminar el fichero temporal de listado de diferencias
	borrarFichero(listado);

	return sinfallos;
}

//Probar la tokenizaci�n de directorios
int runSuiteTemporal(const string& directorioPruebas, const string& directorioSalida, const string& directorioCopia)
{
	string directorioComparaciones =DIRECTORIO_DIFERENCIAS;

	//Limpiar temporales
	limpiarDirectoriosTemporales(directorioPruebas,directorioSalida,directorioComparaciones,directorioCopia);

	//Tokenizaci�n de directorio
	cout <<"------------------------------------------------"<<endl;
	cout <<"Iniciando tokenizaci�n del directorio "+directorioPruebas<<endl;

	//Medir el tiempo de tokenizaci�n
	long double tiempo;
	long double aa = getcputime();
	//Declarar tokenizador
	Tokenizador tok(".,:;/-@*(){}?!|[]'=_\"><&#-\r\t",true,true);

	//Tokenizar directorio
	if(testDirectorio(tok,directorioPruebas,directorioSalida,tiempo))
	{
		//Mostrar resultado temporal
		cout << "Ha tardado " << tiempo - aa << " segundos en tokenizar el directorio" << endl;

		//Comprobar errores de comparaci�n
		if(compararSalidas(directorioPruebas,directorioSalida))
		{
			//Generar un fichero de resumen sobre las diferencias entre la salida esperada y la generada por la tokenizaci�n
			string reporte =REPORTE_TOKEN;
			if(generarInforme(directorioComparaciones,reporte))
				cout<<"Informe de comparaciones generado en "<<reporte<<endl;
		}
		else
			cerr<<"Hubo problemas en la comparaci�n de los ficheros generados"<<endl;

		//Hacer una copia de los ficheros .tk generados a otra carpeta para poder usarla como referencia en pr�ximas ejecuciones
		if(copiarArchivosExtension(directorioPruebas,EXTENSION_TOKEN,directorioCopia))
			cout<<"Salida de la tokenizaci�n movida desde "<<directorioPruebas<<" hacia "<<directorioCopia<<endl;

		//Limpiar ficheros temporales
		limpiarFicherosTemporales();
	}
	//Errores de tokenizaci�n
	else
	{
		cerr<<"Se produjeron errores durante la tokenizaci�n del directorio"<<endl;
		return -1;
	}

	return 0;
}

//Quitar caracter final
void quitarCaracterFinal (string &cadena, const char &car)
{
	//Cadena vac�a
	if(!cadena.empty())
	{
		//Borrar caracter si es el introducido por par�metro
		string::size_type posFin = cadena.length()-1;
		if(cadena[posFin] == car)
			cadena.resize(posFin);
	}
}

//Comprobar los argumentos recibidos como par�metro
bool comprobarArgumentos(const int &argc, char** argv,
		string &directorioPruebas, string &directorioSalida,
		string &directorioCopia)
{
	//Comprobaci�n de par�metros
	if(argc == 1)
		return true;
	//El n�mero de par�metros debe ser impar (el primer par�metro es el nombre del programa)
	if(argc > 7 || argc%2==0)
	{
		cerr<<"ERROR: N�mero de par�metros inv�lido"<<endl;
		return false;
	}

	int i=1;
	//Bucle de reconocimiento de par�metros
	while(i<argc)
	{
		//Opci�n
		char * cad = argv[i];

		//Directorio de entrada de las pruebas
		if(strcmp(cad,"-e")==0)
		{
			++i;
			directorioPruebas =argv[i];
			quitarCaracterFinal(directorioPruebas,'/');
		}
		//Directorio de referencia para las pruebas
		else if(strcmp(cad,"-r")==0)
		{
			++i;
			directorioSalida =argv[i];
			quitarCaracterFinal(directorioSalida,'/');
		}
		//Directorio donde copiar la salida de la tokenizaci�n
		else if(strcmp(cad,"-c")==0)
		{
			++i;
			directorioCopia=argv[3];
			quitarCaracterFinal(directorioCopia,'/');
		}
		//Par�metro err�neo
		else
		{
			cerr<<"ERROR: Par�metro "<<cad<<" desconocido"<<endl;
			return false;
		}

		++i;
	}
	return true;
}


//M�todo de prueba est�ndar y con tiempo (Pasar el valgrind para el consumo de memoria)
int main(int argc, char** argv)
{
	//Pruebas de tokenizaci�n individual
	if(UNIT_TESTS)
		runSuite();

    //Tokenizaci�n de directorios
	string directorioPruebas =DIRECTORIO_PRUEBAS;
	string directorioSalida =DIRECTORIO_SALIDA;
	string directorioCopia=DIRECTORIO_COPIA;

	//Comprobaci�n de par�metros
	if(!comprobarArgumentos(argc,argv,directorioPruebas,directorioSalida,directorioCopia))
	{
		cout<<"USO: "<<argv[0]<<" [-e directorioEntrada] [-r directorioReferencia] [-c directorioCopia] "<<endl;
		return -1;
	}

	//Lanzar la suite te tokenizaci�n de directorios
	if(DIRECTORY_TESTS)
		runSuiteTemporal(directorioPruebas, directorioSalida, directorioCopia);

    return 0;
}



