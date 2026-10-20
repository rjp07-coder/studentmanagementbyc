
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DATA_FILE "students.txt"
#define MAX_STUDENTS 1000
#define MAX_SUBJECTS 10

typedef struct {
    char name[50];
    float marks;
    float max_marks;
} Subject;

typedef struct {
    int id;
    char type[12];
    char name[80];
    char dob[16];
    char gender[24];
    char phone[24];
    char email[120];
    char address[120];
    char institution[120];
    char class_course[120];
    char semester[40];
    int subject_count;
    Subject subjects[MAX_SUBJECTS];
} Student;

Student students[MAX_STUDENTS];
int student_count = 0;

void trim(char *s) {
    s[strcspn(s, "\n")] = '\0';
}

void input(const char *prompt, char *buf, size_t size) {
    printf("%s", prompt);
    if (!fgets(buf, (int)size, stdin)) {
        buf[0] = '\0';
        return;
    }
    trim(buf);
}

int get_int(const char *prompt, int min, int max) {
    char buf[100], extra;
    int value;

    while (1) {
        input(prompt, buf, sizeof(buf));
        if (sscanf(buf, "%d %c", &value, &extra) == 1 &&
            value >= min && value <= max)
            return value;

        printf("Enter a valid number (%d-%d).\n", min, max);
    }
}

float get_float(const char *prompt, float min, float max) {
    char buf[100], extra;
    float value;

    while (1) {
        input(prompt, buf, sizeof(buf));
        if (sscanf(buf, "%f %c", &value, &extra) == 1 &&
            value >= min && value <= max)
            return value;

        printf("Enter a valid value (%.2f-%.2f).\n", min, max);
    }
}

int find_student(int id) {
    for (int i = 0; i < student_count; i++)
        if (students[i].id == id)
            return i;
    return -1;
}

float percentage(const Student *s) {
    float obtained = 0, maximum = 0;

    for (int i = 0; i < s->subject_count; i++) {
        obtained += s->subjects[i].marks;
        maximum += s->subjects[i].max_marks;
    }

    return maximum > 0 ? obtained * 100.0f / maximum : -1;
}

const char *grade(float p) {
    if (p < 0) return "N/A";
    if (p >= 90) return "A+";
    if (p >= 80) return "A";
    if (p >= 70) return "B";
    if (p >= 60) return "C";
    if (p >= 50) return "D";
    if (p >= 33) return "E";
    return "F";
}

void save_data(void) {
    FILE *fp = fopen(DATA_FILE, "w");

    if (!fp) {
        printf("Error saving data!\n");
        return;
    }

    for (int i = 0; i < student_count; i++) {
        Student *s = &students[i];

        fprintf(fp, "STUDENT\n%d\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%d\n%s\n",
                s->id, s->type, s->name, s->dob, s->gender,
                s->phone, s->email, s->address, s->institution,
                s->class_course, s->subject_count, s->semester);

        for (int j = 0; j < s->subject_count; j++) {
            fprintf(fp, "%s\n%.2f\n%.2f\n",
                    s->subjects[j].name,
                    s->subjects[j].marks,
                    s->subjects[j].max_marks);
        }
    }

    fclose(fp);
}

void load_data(void) {
    FILE *fp = fopen(DATA_FILE, "r");
    char line[256];

    if (!fp) return;

    while (student_count < MAX_STUDENTS &&
           fgets(line, sizeof(line), fp)) {
        trim(line);
        if (strcmp(line, "STUDENT") != 0) break;

        Student s = {0};

        if (!fgets(line, sizeof(line), fp)) break;
        s.id = atoi(line);

        char *fields[] = {
            s.type, s.name, s.dob, s.gender, s.phone,
            s.email, s.address, s.institution, s.class_course
        };

        size_t sizes[] = {
            sizeof(s.type), sizeof(s.name), sizeof(s.dob),
            sizeof(s.gender), sizeof(s.phone), sizeof(s.email),
            sizeof(s.address), sizeof(s.institution),
            sizeof(s.class_course)
        };

        int valid = 1;

        for (int k = 0; k < 9; k++) {
            if (!fgets(line, sizeof(line), fp)) {
                valid = 0;
                break;
            }
            trim(line);
            snprintf(fields[k], sizes[k], "%s", line);
        }
        if (!valid || !fgets(line, sizeof(line), fp)) break;

        s.subject_count = atoi(line);
        if (s.subject_count < 0 ||
            s.subject_count > MAX_SUBJECTS) break;

        if (!fgets(line, sizeof(line), fp)) break;
        trim(line);
        snprintf(s.semester, sizeof(s.semester), "%s", line);

        for (int j = 0; j < s.subject_count; j++) {
            if (!fgets(line, sizeof(line), fp)) {
                valid = 0;
                break;
            }
            trim(line);
            snprintf(s.subjects[j].name,
                     sizeof(s.subjects[j].name), "%s", line);

            if (!fgets(line, sizeof(line), fp)) {
                valid = 0;
                break;
            }
            s.subjects[j].marks = (float)atof(line);

            if (!fgets(line, sizeof(line), fp)) {
                valid = 0;
                break;
            }
            s.subjects[j].max_marks = (float)atof(line);
        }

        if (!valid || s.id <= 0 || find_student(s.id) != -1)
            break;

        students[student_count++] = s;
    }

    fclose(fp);
}

void enter_subjects(Student *s) {
    s->subject_count = get_int("Number of subjects (0-10): ",
                               0, MAX_SUBJECTS);

    for (int i = 0; i < s->subject_count; i++) {
        printf("\nSubject %d\n", i + 1);

        input("Subject name: ", s->subjects[i].name,
              sizeof(s->subjects[i].name));

        while (s->subjects[i].name[0] == '\0')
            input("Name cannot be empty. Enter subject: ",
                  s->subjects[i].name,
                  sizeof(s->subjects[i].name));

        s->subjects[i].max_marks =
            get_float("Maximum marks: ", 0.01f, 1000000.0f);

        s->subjects[i].marks =
            get_float("Marks obtained: ", 0,
                      s->subjects[i].max_marks);
    }
}

void enter_details(Student *s, int new_student) {
    if (new_student) {
        do {
            s->id = get_int("Student ID: ", 1, 2147483647);
            if (find_student(s->id) != -1)
                printf("This ID already exists!\n");
            else
                break;
        } while (1);
    }

    int type = get_int("1. School  2. College: ", 1, 2);
    strcpy(s->type, type == 1 ? "School" : "College");

    input("Full name: ", s->name, sizeof(s->name));
    while (s->name[0] == '\0')
        input("Name cannot be empty. Full name: ",
              s->name, sizeof(s->name));

    input("Date of birth (YYYY-MM-DD): ", s->dob, sizeof(s->dob));
    input("Gender: ", s->gender, sizeof(s->gender));
    input("Phone: ", s->phone, sizeof(s->phone));
    input("Email: ", s->email, sizeof(s->email));
    input("Address: ", s->address, sizeof(s->address));
    input("School/College name: ", s->institution,
          sizeof(s->institution));

    if (type == 1) {
        input("Class/Grade: ", s->class_course,
              sizeof(s->class_course));
        input("Section/Academic year: ", s->semester,
              sizeof(s->semester));
    } else {
        input("Course: ", s->class_course,
              sizeof(s->class_course));
        input("Semester/Year: ", s->semester,
              sizeof(s->semester));
    }

    enter_subjects(s);
}

void add_student(void) {
    if (student_count >= MAX_STUDENTS) {
        printf("Storage is full!\n");
        return;
    }

    Student s = {0};
    enter_details(&s, 1);

    students[student_count++] = s;
    save_data();

    printf("Student added and saved successfully!\n");
}

void display_one(const Student *s) {
    printf("\n========================================\n");
    printf("Student ID : %d\n", s->id);
    printf("Type       : %s\n", s->type);
    printf("Name       : %s\n", s->name);
    printf("DOB        : %s\n", s->dob);
    printf("Gender     : %s\n", s->gender);
    printf("Phone      : %s\n", s->phone);
    printf("Email      : %s\n", s->email);
    printf("Address    : %s\n", s->address);
    printf("Institution: %s\n", s->institution);
    printf("Class/Course: %s\n", s->class_course);
    printf("Semester   : %s\n", s->semester);

    printf("\n%-20s %10s %10s\n",
           "Subject", "Marks", "Maximum");

    float obtained = 0, maximum = 0;

    for (int i = 0; i < s->subject_count; i++) {
        printf("%-20.20s %10.2f %10.2f\n",
               s->subjects[i].name,
               s->subjects[i].marks,
               s->subjects[i].max_marks);

        obtained += s->subjects[i].marks;
        maximum += s->subjects[i].max_marks;
    }

    float p = maximum > 0 ? obtained * 100.0f / maximum : -1;

    printf("----------------------------------------\n");
    printf("Total marks: %.2f / %.2f\n", obtained, maximum);

    if (p >= 0) {
        printf("Percentage: %.2f%%\n", p);
        printf("Grade: %s\n", grade(p));
        printf("Result: %s\n", p >= 33 ? "PASS" : "FAIL");
    } else {
        printf("No academic marks recorded.\n");
    }

    printf("========================================\n");
}

void display_all(void) {
    if (student_count == 0) {
        printf("No student records found.\n");
        return;
    }

    printf("\n%-8s %-24s %-10s %-24s %10s\n",
           "ID", "NAME", "TYPE", "CLASS/COURSE", "PERCENT");
    printf("-------------------------------------------------------------------------------\n");

    for (int i = 0; i < student_count; i++) {
        float p = percentage(&students[i]);

        if (p < 0) {
            printf("%-8d %-24.24s %-10s %-24.24s %10s\n",
                   students[i].id, students[i].name,
                   students[i].type, students[i].class_course, "N/A");
        } else {
            printf("%-8d %-24.24s %-10s %-24.24s %9.2f%%\n",
                   students[i].id, students[i].name,
                   students[i].type, students[i].class_course, p);
        }
    }

    printf("\nTotal students: %d\n", student_count);
}

void search_student(void) {
    int choice = get_int("1. Search by ID  2. Search by name: ", 1, 2);

    if (choice == 1) {
        int id = get_int("Enter Student ID: ", 1, 2147483647);
        int index = find_student(id);

        if (index < 0) printf("Student not found.\n");
        else display_one(&students[index]);

        return;
    }

    char query[80], name[80];
    input("Enter full or partial name: ", query, sizeof(query));

    for (int j = 0; query[j]; j++)
        query[j] = (char)tolower((unsigned char)query[j]);

    int found = 0;

    for (int i = 0; i < student_count; i++) {
        strcpy(name, students[i].name);

        for (int j = 0; name[j]; j++)
            name[j] = (char)tolower((unsigned char)name[j]);

        if (strstr(name, query)) {
            display_one(&students[i]);
            found = 1;
        }
    }

    if (!found) printf("No matching student found.\n");
}

void update_student(void) {
    int id = get_int("Enter Student ID to update: ", 1, 2147483647);
    int index = find_student(id);

    if (index < 0) {
        printf("Student not found.\n");
        return;
    }

    printf("Enter the student's updated details:\n");
    Student updated = {0};
    updated.id = id;

    enter_details(&updated, 0);
    students[index] = updated;
    save_data();

    printf("Student updated successfully!\n");
}

void delete_student(void) {
    int id = get_int("Enter Student ID to delete: ", 1, 2147483647);
    int index = find_student(id);

    if (index < 0) {
        printf("Student not found.\n");
        return;
    }

    display_one(&students[index]);

    char confirm[10];
    input("Delete this record? Type yes to confirm: ",
          confirm, sizeof(confirm));

    if (strcmp(confirm, "yes") != 0) {
        printf("Deletion cancelled.\n");
        return;
    }

    for (int i = index; i < student_count - 1; i++)
        students[i] = students[i + 1];

    student_count--;
    save_data();

    printf("Student deleted successfully!\n");
}

void sort_students(void) {
    int choice = get_int("1. Sort by name  2. Sort by ID  3. Sort by percentage: ",
                         1, 3);

    for (int i = 0; i < student_count - 1; i++) {
        for (int j = 0; j < student_count - i - 1; j++) {
            int swap = 0;

            if (choice == 1)
                swap = strcmp(students[j].name,
                              students[j + 1].name) > 0;
            else if (choice == 2)
                swap = students[j].id > students[j + 1].id;
            else
                swap = percentage(&students[j]) <
                       percentage(&students[j + 1]);

            if (swap) {
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }

    save_data();
    display_all();
}

void statistics(void) {
    int school = 0, college = 0, pass = 0, fail = 0, marked = 0;
    float total = 0;

    for (int i = 0; i < student_count; i++) {
        if (strcmp(students[i].type, "School") == 0)
            school++;
        else
            college++;

        float p = percentage(&students[i]);

        if (p >= 0) {
            total += p;
            marked++;
            if (p >= 33) pass++;
            else fail++;
        }
    }

    printf("\n========== STATISTICS ==========\n");
    printf("Total students: %d\n", student_count);
    printf("School students: %d\n", school);
    printf("College students: %d\n", college);
    printf("Students with marks: %d\n", marked);
    printf("Pass: %d\n", pass);
    printf("Fail: %d\n", fail);

    if (marked)
        printf("Average percentage: %.2f%%\n", total / marked);
    else
        printf("Average percentage: N/A\n");

    printf("================================\n");
}

int main(void) {
    load_data();

    int choice;

    do {
        printf("\n========================================\n");
        printf("   ADVANCED STUDENT MANAGEMENT SYSTEM\n");
        printf("========================================\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Sort Students\n");
        printf("7. View Statistics\n");
        printf("0. Save and Exit\n");
        printf("========================================\n");

        choice = get_int("Enter your choice: ", 0, 7);

        switch (choice) {
            case 1: add_student(); break;
            case 2: display_all(); break;
            case 3: search_student(); break;
            case 4: update_student(); break;
            case 5: delete_student(); break;
            case 6: sort_students(); break;
            case 7: statistics(); break;
            case 0:
                save_data();
                printf("Data saved. Goodbye!\n");
                break;
        }

    } while (choice != 0);

    return 0;
}
