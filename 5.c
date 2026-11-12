#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_RECTANGLES 1000
#define EPSILON 0.0001

typedef struct
{
    int id;        /* Unique identifier for each rectangle */
    float width;   /* Width of the rectangle */
    float height;  /* Height of the rectangle */
    float area;    /* Calculated area = width * height */
    float perimeter; /* Calculated perimeter = 2 * (width + height) */
} Rectangle;

/* Function prototypes for all major operations */
void calculate_geometry(Rectangle *r);
int search_rectangle(Rectangle *rectangles, int size, int id);
void display_rectangles(Rectangle *rectangles, int size);
void print_menu(void);
void add_rectangle(Rectangle **rectangles, int *size, int *capacity);
void update_rectangle(Rectangle *rectangles, int size);
void delete_rectangle(Rectangle *rectangles, int *size);
void statistics(Rectangle *rectangles, int size);
void classify_rectangles(Rectangle *rectangles, int size);
void sort_rectangles(Rectangle *rectangles, int size);
void filter_rectangles(Rectangle *rectangles, int size);

/* Compute the area and perimeter of a rectangle from its dimensions */
void calculate_geometry(Rectangle *r)
{
    r->area = r->width * r->height;
    r->perimeter = 2 * (r->width + r->height);
}

/* Search for a rectangle by its ID and return its index in the array */
int search_rectangle(Rectangle *rectangles, int size, int id)
{
    for (int i = 0; i < size; i++)
    {
        if (rectangles[i].id == id)
        {
            return i; /* Return position when found */
        }
    }

    return -1; /* Return -1 if not found */
}

/* Print all stored rectangles in a readable table */
void display_rectangles(Rectangle *rectangles, int size)
{
    if (size == 0)
    {
        printf("\nNo rectangles available.\n");
        return;
    }

    printf("\n--------------------------------------------------------------\n");
    printf("ID\tWidth\tHeight\tArea\tPerimeter\n");
    printf("--------------------------------------------------------------\n");

    for (int i = 0; i < size; i++)
    {
        printf("%d\t%.2f\t%.2f\t%.2f\t%.2f\n",
               rectangles[i].id,
               rectangles[i].width,
               rectangles[i].height,
               rectangles[i].area,
               rectangles[i].perimeter);
    }
}

/* Display the main menu options to the user */
void print_menu(void)
{
    printf("\n");
    printf("========== RECTANGLE INVENTORY ==========");
    printf("\n");
    printf("1. Add Rectangle\n");
    printf("2. Display Rectangles\n");
    printf("3. Search By ID\n");
    printf("4. Update Rectangle\n");
    printf("5. Delete Rectangle\n");
    printf("6. Statistics\n");
    printf("7. Classify Squares\n");
    printf("8. Sort By Area\n");
    printf("9. Filter By Area\n");
    printf("0. Exit\n");
    printf("Enter Choice: ");
}

/* Add a new rectangle to the array and resize if needed */
void add_rectangle(Rectangle **rectangles, int *size, int *capacity)
{
    int id;
    float width, height;

    printf("Enter Rectangle ID: ");
    scanf("%d", &id);

    if (id <= 0)
    {
        printf("ID must be positive.\n");
        return;
    }

    if (search_rectangle(*rectangles, *size, id) != -1)
    {
        printf("Duplicate ID not allowed.\n");
        return;
    }

    printf("Enter Width: ");
    scanf("%f", &width);

    printf("Enter Height: ");
    scanf("%f", &height);

    if (!isfinite(width) || !isfinite(height)
        || width <= 0 || height <= 0)
    {
        printf("Invalid dimensions.\n");
        return;
    }

    if (*size >= *capacity)
    {
        int newCapacity = (*capacity) * 2;

        if (newCapacity > MAX_RECTANGLES)
            newCapacity = MAX_RECTANGLES;

        Rectangle *temp =
            realloc(*rectangles,
                    newCapacity * sizeof(Rectangle));

        if (temp == NULL)
        {
            printf("Memory resizing failed.\n");
            return;
        }

        *rectangles = temp;
        *capacity = newCapacity;
    }

    (*rectangles)[*size].id = id;
    (*rectangles)[*size].width = width;
    (*rectangles)[*size].height = height;

    calculate_geometry(&((*rectangles)[*size]));

    (*size)++;

    printf("Rectangle added successfully.\n");
}

/* Update the width and height of an existing rectangle */
void update_rectangle(Rectangle *rectangles, int size)
{
    int id;

    printf("Enter Rectangle ID to update: ");
    scanf("%d", &id);

    int index = search_rectangle(rectangles, size, id);

    if (index == -1)
    {
        printf("Rectangle not found.\n");
        return;
    }

    float width, height;

    printf("Enter New Width: ");
    scanf("%f", &width);

    printf("Enter New Height: ");
    scanf("%f", &height);

    if (!isfinite(width) || !isfinite(height)
        || width <= 0 || height <= 0)
    {
        printf("Invalid dimensions.\n");
        return;
    }

    rectangles[index].width = width;
    rectangles[index].height = height;

    calculate_geometry(&rectangles[index]);

    printf("Rectangle updated successfully.\n");
}

/* Delete a rectangle by shifting the remaining elements left */
void delete_rectangle(Rectangle *rectangles, int *size)
{
    if (*size == 0)
    {
        printf("No rectangles to delete.\n");
        return;
    }

    int id;

    printf("Enter Rectangle ID to delete: ");
    scanf("%d", &id);

    int index = search_rectangle(rectangles, *size, id);

    if (index == -1)
    {
        printf("Rectangle not found.\n");
        return;
    }

    for (int i = index; i < (*size - 1); i++)
    {
        rectangles[i] = rectangles[i + 1];
    }

    (*size)--;

    printf("Rectangle deleted successfully.\n");
}

/* Compute summary statistics for all rectangles */
void statistics(Rectangle *rectangles, int size)
{
    if (size == 0)
    {
        printf("Collection is empty.\n");
        return;
    }

    float totalArea = 0;

    int maxIndex = 0;
    int minIndex = 0;

    for (int i = 0; i < size; i++)
    {
        totalArea += rectangles[i].area;

        if (rectangles[i].area > rectangles[maxIndex].area)
            maxIndex = i;

        if (rectangles[i].area < rectangles[minIndex].area)
            minIndex = i;
    }

    printf("\nLargest Area Rectangle ID : %d\n",
           rectangles[maxIndex].id);

    printf("Smallest Area Rectangle ID : %d\n",
           rectangles[minIndex].id);

    printf("Total Area : %.2f\n", totalArea);

    printf("Average Area : %.2f\n",
           totalArea / size);
}

/* Check whether each rectangle is a square or a non-square */
void classify_rectangles(Rectangle *rectangles, int size)
{
    if (size == 0)
    {
        printf("No rectangles available.\n");
        return;
    }

    printf("\nClassification:\n");

    for (int i = 0; i < size; i++)
    {
        if (fabs(rectangles[i].width -
                 rectangles[i].height) < EPSILON)
        {
            printf("ID %d -> Square\n",
                   rectangles[i].id);
        }
        else
        {
            printf("ID %d -> Non-Square\n",
                   rectangles[i].id);
        }
    }
}

/* Sort rectangles by area, and if areas are equal, sort by ID */
void sort_rectangles(Rectangle *rectangles, int size)
{
    Rectangle temp;

    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            int swapNeeded = 0;

            if (rectangles[j].area >
                rectangles[j + 1].area)
            {
                swapNeeded = 1;
            }
            else if (fabs(rectangles[j].area -
                          rectangles[j + 1].area) < EPSILON)
            {
                if (rectangles[j].id >
                    rectangles[j + 1].id)
                {
                    swapNeeded = 1;
                }
            }

            if (swapNeeded)
            {
                temp = rectangles[j];
                rectangles[j] = rectangles[j + 1];
                rectangles[j + 1] = temp;
            }
        }
    }

    printf("Sorted successfully.\n");
}

/* Print only those rectangles whose area is greater than a given threshold */
void filter_rectangles(Rectangle *rectangles, int size)
{
    float threshold;

    printf("Enter Area Threshold: ");
    scanf("%f", &threshold);

    printf("\nRectangles with area > %.2f\n",
           threshold);

    for (int i = 0; i < size; i++)
    {
        if (rectangles[i].area > threshold)
        {
            printf("ID=%d Width=%.2f Height=%.2f Area=%.2f\n",
                   rectangles[i].id,
                   rectangles[i].width,
                   rectangles[i].height,
                   rectangles[i].area);
        }
    }
}

/* Main program entry point */
int main()
{
    int capacity; /* Current allocated capacity of the dynamic array */
    int size = 0; /* Number of rectangles currently stored */

    printf("Enter Initial Number of Rectangles (1-1000): ");
    scanf("%d", &capacity);

    if (capacity < 1 || capacity > MAX_RECTANGLES)
    {
        printf("Invalid number of rectangles.\n");
        return 1;
    }

    Rectangle *rectangles =
        (Rectangle *)malloc(capacity * sizeof(Rectangle));

    if (rectangles == NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    int choice; /* User's menu selection */

    do
    {
        print_menu();
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            add_rectangle(&rectangles,
                          &size,
                          &capacity);
            break;

        case 2:
            display_rectangles(rectangles, size);
            break;

        case 3:
        {
            int id; /* Rectangle ID to search */

            printf("Enter ID to search: ");
            scanf("%d", &id);

            int index =
                search_rectangle(rectangles,
                                 size,
                                 id);

            if (index == -1)
            {
                printf("Rectangle not found.\n");
            }
            else
            {
                printf("Found:\n");
                printf("ID=%d Width=%.2f Height=%.2f Area=%.2f Perimeter=%.2f\n",
                       rectangles[index].id,
                       rectangles[index].width,
                       rectangles[index].height,
                       rectangles[index].area,
                       rectangles[index].perimeter);
            }
            break;
        }

        case 4:
            update_rectangle(rectangles, size);
            break;

        case 5:
            delete_rectangle(rectangles, &size);
            break;

        case 6:
            statistics(rectangles, size);
            break;

        case 7:
            classify_rectangles(rectangles, size);
            break;

        case 8:
            sort_rectangles(rectangles, size);
            break;

        case 9:
            filter_rectangles(rectangles, size);
            break;

        case 0:
            printf("Exiting program...\n");
            break;

        default:
            printf("Invalid choice.\n");
        }

    } while (choice != 0);

    free(rectangles); /* Release dynamic memory before exit */

    return 0;
}
