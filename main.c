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

typedef struct fib_node {
    int index;
    int destination;
    long long weight;
    int degree;
    struct fib_node *parent;
    struct fib_node *child;
    struct fib_node *left;
    struct fib_node *right;
    char mark;
} FIB_NODE;

typedef struct fib_heap {
    FIB_NODE *min;
    int n;  // Number of nodes in the heap
} FIB_HEAP;

FIB_HEAP* create_fibonacci_heap() {
    FIB_HEAP* heap = (FIB_HEAP*) malloc(sizeof(FIB_HEAP));
    heap->min = NULL;
    heap->n = 0;
    return heap;
}

FIB_NODE* create_fib_node(int index, int destination, long long weight) {
    FIB_NODE* node = (FIB_NODE*) malloc(sizeof(FIB_NODE));
    node->index = index;
    node->destination = destination;
    node->weight = weight;
    node->degree = 0;
    node->parent = NULL;
    node->child = NULL;
    node->left = node;
    node->right = node;
    node->mark = 'f';
    return node;
}

void fib_heap_insert(FIB_HEAP* heap, FIB_NODE* node) {
    if (heap->min == NULL) {
        heap->min = node;
    } else {
        node->left = heap->min;
        node->right = heap->min->right;
        heap->min->right = node;
        node->right->left = node;
        if (node->weight < heap->min->weight) {
            heap->min = node;
        }
    }
    heap->n++;
}

void fib_heap_link(FIB_HEAP* heap, FIB_NODE* y, FIB_NODE* x) {
    // Remove y from root list
    y->left->right = y->right;
    y->right->left = y->left;
    // Make y a child of x
    if (x->child == NULL) {
        x->child = y;
        y->right = y;
        y->left = y;
    } else {
        y->left = x->child;
        y->right = x->child->right;
        x->child->right = y;
        y->right->left = y;
    }
    y->parent = x;
    x->degree++;
    y->mark = 'f';
}

void consolidate(FIB_HEAP* heap) {
    int D = 40;  // An upper bound on degree, depending on heap->n
    FIB_NODE* A[D];
    for (int i = 0; i < D; i++) A[i] = NULL;

    FIB_NODE* start = heap->min;
    FIB_NODE* w = start;
    do {
        FIB_NODE* x = w;
        w = w->right;
        int d = x->degree;
        while (A[d] != NULL) {
            FIB_NODE* y = A[d];
            if (x->weight > y->weight) {
                FIB_NODE* temp = x;
                x = y;
                y = temp;
            }
            if (y == start) start = start->right;
            if (y == w) w = w->right;
            fib_heap_link(heap, y, x);
            A[d] = NULL;
            d++;
        }
        A[d] = x;
    } while (w != start);

    heap->min = NULL;
    for (int i = 0; i < D; i++) {
        if (A[i] != NULL) {
            if (heap->min == NULL) {
                heap->min = A[i]->left = A[i]->right = A[i];
            } else {
                A[i]->left = heap->min;
                A[i]->right = heap->min->right;
                heap->min->right = A[i];
                A[i]->right->left = A[i];
                if (A[i]->weight < heap->min->weight) {
                    heap->min = A[i];
                }
            }
        }
    }
}



FIB_NODE* fib_heap_extract_min(FIB_HEAP* heap) {
    FIB_NODE* z = heap->min;
    if (z != NULL) {
        FIB_NODE* child = z->child;
        if (child != NULL) {
            do {
                FIB_NODE* next = child->right;
                // add child to root list
                child->left = heap->min;
                child->right = heap->min->right;
                heap->min->right = child;
                child->right->left = child;
                child->parent = NULL;
                child = next;
            } while (child != z->child);
        }

        // Remove z from root list
        z->left->right = z->right;
        z->right->left = z->left;

        if (z == z->right) {
            heap->min = NULL;
        } else {
            heap->min = z->right;
            consolidate(heap);
        }
        heap->n--;
    }
    return z;
}

void destroy_fibonacci_heap(FIB_HEAP *heap) {
    free(heap);
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

void insert_sort(FIB_NODE *spanning_tree[], int capacity, FIB_NODE *min_edge)
{
    if (capacity == 0)
    {
        spanning_tree[0] = min_edge;
        return;
    }
    int j = capacity - 1;
    while (j >= 0 && (spanning_tree[j]->index > min_edge->index ||
                      (spanning_tree[j]->index == min_edge->index &&
                       spanning_tree[j]->destination > min_edge->destination)))
    {
        spanning_tree[j + 1] = spanning_tree[j];
        j--;
    }
    spanning_tree[j + 1] = min_edge;
}

int prim_alg(VERTEX **graph, int starting_vertex, int N, bool *printed)
{
    FIB_NODE *spanning_tree[N];
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

    FIB_HEAP *priorityQueue = create_fibonacci_heap();
    visited[starting_vertex] = true;
    NEIGHBOURS *current = graph[starting_vertex]->neighbours;
    while (current != NULL) {
        FIB_NODE *node = create_fib_node(starting_vertex, current->index, current->weight);
        fib_heap_insert(priorityQueue, node);
        current = current->next;
    }

    long long total_cost = 0;

    while (priorityQueue->n != 0)
    {
        FIB_NODE *min_edge = fib_heap_extract_min(priorityQueue);
        if (visited[min_edge->destination] == false)
        {
            total_cost += min_edge->weight;
            int destination_index = min_edge->destination;
            current = graph[min_edge->destination]->neighbours;
            visited[min_edge->destination] = true;
            if (min_edge->index > min_edge->destination)
            {
                int temp = min_edge->index;
                min_edge->index = min_edge->destination;
                min_edge->destination = temp;
            }
            insert_sort(spanning_tree, capacity, min_edge);
            capacity++;
            while (current != NULL)
            {
                if (visited[current->index] == false)
                {
                    FIB_NODE *node = create_fib_node(destination_index, current->index, current->weight);
                    fib_heap_insert(priorityQueue, node);
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
            printf("(%d, %d)", spanning_tree[i]->index, spanning_tree[i]->destination);
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
    destroy_fibonacci_heap(priorityQueue);
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