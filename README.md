# Expense Splitting Application (C++)

> **Original author: Krish Mehta ([@KM701](https://github.com/KM701)).**
> The base application (menu, expense classes, login, file storage) was written by him and is shared here with his permission.
> Original repository: https://github.com/KM701/expense-splitting-application
>
> **Added in this repository** ([@dpmeena0607-tech](https://github.com/dpmeena0607-tech)): the `Ledger` module, equal splitting of group and friend expenses, the settle-up calculation and the unit tests. See [What was added](#what-was-added).

A console application in C++ for recording personal, friend and group expenses, splitting shared costs, and working out who should pay whom.

## Features

- Add **personal**, **friend** and **group** expenses from a menu
- **Equal split** of group and friend expenses, calculated in integer paise so shares always add up to the total
- **Settle up**: shows each person's balance and a short list of payments that clears all debts
- View expenses as a formatted table
- Email format validation (regex) and input validation loops
- Object-oriented design: abstract `Expense` base class with `PersonalExpense`, `FriendExpense` and `GroupExpense` subclasses
- Unit tests for the split and settle-up logic

## Build and run

Requires a C++17 compiler (uses `std::filesystem`). Run from the repository root.

```bash
git clone https://github.com/dpmeena0607-tech/expense-splitting-application.git
cd expense-splitting-application
g++ -std=c++17 -I headerfiles mainfiles/*.cpp -o expense-splitter
./expense-splitter
```

The program creates a `Data/` folder on first run to store the expense files.

## Run the tests

```bash
g++ -std=c++17 -I headerfiles tests/ledger_test.cpp mainfiles/Ledger.cpp -o ledger_test
./ledger_test
```

## Example

Deepak pays 300 for a dinner with Ravi and Asha, then pays 90 for tickets shared with Ravi. Choosing **Settle Up Amount**:

```
deepak is owed Rs 245.00 in total.

Payments that settle everything:
  ravi pays deepak Rs 145.00   <-- you
  asha pays deepak Rs 100.00   <-- you
```

## How settle-up works

1. Each expense is saved to `Data/Ledger.txt` with the payer, the total and each person's share (equal split; leftover paise go one each to the first people).
2. Balances are computed per person: money paid minus the share owed. Positive means owed money, negative means owes money.
3. A greedy pass repeatedly makes the person who owes the most pay the person owed the most. This clears all debts using at most (people - 1) payments. It is not guaranteed to be the absolute minimum for every possible group.

## Project structure

```
headerfiles/   Expense.h, Dashboard.h, Login.h, Ledger.h
mainfiles/     splitwise.cpp (main + menu), Dashboard.cpp, expense.cpp, Login.cpp, Ledger.cpp
tests/         ledger_test.cpp
Data/          created at runtime (ignored by git)
```

## What was added

| File | Change |
| --- | --- |
| `headerfiles/Ledger.h`, `mainfiles/Ledger.cpp` | New: records splits, computes balances, suggests payments |
| `mainfiles/Dashboard.cpp` | Changed: friend and group expenses are also recorded in the ledger; `settleUpAmount` now does the calculation (it was a placeholder) |
| `tests/ledger_test.cpp` | New: unit tests |
| `README.md`, `.gitignore` | New or rewritten |

All other source files are Krish Mehta's original code, unchanged.

## Known limitations

- The login step checks the email format only. It does not store or verify passwords.
- Splits are always equal; custom shares are not supported yet.
- All users share one ledger file, and names are matched case-insensitively (for example, "Ravi" and "ravi" are the same person).
- Personal, friend and group expenses are also kept in the original text files, which are used for the table view.

## Credits

- Base application: Krish Mehta (KM701), used with permission.
- Ledger, splitting, settle-up and tests: [@dpmeena0607-tech](https://github.com/dpmeena0607-tech).
