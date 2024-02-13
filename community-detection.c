#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXSTRLEN 20

//Queue struct'i
typedef struct Queue {
    int *items;
    int maxSize;
    int currentSize;
    int front;
    int rear;
}Queue;

//Adj matrix kullanan graph struct'i
typedef struct Graph {
    int size;
    int **adjMatrix;
    int *visited;
}Graph;

//Fonksiyon prototipleri
Queue *createQueue(int size);
int isEmpty(Queue* q);
int isFull(Queue *q);
void enqueue(Queue *q, int item);
int dequeue(Queue *q);
void printQueue(Queue *q);

Graph *createGraph(int size);
Graph *readGraphFromFile(const char *filename);
void addEdge(Graph *G, int src, int dest);
void findMaxBetweennessWithBFS(Graph *G);
void printGraph(Graph *G);
int *maxElement(double **arr, int size);


int main()
{
    //Kullanicidan graph olustururken kullanilacak olan .txt dosyasinin ismi alinir
    int i, j;
    char *filename = (char*)malloc(MAXSTRLEN * sizeof(char));
    printf("\nEnter a valid file name: ");
    scanf("%s", filename);

    //Graf olusturulur
    Graph *G = readGraphFromFile(filename);
    printGraph(G);

    //Ana fonksiyon
    findMaxBetweennessWithBFS(G);

    //Olusturulan belleklerin free edilmesi
    for (i=0; i<G->size; i++) {
        free(G->adjMatrix[i]);
    }
    free(G->adjMatrix);
    free(G->visited);
    free(G);

    return 0;
}


Queue *createQueue(int size)
{
    //Queue olusturan fonksiyon
    Queue *q = (Queue*)malloc(sizeof(Queue));
    if (q == NULL) {
		printf("\nError occured while allocating memory, terminating program!");
		exit(1);
	}

    q->items = (int*)malloc(q->maxSize * sizeof(int));
    if (q->items == NULL) {
		printf("\nError occured while allocating memory, terminating program!");
		exit(1);
	}

    //Baslangicta front ve rear queue'nun 0. gozune isaret etmektedir
    q->front = 0;
    q->rear = 0;
    q->maxSize = size;
    q->currentSize = 0;

    return q;
}


int isEmpty(Queue* q)
{
    //Queue'nun bos olup olmadigini soyleyen fonksiyon
    int empty = 0;
    //Eger currentSize degeri 0 iste queue bostur
    if (q->currentSize == 0)
    {
        empty = 1;
    }
    //Empty degeri 1 ise queue bos, 0 ise bos degil
    return empty;
}


int isFull(Queue *q)
{
    //Queue'nun dolu olup olmadigini soyleyen fonksiyon
    int full = 0;
    //Eger currentSize degeri maxSize degerine esit ise queue doludur
    if (q->currentSize == q->maxSize) {
        full = 1;
    }
    //Full degeri 1 ise queue bos, 0 ise bos degil
    return full;
}


void enqueue(Queue *q, int item)
{
    //Queue'ya eleman yerlestiren fonksyion
    //Eger queue doluysa hicbir sey yapma
    if (isFull(q) == 1) {
        printf("\nQueue is full!");
    }
    else {
    //Queue dolu degilse rear'in gosterdigi yere eleman ekle ve rear'i 1 arttir
        q->items[q->rear] = item;
        q->rear++;
        q->currentSize++;
    }
}


int dequeue(Queue *q)
{
    //Queue'dan eleman ceken fonksyion
    int item;
    //Eger queue bossa hicbir sey yapma (-1 dondur)
    if (isEmpty(q) == 1) {
        printf("\nQueue is empty!");
        item = -1;
    }
    else {
    //Queue bos degilse front'un gosterdigi yerdeki elemani dondur ve front'u 1 arttir
        item = q->items[q->front];
        q->front++;
        q->currentSize--;
    }

    return item;
}


void printQueue(Queue *q) 
{
    //Ekrana queue'yu yazdiran fonksiyon (debug amacli)
	int i = q->front;
	printf("\nQueue: ");
				
	while (i != (q->rear % q->maxSize)) {
		printf("%d ", q->items[i]);
		i = (i+1) % q->maxSize;
		
	}
}


Graph *createGraph(int size)
{
    //Graph olusturan fonksiyon
    int i;
    Graph *G = (Graph*)malloc(sizeof(Graph));
    if (G == NULL) {
		printf("\nError occured while allocating memory, terminating program!");
		exit(1);
	}
    //Graph'taki node sayisinin karesi kadar elemana sahip adj matrisini olustur
    G->size = size;
    G->adjMatrix = (int**)calloc(size, sizeof(int*));
    if (G->adjMatrix == NULL) {
		printf("\nError occured while allocating memory, terminating program!");
		exit(1);
	}
    //Adj matrisi satirlarini olustur
    for (i=0; i<size; i++) {
        G->adjMatrix[i] = (int*)calloc(size, sizeof(int));
        if (G->adjMatrix[i] == NULL) {
		    printf("\nError occured while allocating memory, terminating program!");
		    exit(1);
	    }
    }
    //Visited dizisini olustur
    G->visited = (int*)calloc(size, sizeof(int));

    return G;
}


void addEdge(Graph *G, int src, int dest)
{
    //Adj matrisinde i ve j node'lari temsil eder,
    //i ve j'nci goze karsilik gelen deger orada kenar olup olmadigini gosterir
    //Eger 1 ise kenar var, 0 ise yok
    //Simetrik olarak [i][j] ve [j][i] degerleri ayni kenari temsil ettiginden
    //bu gozlerdeki degerleri esitle
    G->adjMatrix[src][dest] = 1;
    G->adjMatrix[dest][src] = 1;
}


Graph *readGraphFromFile(const char *filename)
{
    //Dosyadan graph oku
    //Kullanilan dosya formati videoda daha net aciklanacaktir
    //Gerekli degiskenler olusturulur
	errno = 0;
	int gSize = 0, src, dest;
	Graph *G;
	char c1, c2;
	FILE *fileptr;
	fileptr = fopen(filename, "r");
	if (fileptr == NULL) {
		printf("\nError occured while trying to open file!");
		printf("\nError no %d", errno);
		exit(1);
	}

    //Ilk satirdan graph boyutu okunur
	fscanf(fileptr, "%d", &gSize);
    //Okunan deger boyutunda graph olusturulur
	G = createGraph(gSize);
    //Karakterler 2'ser 2'ser okunur, bir sayi okunur bir de 
    //ardindan gelen noktalama isareti okunur
    //Noktalama isareti virgulse ayni satirdaki siradaki deger okunur
    //Noktali virgul ise alt satira gecilir
	while (fscanf(fileptr, "%d%c", &src, &c1) == 2) {
		do {
			fscanf(fileptr, "%d%c", &dest, &c2);
			addEdge(G, src, dest);
		} while (c2 != ';' && c2 != '\n');
	}
    
    //Olusturulan graph dondurulur
	fclose(fileptr);
	return G;
}


void printGraph(Graph *G)
{
    //Graph'in adj matrisi ekrana yazdirilir
    int i, j;
    printf("\nAdjacency matrix:\n");
    for (i=0; i<G->size; i++) {
        for (j=0; j<G->size; j++) {
            printf("%d ", G->adjMatrix[i][j]);
        }
        printf("\n");
    }
}


void findMaxBetweennessWithBFS(Graph *G)
{
    //BFS icin queue ve diger degiskenler olusturulur
    //Dist ve count matrisleri betweenness hesaplanirken lazim olacak
    Queue *q = createQueue(G->size);
    int i, j, k, currentVal, neighborVal, *dist, *count, totalCountPaths = 0, additionVal;
    double **betweenness;

    dist = (int*)calloc(G->size, sizeof(int));
    if (dist == NULL) {
		printf("\nError occured while allocating memory, terminating program!");
		exit(1);
	}

    count = (int*)calloc(G->size, sizeof(int));
    if (count == NULL) {
		printf("\nError occured while allocating memory, terminating program!");
		exit(1);
	}

    //Betweenness degerlerinin tutuldugu matris olusturulur
    betweenness = (double**)malloc(G->size * sizeof(double*));
    for (i=0; i<G->size; i++) {
        betweenness[i] = (double*)malloc(G->size * sizeof(double));
    }

    for (i=0; i<G->size; i++) {
        for (j=0; j<G->size; j++) {
            betweenness[i][j] = 0.0;
        }
    }

    //Her bir dugum icin BFS calistirilir, bunun icin BFS for dongusu icinde yer almaktadir
    for (i=0; i<G->size; i++)
    {
        printf("\n\n\nBFS ITERATION FOR NODE %d\n\n", i);

        //Gerekli arrayler her yeni dugum icin BFS calistirilacaginda initialize edilir
        for (j=0; j<G->size; j++) {
            dist[j] = -1;
            count[j] = 0;
            G->visited[j] = 0;
        }

        //BFS baslangici
        enqueue(q, i);
        printQueue(q);
        G->visited[i] = 1;
        printf("\nNode %d visited ", i);
        //Baslangic dugumune olan uzaklik zaten o dugumden baslandigi icin 0 olur
        //Bunun icin dist[i] = 0
        //Baslangic dugumunun count degeri de 1 olarak baslatilir
        //Dist: Baslangic dugumunden o anki dugume olan uzaklik
        //Count: Baslangictan o anki dugume kadar olan en kisa yollarin sayisi

        dist[i] = 0;
        count[i] = 1;

        //Queue bos degilken BFS islemleri yapilir
        while (isEmpty(q) != 1) {

            //Komsulari ziyaret edilecek olan deger queue'dan cekilir
            currentVal = dequeue(q);
            printQueue(q);
            
            for (j=0; j<G->size; j++) {
                //Adj matrisi gezilirken j indisi queue'dan cekilen degerin komsularini taramayi saglar
                //Eger adjMatrix[currenVal][j] == 1 ise bu deger komsudur ve bu degerin visited degeri 1 yapilir
                //Ayni zamanda direkt olarak komsu olduklari icin distance degeri de basladigimiz yerden 1 fazla seklinde belirlenir
                neighborVal = j;
                if (G->adjMatrix[currentVal][neighborVal] == 1) {
                    if (G->visited[neighborVal] != 1) {
                        enqueue(q, neighborVal);
                        printQueue(q);
                        G->visited[neighborVal] = 1;
                        printf("\nNode %d visited ", neighborVal);
                        dist[neighborVal] = dist[currentVal] + 1; 
                    }

                    //Eger dist degeri currentNode'un distance'indan 1 fazlaysa en kisa yollardan biri bulunmustur
                    //Dolayisiyla komsunun count degeri de kendisinin count degeri kadar artirilir
                    if (dist[neighborVal] == (dist[currentVal] + 1)) {
                        count[neighborVal] = count[currentVal] + count[neighborVal];
                        totalCountPaths++;
                    }
                }
            }
        }
        //Klasik BFS ardindan betweenness hesaplamaya gecilir
        //Debug icin dist ve count arrayleri print edilmistir
        printf("\nDistance values for node %d: ", i);
        for (j=0; j<G->size; j++) {
            printf("%d ", dist[j]);
        }

        printf("\nCount values for node %d: ", i);
        for (j=0; j<G->size; j++) {
            printf("%d ", count[j]);
        }
        
        additionVal = 0;

        //Betweenness degerinin hesaplanmasi
        //Olusturulan matrisin sadece alt ucgen yarisi doldurulmustur cunku zaten ust ucgen yarisiyla ayni komsuluklari temsil etmektedir
        //Hesaplama yapilirken karisiklik olmamasi icin boyle yapilmistir
        for (j=G->size-1; j>=0; j--) {
            for (k=0; k<i; k++) {

                //Eger j ve k indisleriyle gezinilen dugumler arasi komsuluk varsa
                //j, k icin betweenness degeri
                //(k'den gecen en kisa yol sayisi + onceki dugumden gelen deger) / j'den gecen en kisa yol sayisi olarak hesaplanir
                //additionVal onceki dugumden gelen degeri ifade eder, ilk dugum icin hesaplanirken
                //en sondaki dugumden hesaplamalar yapilmaya baslandigi icin bu deger 0'dir. 
                //Fakat her iterasyonla birlikte onceki dugumden gelen deger additionVal'de saklanir
                //Boylece bu formulle her dugum icin bir deger hesaplanir
                if (G->adjMatrix[j][k] != 0) {
                    betweenness[j][k] += ((double)count[k] + additionVal) / (double)count[j]; 
                    additionVal = betweenness[j][k] / (double)count[k];
                }
            }
            //Her j dugumu icin additionVal sifirlanir
            additionVal = 0;
        }
    }

    //Betweenness degerleri ekrana yazdirilir
    printf("\n\nEdge Betweenness Values: \n");
    for (i=0; i<G->size; i++) {
        for (j=0; j<G->size; j++) {
            if (G->adjMatrix[i][j] != 0) {
                printf("Edge (%d, %d): %lf\n", i, j, betweenness[i][j]);
            }
        }
    }
    
    //En yukse betweenness'a sahip kenarin node'lari bulunur ve ekrana yazdirilir
    int *maxBetweennessEdge;
    maxBetweennessEdge = maxElement(betweenness, G->size);
    printf("\nEdge with the greatest betweenness value is the edge between %d and %d", maxBetweennessEdge[0], maxBetweennessEdge[1]);
    
    //Bulunan bu kenar kaldirilir
    //Ortaya cikan community'ler ekrana yazdirilir
	G->adjMatrix[maxBetweennessEdge[0]][maxBetweennessEdge[1]] = 0;
	printf("\nEdge between nodes %d and %d is deleted", maxBetweennessEdge[0], maxBetweennessEdge[1]);
	printf("\nCommunity 1: ");
	
	for (i=0; i<=maxBetweennessEdge[0]; i++) {
		printf("%d ", i);
	}
	printf("\nCommunity 2: ");
	for (i=maxBetweennessEdge[1]; i<G->size; i++) {
		printf("%d ", i);
	}

    //Olusturulan tum bellek alanlari serbest birakilir
	for (i=0; i<G->size; i++) {
        free(betweenness[i]);
	}
    free(q->items);
    free(q);
    free(betweenness);
    free(dist);
    free(count);
    free(maxBetweennessEdge);
}


int *maxElement(double **arr, int size) 
{
    //Bir matristeki en yuksek degeri bulan ve bu degerin indislerini donduren fonksiyon
    //En yuksek betweenness degerini bulmak icin olsuturulmustur
    int i, j;
	int *maxEdgeNodes = (int*)calloc(2, sizeof(int));
	if (maxEdgeNodes == NULL) {
		printf("\nError occured while allocating memory, terminating program!");
	}
	
	double max = 0.0;
    
    //max degiskeni basta sifirdir ve daha buyuk bir deger bulundukca indisler guncellenir
    /*for (i=size-1; i>=0; i--) {
    	for (j=0; j<i; j++) {
    		if (arr[i][j] > max) {
    			max = arr[i][j];
    			maxEdgeNodes[0] = i;
    			maxEdgeNodes[1] = j;
			}
		}
	}*/
	
	for (i=0; i<size; i++) {
    	for (j=0; j<size; j++) {
    		if (arr[i][j] > max) {
    			max = arr[i][j];
    			maxEdgeNodes[0] = i;
    			maxEdgeNodes[1] = j;
			}
		}
	}

    return maxEdgeNodes;
}
