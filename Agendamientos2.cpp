#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <stdio.h>
#include <limits>
#include <thread>
#include <tuple>
#include <sys/stat.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <queue>
using namespace std;
const int cambCtx = 0;
const int quantum = 3;
string registro = "";
vector< vector<int> > leerEntrada(string archivo){
    string myText;
    ifstream MyReadFile(archivo);
    vector< vector<int> > procs;

    //Lee la primera linea
    getline (MyReadFile, myText);
    cout << myText << endl;

    while (getline (MyReadFile, myText)) {
        int id, prioridad, burst, arrival;
        sscanf(myText.c_str() , "%d %d %d %d" , &id, &prioridad, &burst, &arrival );
        int aux[] = {id, prioridad, burst, arrival};
		int n = sizeof(aux) / sizeof(aux[0]);
     	vector<int> t(aux, aux + n);
        procs.push_back(t);
        cout << myText << endl;
    }

    MyReadFile.close();
    return procs;
}



int indice(int id, vector< vector<int> > vec){ // retorna el indice donde se encutra el proceso con un id especifico
	for(int i = 0; i < vec.size(); i++){
		if(id == vec[i][0]){
			return i;
		}
	}
}

vector<int> extraer(vector< vector<int> > procs, int tiempo){
	vector< vector<int> > posibles;
	int max;
	vector<int> res;
	
	for(int i = 0; i < procs.size(); i++){ //calculo de las actividades que pueden ser ejecutadas
		if(procs[i][3] <= tiempo){
			posibles.push_back(procs[i]);
		}
	}
	
	if(posibles.empty()){ //si no hay actividades posibles, se retorna {-1, -1, -1, -1}
		int aux[] = {-1,-1,-1,-1};
		res.assign(aux,aux+4);
	}else{
		max = -1;
		for(int i = 0; i < posibles.size(); i++){ //de las posibles se toma la de mayor prioridad. Si dos tienen la mayor prioridad, se toma 
			if(posibles[i][1] > max){			 //la que se deba ejecutar primero
				res = posibles[i];
				max = posibles[i][1];
			}else if(posibles[i][1] == max and posibles[i][3] < res[3]){
				res = posibles[i];
			}
		}	
	}
	return res;
}

bool compareFCFS(vector<int> v1, vector<int> v2){
    return(v1[3]> v2[3]);
}

bool compareSJF(vector<int> v1, vector<int> v2){
    return(v1[2] < v2[2]);
}

bool compareRR(vector<int> v1, vector<int> v2){
    return(v1[3] < v2[3]);
}

bool compareRRPrio(vector<int> v1, vector<int> v2){
    return(v1[1] > v2[1]);
}

void sjf(vector< vector<int> > procs){
    string res;
    float wait = 0;
    float burst = 0;
    float cant = 0;
    int id;
    float turn = 0;
    float totalTurn = 0;
    float totalWait = 0;
    int clock = 0;
    bool encontro = false;
    string gant;
    //Tabla de gant
    res += "==(SJF)============================================= \n";
    res += "ID | Burst | Waiting-Time | Response-Time \n";
    sort(procs.begin(), procs.end(), compareSJF);

    while(!(procs.empty())){
        
        for(auto it = begin(procs); it != end(procs); it++) {
            if (clock >= (*it)[3]){
                encontro = true;
                clock += cambCtx; //agrega el tiempo del cambio de contexto al entrar y salir el proceso
                id = (*it)[0];
                burst = (*it)[2];
                wait = clock - (*it)[3];
                turn = burst + clock;
                res += to_string((int)id) + " | " + to_string((int)burst) + " | "  + to_string((int)wait) +  " | " + to_string((int)turn) + "\n";
                //Contruiur GANT
                for(int j =0; j < cambCtx;  j++){ //se simboliza con c el cambio de contexto
            	gant += "c";
        		}
                gant += "P[" + to_string(id) + "]";
                for(int j = 0; j < burst;  j++){
                    gant += "-";
                    clock++;
                }
                cant++;
                procs.erase(it);
                totalTurn += turn;
                totalWait += wait;
                break;
            }
        }
        if(!encontro){
            clock++;
            gant += "*";
        }
        encontro = false;
    }
    //Imprimir 
            res += "-Promedio Waiting-Time: " +  to_string(totalWait / cant) + "\n";
            res += "-Promedio Turnaraound-Time: " + to_string( totalTurn / cant ) + "\n";
            res +=  gant  + "\n";
            res += "======================================================= \n ";

	cout << res << endl;
	registro += res;
}

void fcfs(vector< vector<int> > procs)
{
    float wait = 0;
    float burst = 0;
    float cant = 0;
    int id;
    float turn = 0;
    float totalTurn = 0;
    float totalWait = 0;
    int clock = 0;
    string res;
    string gant;
    //Tabla de gant
    res += "==(FCFS)============================================= \n";
    res += "ID | Burst | Waiting-Time | Response-Time \n";
    sort(procs.begin(), procs.end(), compareFCFS);

    while(!(procs.empty())){
        //Si ya esta en la hora de ejecucion
        if ((procs.back())[3] <= clock) {
        	clock += cambCtx; //agrega el tiempo del cambio de contexto al entrar y salir el proceso
            id = (procs.back())[0];
            burst = (procs.back())[2];
            wait = clock - (procs.back())[3];
            turn = burst + clock;
            res += to_string((int)id) + " | " + to_string((int)burst) + " | "  + to_string((int)wait) +  " | " + to_string((int)turn) + "\n";
            //Contruiur GANT
            
            for(int j =0; j < cambCtx;  j++){ //se simboliza con c el cambio de contexto
            	gant += "c";
        	}
            gant += "P[" + to_string(id) + "]";
            for(int j = 0; j < burst;  j++){
                gant += "-";
                clock++;
            }
            cant++;
            procs.pop_back();
            totalTurn += turn;
            totalWait += wait;
        }
        else{
            clock++;
            gant += "*";

        }
    }
    //Imprimir 
            res += "-Promedio Waiting-Time: " +  to_string(totalWait / cant) + "\n";
            res += "-Promedio Turnaraound-Time: " + to_string( totalTurn / cant ) + "\n";
            res +=  gant  + "\n";
            res += "======================================================= \n ";

    cout << res << endl;
    registro += res;
}




void priority(vector< vector<int> > procs)
{	
	string ans;
	int cant = procs.size();
	float tiempo = 0;
	vector<int> proceso;
	int ind;	
    float burst;
    int id;
    float turn = 0;
    string gant;
    
    ans = "==(Priority)=============================================\n";
    ans += "ID | Burst | Waiting-Time | Response-Time\n";

    
    while(!procs.empty()){ //hasta que haya agendado todas las actividades
    	proceso = extraer(procs, tiempo);
    	if(proceso[0] == -1 and proceso[1] == -1 and proceso[2] == -1 and proceso[3] == -1){ //si no hay proceso continuar con el reloj
    		tiempo += 1;
    		gant += "*";
		}else{
			tiempo += cambCtx; //agrega el tiempo del cambio de contexto al entrar y salir el proceso
			id = proceso[0];
			burst = proceso[2];
			turn += tiempo + burst;
			
			ans += to_string((int)id) + " | " + to_string((int)burst) + " | " + to_string((int)(tiempo - proceso[3])) + " | " + to_string((int)(burst + tiempo)) + "\n";
			
			for(int j =0; j < cambCtx;  j++){ //se simboliza con c el cambio de contexto
            	gant += "c";
        	}
			
			gant += "p[" + to_string(id) + "]";
			
			tiempo += burst;
			
			for(int j =0; j < burst;  j++){
            	gant += "-";
        	}
			
			ind = indice(proceso[0], procs);  
			procs.erase(procs.begin() + ind);
		}
	}
	
	ans += "-Promedio Waiting-Time: " + to_string(tiempo / cant) + "\n";
	ans += "-Promedio Turnaraound-Time: " + to_string(turn / cant) + "\n";
	ans += gant + "\n";
	ans += "=======================================================\n";
    cout << ans << endl;
    registro += ans;
}



void RR(vector< vector<int> > procs){
	string ans;
	string gant;
	vector< vector<int> > procs2 = procs; //copia de procesos
	vector< vector<int> > procs3 = procs; //copia de procesos
	vector<int> queue; // cola de procesos a ejecutar
	int cantTer = 0; //cuenta cuantos procesos han terminado
	int tam = procs.size(); // cantidad de procesos
	int tiempo = 0;
	int ind;
	sort(procs.begin(), procs.end(), compareRR);
	sort(procs2.begin(), procs2.end(), compareRR);
	sort(procs3.begin(), procs3.end(), compareRR);
	vector< vector<int> > datos; //para cada proceso almacena datos[n][0] = id, datos[n][1] = completion Time, datos[n][2] = turn arround time,  datos[n][3] = waiting time
								//datos[n][4] = response time
	vector<int> response; //para cada proceso guardo el tiempo en que llego de primero
	for(int i = 0 ; i < tam;  i++){ //llenado del vector de datos
        vector <int> dato;
        dato.push_back(procs[i][0]);
        dato.push_back(0);
        dato.push_back(0);
        dato.push_back(0);
        datos.push_back(dato);
        response.push_back(-1);
    }
			
	
	ans = "==(RR)=============================================\n";
    ans += "ID | Burst | Waiting-Time | Response-Time | Completition-Time | Turnaround-Time | \n" ;
	
	while(cantTer != tam){
		while(!procs2.empty() and procs2[0][3] <= tiempo){ //calculo de las actividades que pueden ser ejecutadas en ese tiempo
			queue.push_back(procs2[0][0]); //la cola almacena el id
			procs2.erase(procs2.begin());
		}
			
		if(!queue.empty()){ 
			while(!queue.empty()){ //realizar las operaciones en la cola
				//ejecuto la operacion
				for(int j =0; j < cambCtx;  j++){ //se simboliza con c el cambio de contexto
            		gant += "c";
        		}
        		tiempo += cambCtx; //se suma el cambio de contexto al tiempo
				gant += "p[" + to_string(queue[0]) + "]";
				
				ind = indice(queue[0], procs);
				//cout << "Tiempo: " << tiempo << " ind: " << ind  << " id: " << queue[0] << endl;
				if(response[ind] == -1){
					response[ind] = tiempo;
				}
				if(procs[ind][2] <= quantum){
					tiempo += procs[ind][2];
					for(int j =0; j < procs[ind][2];  j++){
            			gant += "-";
        			}
					procs[ind][2] = 0;
					datos[ind][1] = tiempo; //se coloca el completition time
					datos[ind][2] = tiempo - procs[ind][3]; //se coloca el turn around time (CT - AT)
					datos[ind][3] = (tiempo - procs[ind][3]) - procs3[ind][2]; //se coloca el waiting time (TT - BT)
					datos[ind][4] = response[ind] - procs3[ind][3]; //se coloca el rsponse Time (Donde se ejecuta - AT)
					cantTer += 1; //un proceso ya termino
				}else{
					tiempo += quantum;
					for(int j =0; j < quantum;  j++){
            			gant += "-";
        			}
					procs[ind][2] = procs[ind][2] - quantum;
					while(!procs2.empty() and procs2[0][3] <= tiempo){ //calculo de las actividades que pueden ser ejecutadas en ese tiempo
						queue.push_back(procs2[0][0]); //la cola almacena el id
						procs2.erase(procs2.begin());
					}
					queue.push_back(queue[0]);
				}
				queue.erase(queue.begin());
			}
		}else{
			tiempo += 1;
			gant += "*";
		}
	}
	for(int i = 0; i < tam;  i++){
        ans += to_string(procs3[i][0]) + " | " + to_string(procs3[i][2]) + " | " + to_string(datos[i][3]) + " | " + to_string(datos[i][4]) + " | " + to_string(datos[i][1]) +  " | " + to_string(datos[i][2]) + "\n";
    }
    float turnProm = 0;
    float waitProm = 0;
    for(int i = 0; i < tam;  i++){ 
     	turnProm += datos[i][2];
     	waitProm += datos[i][3];
    }
    turnProm = turnProm / tam;
    waitProm = waitProm / tam;
    ans += "-Promedio Waiting-Time: " + to_string(waitProm) + "\n";
	ans += "-Promedio Turnaraound-Time: " + to_string(turnProm) + "\n";
	ans += gant + "\n";
    ans += "=======================================================\n";
    cout << ans << endl;
    registro += ans;
}


void RRPrio(vector< vector<int> > procs){
	string ans;
	string gant;
	vector< vector<int> > procs2 = procs; //copia de procesos
	vector< vector<int> > procs3 = procs; //copia de procesos
	vector<int> queue; // cola de procesos a ejecutar
	vector< vector<int> > aux;
	int cantTer = 0; //cuenta cuantos procesos han terminado
	int tam = procs.size(); // cantidad de procesos
	int tiempo = 0;
	int ind;
	sort(procs.begin(), procs.end(), compareRR);
	sort(procs2.begin(), procs2.end(), compareRR);
	sort(procs3.begin(), procs3.end(), compareRR);
	vector< vector<int> > datos; //para cada proceso almacena datos[n][0] = id, datos[n][1] = completion Time, datos[n][2] = turn arround time,  datos[n][3] = waiting time
								//datos[n][4] = response time
	vector<int> response; //para cada proceso guardo el tiempo en que llego de primero
	for(int i = 0 ; i < tam;  i++){ //llenado del vector de datos
        vector <int> dato;
        dato.push_back(procs[i][0]);
        dato.push_back(0);
        dato.push_back(0);
        dato.push_back(0);
        datos.push_back(dato);
        response.push_back(-1);
    }
			
	
	ans = "==(RRPrio)=============================================\n";
    ans += "ID | Burst | Waiting-Time | Response-Time | Completition-Time | Turnaround-Time | \n" ;
	
	while(cantTer != tam){
		while(!procs2.empty() and procs2[0][3] <= tiempo){ //calculo de las actividades que pueden ser ejecutadas en ese tiempo
			aux.push_back(procs2[0]);
			procs2.erase(procs2.begin());
		}
		sort(aux.begin(), aux.end(), compareRRPrio);
		for(int j = 0; j < aux.size(); j++){
			queue.push_back(aux[j][0]); //la cola almacena el id	
		}
		aux.clear();
		 
			
		if(!queue.empty()){ 
			while(!queue.empty()){ //realizar las operaciones en la cola
				//ejecuto la operacion
				for(int j =0; j < cambCtx;  j++){ //se simboliza con c el cambio de contexto
            		gant += "c";
        		}
        		tiempo += cambCtx; //se suma el cambio de contexto al tiempo
				gant += "p[" + to_string(queue[0]) + "]";
				
				ind = indice(queue[0], procs);
				//cout << "Tiempo: " << tiempo << " ind: " << ind  << " id: " << queue[0] << endl;
				if(response[ind] == -1){
					response[ind] = tiempo;
				}
				if(procs[ind][2] <= quantum){
					tiempo += procs[ind][2];
					for(int j =0; j < procs[ind][2];  j++){
            			gant += "-";
        			}
					procs[ind][2] = 0;
					datos[ind][1] = tiempo; //se coloca el completition time
					datos[ind][2] = tiempo - procs[ind][3]; //se coloca el turn around time (CT - AT)
					datos[ind][3] = (tiempo - procs[ind][3]) - procs3[ind][2]; //se coloca el waiting time (TT - BT)
					datos[ind][4] = response[ind] - procs3[ind][3]; //se coloca el rsponse Time (Donde se ejecuta - AT)
					cantTer += 1; //un proceso ya termino
				}else{
					tiempo += quantum;
					for(int j =0; j < quantum;  j++){
            			gant += "-";
        			}
					procs[ind][2] = procs[ind][2] - quantum;
					while(!procs2.empty() and procs2[0][3] <= tiempo){ //calculo de las actividades que pueden ser ejecutadas en ese tiempo
						queue.push_back(procs2[0][0]); //la cola almacena el id
						procs2.erase(procs2.begin());
					}
					queue.push_back(queue[0]);
				}
				queue.erase(queue.begin());
			}
		}else{
			tiempo += 1;
			gant += "*";
		}
	}
	for(int i = 0; i < tam;  i++){
        ans += to_string(procs3[i][0]) + " | " + to_string(procs3[i][2]) + " | " + to_string(datos[i][3]) + " | " + to_string(datos[i][4]) + " | " + to_string(datos[i][1]) +  " | " + to_string(datos[i][2]) + "\n";
    }
    float turnProm = 0;
    float waitProm = 0;
    for(int i = 0; i < tam;  i++){ 
     	turnProm += datos[i][2];
     	waitProm += datos[i][3];
    }
    turnProm = turnProm / tam;
    waitProm = waitProm / tam;
    ans += "-Promedio Waiting-Time: " + to_string(waitProm) + "\n";
	ans += "-Promedio Turnaraound-Time: " + to_string(turnProm) + "\n";
	ans += gant + "\n";
    ans += "=======================================================\n";
    cout << ans << endl;
    registro += ans;
}



inline bool exists(const std::string& name) {
    return ( access( name.c_str(), F_OK ) != -1 );
}


//-------------------------------------------------------------------------------------------------------

typedef long long int ll;
 
// Utility function to find
// GCD of 'a' and 'b'
int gcd(int a, int b)
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}
struct Tupla {
 
    int id;
 
    int capacidad;

    int prioridad;
 
    // this will used to initialize the variables
    // of the structure
    Tupla(int i, int c, int p)
        : id(i), capacidad(c), prioridad(p)
    {
    }
};

struct CompTupla {
    bool operator()(Tupla t1, Tupla t2)
    {
        // return "true" if "p1" is ordered
        // before "p2", for example:
        return t1.prioridad > t2.prioridad;
    }
};

// Returns LCM of array elements
ll findlcm(vector< vector<int> > vec)
{
    // Initialize result
    ll ans = vec[0][2];
 
    // ans contains LCM of arr[0], ..arr[i]
    // after i'th iteration,
    for (auto it = vec.begin(); it != vec.end(); it++)
        ans = ((((*it)[2] * ans)) /
                (gcd((*it)[2], ans)));
 
    return ans;
}


vector< vector<int> > leerReal(string archivo){
    string myText;
    ifstream MyReadFile("realtime.txt");
    vector< vector<int> > procs;

    //Lee la primera linea
    getline (MyReadFile, myText);
    cout << myText << endl;

    while (getline (MyReadFile, myText)) {
        int id, capacidad, periodo;
        sscanf(myText.c_str() , "%d %d %d %d" , &id, &capacidad, &periodo );
        int aux[] = {id, capacidad, periodo};
		int n = sizeof(aux) / sizeof(aux[0]);
     	vector<int> t(aux, aux + n);
        procs.push_back(t);
        cout << myText << endl;
    }

    MyReadFile.close();
    return procs;
}

bool compare(vector<int> v1, vector<int> v2){
    return(v1[2] < v2[2]);
}


void rms(vector< vector<int> > procs)
{	
	int clock = 0;
	vector< vector<int> >::iterator actual = procs.end();
    string gant;
    ll lcm = findlcm(procs);
    priority_queue<Tupla, vector<Tupla>, CompTupla> cola;

    gant = "==(RMS)===============================================\n";
    sort(procs.begin(), procs.end(), compare); //Ordena segun el Periodo

    for(int i = 0; i < lcm ; i++){
        for (auto it = procs.begin(); it != procs.end(); it++ ){
            //Reinserta todas las tareas que se tienen que ejecutar por periodo
            if (clock % (*it)[2] == 0){
                cola.push(Tupla((*it)[0],(*it)[1],(*it)[2]));
            }
        }
        // while(!cola.empty()){
        //     cout << cola.top().id << endl;
        //     cola.pop();
        // }
        if (cola.empty()){
            gant += "*";
        }
        else if (cola.top().capacidad == 1){
            
            gant += "P[" + to_string(cola.top().id) + "]";
            cola.pop();
        }
        else{

            gant += "P[" + to_string(cola.top().id) + "]";
            Tupla c = cola.top();
            cola.pop();
            cola.push(Tupla(c.id, c.capacidad-1, c.prioridad));
            
            
        }
        clock++;
    }
    cout << gant << endl;
    registro += gant;
}



void edf(vector< vector<int> > procs)
{	
	string ans;
	int cant = procs.size();
	int clock = 0;
	vector< vector<int> >::iterator actual = procs.end();
    float turn = 0;
    string gant;
    
    ans = "==(EDF)===============================================\n";
    ans += "ID | Burst | Response-Time\n";
        
    for(auto it = procs.begin(); it != procs.end(); it++){
        (*it)[1] = (*it)[2];
    }
    
    while(!procs.empty()){ //hasta que haya agendado todas las actividades
        //Saca el siguiente a ejecutar------------------------------------
    	for(auto it = procs.begin(); it != procs.end(); it++){
            if ((*it)[3] > clock){
                break;
            }
            if (actual == procs.end()){
                actual = it;
                //Cambia de contexto
                    for(int j =0; j < cambCtx;  j++){ //se simboliza con c el cambio de contexto
                        gant += "c";
                        clock++;
                    }
                    gant += "P[" + to_string((*actual)[0]) + "]";
                    break;
            }
            else{
                if ((*it)[2] < (*actual)[2]){
                    actual = it;
                    //Cambia de contexto
                    for(int j =0; j < cambCtx;  j++){ //se simboliza con c el cambio de contexto
                        gant += "c";
                        clock++;
                    }
                    gant += "P[" + to_string((*actual)[0]) + "]";
                    break;
                }
            }
        }
        //-----------------------------------------------------------------
        if (actual == procs.end()){
            gant += "*";
        }
        else{
            //SI es la ultima ejecucion termina
            if( (*actual)[2] == 1){
                gant += "-";
                ans += to_string((int)(*actual)[0]) + " | " + to_string((int)(*actual)[1]) + " | " + to_string(clock) + "\n";
                turn += clock + 1;
                procs.erase(actual);
                actual = procs.end();
            }
            else{
                gant += "-";
                (*actual)[2]--;
            }
        }

        clock++;
    }
	ans += "-Promedio Turnaraound-Time: " + to_string(turn / cant) + "\n";
	ans += gant + "\n";
	ans += "=======================================================\n";
    cout << ans << endl;
    registro += ans;
}




//----------------------------------------------------------------------------------------------------------








int main(){
    string arch;
    vector< vector<int> > proc;
    vector< vector<int> > copia1;
    vector< vector<int> > copia2;
    vector< vector<int> > copia3;
    vector< vector<int> > copia4;
    vector< vector<int> > copia5;
	/*
	arch = "procesos.txt";
    proc = leerEntrada(arch);
    copia4 = proc;
    copia5 = proc;
    RR(copia4);
    RRPrio(copia5);
    */
	
    cout << "ALGORITMOS DE AGENDAMIENTO:" << endl;
	cout << "=======================================================" << endl;
	while(true){
		cout << "Ingrese el nombre del archivo donde se especificaron los procesos: ";
		cin >> arch;
		if(exists(arch)){
			cout << "El archivo escogido es: " << endl;
			proc = leerEntrada(arch);
			while(true){
				int opcion;
				cout << "MENU"<< endl;
				cout << "=======================================================" << endl;
				cout << "1) Planificadores  no real-time" << endl;
				cout << "2) Planificadores  real-time" << endl;
				cout << "3) Salir" << endl;
				cout << "Ingrese 1, 2 o 3 dependiendo de la opcion: " ;
				cin >> opcion;
				if(opcion == 1){
					string select;
					string arch2;
					cout << "=======================================================" << endl;
					cout << "1) First-Come, First-Served (FCFS)" << endl;  
					cout << "2) Shortest-job-rst (SJF)" << endl;
					cout << "3) Priority" << endl;
					cout << "4) Round-Robin (RR)" << endl;
					cout << "5) Priority/Round-Robin" << endl;
					cout << "6) Ejecutar todos en paralelo" << endl;
					cout << "Ingrese los numeros entre 1 y 4 sin separarlos (ej: 12, 432, 34215, 6): " ;
					cin >> select;
					cout << "Ingrese el arhivo donde desea guardar el registro: " ;
					cin >> arch2;
	
					copia1 = proc;
					copia2 = proc;
					copia3 = proc;
					copia4 = proc;
					copia5 = proc;
					thread t1;
					thread t2;
					thread t3;
					thread t4;
					thread t5;
					for(int i = 0; i < select.size(); i++){
						copia1 = proc;
						if(select[i] == '1'){
							fcfs(copia1);
						}else if(select[i] =='2'){
							sjf(copia1);
						}else if(select[i] == '3'){
							priority(copia1);
						}else if(select[i] == '4'){
							RR(copia1);
						}else if(select[i] == '5'){
							RRPrio(copia1);
						}else if(select[i] == '6'){
							copia1 = proc;
							copia2 = proc;
							copia3 = proc;
							copia4 = proc;
							copia5 = proc;
							thread t1(fcfs, ref(copia1));
	    					thread t2(sjf, ref(copia2));
	    					thread t3(priority, ref(copia3));
	    					thread t4(RR, ref(copia4));
	    					thread t5(RRPrio, ref(copia5));
							t1.join();
	    					t2.join();
	    					t3.join();
	    					t4.join();
	    					t5.join();
						}
					}
					ofstream myfile(arch2);
	        		myfile << registro;
	        		myfile.close();
				    registro = "";
				    
				}else if(opcion == 2){
					string select;
					string arch2;
					cout << "=======================================================" << endl;
					cout << "1) RMS" << endl;  
					cout << "2) EDF" << endl;
					cout << "3) Ejecutar todos en paralelo" << endl;
					cout << "Ingrese los numeros entre 1 y 3 sin separarlos (ej: 12, 132): " ;
					cin >> select;
					cout << "Ingrese el arhivo donde desea guardar el registro: " ;
					cin >> arch2;
					
					copia1 = proc;
					copia2 = proc;
					thread t1;
					thread t2;
					for(int i = 0; i < select.size(); i++){
						copia1 = proc;
						if(select[i] == '1'){
							string file;
							while(true){
								cout << "Ingrese el arhivo donde desea leer los procesos de RMS: " ;
								cin >> file;
								if(exists(file)){
									cout << "El archivo escogido es: " << endl;
									copia1 = leerReal(file);
									break;
								}else{
									cout << "El archivo no existe" << endl;
								}
							}
							rms(copia1);
						}else if(select[i] =='2'){
							edf(copia1);
						}else if(select[i] == '3'){
							string file;
							while(true){
								cout << "Ingrese el arhivo donde desea leer los procesos de RMS: " ;
								cin >> file;
								if(exists(file)){
									cout << "El archivo escogido es: " << endl;
									copia1 = leerReal(file);
									break;
								}else{
									cout << "El archivo no existe" << endl;
								}
							}
							copia2 = proc;
							thread t1(rms, ref(copia1));
	    					thread t2(edf, ref(copia2));
							t1.join();
	    					t2.join();
						}
					}
					ofstream myfile(arch2);
	        		myfile << registro;
	        		myfile.close();
				    registro = "";
				}else if(opcion == 3){
					break;
				}
			}
		}else{
			cout << "El archivo no existe" << endl;
		}
	}    
    
    return 0;
}
