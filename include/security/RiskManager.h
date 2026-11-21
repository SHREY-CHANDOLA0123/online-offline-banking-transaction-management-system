#ifndef RISK_MANAGER_H
#define RISK_MANAGER_H
#include "../../src/models/Account.h"

// Decides whether a payment is safe to settle. Offline payments are pre-authorised up to a fixed limit.
class RiskManager
{
public:
    static constexpr long long OFFLINE_LIMIT = 500000;   // Rs 5,000 in paise

    static bool canSettleOnline(const Account& sender, long long amount)
    { return sender.getStatus() == AccountStatus::ACTIVE && sender.getBalance() >= amount; }

    static bool canSettleOffline(const Account& sender, long long amount)
    { return sender.getStatus() == AccountStatus::ACTIVE && amount <= OFFLINE_LIMIT; }
};
#endif
