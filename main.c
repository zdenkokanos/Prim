#include <stdio.h>
#include <stdlib.h>

#define true 't'
#define false 'f'
#define MAX_WEIGHT 999999
typedef char bool;

typedef struct neighbour
{
    int index;
    long long weight;
    struct neighbour *next;
} NEIGHBOURS;

typedef struct vertex
{
    NEIGHBOURS *neighbours;
} VERTEX;

typedef struct pq_e
{
    int index;
    int destination;
    long long weight;
} PQ_E;

typedef struct pq
{
    PQ_E *heap;
    int size;
} PQ;

PQ *create_priorityQueue(int capacity)
{
    PQ *priorityQueue = (PQ *) malloc(sizeof(PQ));
    priorityQueue->heap = (PQ_E *) malloc(2000 * sizeof(PQ_E));
    priorityQueue->size = 0;
    return priorityQueue;
}

void swap(PQ_E *a, PQ_E *b)
{
    PQ_E temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_up(PQ *priorityQueue, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (priorityQueue->heap[parent].weight > priorityQueue->heap[index].weight) {
            swap(&priorityQueue->heap[parent], &priorityQueue->heap[index]);
            index = parent;
        } else {
            break;
        }
    }
}

void heapify_down(PQ *priorityQueue, int index) {
    int size = priorityQueue->size;
    while (true) {
        int left_child = 2 * index + 1;
        int right_child = 2 * index + 2;
        int smallest = index;

        if (left_child < size && priorityQueue->heap[left_child].weight < priorityQueue->heap[smallest].weight) {
            smallest = left_child;
        }
        if (right_child < size && priorityQueue->heap[right_child].weight < priorityQueue->heap[smallest].weight) {
            smallest = right_child;
        }
        if (smallest != index) {
            swap(&priorityQueue->heap[index], &priorityQueue->heap[smallest]);
            index = smallest;
        } else {
            break;
        }
    }
}

void insert(PQ *priorityQueue, int index, int destination, long long weight)
{
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

int update(VERTEX **graph, int vertex1, int vertex2, long long weight, bool first_one, int N)
{
    if (vertex1 > N - 1 || vertex2 > N - 1)
    {
        return 1;
    }
    if (vertex1 == vertex2)
    {
        return 1;
    }
    NEIGHBOURS *current = graph[vertex1]->neighbours;
    while (current != NULL)
    {
        if (current->index == vertex2)
        {
            break;
        }
        current = current->next;
    }
    if (current == NULL)
    {
        return 1;
    }
    int newWeight = current->weight + weight;
    if (newWeight <= 0)
    {
        return 1;
    }
    else
    {
        current->weight = newWeight;
    }

    if (first_one == true)
    {
        update(graph, vertex2, vertex1, weight, false, N);
    }
    return 0;
}

int delete(VERTEX **graph, int vertex1, int vertex2, bool first_one)
{
    NEIGHBOURS *current = graph[vertex1]->neighbours;
    NEIGHBOURS *previous = NULL;
    if (vertex1 == vertex2)
    {
        return 1;
    }
    while (current != NULL)
    {
        if (current->index == vertex2)
        {
            break;
        }
        previous = current;
        current = current->next;
    }
    if (current == NULL)
    {
        return 1;
    }
    if (current == graph[vertex1]->neighbours)
    {
        graph[vertex1]->neighbours = current->next;
        free(current);
    }
    else
    {
        previous->next = current->next;
        free(current);
    }
    if (first_one == true)
    {
        delete(graph, vertex2, vertex1, false);
    }
    return 0;
}

int add_edge(VERTEX **graph, int vertex1, int vertex2, long long weight, bool first_one, int N)
{
    if (vertex1 > N - 1 || vertex2 > N - 1)
    {
        return 1;
    }
    if (vertex1 == vertex2)
    {
        return 1;
    }
    NEIGHBOURS *current = graph[vertex1]->neighbours;
    while (current != NULL)
    {
        if (current->index == vertex2)
        {
            return 1;
        }
        current = current->next;
    }
    NEIGHBOURS *neighbour = (NEIGHBOURS *) malloc(sizeof(NEIGHBOURS));
    neighbour->index = vertex2;
    neighbour->weight = weight;
    neighbour->next = NULL;
    if (graph[vertex1]->neighbours == NULL)
    {
        graph[vertex1]->neighbours = neighbour;
    }
    else
    {
        neighbour->next = graph[vertex1]->neighbours;
        graph[vertex1]->neighbours = neighbour;
    }
    if (first_one == true)
    {
        add_edge(graph, vertex2, vertex1, weight, false, N);
    }
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

int prim_alg(VERTEX **graph, int starting_vertex, int N, bool *printed)
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
    NEIGHBOURS *current = graph[starting_vertex]->neighbours;
    while (current != NULL)
    {
        insert(priorityQueue, starting_vertex, current->index, current->weight);
        current = current->next;
    }

    long long total_cost = 0;

    while (!is_empty(priorityQueue))
    {
        PQ_E min_edge = extract_min(priorityQueue);
        if (visited[min_edge.destination] == false)
        {
            total_cost += min_edge.weight;
            int destination_index = min_edge.destination;
            current = graph[min_edge.destination]->neighbours;
            visited[min_edge.destination] = true;
            if (min_edge.index > min_edge.destination)
            {
                int temp = min_edge.index;
                min_edge.index = min_edge.destination;
                min_edge.destination = temp;
            }
            insert_sort(spanning_tree, capacity, min_edge);
            capacity++;
            while (current != NULL)
            {
                if (visited[current->index] == false)
                {
                    insert(priorityQueue, destination_index, current->index, current->weight);
                }
                current = current->next;
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
    VERTEX **graph = (VERTEX **) malloc(N * sizeof(VERTEX *));
    for (int i = 0; i < N; i++)
    {
        VERTEX *newVertex = (VERTEX *) malloc(sizeof(VERTEX));
        newVertex->neighbours = NULL;
        graph[i] = newVertex;
    }
    while (scanf(" (%d, %d, %lld)", &vertex1, &vertex2, &weight) == 3)
    {

        if (add_edge(graph, vertex1, vertex2, weight, true, N) == 1)
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
                if (delete(graph, vertex1, vertex2, true) == 1)
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
                if (add_edge(graph, vertex1, vertex2, weight, true, N) == 1)
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
                if (update(graph, vertex1, vertex2, weight, true, N) == 1)
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

    for (int i = 0; i < N; i++)
    {
        NEIGHBOURS *current = graph[i]->neighbours;
        while (current != NULL)
        {
            NEIGHBOURS *temp = current;
            current = current->next;
            free(temp);
        }
        free(graph[i]);
    }
    free(graph);

    return 0;
}