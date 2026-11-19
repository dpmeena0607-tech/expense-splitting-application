#include "Dashboard.h"
#include "Expense.h"
#include "Ledger.h"
#include <iostream>
#include <fstream>

void Dashboard::addPersonalExpense(const string& description, double amount, const string& payer) {
    PersonalExpense expense(description, amount, payer);
    ofstream personalFile("Data/PersonalExpense.txt", ios::app);
    if (personalFile) {
        expense.writeToFile(personalFile);
    } else {
        cerr << "Error: Could not open file for personal expense." << endl;
    }
}

void Dashboard::addFriendExpense(const string& description, double amount, const string& payer, const string& friendName) {
    FriendExpense expense(description, amount, payer, friendName);
    ofstream friendFile("Data/FriendExpense.txt", ios::app);
    if (friendFile) {
        friendFile << friendName << "," << description << "," << amount << endl;

        // Added: split the expense equally between the payer and the friend.
        Ledger ledger;
        if (!ledger.recordEqualSplit(payer, description, amount, {payer, friendName})) {
            cerr << "Warning: could not record this expense for settle-up." << endl;
        }
    } else {
        cerr << "Error: Could not open file for friend expense." << endl;
    }
}

void Dashboard::addGroupExpense(const string& description, double amount, const string& payer, const vector<string>& groupMembers) {
    GroupExpense expense(description, amount, payer, groupMembers);
    ofstream groupFile("Data/GroupExpense.txt", ios::app);
    if (groupFile) {
        groupFile << payer << "," << description << "," << amount << endl;

        // Added: split the expense equally among the members (the payer is part of the group).
        vector<string> participants = groupMembers;
        participants.push_back(payer);
        Ledger ledger;
        if (!ledger.recordEqualSplit(payer, description, amount, participants)) {
            cerr << "Warning: could not record this expense for settle-up." << endl;
        }
    } else {
        cerr << "Error: Could not open file for group expense." << endl;
    }
}

// Added: works out everyone's balance from the ledger and prints the payments that settle it.
void Dashboard::settleUpAmount(const string& payer) {
    Ledger ledger;
    const map<string, long long> net = ledger.balances();
    if (net.empty()) {
        cout << "No shared expenses recorded yet.\n";
        return;
    }

    cout << "Settling up amounts for " << payer << "...\n";
    const string me = Ledger::normalize(payer);
    const auto mine = net.find(me);
    if (mine == net.end()) {
        cout << "No shared expenses found for " << payer << ".\n";
    } else if (mine->second > 0) {
        cout << payer << " is owed Rs " << Ledger::formatMoney(mine->second) << " in total.\n";
    } else if (mine->second < 0) {
        cout << payer << " owes Rs " << Ledger::formatMoney(-mine->second) << " in total.\n";
    } else {
        cout << payer << " is all settled up.\n";
    }

    const vector<Transfer> transfers = Ledger::minimizeTransfers(net);
    if (transfers.empty()) {
        cout << "Everyone is settled up.\n";
        return;
    }
    cout << "\nPayments that settle everything:\n";
    for (const Transfer& t : transfers) {
        cout << "  " << t.from << " pays " << t.to << " Rs " << Ledger::formatMoney(t.paise);
        if (t.from == me || t.to == me) cout << "   <-- you";
        cout << "\n";
    }
}
