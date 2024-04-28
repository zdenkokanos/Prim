#include <stdio.h>
#include <stdlib.h>

#define true 't'
#define false 'f'
typedef char bool;

typedef struct pq_e
{
    int index;
    int destination;
    long long weight;
} PQ_E;

typedef struct pq
{
    PQ_E *heap;
    int capacity;
    int size;
} PQ;

PQ *create_priorityQueue(int capacity)
{
    PQ *priorityQueue = (PQ *) malloc(sizeof(PQ));
    priorityQueue->heap = (PQ_E *) malloc(capacity * sizeof(PQ_E));
    priorityQueue->size = 0;
    priorityQueue->capacity = capacity;
    return priorityQueue;
}

void resize(PQ *priorityQueue)
{
    int new_capacity = priorityQueue->capacity * 2;
    PQ_E *new_heap = realloc(priorityQueue->heap, new_capacity * sizeof(PQ_E));

    priorityQueue->heap = new_heap;
    priorityQueue->capacity = new_capacity;
}

void swap(PQ_E *a, PQ_E *b)
{
    PQ_E temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_up(PQ *priorityQueue, int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;
        if (priorityQueue->heap[parent].weight > priorityQueue->heap[index].weight)
        {
            swap(&priorityQueue->heap[parent], &priorityQueue->heap[index]);
            index = parent;
        }
        else
        {
            break;
        }
    }
}

void heapify_down(PQ *priorityQueue, int index)
{
    int size = priorityQueue->size;
    while (true)
    {
        int left_child = 2 * index + 1;
        int right_child = 2 * index + 2;
        int smallest = index;

        if (left_child < size && priorityQueue->heap[left_child].weight < priorityQueue->heap[smallest].weight)
        {
            smallest = left_child;
        }
        if (right_child < size && priorityQueue->heap[right_child].weight < priorityQueue->heap[smallest].weight)
        {
            smallest = right_child;
        }
        if (smallest != index)
        {
            swap(&priorityQueue->heap[index], &priorityQueue->heap[smallest]);
            index = smallest;
        }
        else
        {
            break;
        }
    }
}

void insert(PQ *priorityQueue, int index, int destination, long long weight)
{
    if (priorityQueue->size == priorityQueue->capacity)
    {
        resize(priorityQueue);
    }
    priorityQueue->heap[priorityQueue->size].index = index;
    priorityQueue->heap[priorityQueue->size].weight = weight;
    priorityQueue->heap[priorityQueue->size].destination = destination;
    priorityQueue->size++;
    heapify_up(priorityQueue, priorityQueue->size - 1);
}

PQ_E extract_min(PQ *priorityQueue)
{
    PQ_E min = priorityQueue->heap[0];
    priorityQueue->heap[0] = priorityQueue->heap[priorityQueue->size - 1];
    priorityQueue->size--;
    heapify_down(priorityQueue, 0);
    return min;
}

int is_empty(PQ *priorityQueue)
{
    return priorityQueue->size == 0;
}

void destroy_priorityQueue(PQ *priorityQueue)
{
    free(priorityQueue->heap);
    free(priorityQueue);
}

int update(long long **graph, int vertex1, int vertex2, long long weight, int N)
{
    if (vertex1 >= N || vertex2 >= N || vertex1 == vertex2 || graph[vertex1][vertex2] == -1)
    {
        return 1;
    }
    if (graph[vertex1][vertex2] + weight > 0)
    {
        graph[vertex1][vertex2] += weight;
        graph[vertex2][vertex1] += weight;
        return 0;
    }
    else
    {
        return 1;
    }
}

int delete(long long **graph, int vertex1, int vertex2)
{
    if (vertex1 == vertex2)
    {
        return 1;
    }
    if (graph[vertex1][vertex2] != -1)
    {
        graph[vertex1][vertex2] = -1;
        graph[vertex2][vertex1] = -1;
        return 0;
    }
    else
    {
        return 1;
    }
}

int add_edge(long long **graph, int vertex1, int vertex2, long long weight, int N)
{
    if (vertex1 > N - 1 || vertex2 > N - 1)
    {
        return 1;
    }
    if (vertex1 == vertex2)
    {
        return 1;
    }
    if(graph[vertex1][vertex2]!=-1){
        return 1;
    }

    graph[vertex1][vertex2] = weight;
    graph[vertex2][vertex1] = weight;
    return 0;
}

void insert_sort(PQ_E spanning_tree[], int capacity, PQ_E min_edge)
{
    if (capacity == 0)
    {
        spanning_tree[0] = min_edge;
        return;
    }
    int j = capacity - 1;
    while (j >= 0 && (spanning_tree[j].index > min_edge.index ||
                      (spanning_tree[j].index == min_edge.index &&
                       spanning_tree[j].destination > min_edge.destination)))
    {
        spanning_tree[j + 1] = spanning_tree[j];
        j--;
    }
    spanning_tree[j + 1] = min_edge;
}

int prim_alg(long long **graph, int starting_vertex, int N, bool *printed)
{
    PQ_E spanning_tree[N];
    bool visited[N];
    int capacity = 0;

    if (starting_vertex > N)
    {
        return 1;
    }

    for (int i = 0; i < N; i++)
    {
        visited[i] = false;
    }

    PQ *priorityQueue = create_priorityQueue(N);
    visited[starting_vertex] = true;
    for (int j = 0; j < N; j++)
    {

        if (graph[starting_vertex][j] != -1)
        {
            insert(priorityQueue, starting_vertex, j, graph[starting_vertex][j]);
        }
    }

    long long total_cost = 0;

    while (!is_empty(priorityQueue))
    {
        PQ_E min_edge = extract_min(priorityQueue);
        if (visited[min_edge.destination] == false)
        {
            total_cost += min_edge.weight;
            int destination_index = min_edge.destination;
            visited[min_edge.destination] = true;
            if (min_edge.index > min_edge.destination)
            {
                int temp = min_edge.index;
                min_edge.index = min_edge.destination;
                min_edge.destination = temp;
            }
            insert_sort(spanning_tree, capacity, min_edge);
            capacity++;
            for (int i = 0; i < N; i++)
            {
                if (visited[i] == false)
                {
                    if (graph[destination_index][i] != -1)
                    {
                        insert(priorityQueue, destination_index, i, graph[destination_index][i]);
                    }
                }
            }
        }
    }

    if (capacity > 0)
    {
        if (*printed == true)
        {
            printf("\n");
        }
        printf("%lld: [", total_cost);

        for (int i = 0; i < capacity; i++)
        {
            printf("(%d, %d)", spanning_tree[i].index, spanning_tree[i].destination);
            if (i != capacity - 1)
            {
                printf(", ");
            }
        }
        *printed = true;
        printf("]");
    }
    else
    {
        return 1;
    }
//    for (int i = 0; i < N; i++) {
//        free(addedToHeap[i]);
//    }
//    free(addedToHeap);
    destroy_priorityQueue(priorityQueue);
    return 0;
}


int main()
{
    int N;
    int vertex1, vertex2;
    long long weight;
    char input;
    bool printed = false;
    scanf("%d", &N);
    long long **graph = (long long **) malloc(N * sizeof(long long *));
    for (int i = 0; i < N; i++)
    {
        graph[i] = (long long *) malloc(N * sizeof(long long));
        for (int j = 0; j < N; j++) {
            graph[i][j] = -1;
        }
    }
    while (scanf(" (%d, %d, %lld)", &vertex1, &vertex2, &weight) == 3)
    {

        if (add_edge(graph, vertex1, vertex2, weight, N) == 1)
        {
            if (printed == false)
            {
                printf("insert %d %d failed", vertex1, vertex2);
                printed = true;
            }
            else
            {
                printf("\ninsert %d %d failed", vertex1, vertex2);
            }
        }
    }


    while (scanf("%c", &input) == 1)
    {
        switch (input)
        {
            case 's':
                scanf(" %d", &vertex1);
                if (prim_alg(graph, vertex1, N, &printed) == 1)
                {
                    if (printed == false)
                    {
                        printf("search %d failed", vertex1);
                        printed = true;
                    }
                    else
                    {
                        printf("\nsearch %d failed", vertex1);
                    }
                }
                break;
            case 'd':
                scanf(" %d %d", &vertex1, &vertex2);
                if (delete(graph, vertex1, vertex2) == 1)
                {
                    if (printed == false)
                    {
                        printf("delete %d %d failed", vertex1, vertex2);
                        printed = true;
                    }
                    else
                    {
                        printf("\ndelete %d %d failed", vertex1, vertex2);
                    }
                }
                break;
            case 'i':
                scanf(" %d %d %lld", &vertex1, &vertex2, &weight);
                if (add_edge(graph, vertex1, vertex2, weight, N) == 1)
                {
                    if (printed == false)
                    {
                        printf("insert %d %d failed", vertex1, vertex2);
                        printed = true;
                    }
                    else
                    {
                        printf("\ninsert %d %d failed", vertex1, vertex2);
                    }
                }
                break;
            case 'u':
                scanf(" %d %d %lld", &vertex1, &vertex2, &weight);
                if (update(graph, vertex1, vertex2, weight, N) == 1)
                {
                    if (printed == false)
                    {
                        printf("update %d %d failed", vertex1, vertex2);
                        printed = true;
                    }
                    else
                    {
                        printf("\nupdate %d %d failed", vertex1, vertex2);
                    }
                }
                break;
            default:
                break;
        }
    }

    for (int i = 0; i < N; i++) {
        free(graph[i]);
    }
    free(graph);

    return 0;
}