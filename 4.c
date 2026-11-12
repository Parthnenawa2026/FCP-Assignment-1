#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STUDENT_NAME_SIZE 50
#define MAX_NAME_LENGTH 49
#define MIN_AGE 16
#define MAX_AGE 100
#define MIN_GPA 0.00f
#define MAX_GPA 4.00f

typedef struct
{
    char name[STUDENT_NAME_SIZE];
    int age;
    float gpa;
    int rollno;
} Student;

/* Allocate memory for the student database. */
Student *create_student_list(int n)
{
    Student *studs = (Student *)malloc(n * sizeof(Student));
    return studs;
}

/* Global student database and its current state. */
Student *students;
int count = 0;
int capacity = 0;

/* Validate data for a specific student entry before storing or updating it. */
int check_input(int i)
{
    if (strlen(students[i].name) > MAX_NAME_LENGTH)
        return -1;

    if (students[i].age > MAX_AGE || students[i].age < MIN_AGE)
        return -1;

    if (students[i].gpa < MIN_GPA || students[i].gpa > MAX_GPA)
        return -1;

    if (students[i].rollno <= 0)
        return -1;

    return 0;
}

/* Store a student record at a given index. */
void add_student(int i, char *name, int age, float gpa, int rollno)
{
    strcpy(students[i].name, name);
    students[i].age = age;
    students[i].gpa = gpa;
    students[i].rollno = rollno;
}

/* Print all saved student records. */
void display_students()
{
    for (int i = 0; i < count; i++)
    {
        printf("Name: %s, Roll Number: %d, Age: %d, GPA: %.2f\n", students[i].name, students[i].rollno, students[i].age, students[i].gpa);
    }
}

/* Find the index of a student by roll number. Returns -1 if not found. */
int search_students(int rollno)
{
    for (int i = 0; i < count; i++)
    {
        if (students[i].rollno == rollno)
            return i;
    }
    return -1;
}

/* Update the details of a student whose roll number matches. */
int update_student(int rollno, char *new_name, int new_age, float new_gpa)
{
    int idx = search_students(rollno);
    if (idx == -1)
        return -1;

    if (strlen(new_name) > 49)
        return -1;

    Student temp;
    strcpy(temp.name, new_name);
    temp.age = new_age;
    temp.gpa = new_gpa;
    temp.rollno = rollno;

    if (check_input(idx) == -1)
        return -1;

    students[idx] = temp;
    return 0;
}

/* Remove a student record by shifting later records left to fill the gap. */
int delete_students(int rollno)
{
    int idx = search_students(rollno);
    if (idx == -1)
        return -1;

    for (int i = idx; i < count - 1; i++)
        students[i] = students[i + 1];

    (count)--;
    return 0;
}

/* Increase the database size to allow more student entries. */
int expand()
{
    int new_cap = capacity * 2;
    Student *temp = realloc(students, new_cap * sizeof(Student));
    if (temp == NULL)
        return -1;

    students = temp;
    capacity = new_cap;
    return 0;
}

/* Show the highest, lowest, and average GPA across all students. */
void display_summary()
{
    if (count == 0)
    {
        printf("No records.\n");
        return;
    }

    int best = 0, worst = 0;
    float total = 0;

    for (int i = 0; i < count; i++)
    {
        if (students[i].gpa > students[best].gpa)
            best = i;
        if (students[i].gpa < students[worst].gpa)
            worst = i;
        total += students[i].gpa;
    }

    printf("Highest: %s %.2f\n", students[best].name, students[best].gpa);
    printf("Lowest : %s %.2f\n", students[worst].name, students[worst].gpa);
    printf("Average: %.2f\n", total / count);
}

/* Display students sorted by GPA in descending order. */
void display_sorted()
{
    Student *copy = malloc(count * sizeof(Student));

    for (int i = 0; i < count; i++)
        copy[i] = students[i];

    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < count - 1; j++)
        {
            if (copy[j].gpa < copy[j + 1].gpa)
            {
                Student temp = copy[j];
                copy[j] = copy[j + 1];
                copy[j + 1] = temp;
            }
            else if (copy[j].gpa == copy[j + 1].gpa && copy[j].rollno > copy[j + 1].rollno)
            {
                Student temp = copy[j];
                copy[j] = copy[j + 1];
                copy[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < count; i++)
        printf("%d %s %d %.2f\n", copy[i].rollno, copy[i].name, copy[i].age, copy[i].gpa);
}

/* Release the allocated memory before exiting the program. */
void free_student_list()
{
    free(students);
}

/* Main menu for managing the student record system. */
int main()
{
    int ch, age, rollno, idx;
    char name[100];
    float gpa;

    /* Ask the user for the initial number of student slots. */
    printf("Enter capacity of the Structure.");
    scanf("%d", &capacity);
    if (capacity <= 0 || capacity > 1000000)
    {
        printf("Invalid capacity.\n");
        return 0;
    }
    students = create_student_list(capacity);
    if (students == NULL)
    {
        printf("Memory allocation failed.\n");
        return 0;
    }

    do
    {
        /* Show all available operations to the user. */
        printf("\n1.Add  2.Display  3.Search  4.Update  5.Delete 6.Sorted by GPA  7.GPA summary  0.Exit\n");
        printf("Enter your choice");
        if (scanf("%d", &ch) != 1)
            break;

        switch (ch)
        {
        case 1:
            /* Add a new student record if the roll number is unique and the data is valid. */
            printf("Enter roll number: ");
            scanf("%d", &rollno);
            printf("Enter name: ");
            scanf(" %99[^\n]", name);
            printf("Enter age: ");
            scanf("%d", &age);
            printf("Enter gpa: ");
            scanf("%f", &gpa);
            if (strlen(name) > 49)
            {
                printf("Name is too long.\n");
            }
            else if (search_students(rollno) != -1)
            {
                printf("Roll number already exists.\n");
            }
            else
            {
                if (count == capacity)
                {
                    if (expand() == -1)
                    {
                        printf("Could not expand memory.\n");
                        break;
                    }
                }
                add_student(count, name, age, gpa, rollno);
                if (check_input(count) == -1)
                {
                    printf("Invalid input.\n");
                }
                else
                {
                    count++;
                    printf("Student added.\n");
                }
            }
            break;
        case 2:
            /* Display every stored student if at least one exists. */
            if (count == 0)
                printf("No records.\n");
            else
                display_students();
            break;
        case 3:
            /* Search and print the record for a specific roll number. */
            printf("Enter roll number: ");
            scanf("%d", &rollno);
            idx = search_students(rollno);
            if (idx == -1)
                printf("Roll number not found.\n");
            else
                printf("Name: %s, Roll Number: %d, Age: %d, GPA: %.2f\n", students[idx].name, students[idx].rollno, students[idx].age, students[idx].gpa);
            break;
        case 4:
            /* Update the selected student's name, age, and GPA. */
            printf("Enter roll number to update: ");
            scanf("%d", &rollno);
            printf("Enter new name: ");
            scanf(" %99[^\n]", name);
            printf("Enter new age: ");
            scanf("%d", &age);
            printf("Enter new gpa: ");
            scanf("%f", &gpa);
            if (strlen(name) > 49 || age < 16 || age > 100 || gpa < 0 || gpa > 4)
                printf("Invalid input.\n");
            else if (update_student(rollno, name, age, gpa) == -1)
                printf("Roll number not found.\n");
            else
                printf("Student updated.\n");
            break;
        case 5:
            /* Delete a student record using the roll number as the key. */
            printf("Enter roll number to delete: ");
            scanf("%d", &rollno);
            if (delete_students(rollno) == -1)
                printf("Roll number not found.\n");
            else
                printf("Student deleted.\n");
            break;
        case 6:
            /* Show all students in descending GPA order. */
            if (count == 0)
                printf("No records.\n");
            else
                display_sorted();
            break;
        case 7:
            /* Show summary statistics for the current student list. */
            display_summary();
            break;
        case 0:
            /* Exit the program. */
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (ch != 0);

    free_student_list();
    printf("Goodbye!\n");
    return 0;
}
