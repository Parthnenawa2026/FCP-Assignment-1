#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
   DataType tells us which field inside the union is currently valid.
   TYPE_EMPTY means the container currently holds no value.
*/
typedef enum { TYPE_EMPTY, TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING } DataType;

/*
   Union stores any one value at a time.
   Only the field matching the current DataType is valid.
*/
typedef union {
    int i;        /* integer value */
    float f;      /* floating-point value */
    char ch;      /* character value */
    char *str;    /* pointer to a dynamically allocated string */
} Data;

/*
   Container combines a value and its type tag.
   The type tag tells us how to interpret the union field.
*/
typedef struct {
    Data data;
    DataType type;
} Container;

/*
   This flag is used only in tests to simulate malloc failure.
   When set to 1, the next string allocation returns NULL.
*/
static int fail_allocation = 0;

/*
   copy_text makes a new copy of a C string.
   It is used for container_set_string so that the container owns its own memory.
*/
static char *copy_text(const char *text)
{
    size_t len = strlen(text);
    char *copy;

    if (fail_allocation) {
        fail_allocation = 0;
        return NULL;
    }

    copy = malloc(len + 1);
    if (copy != NULL) {
        memcpy(copy, text, len + 1);
    }

    return copy;
}

/*
   Initialize the container as empty.
   An empty container should not contain any valid data.
*/
void container_init(Container *c)
{
    if (c != NULL) {
        c->type = TYPE_EMPTY;
        c->data.str = NULL;
    }
}

/*
   Clear frees any dynamically allocated string memory and resets the type.
   This prevents memory leaks when a container is reused.
*/
void container_clear(Container *c)
{
    if (c == NULL) {
        return;
    }

    if (c->type == TYPE_STRING) {
        free(c->data.str);
    }

    c->type = TYPE_EMPTY;
    c->data.str = NULL;
}

/*
   Setters replace the old value with a new one.
   They call container_clear first so no stale memory remains.
*/
int container_set_int(Container *c, int value)
{
    if (c == NULL) return 0;
    container_clear(c);
    c->data.i = value;
    c->type = TYPE_INT;
    return 1;
}

int container_set_float(Container *c, float value)
{
    if (c == NULL) return 0;
    container_clear(c);
    c->data.f = value;
    c->type = TYPE_FLOAT;
    return 1;
}

int container_set_char(Container *c, char value)
{
    if (c == NULL) return 0;
    container_clear(c);
    c->data.ch = value;
    c->type = TYPE_CHAR;
    return 1;
}

/*
   String assignment is slightly different:
   1. We allocate a new copy first.
   2. If allocation fails, the previous value is preserved.
   3. Only after allocation succeeds do we clear and replace the old string.
*/
int container_set_string(Container *c, const char *value)
{
    char *copy;

    if (c == NULL || value == NULL) return 0;
    copy = copy_text(value);
    if (copy == NULL) return 0;

    container_clear(c);
    c->data.str = copy;
    c->type = TYPE_STRING;
    return 1;
}

/*
   Getters validate both the pointer and the expected type.
   They return 0 when the container does not hold that type.
*/
int container_get_int(const Container *c, int *out)
{
    if (c == NULL || out == NULL || c->type != TYPE_INT) return 0;
    *out = c->data.i;
    return 1;
}

int container_get_float(const Container *c, float *out)
{
    if (c == NULL || out == NULL || c->type != TYPE_FLOAT) return 0;
    *out = c->data.f;
    return 1;
}

int container_get_char(const Container *c, char *out)
{
    if (c == NULL || out == NULL || c->type != TYPE_CHAR) return 0;
    *out = c->data.ch;
    return 1;
}

int container_get_string(const Container *c, const char **out)
{
    if (c == NULL || out == NULL || c->type != TYPE_STRING) return 0;
    *out = c->data.str;
    return 1;
}

/*
   This helper returns a human-readable label for debugging and output.
*/
const char *container_type_name(const Container *c)
{
    if (c == NULL) return "INVALID";

    switch (c->type) {
        case TYPE_EMPTY: return "EMPTY";
        case TYPE_INT: return "INT";
        case TYPE_FLOAT: return "FLOAT";
        case TYPE_CHAR: return "CHAR";
        case TYPE_STRING: return "STRING";
        default: return "INVALID";
    }
}

/*
   This prints the current value according to its stored type.
   It is useful for testing and demonstration output.
*/
void container_print(const Container *c)
{
    if (c == NULL) {
        printf("<invalid>");
        return;
    }

    switch (c->type) {
        case TYPE_EMPTY: printf("<empty>"); break;
        case TYPE_INT: printf("%d", c->data.i); break;
        case TYPE_FLOAT: printf("%.2f", c->data.f); break;
        case TYPE_CHAR: printf("%c", c->data.ch); break;
        case TYPE_STRING: printf("%s", c->data.str); break;
        default: printf("<invalid>"); break;
    }
}

/*
   container_copy creates a separate copy of the source container.
   For strings, it allocates a new string so the two containers do not share memory.
*/
int container_copy(Container *dest, const Container *src)
{
    Container temp;

    if (dest == NULL || src == NULL) return 0;
    if (dest == src) return 1;

    container_init(&temp);

    switch (src->type) {
        case TYPE_EMPTY: break;
        case TYPE_INT:
            temp.type = TYPE_INT;
            temp.data.i = src->data.i;
            break;
        case TYPE_FLOAT:
            temp.type = TYPE_FLOAT;
            temp.data.f = src->data.f;
            break;
        case TYPE_CHAR:
            temp.type = TYPE_CHAR;
            temp.data.ch = src->data.ch;
            break;
        case TYPE_STRING:
            if (src->data.str == NULL ||
                !container_set_string(&temp, src->data.str)) return 0;
            break;
        default: return 0;
    }

    container_clear(dest);
    *dest = temp;
    return 1;
}

int main(void)
{
    Container records[7];
    int number;
    float decimal;
    const char *text;
    size_t i;

    /*
       Start every record in an empty state before using it.
       This avoids undefined behavior caused by uninitialized memory.
    */
    for (i = 0; i < 7; i++) container_init(&records[i]);

    /*
       Demonstrate the same container changing type over time.
       The union reuses memory, while type tells us which value is valid.
    */
    container_set_int(&records[0], 10);
    printf("Int: "); container_print(&records[0]); printf("\n");

    container_set_float(&records[0], 3.14f);
    printf("Float: "); container_print(&records[0]); printf("\n");

    container_set_char(&records[0], 'A');
    printf("Char: "); container_print(&records[0]); printf("\n");

    container_set_string(&records[0], "Hello, World!");
    printf("String: "); container_print(&records[0]); printf("\n\n");

    /*
       Fill several records with mixed data types.
       The last record is a deep copy of the string in records[5].
    */
    container_set_int(&records[0], 10);
    container_set_float(&records[1], 3.14f);
    container_set_char(&records[2], 'A');
    container_set_string(&records[3], "Hello");
    container_set_int(&records[4], 0);
    container_set_string(&records[5], "Data");
    container_copy(&records[6], &records[5]);
    container_set_string(&records[5], "Changed");

    for (i = 0; i < 6; i++) {
        printf("[%zu] %s: ", i, container_type_name(&records[i]));
        container_print(&records[i]);
        printf("\n");
    }
    printf("Deep copy: "); container_print(&records[6]); printf("\n");

    /*
       Edge-case tests: empty state, wrong getter use, and zero values.
       These verify the type-checking and safety behavior.
    */
    container_clear(&records[0]);
    printf("Empty container: "); container_print(&records[0]); printf("\n");

    container_set_int(&records[0], 0);
    printf("Integer zero getter: %s\n",
           container_get_int(&records[0], &number) && number == 0 ? "PASS" : "FAIL");
    printf("Wrong-type getter: %s\n",
           container_get_float(&records[0], &decimal) ? "FAIL" : "PASS");

    container_set_string(&records[0], "");
    printf("Empty string: "); container_print(&records[0]); printf("\n");

    container_set_string(&records[0], "one");
    container_set_int(&records[0], 5);
    container_set_string(&records[0], "two");
    container_set_float(&records[0], 2.5f);
    printf("STRING->INT->STRING->FLOAT: %s\n",
           records[0].type == TYPE_FLOAT ? "PASS" : "FAIL");

    /*
       Simulate allocation failure to confirm the old value remains valid.
    */
    fail_allocation = 1;
    printf("Allocation failure preserves old value: %s\n",
           !container_set_string(&records[0], "fail") &&
           records[0].type == TYPE_FLOAT ? "PASS" : "FAIL");

    /*
       Test same-pointer input and self-copy safety.
       These ensure the implementation handles aliases safely.
    */
    container_set_string(&records[0], "same");
    container_get_string(&records[0], &text);
    printf("Same-pointer replacement: %s\n",
           container_set_string(&records[0], text) ? "PASS" : "FAIL");
    printf("Self-copy: %s\n",
           container_copy(&records[0], &records[0]) ? "PASS" : "FAIL");

    /*
       Final cleanup: repeated clear must be safe and should not crash.
    */
    for (i = 0; i < 7; i++) {
        container_clear(&records[i]);
        container_clear(&records[i]);
    }

    return 0;
}
