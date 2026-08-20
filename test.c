#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_BOOKS 200
#define MAX_MEMBERS 200
#define MAX_ISSUES 300
#define TITLE_LEN 100
#define NAME_LEN 100
#define PHONE_LEN 30
#define DATE_LEN 11
#define FINE_PER_DAY 5.0

#define BOOK_FILE "books.txt"
#define MEMBER_FILE "members.txt"
#define ISSUE_FILE "issues.txt"
typedef struct {
    int id;
    char title[TITLE_LEN];
    char author[NAME_LEN];
    int year;
    int available;
} Book;

typedef struct {
    int id;
    char name[NAME_LEN];
    char phone[PHONE_LEN];
} Member;

typedef struct {
    int issueId;
    int bookId;
    int memberId;
    char issueDate[DATE_LEN];
    char dueDate[DATE_LEN];
    char returnDate[DATE_LEN];
    int returned;
    double fine;
} IssueRecord;

static Book books[MAX_BOOKS];
static Member members[MAX_MEMBERS];
static IssueRecord issues[MAX_ISSUES];
static int bookCount = 0;
static int memberCount = 0;
static int issueCount = 0;

static void trimNewline(char *text) {
    size_t length = strlen(text);
    if (length > 0 && text[length - 1] == '\n') {
        text[length - 1] = '\0';
    }
}

static void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

static void readLine(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    trimNewline(buffer);
}

static int readInt(const char *prompt) {
    char buffer[64];
    int value;

    for (;;) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return 0;
        }
        if (sscanf(buffer, "%d", &value) == 1) {
            return value;
        }
        printf("Invalid input. Try again.\n");
    }
}

static double readDouble(const char *prompt) {
    char buffer[64];
    double value;

    for (;;) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return 0.0;
        }
        if (sscanf(buffer, "%lf", &value) == 1) {
            return value;
        }
        printf("Invalid input. Try again.\n");
    }
}

static void currentDate(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d", local);
}

static int parseDate(const char *text, struct tm *result) {
    int year, month, day;
    if (sscanf(text, "%d-%d-%d", &year, &month, &day) != 3) {
        return 0;
    }
    memset(result, 0, sizeof(*result));
    result->tm_year = year - 1900;
    result->tm_mon = month - 1;
    result->tm_mday = day;
    result->tm_hour = 12;
    return 1;
}

static void addDays(const char *dateText, int days, char *output, size_t size) {
    struct tm date;
    time_t rawDate;
    if (!parseDate(dateText, &date)) {
        snprintf(output, size, "%s", dateText);
        return;
    }
    rawDate = mktime(&date);
    rawDate += (time_t)days * 24 * 60 * 60;
    date = *localtime(&rawDate);
    strftime(output, size, "%Y-%m-%d", &date);
}

static int daysBetween(const char *startText, const char *endText) {
    struct tm startTm;
    struct tm endTm;
    time_t startTime;
    time_t endTime;
    double seconds;

    if (!parseDate(startText, &startTm) || !parseDate(endText, &endTm)) {
        return 0;
    }

    startTime = mktime(&startTm);
    endTime = mktime(&endTm);
    seconds = difftime(endTime, startTime);
    return (int)(seconds / (24 * 60 * 60));
}

static int findBookIndex(int id) {
    for (int i = 0; i < bookCount; i++) {
        if (books[i].id == id) {
            return i;
        }
    }
    return -1;
}

static int findMemberIndex(int id) {
    for (int i = 0; i < memberCount; i++) {
        if (members[i].id == id) {
            return i;
        }
    }
    return -1;
}

static int findIssueIndexByBook(int bookId) {
    for (int i = 0; i < issueCount; i++) {
        if (issues[i].bookId == bookId && !issues[i].returned) {
            return i;
        }
    }
    return -1;
}

static int nextIssueId(void) {
    int maxId = 0;
    for (int i = 0; i < issueCount; i++) {
        if (issues[i].issueId > maxId) {
            maxId = issues[i].issueId;
        }
    }
    return maxId + 1;
}

static int readBooksFromText(FILE *file) {
    char line[512];
    int count = 0;

    if (!fgets(line, sizeof(line), file)) {
        return 0;
    }
    if (sscanf(line, "%d", &count) != 1 || count < 0 || count > MAX_BOOKS) {
        return 0;
    }

    for (int i = 0; i < count; i++) {
        if (!fgets(line, sizeof(line), file)) {
            return 0;
        }
        if (sscanf(line, "%d|%99[^|]|%99[^|]|%d|%d",
                   &books[i].id,
                   books[i].title,
                   books[i].author,
                   &books[i].year,
                   &books[i].available) != 5) {
            return 0;
        }
    }

    bookCount = count;
    return 1;
}

static int readMembersFromText(FILE *file) {
    char line[512];
    int count = 0;

    if (!fgets(line, sizeof(line), file)) {
        return 0;
    }
    if (sscanf(line, "%d", &count) != 1 || count < 0 || count > MAX_MEMBERS) {
        return 0;
    }

    for (int i = 0; i < count; i++) {
        if (!fgets(line, sizeof(line), file)) {
            return 0;
        }
        if (sscanf(line, "%d|%99[^|]|%29[^|]",
                   &members[i].id,
                   members[i].name,
                   members[i].phone) != 3) {
            return 0;
        }
    }

    memberCount = count;
    return 1;
}

static int readIssuesFromText(FILE *file) {
    char line[512];
    int count = 0;

    if (!fgets(line, sizeof(line), file)) {
        return 0;
    }
    if (sscanf(line, "%d", &count) != 1 || count < 0 || count > MAX_ISSUES) {
        return 0;
    }

    for (int i = 0; i < count; i++) {
        if (!fgets(line, sizeof(line), file)) {
            return 0;
        }
        if (sscanf(line, "%d|%d|%d|%10[^|]|%10[^|]|%10[^|]|%d|%lf",
                   &issues[i].issueId,
                   &issues[i].bookId,
                   &issues[i].memberId,
                   issues[i].issueDate,
                   issues[i].dueDate,
                   issues[i].returnDate,
                   &issues[i].returned,
                   &issues[i].fine) != 8) {
            return 0;
        }
    }

    issueCount = count;
    return 1;
}

static int loadBooks(void) {
    FILE *file = fopen(BOOK_FILE, "r");
    if (!file) {
        bookCount = 0;
        return 0;
    }
    if (!readBooksFromText(file)) {
        bookCount = 0;
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}

static int loadMembers(void) {
    FILE *file = fopen(MEMBER_FILE, "r");
    if (!file) {
        memberCount = 0;
        return 0;
    }
    if (!readMembersFromText(file)) {
        memberCount = 0;
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}

static int loadIssues(void) {
    FILE *file = fopen(ISSUE_FILE, "r");
    if (!file) {
        issueCount = 0;
        return 0;
    }
    if (!readIssuesFromText(file)) {
        issueCount = 0;
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}

static int saveBooks(void) {
    FILE *file = fopen(BOOK_FILE, "w");
    if (!file) {
        return 0;
    }
    fprintf(file, "%d\n", bookCount);
    for (int i = 0; i < bookCount; i++) {
        fprintf(file, "%d|%s|%s|%d|%d\n",
                books[i].id,
                books[i].title,
                books[i].author,
                books[i].year,
                books[i].available);
    }
    fclose(file);
    return 1;
}

static int saveMembers(void) {
    FILE *file = fopen(MEMBER_FILE, "w");
    if (!file) {
        return 0;
    }
    fprintf(file, "%d\n", memberCount);
    for (int i = 0; i < memberCount; i++) {
        fprintf(file, "%d|%s|%s\n",
                members[i].id,
                members[i].name,
            members[i].phone);
    }
    fclose(file);
    return 1;
}

static int saveIssues(void) {
    FILE *file = fopen(ISSUE_FILE, "w");
    if (!file) {
        return 0;
    }
    fprintf(file, "%d\n", issueCount);
    for (int i = 0; i < issueCount; i++) {
        fprintf(file, "%d|%d|%d|%s|%s|%s|%d|%.2f\n",
                issues[i].issueId,
                issues[i].bookId,
                issues[i].memberId,
                issues[i].issueDate,
                issues[i].dueDate,
                issues[i].returnDate[0] ? issues[i].returnDate : "-",
                issues[i].returned,
                issues[i].fine);
    }
    fclose(file);
    return 1;
}

static void saveAll(void) {
    saveBooks();
    saveMembers();
    saveIssues();
}

static void addBook(void) {
    Book book;

    if (bookCount >= MAX_BOOKS) {
        printf("Book storage is full.\n");
        return;
    }

    book.id = readInt("Book ID: ");
    if (findBookIndex(book.id) != -1) {
        printf("A book with this ID already exists.\n");
        return;
    }

    readLine("Title: ", book.title, sizeof(book.title));
    readLine("Author: ", book.author, sizeof(book.author));
    book.year = readInt("Publication Year: ");
    book.available = 1;

    books[bookCount++] = book;
    saveBooks();
    printf("Book added successfully.\n");
}

static void listBooks(void) {
    printf("\n%-8s %-30s %-24s %-8s %-10s\n", "ID", "Title", "Author", "Year", "Status");
    printf("-------------------------------------------------------------------------------\n");
    for (int i = 0; i < bookCount; i++) {
        printf("%-8d %-30.30s %-24.24s %-8d %-10s\n",
               books[i].id,
               books[i].title,
               books[i].author,
               books[i].year,
               books[i].available ? "Available" : "Issued");
    }
    if (bookCount == 0) {
        printf("No books found.\n");
    }
}

static void searchBook(void) {
    int id = readInt("Enter Book ID to search: ");
    int index = findBookIndex(id);

    if (index == -1) {
        printf("Book not found.\n");
        return;
    }

    printf("Book ID   : %d\n", books[index].id);
    printf("Title     : %s\n", books[index].title);
    printf("Author    : %s\n", books[index].author);
    printf("Year      : %d\n", books[index].year);
    printf("Status    : %s\n", books[index].available ? "Available" : "Issued");
}

static void updateBook(void) {
    int id = readInt("Enter Book ID to update: ");
    int index = findBookIndex(id);

    if (index == -1) {
        printf("Book not found.\n");
        return;
    }

    readLine("New Title: ", books[index].title, sizeof(books[index].title));
    readLine("New Author: ", books[index].author, sizeof(books[index].author));
    books[index].year = readInt("New Publication Year: ");
    saveBooks();
    printf("Book updated successfully.\n");
}

static void deleteBook(void) {
    int id = readInt("Enter Book ID to delete: ");
    int index = findBookIndex(id);

    if (index == -1) {
        printf("Book not found.\n");
        return;
    }

    if (!books[index].available) {
        printf("Cannot delete an issued book. Return it first.\n");
        return;
    }

    for (int i = index; i < bookCount - 1; i++) {
        books[i] = books[i + 1];
    }
    bookCount--;
    saveBooks();
    printf("Book deleted successfully.\n");
}

static void addMember(void) {
    Member member;

    if (memberCount >= MAX_MEMBERS) {
        printf("Member storage is full.\n");
        return;
    }

    member.id = readInt("Member ID: ");
    if (findMemberIndex(member.id) != -1) {
        printf("A member with this ID already exists.\n");
        return;
    }

    readLine("Member Name: ", member.name, sizeof(member.name));
    readLine("Phone Number: ", member.phone, sizeof(member.phone));

    members[memberCount++] = member;
    saveMembers();
    printf("Member added successfully.\n");
}

static void listMembers(void) {
    printf("\n%-8s %-30s %-18s\n", "ID", "Name", "Phone");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < memberCount; i++) {
        printf("%-8d %-30.30s %-18.18s\n",
               members[i].id,
               members[i].name,
               members[i].phone);
    }
    if (memberCount == 0) {
        printf("No members found.\n");
    }
}

static void searchMember(void) {
    int id = readInt("Enter Member ID to search: ");
    int index = findMemberIndex(id);

    if (index == -1) {
        printf("Member not found.\n");
        return;
    }

    printf("Member ID : %d\n", members[index].id);
    printf("Name      : %s\n", members[index].name);
    printf("Phone     : %s\n", members[index].phone);
}

static void updateMember(void) {
    int id = readInt("Enter Member ID to update: ");
    int index = findMemberIndex(id);

    if (index == -1) {
        printf("Member not found.\n");
        return;
    }

    readLine("New Name: ", members[index].name, sizeof(members[index].name));
    readLine("New Phone Number: ", members[index].phone, sizeof(members[index].phone));
    saveMembers();
    printf("Member updated successfully.\n");
}

static void deleteMember(void) {
    int id = readInt("Enter Member ID to delete: ");
    int index = findMemberIndex(id);

    if (index == -1) {
        printf("Member not found.\n");
        return;
    }

    for (int i = 0; i < issueCount; i++) {
        if (issues[i].memberId == id && !issues[i].returned) {
            printf("Cannot delete member with active issued books.\n");
            return;
        }
    }

    for (int i = index; i < memberCount - 1; i++) {
        members[i] = members[i + 1];
    }
    memberCount--;
    saveMembers();
    printf("Member deleted successfully.\n");
}

static void issueBook(void) {
    int bookId = readInt("Enter Book ID to issue: ");
    int memberId = readInt("Enter Member ID: ");
    int bookIndex = findBookIndex(bookId);
    int memberIndex = findMemberIndex(memberId);
    IssueRecord issue;

    if (bookIndex == -1) {
        printf("Book not found.\n");
        return;
    }
    if (memberIndex == -1) {
        printf("Member not found.\n");
        return;
    }
    if (!books[bookIndex].available) {
        printf("This book is already issued.\n");
        return;
    }
    if (issueCount >= MAX_ISSUES) {
        printf("Issue register is full.\n");
        return;
    }

    issue.issueId = nextIssueId();
    issue.bookId = bookId;
    issue.memberId = memberId;
    currentDate(issue.issueDate, sizeof(issue.issueDate));
    addDays(issue.issueDate, 14, issue.dueDate, sizeof(issue.dueDate));
    issue.returned = 0;
    issue.returnDate[0] = '\0';
    issue.fine = 0.0;

    books[bookIndex].available = 0;
    issues[issueCount++] = issue;
    saveAll();

    printf("Book issued successfully.\n");
    printf("Issue Date: %s\n", issue.issueDate);
    printf("Due Date  : %s\n", issue.dueDate);
}

static void returnBook(void) {
    int bookId = readInt("Enter Book ID to return: ");
    int issueIndex = findIssueIndexByBook(bookId);
    char today[DATE_LEN];
    int overdueDays;

    if (issueIndex == -1) {
        printf("No active issue found for this book.\n");
        return;
    }

    currentDate(today, sizeof(today));
    overdueDays = daysBetween(issues[issueIndex].dueDate, today);
    if (overdueDays > 0) {
        issues[issueIndex].fine = overdueDays * FINE_PER_DAY;
    } else {
        issues[issueIndex].fine = 0.0;
    }

    issues[issueIndex].returned = 1;
    snprintf(issues[issueIndex].returnDate, sizeof(issues[issueIndex].returnDate), "%s", today);
    {
        int bookIndex = findBookIndex(bookId);
        if (bookIndex != -1) {
            books[bookIndex].available = 1;
        }
    }

    saveAll();
    printf("Book returned successfully.\n");
    printf("Return Date : %s\n", issues[issueIndex].returnDate);
    printf("Fine        : %.2f\n", issues[issueIndex].fine);
}

static void listIssues(void) {
    printf("\n%-8s %-8s %-8s %-12s %-12s %-12s %-8s\n",
           "IssueID", "BookID", "MemberID", "IssueDate", "DueDate", "ReturnDate", "Fine");
    printf("--------------------------------------------------------------------------------------\n");
    for (int i = 0; i < issueCount; i++) {
        printf("%-8d %-8d %-8d %-12s %-12s %-12s %-8.2f\n",
               issues[i].issueId,
               issues[i].bookId,
               issues[i].memberId,
               issues[i].issueDate,
               issues[i].dueDate,
               issues[i].returned ? issues[i].returnDate : "-",
               issues[i].fine);
    }
    if (issueCount == 0) {
        printf("No issue records found.\n");
    }
}

static void showReports(void) {
    int availableBooks = 0;
    int issuedBooks = 0;
    int activeIssues = 0;
    int overdueIssues = 0;
    char today[DATE_LEN];

    currentDate(today, sizeof(today));

    for (int i = 0; i < bookCount; i++) {
        if (books[i].available) {
            availableBooks++;
        } else {
            issuedBooks++;
        }
    }

    for (int i = 0; i < issueCount; i++) {
        if (!issues[i].returned) {
            activeIssues++;
            if (daysBetween(issues[i].dueDate, today) > 0) {
                overdueIssues++;
            }
        }
    }

    printf("\nLibrary Report\n");
    printf("Total Books     : %d\n", bookCount);
    printf("Available Books : %d\n", availableBooks);
    printf("Issued Books    : %d\n", issuedBooks);
    printf("Total Members   : %d\n", memberCount);
    printf("Active Issues   : %d\n", activeIssues);
    printf("Overdue Issues  : %d\n", overdueIssues);
}

static void manageBooksMenu(void) {
    int choice;

    do {
        printf("\nBook Management\n");
        printf("1. Add Book\n");
        printf("2. List Books\n");
        printf("3. Search Book\n");
        printf("4. Update Book\n");
        printf("5. Delete Book\n");
        printf("0. Back\n");
        choice = readInt("Choose an option: ");

        switch (choice) {
            case 1: addBook(); break;
            case 2: listBooks(); break;
            case 3: searchBook(); break;
            case 4: updateBook(); break;
            case 5: deleteBook(); break;
            case 0: break;
            default: printf("Invalid choice.\n"); break;
        }
    } while (choice != 0);
}

static void manageMembersMenu(void) {
    int choice;

    do {
        printf("\nMember Management\n");
        printf("1. Add Member\n");
        printf("2. List Members\n");
        printf("3. Search Member\n");
        printf("4. Update Member\n");
        printf("5. Delete Member\n");
        printf("0. Back\n");
        choice = readInt("Choose an option: ");

        switch (choice) {
            case 1: addMember(); break;
            case 2: listMembers(); break;
            case 3: searchMember(); break;
            case 4: updateMember(); break;
            case 5: deleteMember(); break;
            case 0: break;
            default: printf("Invalid choice.\n"); break;
        }
    } while (choice != 0);
}

static void issueReturnMenu(void) {
    int choice;

    do {
        printf("\nIssue & Return\n");
        printf("1. Issue Book\n");
        printf("2. Return Book\n");
        printf("3. View Issue Records\n");
        printf("0. Back\n");
        choice = readInt("Choose an option: ");

        switch (choice) {
            case 1: issueBook(); break;
            case 2: returnBook(); break;
            case 3: listIssues(); break;
            case 0: break;
            default: printf("Invalid choice.\n"); break;
        }
    } while (choice != 0);
}

static int authenticate(void) {
    char username[64];
    char password[64];

    printf("Library Management System Login\n");
    readLine("Username: ", username, sizeof(username));
    readLine("Password: ", password, sizeof(password));

    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
        return 1;
    }

    printf("Invalid credentials.\n");
    return 0;
}

int main(void) {
    int choice;
    loadBooks();
    loadMembers();
    loadIssues();
    
    if (!authenticate()) {
        return 0;
    }

    do {
        printf("\nMain Menu\n");
        printf("1. Book Management\n");
        printf("2. Member Management\n");
        printf("3. Issue & Return\n");
        printf("4. Reports & Statistics\n");
        printf("0. Exit\n");
        choice = readInt("Choose an option: ");

        switch (choice) {
            case 1: manageBooksMenu(); break;
            case 2: manageMembersMenu(); break;
            case 3: issueReturnMenu(); break;
            case 4: showReports(); break;
            case 0:
                saveAll();
                printf("Goodbye.\n");
                break;
            default:
                printf("Invalid choice.\n");
                break;
        }
    } while (choice != 0);

    return 0;
}