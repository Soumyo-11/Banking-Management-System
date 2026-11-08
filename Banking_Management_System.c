#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_ACCOUNTS 1000
#define MAX_FDS 1000

#define ACCOUNT_FILE "accounts.dat"
#define FD_FILE "fixed_deposits.dat"
#define TRANSACTION_FILE "transactions.txt"

struct Account
{
    long account_number;
    char name[50];
    int age;
    char gender[20];
    char occupation[50];
    double balance;
};

struct FD
{
    long fd_number;
    long account_number;
    double deposit;
    int tenure;
    double maturity_amount;
};

struct Account accounts[MAX_ACCOUNTS];
struct FD fds[MAX_FDS];

int account_count = 0;
int fd_count = 0;


// Read a line of input
void read_line(char str[], int size)
{
    fgets(str, size, stdin);
    str[strcspn(str, "\n")] = '\0';
}


// Read an integer safely
int read_int()
{
    char input[100];
    char *end;
    long value;

    while (1)
    {
        read_line(input, sizeof(input));
        value = strtol(input, &end, 10);

        if (end != input && *end == '\0')
        {
            return (int)value;
        }

        printf("Invalid input. Enter an integer: ");
    }
}


// Read a positive amount
double read_amount()
{
    char input[100];
    char *end;
    double value;

    while (1)
    {
        read_line(input, sizeof(input));
        value = strtod(input, &end);

        if (end != input && *end == '\0' &&
            value > 0 && isfinite(value))
        {
            return value;
        }

        printf("Invalid amount. Enter a positive value: ");
    }
}


// Save accounts permanently
void save_accounts()
{
    FILE *fp = fopen(ACCOUNT_FILE, "wb");

    if (fp == NULL)
    {
        printf("Error saving accounts!\n");
        return;
    }

    fwrite(&account_count, sizeof(int), 1, fp);
    fwrite(accounts, sizeof(struct Account),
           account_count, fp);

    fclose(fp);
}


// Load accounts from file
void load_accounts()
{
    FILE *fp = fopen(ACCOUNT_FILE, "rb");

    if (fp == NULL)
    {
        account_count = 0;
        return;
    }

    if (fread(&account_count, sizeof(int), 1, fp) != 1 ||
        account_count < 0 ||
        account_count > MAX_ACCOUNTS)
    {
        account_count = 0;
        fclose(fp);
        return;
    }

    if (fread(accounts, sizeof(struct Account),
               account_count, fp) !=
        (size_t)account_count)
    {
        account_count = 0;
    }

    fclose(fp);
}


// Save fixed deposits permanently
void save_fds()
{
    FILE *fp = fopen(FD_FILE, "wb");

    if (fp == NULL)
    {
        printf("Error saving fixed deposits!\n");
        return;
    }

    fwrite(&fd_count, sizeof(int), 1, fp);
    fwrite(fds, sizeof(struct FD), fd_count, fp);

    fclose(fp);
}


// Load fixed deposits
void load_fds()
{
    FILE *fp = fopen(FD_FILE, "rb");

    if (fp == NULL)
    {
        fd_count = 0;
        return;
    }

    if (fread(&fd_count, sizeof(int), 1, fp) != 1 ||
        fd_count < 0 ||
        fd_count > MAX_FDS)
    {
        fd_count = 0;
        fclose(fp);
        return;
    }

    if (fread(fds, sizeof(struct FD), fd_count, fp) !=
        (size_t)fd_count)
    {
        fd_count = 0;
    }

    fclose(fp);
}


// Generate unique account number
long generate_account_number()
{
    long num;
    int found;
    int i;

    do
    {
        num = rand() % 90000000 + 10000000;
        found = 0;

        for (i = 0; i < account_count; i++)
        {
            if (accounts[i].account_number == num)
            {
                found = 1;
                break;
            }
        }

    } while (found);

    return num;
}


// Generate unique FD number
long generate_fd_number()
{
    long num;
    int found;
    int i;

    do
    {
        num = rand() % 90000000 + 10000000;
        found = 0;

        for (i = 0; i < fd_count; i++)
        {
            if (fds[i].fd_number == num)
            {
                found = 1;
                break;
            }
        }

    } while (found);

    return num;
}


// Find account
int find_account(long num)
{
    int i;

    for (i = 0; i < account_count; i++)
    {
        if (accounts[i].account_number == num)
        {
            return i;
        }
    }

    return -1;
}


// Record transaction permanently
void record_transaction(long account_number,
                        const char type[],
                        double amount,
                        double balance)
{
    FILE *fp = fopen(TRANSACTION_FILE, "a");

    if (fp == NULL)
    {
        printf("Error saving transaction history!\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    char timestamp[30];

    strftime(timestamp, sizeof(timestamp),
             "%d-%m-%Y %H:%M:%S", current_time);

    fprintf(fp, "%ld|%s|%s|%.2f|%.2f\n",
            account_number,
            timestamp,
            type,
            amount,
            balance);

    fclose(fp);
}


// Open savings account
void savings_account()
{
    struct Account new_account;

    if (account_count >= MAX_ACCOUNTS)
    {
        printf("Account limit reached.\n");
        return;
    }

    printf("Enter your name: ");
    read_line(new_account.name, sizeof(new_account.name));

    printf("Enter your age: ");
    new_account.age = read_int();

    printf("Enter your gender: ");
    read_line(new_account.gender, sizeof(new_account.gender));

    printf("Enter your occupation: ");
    read_line(new_account.occupation,
              sizeof(new_account.occupation));

    printf("Enter initial deposit (minimum Rs. 500): ");
    new_account.balance = read_amount();

    if (new_account.age < 18 ||
        new_account.balance < 500)
    {
        printf("Invalid input. Account opening failed.\n");
        return;
    }

    new_account.account_number =
        generate_account_number();

    accounts[account_count] = new_account;
    account_count++;

    save_accounts();

    record_transaction(new_account.account_number,
                      "Account Opening",
                      new_account.balance,
                      new_account.balance);

    printf("\nAccount successfully opened!\n");
    printf("Account number: %ld\n",
           new_account.account_number);
    printf("Current balance: Rs. %.2f\n",
           new_account.balance);
}


// Calculate FD maturity amount
double calculate_fd_return(int age,
                           double deposit,
                           int tenure)
{
    double rate;

    if (age >= 60)
    {
        rate = 0.07;
    }
    else
    {
        rate = 0.067;
    }

    return deposit * pow(1 + rate / 4, 4 * tenure);
}


// Open fixed deposit
void fixed_deposit()
{
    struct FD new_fd;
    long account_num;
    int index;
    int age;

    if (fd_count >= MAX_FDS)
    {
        printf("FD limit reached.\n");
        return;
    }

    printf("Enter your savings account number: ");
    account_num = read_int();

    index = find_account(account_num);

    if (index == -1)
    {
        printf("Account not found.\n");
        return;
    }

    age = accounts[index].age;

    printf("Enter FD deposit amount (minimum Rs. 10000): ");
    new_fd.deposit = read_amount();

    printf("Enter tenure in years: ");
    new_fd.tenure = read_int();

    if (new_fd.deposit < 10000 ||
        new_fd.tenure <= 0)
    {
        printf("Invalid FD details.\n");
        return;
    }

    if (new_fd.deposit > accounts[index].balance)
    {
        printf("Insufficient balance.\n");
        return;
    }

    new_fd.fd_number = generate_fd_number();
    new_fd.account_number = account_num;

    new_fd.maturity_amount =
        calculate_fd_return(age,
                            new_fd.deposit,
                            new_fd.tenure);

    accounts[index].balance -= new_fd.deposit;

    fds[fd_count] = new_fd;
    fd_count++;

    save_accounts();
    save_fds();

    record_transaction(account_num,
                      "FD Opening",
                      new_fd.deposit,
                      accounts[index].balance);

    printf("\nFixed deposit successfully opened!\n");
    printf("FD number: %ld\n", new_fd.fd_number);
    printf("Maturity amount: Rs. %.2f\n",
           new_fd.maturity_amount);
    printf("Remaining savings balance: Rs. %.2f\n",
           accounts[index].balance);
}


// Deposit money
void money_deposit()
{
    long num;
    int index;
    double amount;

    printf("Enter account number: ");
    num = read_int();

    index = find_account(num);

    if (index == -1)
    {
        printf("Account not found.\n");
        return;
    }

    printf("Enter deposit amount: Rs. ");
    amount = read_amount();

    accounts[index].balance += amount;

    save_accounts();

    record_transaction(num,
                      "Deposit",
                      amount,
                      accounts[index].balance);

    printf("Deposit successful!\n");
    printf("Current balance: Rs. %.2f\n",
           accounts[index].balance);
}


// Withdraw money
void money_withdrawal()
{
    long num;
    int index;
    double amount;

    printf("Enter account number: ");
    num = read_int();

    index = find_account(num);

    if (index == -1)
    {
        printf("Account not found.\n");
        return;
    }

    printf("Enter withdrawal amount: Rs. ");
    amount = read_amount();

    if (amount > accounts[index].balance)
    {
        printf("Insufficient balance!\n");
        return;
    }

    accounts[index].balance -= amount;

    save_accounts();

    record_transaction(num,
                      "Withdrawal",
                      amount,
                      accounts[index].balance);

    printf("Withdrawal successful!\n");
    printf("Current balance: Rs. %.2f\n",
           accounts[index].balance);
}


// Check balance
void balance_check()
{
    long num;
    int index;

    printf("Enter account number: ");
    num = read_int();

    index = find_account(num);

    if (index == -1)
    {
        printf("Account not found.\n");
        return;
    }

    printf("\nAccount holder: %s\n",
           accounts[index].name);

    printf("Account number: %ld\n",
           accounts[index].account_number);

    printf("Current balance: Rs. %.2f\n",
           accounts[index].balance);
}


// Display transaction history
void transaction_history()
{
    long num;
    FILE *fp;
    char line[200];

    long transaction_account;
    char timestamp[30];
    char type[30];
    double amount;
    double balance;

    printf("Enter account number: ");
    num = read_int();

    if (find_account(num) == -1)
    {
        printf("Account not found.\n");
        return;
    }

    fp = fopen(TRANSACTION_FILE, "r");

    if (fp == NULL)
    {
        printf("No transaction history available.\n");
        return;
    }

    printf("\n========== TRANSACTION HISTORY ==========\n");
    printf("Account number: %ld\n\n", num);

    printf("%-20s %-20s %-15s %-15s\n",
           "Date & Time",
           "Transaction",
           "Amount",
           "Balance");

    printf("------------------------------------------------------------\n");

    int found = 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (sscanf(line,
                   "%ld|%29[^|]|%29[^|]|%lf|%lf",
                   &transaction_account,
                   timestamp,
                   type,
                   &amount,
                   &balance) == 5)
        {
            if (transaction_account == num)
            {
                printf("%-20s %-20s Rs. %-11.2f Rs. %-11.2f\n",
                       timestamp,
                       type,
                       amount,
                       balance);

                found = 1;
            }
        }
    }

    if (!found)
    {
        printf("No transactions found.\n");
    }

    fclose(fp);
}


// Main program
int main()
{
    int choice;

    srand((unsigned int)time(NULL));

    load_accounts();
    load_fds();

    printf("\n");
    printf("============================================================\n");
    printf("\t\tWELCOME TO YATSKU NATIONAL BANK\n");
    printf("============================================================\n");

    while (1)
    {
        printf("\nPlease choose an operation:\n");

        printf("1. Open Savings Account\n");
        printf("2. Fixed Deposit\n");
        printf("3. Money Deposit\n");
        printf("4. Money Withdrawal\n");
        printf("5. Balance Check\n");
        printf("6. Transaction History\n");
        printf("7. Exit\n");

        printf("\nEnter your choice: ");
        choice = read_int();

        switch (choice)
        {
            case 1:
                savings_account();
                break;

            case 2:
                fixed_deposit();
                break;

            case 3:
                money_deposit();
                break;

            case 4:
                money_withdrawal();
                break;

            case 5:
                balance_check();
                break;

            case 6:
                transaction_history();
                break;

            case 7:
                printf("\nThank you for visiting "
                       "Yatsku National Bank.\n");

                printf("============================================================\n");
                return 0;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}