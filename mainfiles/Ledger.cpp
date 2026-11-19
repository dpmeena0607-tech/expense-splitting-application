#include "Ledger.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>

using std::map;
using std::string;
using std::vector;

namespace {

// Keeps the file's separator characters out of free text.
string clean(string s) {
    for (char& c : s) {
        if (c == '|' || c == ';' || c == ':' || c == '\n' || c == '\r') c = '_';
    }
    return s;
}

vector<string> split(const string& s, char sep) {
    vector<string> parts;
    std::stringstream ss(s);
    string item;
    while (std::getline(ss, item, sep)) parts.push_back(item);
    return parts;
}

}  // namespace

Ledger::Ledger(string path) : path_(std::move(path)) {}

string Ledger::normalize(const string& name) {
    const size_t begin = name.find_first_not_of(" \t\r\n");
    if (begin == string::npos) return "";
    const size_t end = name.find_last_not_of(" \t\r\n");
    string out = name.substr(begin, end - begin + 1);
    for (char& c : out) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return clean(out);
}

string Ledger::formatMoney(long long paise) {
    std::ostringstream os;
    if (paise < 0) {
        os << '-';
        paise = -paise;
    }
    os << paise / 100 << '.' << std::setw(2) << std::setfill('0') << paise % 100;
    return os.str();
}

// File format, one expense per line:  payer|description|totalPaise|name:share;name:share
bool Ledger::recordEqualSplit(const string& payer,
                              const string& description,
                              double amount,
                              const vector<string>& participants) const {
    const long long total = std::llround(amount * 100.0);
    const string payerKey = normalize(payer);
    if (total <= 0 || payerKey.empty()) return false;

    vector<string> people;
    for (const string& p : participants) {
        const string key = normalize(p);
        if (!key.empty() && std::find(people.begin(), people.end(), key) == people.end()) {
            people.push_back(key);
        }
    }
    if (people.empty()) return false;

    const long long count = static_cast<long long>(people.size());
    const long long base = total / count;
    const long long extra = total % count;

    std::ofstream out(path_, std::ios::app);
    if (!out) return false;

    out << payerKey << '|' << clean(description) << '|' << total << '|';
    for (long long i = 0; i < count; ++i) {
        if (i > 0) out << ';';
        out << people[static_cast<size_t>(i)] << ':' << base + (i < extra ? 1 : 0);
    }
    out << '\n';
    return static_cast<bool>(out);
}

map<string, long long> Ledger::balances() const {
    map<string, long long> net;
    std::ifstream in(path_);
    string line;
    while (std::getline(in, line)) {
        const vector<string> fields = split(line, '|');
        if (fields.size() != 4) continue;

        long long total = 0;
        try {
            total = std::stoll(fields[2]);
        } catch (...) {
            continue;
        }

        vector<std::pair<string, long long>> shares;
        long long sum = 0;
        bool ok = true;
        for (const string& item : split(fields[3], ';')) {
            const size_t colon = item.rfind(':');
            if (colon == string::npos) {
                ok = false;
                break;
            }
            try {
                const long long share = std::stoll(item.substr(colon + 1));
                shares.emplace_back(item.substr(0, colon), share);
                sum += share;
            } catch (...) {
                ok = false;
                break;
            }
        }
        if (!ok || sum != total) continue;  // skip damaged lines instead of corrupting balances

        net[fields[0]] += total;
        for (const auto& share : shares) net[share.first] -= share.second;
    }
    return net;
}

vector<Transfer> Ledger::minimizeTransfers(map<string, long long> balances) {
    vector<Transfer> transfers;
    const auto byAmount = [](const auto& a, const auto& b) { return a.second < b.second; };
    while (!balances.empty()) {
        auto creditor = std::max_element(balances.begin(), balances.end(), byAmount);
        auto debtor = std::min_element(balances.begin(), balances.end(), byAmount);
        if (creditor->second <= 0 || debtor->second >= 0) break;

        const long long amount = std::min(creditor->second, -debtor->second);
        transfers.push_back({debtor->first, creditor->first, amount});
        creditor->second -= amount;
        debtor->second += amount;
    }
    return transfers;
}
