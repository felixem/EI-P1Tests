#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include <sys/resource.h>
#include <sys/stat.h>
#include <fstream>

#include "tokenizador.h"
#include "tokenizadorClase.h"

//Constante de iteraciones para suite
const int IT_SUITE_TIEMPOS=20;
const int IT_MEDIAS=1000;

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

	a.Tokenizar("pal1&10.00@10.000&000@10/12/85", tokens);
	// La lista de tokens a devolver debería contener: "pal1 10.00 10.000 000 10/12/85"
	resultados.clear();
	resultados.merge({"pal1","10.00","10.000","000", "10/12/85"});
	compararListas(tokens, resultados);

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

//Palabras compuestas
namespace testTiempos
{
void testCompuestas() {
	Tokenizador a("-#", true, false);
	list<string> tokens;

	a.Tokenizar("MS-DOS p1 p2 UN-DOS-TRES", tokens);
	a.Tokenizar("pal1 -MS-DOS p1 p2", tokens);
	a.Tokenizar("pal1 MS-DOS#p3 p1 p2", tokens);
	a.Tokenizar("pal1#MS-DOS#p3 p1 p2", tokens);

	a.DelimitadoresPalabra("/ ");
	a.Tokenizar("MS-DOS p1 p2", tokens);
	a.Tokenizar("pal1 -MS-DOS p1 p2", tokens);
	a.Tokenizar("pal1 MS-DOS#p3 p1 p2", tokens);
	a.Tokenizar("pal1#MS-DOS#p3 p1 p2", tokens);
}

//Tests de url
void testURLs()
{
	Tokenizador a(",", true, false);
	list<string> tokens;
	string s = "p0 http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es p1 p2";

	a.Tokenizar(s, tokens);

	a.DelimitadoresPalabra("/ ");
	a.Tokenizar(s, tokens);

	a.DelimitadoresPalabra("/ &");
	a.Tokenizar(s, tokens);

	a.DelimitadoresPalabra("/&");
	s = "p0 hhttp://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013 p1 p2";
	a.Tokenizar(s, tokens);
}

//Test emails
void testEmails()
{
	Tokenizador a(",", true, false);
	list<string> tokens;

	a.DelimitadoresPalabra("@.&");
	a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", tokens);
	a.Tokenizar("pal1 @iuii.ua.es p1 p2", tokens);
	a.Tokenizar("pal1 cat@iuii.ua.es@cd p1 p2", tokens);

	a.DelimitadoresPalabra("&.");
	a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", tokens);
	a.Tokenizar("pal1 @iuii.ua.es p1 p2", tokens);

	a.Tokenizar("pal1&@iuii.ua.es p1 p2", tokens);
	a.Tokenizar("pal1&catedra@iuii.ua.es p1 p2", tokens);
	a.Tokenizar("pal1 cat@iuii.ua.es@cd p1 p2", tokens);
}

//Test acrónimos
void testAcronimos()
{
	Tokenizador a(",", true, false);
	list<string> tokens;

	a.DelimitadoresPalabra("@.&");
	a.Tokenizar("U.S.A p1 e.g. p2. La", tokens);

	a.DelimitadoresPalabra("");
	a.Tokenizar("U.S.A .U.S.A .p1 p1 e.g. p2. La", tokens);
	a.Tokenizar("a&U.S.A p1 e.g. p2. La", tokens);

	a.DelimitadoresPalabra("&");
	a.Tokenizar("a&U.S.A p1 e.g. p2. La", tokens);
}

//Test números
void testNumeros()
{
	Tokenizador a(",", true, false);
	list<string> tokens;

	a.DelimitadoresPalabra("@.,&");
	a.Tokenizar("pal1 10.000,34 10,000.34 10.000.123.456.789.009,34 10,000,123,456,789,009.34 20.03 40,03 2005 10. 20, 10.0 20,0 La 20,12.456,7.8.9,", tokens);
	a.Tokenizar(".34 ,56", tokens);
	a.Tokenizar("pal1 10.35% 10,35% 23.000,3% 23€ 23.05€ 23,05€ 11$ 11.05$ 3º 4ª", tokens);
	a.Tokenizar("pal1 10.00a 10.000.a.000 10/12/85 1,23E+10", tokens);
	a.Tokenizar("pal1&10.00@10.000&000@10/12/85", tokens);
	a.Tokenizar(".34@@&,56", tokens);
}
}

//Limpiar el directorio de los ficheros .tk generados en ejecuciones anteriores
bool limpiarDirectorio(const string &dirAIndexar)
{
	struct stat dir;
	// Compruebo la existencia del directorio
	int err=stat(dirAIndexar.c_str(), &dir);
	if(err==-1 || !S_ISDIR(dir.st_mode))
	{
		cerr<<"ERROR: No existe el directorio "<<dirAIndexar<<endl;
		return false;
	}
	else
	{
		// Hago una lista en un fichero con find>fich
		string cmd="find "+dirAIndexar+" -follow -name \"*.tk\" |sort > aBorrar";
		system(cmd.c_str());
		//Borrar la lista de ficheros y los ficheros de borrado y de lista de ficheros
		string nombreFic1 = "aBorrar";
		string nombreFic2 = "lista_fich";

		borrarListaFicheros(nombreFic1);
		string str = "rm "+nombreFic1;
		system(str.c_str());
		str = "rm "+nombreFic2;
		system(str.c_str());
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
		{
			string str = "rm "+cadena;
			system(str.c_str());
		}
	}

	fic.close();
	return sinfallos;
}

//Probar la tokenización de directorios
int runSuiteTemporal()
{
	string nomDirectorio ="prueba";
	cout<<"Limpiando directorio "+nomDirectorio<<endl;
	limpiarDirectorio(nomDirectorio);
	cout<<"Directorio "+nomDirectorio+" limpiado"<<endl;

	//Medir el tiempo de tokenización
	long double aa = getcputime();
	//Tokenizar directorio
	Tokenizador tok(".,:;/-@(){}?|[]'=_\"><&#-\r\t",true,true);
	cout <<"Iniciando tokenización del directorio "+nomDirectorio<<endl;
	tok.TokenizarDirectorio(nomDirectorio);
	cout<<"Directorio "+nomDirectorio+" tokenizado"<<endl;
	//Tiempo de tokenización
	cout << "Ha tardado " << getcputime() - aa << " segundos" << endl;

	return 0;
}

//Suite a lanzar pasando el valgrind
int runSuiteEspacio()
{
	for(int i=0; i<IT_SUITE_TIEMPOS; i++)
	{
		testTiempos::testAcronimos();
		testTiempos::testCompuestas();
		testTiempos::testEmails();
		testTiempos::testNumeros();
		testTiempos::testURLs();
	}

	return 0;
}

//Método de prueba estándar y con tiempo
int main()
{
    runSuite();
    runSuiteTemporal();
    return 0;
}

//Probar la tokenización vigilando el espacio
int mainEspacial(){
    runSuite();
	runSuiteEspacio();
    return 0;
}



