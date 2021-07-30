#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 100 // Max number of records our binary file can store
#define MAX_BUF 200 // Max buffer for cwd call
#define STRING_PARAM_LENGTH 30 // Length of string fields on a record
static char FILE_LOCATION[] = "binary_file.txt"; // Default binary file name
static char TEMP_FILE[] = "temp.txt"; // Temp store used when updating file
const static int MAX_INIT_RETRIES = 5; // How many times we attempt we try to read the binary file on program init

/******************
 * Menu Constants *
 ******************/
static char CREATE_FILE[] = "Create the Binary File";
static char DISPLAY_CONTENTS[] = "Display the contents of the file";
static char SEEK[] = "Seek a specific record";
static char UPDATE_CONTENTS[] = "Update the contents of a record";
static char DELETE[] = "Delete a record with a specific name";
static char EDIT[] = "Exit";

/**************
 * Menu Enums *
 **************/
enum options
{
    OPT_CREATE = 1,
    OPT_DISPLAY = 2,
    OTP_SEEK = 3,
    OTP_UPDATE = 4,
    OTP_DELETE = 5,
    OPT_EXIT = 6
};

/**********************
 * Boolean Definition *
 **********************/
enum bool
{
    FALSE,
    TRUE
};

/******************
 * Record Structs *
 *****************/
struct Record
{
    char Student_ID[STRING_PARAM_LENGTH];
    char Student_Name[STRING_PARAM_LENGTH];
    char Email_ID[STRING_PARAM_LENGTH];
    char Course_ID[STRING_PARAM_LENGTH];
    int Grade;
};

struct Record_Store
{
    struct Record records[MAX_SIZE];
    int last;
};

/********************
 * Helper Functions *
 ********************/

char *get_file_path(char *filename)
{
    char path[MAX_BUF];

    getcwd(path, MAX_BUF);

    return strcat(strcat(path, "/"), filename);
}

enum bool didUserInputStrVal(char *str1)
{
    if (strcmp(str1, "-1") == 0)
    {
        return FALSE;
    }
    return TRUE;
}

enum bool isRecordStrAttrSet(char *attr)
{
    if (strlen(attr) > 0)
    {
        return TRUE;
    }

    return FALSE;
}

/**************************
 * Display Functions *
 **************************/

void print_menu()
{
    printf("1. %s\n", CREATE_FILE);
    printf("2. %s\n", DISPLAY_CONTENTS);
    printf("3. %s\n", SEEK);
    printf("4. %s\n", UPDATE_CONTENTS);
    printf("5. %s\n", DELETE);
    printf("6. %s\n", EDIT);
    printf("\n");
}

void print_record(struct Record current_record)
{

    printf("--------------------------------------------------\n");
    printf("Student ID: %s\n", current_record.Student_ID);
    printf("Student Name: %s\n", current_record.Student_Name);
    printf("Email ID: %s\n", current_record.Email_ID);
    printf("Course ID: %s\n", current_record.Course_ID);
    printf("Grade: %d\n", current_record.Grade);
    printf("--------------------------------------------------\n");
}


/******************
 * File Functions *
 ******************/

void create_new_file()
{
    FILE *fptr;
    fptr = fopen(get_file_path(FILE_LOCATION), "wb+");
    fclose(fptr);
}

void list_file_contents()
{
    struct Record current_record;
    FILE *fptr = fopen(get_file_path(FILE_LOCATION), "rb");
    int counter = 0;

    while (fread(&current_record, sizeof(struct Record), 1, fptr))
    {
        printf("Record %d\n", counter + 1);
        print_record(current_record);
        counter++;
    }

    if (counter == 0) {
        printf("********************\n");
        printf("* No Records Exist *\n");
        printf("********************\n");
    }

    fclose(fptr);
}

void write_to_file(struct Record_Store *rec_store)
{
    FILE *temp_ptr = fopen(get_file_path(TEMP_FILE), "wb+");

    int current;
    struct Record current_record;

    for (current = 0; current < rec_store->last; current++)
    {
        current_record = rec_store->records[current];

        fwrite(&current_record, sizeof(struct Record), 1, temp_ptr);
    }

    fclose(temp_ptr);

    remove(FILE_LOCATION);
    rename(TEMP_FILE, FILE_LOCATION);
    remove(TEMP_FILE);
}

/********************
 * Record Functions *
 ********************/

struct Record new_record()
{
    struct Record rec;
    strcpy(rec.Student_ID, "");
    rec.Grade = 0;
    return rec;
};

struct Record create_update_record(struct Record rec, enum bool is_update)
{
    // Defaults
    char student_id[STRING_PARAM_LENGTH] = "-1";
    char student_name[STRING_PARAM_LENGTH];
    char email_id[STRING_PARAM_LENGTH];
    char course_id[STRING_PARAM_LENGTH];
    float grade;

    if (is_update)
    {
        printf("Current Student ID: %s. Enter a new value or -1 to skip\n", rec.Student_ID);
        scanf("%s", student_id);
    }

    if (is_update)
    {
        printf("Current Student Name:  %s. Enter a new value or -1 to skip\n", rec.Student_Name);
    }
    else
    {
        printf("New Student Name: \n");
    }
    scanf("%s", student_name);

    if (is_update)
    {
        printf("Current Email ID:  %s. Enter a new value or -1 to skip\n", rec.Email_ID);
    }
    else
    {
        printf("New Email ID: \n");
    }
    scanf("%s", email_id);

    if (is_update)
    {
        printf("Current Course ID: %s. Enter a new value or -1 to skip\n", rec.Course_ID);
    }
    else
    {
        printf("New Course ID: \n");
    }
    scanf("%s", course_id);

    if (is_update)
    {
        printf("Current Grade: %d. Enter a new value or -1 to skip\n", rec.Grade);
    }
    else
    {
        printf("New Grade: \n");
    }
    scanf("%f", &grade);

    if (didUserInputStrVal(student_id) == TRUE)
    {
        strcpy(rec.Student_ID, student_id);
    }

    if (didUserInputStrVal(student_name) == TRUE)
    {
        strcpy(rec.Student_Name, student_name);
    }

    if (didUserInputStrVal(course_id) == TRUE)
    {
        strcpy(rec.Course_ID, course_id);
    }

    if (didUserInputStrVal(email_id) == TRUE)
    {
        strcpy(rec.Email_ID, email_id);
    }

    if (grade > 0)
    {
        rec.Grade = grade;
    }

    return rec;
}

struct Record create_record(char student_id[])
{
    struct Record rec = new_record();
    strcpy(rec.Student_ID, student_id);
    return create_update_record(rec, FALSE);
}

struct Record_Store new_record_store()
{
    struct Record_Store rec_store;
    rec_store.last = 0;
    return rec_store;
}

void add_record_to_store(struct Record_Store *rec_store, struct Record *rec)
{
    if (rec_store->last == MAX_SIZE)
    {
        printf("****************************\n");
        printf("* Max Binary Size Reached! *\n");
        printf("****************************\n");
    }
    else
    {
        rec_store->records[rec_store->last++] = *rec;
    }
}

int get_index_of_record(struct Record_Store *rec_store, char student_id[])
{
    int current;
    struct Record current_record;

    for (current = 0; current < rec_store->last; current++)
    {
        current_record = rec_store->records[current];

        if (strcmp(current_record.Student_ID, student_id) == 0)
        {
            return current;
        }
    }

    return -1;
}

void delete_record_from_store(struct Record_Store *rec_store, int index_to_delete)
{
    rec_store->records[index_to_delete] = rec_store->records[--rec_store->last];
}

void delete_record(struct Record_Store *rec_store)
{
    char search_student_id[20];
    struct Record current_record;
    printf("Please enter the Student ID of the record you wish to delete..\n");
    scanf("%s", search_student_id);

    int current = get_index_of_record(rec_store, search_student_id);

    if (current == -1)
    {
        printf("**************************\n");
        printf("* No such record exists! *\n");
        printf("**************************\n");
    }
    else
    {
        delete_record_from_store(rec_store, current);
        printf("*******************************\n");
        printf("* Record Deleted Successfully *\n");
        printf("*******************************\n");
    }

    write_to_file(rec_store);
}

void seek_record(struct Record_Store *rec_store)
{
    char search_student_id[STRING_PARAM_LENGTH];
    struct Record current_record;
    printf("Please enter the Student ID of the record you wish to see..\n");
    scanf("%s", search_student_id);

    int current = get_index_of_record(rec_store, search_student_id);

    if (current == -1)
    {
        printf("**************************\n");
        printf("* No such record exists! *\n");
        printf("**************************\n");
    }
    else
    {
        current_record = rec_store->records[current];
        print_record(current_record);
    }
};

/**************************
 * Program Flow Functions *
 **************************/

void update(struct Record_Store *rec_store)
{
    char yes_no[2];
    char search_student_id[STRING_PARAM_LENGTH];
    struct Record current_record;
    printf("Please enter the Student ID of the record you wish to update..\n");
    scanf("%s", search_student_id);

    int current = get_index_of_record(rec_store, search_student_id);

    if (current == -1)
    {
        printf("That Student ID does not exist. Would you like to create it? y/n\n");
        scanf("%s", yes_no);
        if (strcmp(yes_no, "y") == 0)
        {
            current_record = create_record(search_student_id);
            add_record_to_store(rec_store, &current_record);
        }
    }
    else
    {
        current_record = rec_store->records[current];
        current_record = create_update_record(current_record, TRUE);
        rec_store->records[current] = current_record;
    }

    write_to_file(rec_store);
}

enum options get_user_selection()
{
    enum options selection;
    printf("Please enter your choice....");
    scanf("%d", &selection);
    printf("\n");
    return selection;
}

/**
 * Attempt to load the binary file into memory. If no file exists or tries exceeded init a new file
 **/
struct Record_Store init_record_store()
{
    FILE *fptr = fopen(get_file_path(FILE_LOCATION), "rb");
    int tries = 1;

    struct Record found_record;
    struct Record_Store rec_store = new_record_store();

    // If file fails to open retry by configured amount
    while (fptr == NULL && tries++ < MAX_INIT_RETRIES)
    {
        fptr = fopen(get_file_path(FILE_LOCATION), "rb");
    }

    if (fptr != NULL)
    {
        // Load file into memory
        while (fread(&found_record, sizeof(struct Record), 1, fptr))
        {
            add_record_to_store(&rec_store, &found_record);
        }
    }
    else
    {
        create_new_file();
    }

    fclose(fptr);
    return rec_store;
}

int main(void)
{
    int loop = 1;
    int selection;
    struct Record_Store rec_store = init_record_store();

    while (loop == 1)
    {
        print_menu();
        selection = get_user_selection();

        switch (selection)
        {
        case OPT_CREATE:
            create_new_file();
            rec_store = new_record_store();
            break;
        case OTP_UPDATE:
            update(&rec_store);
            break;
        case OPT_DISPLAY:
            list_file_contents();
            break;
        case OTP_DELETE:
            delete_record(&rec_store);
            break;
        case OTP_SEEK:
            seek_record(&rec_store);
            break;
        case OPT_EXIT:
            loop = 0;
            break;
        };
    }
}
