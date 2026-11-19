#ifndef LEDGER_H
#define LEDGER_H

#include <map>
#include <string>
#include <vector>

// One payment suggested by the settle-up calculation: `from` pays `to`.
struct Transfer {
    std::string from;
    std::string to;
    long long paise;  // amount in paise (1/100 rupee) to avoid floating-point errors
};

// Records shared expenses and works out who owes whom.
// Money is stored as integer paise; names are stored lower-case and trimmed.
class Ledger {
public:
    explicit Ledger(std::string path = "Data/Ledger.txt");

    // Records that `payer` paid `amount` and the cost is split equally among
    // `participants` (duplicates ignored). Any leftover paise is handed out one
    // each to the first participants, so shares always add up to the total.
    // Returns false if the amount or names are invalid or the file cannot be written.
    bool recordEqualSplit(const std::string& payer,
                          const std::string& description,
                          double amount,
                          const std::vector<std::string>& participants) const;

    // Net balance per person in paise: positive = is owed money, negative = owes money.
    std::map<std::string, long long> balances() const;

    // Greedy settle-up: repeatedly lets the person who owes the most pay the person
    // owed the most. Uses at most (people - 1) payments.
    static std::vector<Transfer> minimizeTransfers(std::map<std::string, long long> balances);

    static std::string formatMoney(long long paise);
    static std::string normalize(const std::string& name);

private:
    std::string path_;
};

#endif  // LEDGER_H
