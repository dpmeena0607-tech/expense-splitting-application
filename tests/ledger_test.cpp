// Unit tests for the Ledger (equal split, balances, settle-up).
// Build and run from the repository root:
//   g++ -std=c++17 -I headerfiles tests/ledger_test.cpp mainfiles/Ledger.cpp -o ledger_test && ./ledger_test
#include "Ledger.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <map>
#include <string>

namespace {

long long sumOf(const std::map<std::string, long long>& m) {
    long long s = 0;
    for (const auto& kv : m) s += kv.second;
    return s;
}

// Applying every suggested payment must bring every balance back to zero.
void assertSettles(std::map<std::string, long long> net, const std::vector<Transfer>& transfers) {
    for (const Transfer& t : transfers) {
        net[t.from] += t.paise;
        net[t.to] -= t.paise;
    }
    for (const auto& kv : net) assert(kv.second == 0);
}

}  // namespace

int main() {
    const std::string path = "ledger_test_data.txt";

    // 1. Even split: Alice pays 300 for three people.
    std::remove(path.c_str());
    {
        Ledger ledger(path);
        assert(ledger.recordEqualSplit("Alice", "dinner", 300.00, {"alice", "bob", "carol"}));
        auto net = ledger.balances();
        assert(net["alice"] == 20000);
        assert(net["bob"] == -10000);
        assert(net["carol"] == -10000);
        assert(sumOf(net) == 0);
    }

    // 2. Uneven amount: 100.00 among three is 33.34 + 33.33 + 33.33, nothing lost.
    std::remove(path.c_str());
    {
        Ledger ledger(path);
        assert(ledger.recordEqualSplit("bob", "cab", 100.00, {"alice", "bob", "carol"}));
        auto net = ledger.balances();
        assert(net["alice"] == -3334);
        assert(net["bob"] == 6667);
        assert(net["carol"] == -3333);
        assert(sumOf(net) == 0);
    }

    // 3. Several expenses, then settle-up uses at most (people - 1) payments.
    std::remove(path.c_str());
    {
        Ledger ledger(path);
        assert(ledger.recordEqualSplit("alice", "dinner", 300.00, {"alice", "bob", "carol"}));
        assert(ledger.recordEqualSplit("bob", "tickets", 90.00, {"bob", "carol"}));
        auto net = ledger.balances();
        assert(net["alice"] == 20000);
        assert(net["bob"] == -5500);
        assert(net["carol"] == -14500);

        auto transfers = Ledger::minimizeTransfers(net);
        assert(transfers.size() == 2);
        assertSettles(net, transfers);
    }

    // 4. Names are trimmed and case-insensitive; duplicates in the member list count once.
    std::remove(path.c_str());
    {
        Ledger ledger(path);
        assert(ledger.recordEqualSplit("  ALICE ", "snacks", 50.00, {"Alice", "alice", " Bob "}));
        auto net = ledger.balances();
        assert(net.size() == 2);
        assert(net["alice"] == 2500);
        assert(net["bob"] == -2500);
    }

    // 5. Invalid input is rejected and nothing is written.
    std::remove(path.c_str());
    {
        Ledger ledger(path);
        assert(!ledger.recordEqualSplit("alice", "free", 0.0, {"alice", "bob"}));
        assert(!ledger.recordEqualSplit("alice", "negative", -5.0, {"alice", "bob"}));
        assert(!ledger.recordEqualSplit("", "no payer", 10.0, {"alice"}));
        assert(!ledger.recordEqualSplit("alice", "nobody", 10.0, {}));
        assert(ledger.balances().empty());
    }

    // 6. Already balanced: no payments needed.
    assert(Ledger::minimizeTransfers({{"a", 0}, {"b", 0}}).empty());
    assert(Ledger::minimizeTransfers({}).empty());

    // 7. Money formatting.
    assert(Ledger::formatMoney(14500) == "145.00");
    assert(Ledger::formatMoney(5) == "0.05");
    assert(Ledger::formatMoney(-3334) == "-33.34");

    std::remove(path.c_str());
    std::cout << "All ledger tests passed.\n";
    return 0;
}
