#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H
#include <fstream>
#include "../managers/AccountManager.h"
#include "../managers/TransactionManager.h"
#include "../utils/InputValidation.h"

// Saves and loads accounts and transactions: data/accounts.dat and data/transaction.dat
class FileManager
{
public:
    static const char* statusName(TransactionStatus s)
    {
        switch (s) { case TransactionStatus::PENDING: return "PENDING"; case TransactionStatus::SUCCESSFUL: return "SUCCESSFUL";
                     case TransactionStatus::FAILED: return "FAILED"; case TransactionStatus::CANCELLED: return "CANCELLED"; default: return "REVERSED"; }
    }
    static void save(const AccountManager& am, const TransactionManager& tm)
    {
        std::ofstream a("data/accounts.dat");
        for (const Account* x : am.getAllAccounts())
            a << x->getAccountId() << '|' << x->getHolderName() << '|' << x->getBalance() << '|' << (x->getStatus() == AccountStatus::ACTIVE ? "ACTIVE" : "BLOCKED") << '\n';
        std::ofstream t("data/transaction.dat");
        for (const Transaction& x : tm.getAllTransactions())
            t << x.getTransactionId() << '|' << x.getSenderAccountId() << '|' << x.getReceiverAccountId() << '|' << x.getAmount() << '|' << x.getCreatedAt() << '|'
              << statusName(x.getStatus()) << '|' << x.getMode() << '|' << (x.getReason().empty() ? "-" : x.getReason()) << '|' << x.getSettledAt() << '\n';
    }
    static void load(AccountManager& am, TransactionManager& tm)
    {
        std::string line;
        std::ifstream a("data/accounts.dat");
        while (std::getline(a, line)) {
            auto v = InputValidation::split(line, '|');
            try { if (v.size() >= 4) am.addAccount(Account(v[0], v[1], std::stoll(v[2]), v[3] == "BLOCKED" ? AccountStatus::BLOCKED : AccountStatus::ACTIVE)); } catch (...) {}
        }
        std::ifstream t("data/transaction.dat");
        while (std::getline(t, line)) {
            auto v = InputValidation::split(line, '|');
            try {
                if (v.size() < 9) continue;
                Transaction x(v[0], v[1], v[2], std::stoll(v[3]), v[4]);
                x.setSettlement(v[6], v[7] == "-" ? "" : v[7], v[8]);
                if (v[5] == "SUCCESSFUL") x.updateStatus(TransactionStatus::SUCCESSFUL);
                else if (v[5] == "FAILED") x.updateStatus(TransactionStatus::FAILED);
                tm.addTransaction(x);
            } catch (...) {}
        }
    }
};
#endif
