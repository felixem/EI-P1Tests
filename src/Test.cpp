/*Copyright (c) 2015 "Félix Escalona Moncholí"
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

//Constante de iteraciones para suite
const char* LISTA_FICHEROS="lista_fich";
const char* RESULTADO="resultado";
const char* SALIDA_ESPERADA="salida_esperada";
const char* REPORTE_TOKEN="reporte_tokenizacion";
const char* DIRECTORIO_PRUEBAS ="entradas/prueba";
const char* DIRECTORIO_SALIDA ="salidas/salida";
const char* DIRECTORIO_COPIA="salida_ejecucion";
const char* DIRECTORIO_DIFERENCIAS="diferencias";
const char* EXTENSION_TOKEN="tk";
const char* EXTENSION_DIFERENCIAS="diff";

//Declaración de funciones
bool borrarListaFicheros(const string &listaFic);

//Mostrar resultado esperado y obtenido
void MostrarResultados(list<string> &tokens, list<string> &resultados)
{
	cout <<"Resultado esperado:"<<endl;
	for(list<string>::iterator it = resultados.begin(); it!=resultados.end(); it++)
	{
		cout << (*it) << "\t";
	}
	cout <<endl;
	cout <<"Resultado obtenido:"<<endl;
	for(list<string>::iterator it = tokens.begin(); it!=tokens.end(); it++)
	{
		cout << (*it) << "\t";
	}
}

//Comparar listas
void compararListas(list<string> &tokens, list<string> &resultados)
{
	//Igualdad de tamaños
	if(resultados.size() != tokens.size())
		//Mostrar resultados esperados
		MostrarResultados(tokens,resultados);

	ASSERT_EQUAL(resultados.size(), tokens.size());

	//Igualdad de contenido
	list<string>::iterator result = resultados.begin();
	for(list<string>::iterator it = tokens.begin(); it!=tokens.end(); it++)
	{
		//Mostrar errores
		if((*result)!=(*it))
			MostrarResultados(tokens,resultados);

		ASSERT_EQUAL(*result, *it);
		result++;
	}
}

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
	// La lista de tokens a devolver debería contener: " p0, http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"p0",
		"http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es",
		"p1","p2"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/ ");
	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver debería contener: " p0, http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es, p1, p2"
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/ &");
	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver debería contener: " p0, http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es, p1, p2"
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("/&");
	s = "p0 hhttp://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013 p1 p2";
	a.Tokenizar(s, tokens);
	// La lista de tokens a devolver debería contener: " p0, hhttp:,intime.dlsi.ua.es:8080, dossierct, index.jsp?lang=es, status=probable, date=22-01-2013, p1, p2"
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
	// La lista de tokens a devolver debería contener:	"catedraTelefonicaUA@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"catedraTelefonicaUA@iuii.ua.es","p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 @iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver debería contener: "pal1, iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 cat@iuii.ua.es@cd p1 p2", tokens);
	// La lista de tokens a devolver debería contener: "pal1, cat, iuii.ua.es@cd, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","cat", "iuii.ua.es@cd", "p1", "p2"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("&.");
	a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver debería contener: "catedraTelefonicaUA@iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"catedraTelefonicaUA@iuii.ua.es","p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 @iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver debería contener: "pal1, @iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","@iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1&@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver debería contener: "pal1, @iuii.ua.es, p1, p2"
	resultados.clear();
	resultados.merge({"pal1","@iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1&catedra@iuii.ua.es p1 p2", tokens);
	// La lista de tokens a devolver debería contener: "pal1, catedra@iuii.ua.es,	p1, p2"
	resultados.clear();
	resultados.merge({"pal1","catedra@iuii.ua.es", "p1", "p2"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 cat@iuii.ua.es@cd p1 p2", tokens);
	// La lista de tokens a devolver debería contener: "pal1, cat@iuii.ua.es@cd,	p1, p2"
	resultados.clear();
	resultados.merge({"pal1","cat@iuii.ua.es@cd", "p1", "p2"});
	compararListas(tokens, resultados);

}

//Test acrónimos
void testAcronimos()
{
	Tokenizador a(",", true, false);
	list<string> tokens, resultados;

	a.DelimitadoresPalabra("@.&");
	a.Tokenizar("U.S.A p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver debería contener: "U.S.A, p1, e.g, p2, La"
	resultados.clear();
	resultados.merge({"U.S.A","p1", "e.g", "p2", "La"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("");
	a.Tokenizar("U.S.A .U.S.A .p1 p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver debería contener: "U.S.A, U.S.A, .p1, p1, e.g, p2., La"
	resultados.clear();
	resultados.merge({"U.S.A","U.S.A",".p1","p1", "e.g", "p2.", "La"});
	compararListas(tokens, resultados);

	a.Tokenizar("a&U.S.A p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver debería contener: "a&U.S.A, p1, e.g, p2., La"
	resultados.clear();
	resultados.merge({"a&U.S.A","p1", "e.g", "p2.", "La"});
	compararListas(tokens, resultados);

	a.DelimitadoresPalabra("&");
	a.Tokenizar("a&U.S.A p1 e.g. p2. La", tokens);
	// La lista de tokens a devolver debería contener: "a, U.S.A, p1, e.g, p2.,	La"
	resultados.clear();
	resultados.merge({"a", "U.S.A","p1", "e.g", "p2.", "La"});
	compararListas(tokens, resultados);
}

//Test números
void testNumeros()
{
	Tokenizador a(",", true, false);
	list<string> tokens, resultados;

	a.DelimitadoresPalabra("@.,&");
	a.Tokenizar("pal1 10.000,34 10,000.34 10.000.123.456.789.009,34 10,000,123,456,789,009.34 20.03 40,03 2005 10. 20, 10.0 20,0 La 20,12.456,7.8.9,", tokens);
	// La lista de tokens a devolver debería contener: "pal1 10.000,34 10,000.34 10.000.123.456.789.009,34 10,000,123,456,789,009.34 20.03
	// 40,03 2005 10 20 10.0 20,0 La 20,12.456,7.8.9"
	resultados.clear();
	resultados.merge({"pal1","10.000,34", "10,000.34", "10.000.123.456.789.009,34", "10,000,123,456,789,009.34", "20.03",
		"40,03","2005", "10", "20", "10.0", "20,0", "La", "20,12.456,7.8.9"});
	compararListas(tokens, resultados);

	a.Tokenizar(".34 ,56", tokens);
	// La lista de tokens a devolver debería contener: "0.34 0,56"
	resultados.clear();
	resultados.merge({"0.34","0,56"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 10.35% 10,35% 23.000,3% 23€ 23.05€ 23,05€ 11$ 11.05$ 3º 4ª", tokens);
	// La lista de tokens a devolver debería contener: "pal1 10.35 % 10,35 % 23.000,3 %	23 € 23.05 € 23,05 € 11	$
	// 11.05 $ 3 º 4 ª"
	resultados.clear();
	resultados.merge({"pal1","10.35", "%", "10,35", "%", "23.000,3","%", "23", "€", "23.05", "€",
		"23,05", "€", "11", "$", "11.05", "$", "3", "º", "4", "ª"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 10.00a 10.000.a.000 10/12/85 1,23E+10", tokens);
	// La lista de tokens a devolver debería contener: "pal1 10.00a 10.000.a.000 10/12/85 1	23E+10"
	resultados.clear();
	resultados.merge({"pal1","10.00a", "10.000.a.000", "10/12/85", "1", "23E+10"});
	compararListas(tokens, resultados);

	/* PRUEBA INVALIDADA POR POSIBLE AMBIGÜEDAD ENTRE EMAIL Y NÚMERO
	a.Tokenizar("pal1&10.00@10.000&000@10/12/85", tokens);
	// La lista de tokens a devolver debería contener: "pal1 10.00 10.000 000 10/12/85"
	resultados.clear();
	resultados.merge({"pal1","10.00","10.000","000", "10/12/85"});
	compararListas(tokens, resultados);*/

	a.Tokenizar(".34@@&,56", tokens);
	// La lista de tokens a devolver debería contener: "0.34 0,56"
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
	a.Tokenizar("MS#DOS 10 España Éspáñé OS_2 [high low]", lt3);
	resultados.clear();
	resultados.merge({"MS#DOS", "10", "España", "Éspáñé", "OS", "2", "[high", "low]"});
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
	a.Tokenizar("MS#DOS 10 España Éspáñé OS_2 [high low]", lt3);
	resultados.clear();
	resultados.merge({"ms#dos", "10", "españa", "españe", "os", "2", "[high", "low]" });
	compararListas(lt3, resultados);

	a.Tokenizar("áéíóú ÁÉÍÓÚ Ññ ", lt3);
	resultados.clear();
	resultados.merge({"aeiou", "aeiou", "ññ"});
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

	a.Tokenizar("pal1 10.35% 10,35% 23.000,3% 23€ 23.05€ 23,05€ 11$ 11.05$ 3º 4ª", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1	10.35	%	10,35	%	23.000,3	%	23	€	23.05	€	23,05	€
	//	11	$	11.05	$	3	º	4	ª"
	resultados.clear();
	resultados.merge({"pal1", "10.35", "%", "10,35", "%", "23.000,3", "%",
					"23", "€", "23.05", "€", "23,05", "€", "11",
					"$", "11.05","$","3","º","4","ª"});
	compararListas(tokens, resultados);

	a.Tokenizar("pal1 10.00a 10.000.a.000 10/12/85 1,23E+10", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1	10.00a	10.000.a.000	10/12/85 1	23E+10	"
	resultados.clear();
	resultados.merge({"pal1", "10.00a", "10.000.a.000",
					"10/12/85", "1", "23E+10"});
	compararListas(tokens, resultados);

	//NO SE COMPRUEBA POR POSIBLE AMBIGÜEDAD
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
	a.Tokenizar("Pal1&10.00@10.000&000@10/12/85 La", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1	10.00	10.000	000	10/12/85	la"
	resultados.clear();
	resultados.merge({"pal1", "10.00", "10.000", "000", "10/12/85", "la"});
	compararListas(tokens, resultados);

	a.CasosEspeciales (false);
	a.Tokenizar("Pal1&10.00@10.000&000@10/12/85 La", tokens);
	// La lista de tokens a devolver deber�a contener: "pal1	10	00	10	000	000	10/12/85 la"
	resultados.clear();
	resultados.merge({"pal1", "10", "00", "10", "000", "000", "10/12/85 la"});
	compararListas(tokens, resultados);
}

void runSuite(){
	cute::suite s;

	//Test casos especiales
	s.push_back(CUTE(testCompuestas));
	s.push_back(CUTE(testURLs));
	s.push_back(CUTE(testEmails));
	s.push_back(CUTE(testAcronimos));
	s.push_back(CUTE(testNumeros));

	//Tests tokenizador
	s.push_back(CUTE(tokenizador01));
	s.push_back(CUTE(tokenizador02));
	s.push_back(CUTE(tokenizador03));
	s.push_back(CUTE(tokenizador04));
	s.push_back(CUTE(tokenizador05));
	s.push_back(CUTE(tokenizador06));
	s.push_back(CUTE(tokenizador07));
	s.push_back(CUTE(tokenizador08));
	s.push_back(CUTE(tokenizador09));

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

//Borrar fichero según su ruta
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

//Volcar la indexación de un directorio en un fichero según un filtro de extensión de archivos
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
		string cmd="find "+directorio+" -follow -name \"*."+extension+"\" |sort > "+nomArchivo;
		system(cmd.c_str());
		return true;
	}
}

//Limpiar el directorio de los ficheros con la extensión deseada
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
		//Borrar la lista de ficheros que se generan en cada ejecución
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
	while(!fic.eof())
	{
		cadena="";
		getline(fic, cadena);
		//Borrar fichero
		if(cadena.length()!=0)
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

		//Leer línea
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

			//Error en la comparación de rutas
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
		i++;
	}

	//Mala generación en la tokenización de ficheros
	if(!res.eof())
	{
		cerr<<"El número de ficheros resultado es mayor del esperado"<<endl;
		sinfallos=false;
	}
	else if(!esp.eof())
	{
		cerr<<"El número de ficheros resultado es menor del esperado"<<endl;
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
	string cmd="find "+directorio+" -follow -name \"*."+ext+"\" | xargs rm -rf";
	system(cmd.c_str());
}

//Copiar los ficheros de una extension dentro de un directorio a otro directorio respectando la jerarquía de directorios interna
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
		//Copiar todos los ficheros que tengan la extensión de entrada dentro del directorio origen
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
	if(calcarEstructuraDirectorios(directorioSalida,directorioComparaciones))
		cout<<"Directorio "+directorioComparaciones+" regenerado"<<endl;
	else
		cerr<<"No se pudo regenerar el directorio "<<directorioComparaciones<<endl;

	//Borrar el directorio de copia
	if(borrarDirectorio(directorioCopia))
		cout<<"Directorio "+directorioCopia<<" borrado"<<endl;

}

//Método para testear la tokenización de directorios
bool testDirectorio(const Tokenizador &tok, const string& directorioPruebas,
		const string& directorioSalida, long double &tiempoFinal)
{
	bool sinerrores = true;

	//Tokenizar el directorio
	if(tok.TokenizarDirectorio(directorioPruebas))
		tiempoFinal = getcputime();
	else
	{
		cerr<<"Hubo problemas graves en la tokenización del directorio"<<endl;
		sinerrores = false;
		tiempoFinal = getcputime();
	}

	return sinerrores;
}

//Saber si un fichero está vacío
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

//Escribir en el informe los ficheros que no estén vacíos
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
		//Comprobar creación del archivo de reporte
		if(ficSalida.is_open())
		{
			//Añadir un título dentro del reporte
			ficSalida<<"Listado de ficheros de diferencia que reportan errores de tokenización:";

			//Recorrer el fichero mientras no se llegue al final
			while(!ficEntrada.eof())
			{
				string cadena="";
				getline(ficEntrada,cadena);

				//Comprobar que se ha leído algo
				if(cadena.length()!=0)
				{
					//Añadir en el reporte los ficheros no vacíos
					if(!esFicheroVacio(cadena))
						ficSalida<<endl<<cadena;
				}
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

//Probar la tokenización de directorios
int runSuiteTemporal(const string& directorioPruebas, const string& directorioSalida, const string& directorioCopia)
{
	string directorioComparaciones =DIRECTORIO_DIFERENCIAS;

	//Limpiar temporales
	limpiarDirectoriosTemporales(directorioPruebas,directorioSalida,directorioComparaciones,directorioCopia);

	//Tokenización de directorio
	cout <<"------------------------------------------------"<<endl;
	cout <<"Iniciando tokenización del directorio "+directorioPruebas<<endl;

	//Medir el tiempo de tokenización
	long double tiempo;
	long double aa = getcputime();
	//Declarar tokenizador
	Tokenizador tok(".,:;/-@*(){}?!|[]'=_\"><&#-\r\t",true,true);

	//Tokenizar directorio
	if(testDirectorio(tok,directorioPruebas,directorioSalida,tiempo))
	{
		//Mostrar resultado temporal
		cout << "Ha tardado " << tiempo - aa << " segundos en tokenizar el directorio" << endl;

		//Comprobar errores de comparación
		if(compararSalidas(directorioPruebas,directorioSalida))
		{
			//Generar un fichero de resumen sobre las diferencias entre la salida esperada y la generada por la tokenización
			string reporte =REPORTE_TOKEN;
			if(generarInforme(directorioComparaciones,reporte))
				cout<<"Informe de comparaciones generado en "<<reporte<<endl;
		}
		else
			cerr<<"Hubo problemas en la comparación de los ficheros generados"<<endl;

		//Hacer una copia de los ficheros .tk generados a otra carpeta para poder usarla como referencia en próximas ejecuciones
		if(copiarArchivosExtension(directorioPruebas,EXTENSION_TOKEN,directorioCopia))
			cout<<"Salida de la tokenización movida desde "<<directorioPruebas<<" hacia "<<directorioCopia<<endl;

		//Limpiar ficheros temporales
		limpiarFicherosTemporales();
	}
	//Errores de tokenización
	else
	{
		cerr<<"Se produjeron errores durante la tokenización del directorio"<<endl;
		return -1;
	}

	return 0;
}

//Quitar caracter final
void quitarCaracterFinal (string &cadena, const char &car)
{
	//Cadena vacía
	if(!cadena.empty())
	{
		//Borrar caracter si es el introducido por parámetro
		string::size_type posFin = cadena.length()-1;
		if(cadena[posFin] == car)
			cadena.resize(posFin);
	}
}

//Comprobar los argumentos recibidos como parámetro
bool comprobarArgumentos(const int &argc, char** argv,
		string &directorioPruebas, string &directorioSalida,
		string &directorioCopia)
{
	//Comprobación de parámetros
	if(argc == 1)
		return true;
	//El número de parámetros debe ser impar (el primer parámetro es el nombre del programa)
	if(argc > 7 || argc%2==0)
	{
		cerr<<"ERROR: Número de parámetros inválido"<<endl;
		return false;
	}

	int i=1;
	//Bucle de reconocimiento de parámetros
	while(i<argc)
	{
		//Opción
		char * cad = argv[i];

		//Directorio de entrada de las pruebas
		if(strcmp(cad,"-e")==0)
		{
			i++;
			directorioPruebas =argv[i];
			quitarCaracterFinal(directorioPruebas,'/');
		}
		//Directorio de referencia para las pruebas
		else if(strcmp(cad,"-r")==0)
		{
			i++;
			directorioSalida =argv[i];
			quitarCaracterFinal(directorioSalida,'/');
		}
		//Directorio donde copiar la salida de la tokenización
		else if(strcmp(cad,"-c")==0)
		{
			i++;
			directorioCopia=argv[3];
			quitarCaracterFinal(directorioCopia,'/');
		}
		//Parámetro erróneo
		else
		{
			cerr<<"ERROR: Parámetro "<<cad<<" desconocido"<<endl;
			return false;
		}

		i++;
	}
	return true;
}


//Método de prueba estándar y con tiempo (Pasar el valgrind para el consumo de memoria)
int main(int argc, char** argv)
{
	//Pruebas de tokenización individual
    runSuite();

    //Tokenización de directorios
	string directorioPruebas =DIRECTORIO_PRUEBAS;
	string directorioSalida =DIRECTORIO_SALIDA;
	string directorioCopia=DIRECTORIO_COPIA;

	//Comprobación de parámetros
	if(!comprobarArgumentos(argc,argv,directorioPruebas,directorioSalida,directorioCopia))
	{
		cout<<"USO: "<<argv[0]<<" [-e directorioEntrada] [-r directorioReferencia] [-c directorioCopia] "<<endl;
		return -1;
	}

	//Lanzar la suite te tokenización de directorios
    runSuiteTemporal(directorioPruebas, directorioSalida, directorioCopia);

    return 0;
}



