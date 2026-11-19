# Expense Splitting Application (C++)

> **Original author: Krish Mehta ([@KM701](https://github.com/KM701)).**
> The source code in `mainfiles/` and `headerfiles/` was written by him and is shared in this repository with his permission.
> Original repository: https://github.com/KM701/expense-splitting-application

A console application in C++ for recording personal, friend and group expenses and viewing them in a table.

## Features

- Add **personal**, **friend** and **group** expenses from a menu
- Email format validation (regex) at startup
- Input validation loops for menu choices and amounts
- Expenses saved to plain text files and shown as a formatted table
- Object-oriented design: an abstract `Expense` base class with `PersonalExpense`, `FriendExpense` and `GroupExpense` subclasses (inheritance, virtual functions)

## Build and run

Requires a C++17 compiler (uses `std::filesystem`).

```bash
git clone https://github.com/dpmeena0607-tech/expense-splitting-application.git
cd expense-splitting-application
g++ -std=c++17 -I headerfiles mainfiles/*.cpp -o expense-splitter
./expense-splitter
```

Run it from the repository root. The program creates a `Data/` folder on first run to store the expense files.

## Project structure

```
headerfiles/   Expense.h, Dashboard.h, Login.h
mainfiles/     splitwise.cpp (main + menu), Dashboard.cpp, expense.cpp, Login.cpp
Data/          created at runtime (ignored by git)
```

## Known limitations

- The login step checks the email format only. It does not store or verify passwords.
- **Settle Up** is a placeholder: it prints a message but does not calculate balances.
- Group expenses are saved with the payer, description and total, but the amount is not yet divided among members.

## Credits

Original code by Krish Mehta (KM701). Used with permission.
